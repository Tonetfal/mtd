#include "Gameplay/Spawner/MTD_FoeSpawner.h"

#include "Character/MTD_BaseFoeCharacter.h"
#include "Engine/DataTable.h"
#include "Gameplay/Difficulty/MTD_GameDifficultySubsystem.h"
#include "Gameplay/Levels/MTD_LevelDifficultyDefinition.h"
#include "Gameplay/Spawner/MTD_SpawnerCoreTypes.h"
#include "Gameplay/Wave/MTD_GameWavesSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Settings/MTD_GameDifficultySettings.h"

AMTD_FoeSpawner::AMTD_FoeSpawner()
{
    // Tick to spawn foes
    PrimaryActorTick.bCanEverTick = true;

    // Tick only if told so
    PrimaryActorTick.bStartWithTickEnabled = false;

    // Compute fixed delta seconds
    TickInterval = (1.f / TicksPerSecond);

    // Set tickrate to a fixed amount
    SetActorTickInterval(TickInterval);
}

void AMTD_FoeSpawner::StartSpawning()
{
    // Avoid starting spawning if there is nothing to spawn
    const int32 Total = GetTotalFoesToSpawn();
    if (Total <= 0)
    {
        MTDS_WARN("There is no foe to spawn. Avoid starting spawning.");
        return;
    }
    
    SetIsSpawning(true);
}

void AMTD_FoeSpawner::StopSpawning()
{
    SetIsSpawning(false);
}

int32 AMTD_FoeSpawner::GetTotalFoesOfClassToSpawn(TSubclassOf<AMTD_BaseFoeCharacter> FoeClass) const
{
    if (!FoeClass)
    {
        MTDS_WARN("Foe class is NULL.");
        return 0;
    }

    const auto Found = CachedFoesToSpawn.Find(FoeClass);
    if (!Found)
    {
        MTDS_LOG("No foes of class [%s] will be spawned.", *FoeClass->GetName());
        return 0;
    }

    // Sum up all entries
    int32 TotalTimes = 0;
    for (const int32 Times : *Found)
    {
        TotalTimes += Times;
    }
    
    return TotalTimes;
}

TMap<TSubclassOf<AMTD_BaseFoeCharacter>, int32> AMTD_FoeSpawner::GetTotalFoesOfClassesToSpawn() const
{
    TMap<TSubclassOf<AMTD_BaseFoeCharacter>, int32> TotalTimesMap;
    for (const auto &[FoeClass, CachedSpawnValues] : CachedFoesToSpawn)
    {
        const int32 TotalTimes = GetTotalFoesOfClassToSpawn(FoeClass);
        TotalTimesMap.Add(FoeClass, TotalTimes);
    }

    return TotalTimesMap;
}

int32 AMTD_FoeSpawner::GetTotalFoesToSpawn() const
{
    int32 OverallTotalTimes = 0;
    const TMap<TSubclassOf<AMTD_BaseFoeCharacter>, int32> TotalFoeToSpawnList = GetTotalFoesOfClassesToSpawn();
    for (const auto &[_, SpecificTotalTimes] : TotalFoeToSpawnList)
    {
        OverallTotalTimes += SpecificTotalTimes;
    }

    return OverallTotalTimes;
}

void AMTD_FoeSpawner::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    ensure(TicksSinceWaveStart < TotalTicksCurrentWave);

    // Use the cached values to check how many entities of particular class has to be spawned at current tick
    for (const auto &[FoeClass, CachedSpawnValues] : CachedFoesToSpawn)
    {
        const int32 SpawnTimes = CachedSpawnValues[TicksSinceWaveStart];
        for (int32 i = 0; i < SpawnTimes; i++)
        {
            SpawnFoe(FoeClass);
        }
    }

    TicksSinceWaveStart++;
    if (TicksSinceWaveStart >= TotalTicksCurrentWave)
    {
        // All foes have been spawned at this point, stop spawning
        SetIsSpawning(false);
    }
}

void AMTD_FoeSpawner::Initialize(const UMTD_LevelDefinition *SelectedLevelDefinition,
	const UMTD_LevelDifficultyDefinition *SelectedLevelDifficultyDefinition)
{
    check(IsValid(SelectedLevelDifficultyDefinition));
	LevelDiffDefinition = SelectedLevelDifficultyDefinition;
    
    ListenForWaveEnd();
    CacheLevelData();
    BuildCacheMap();
    PrepareForNextWave();
}

FTransform AMTD_FoeSpawner::GetSpawnTransform() const
{
    FTransform Transform = GetActorTransform();
    const FVector &Offset = GetRandomPointInSpawnArea();
    Transform.AddToTranslation(Offset);

    return Transform;
}

FVector AMTD_FoeSpawner::GetRandomPointInSpawnArea() const
{
    // Randomize displacement
    const float X = FMath::FRandRange(-1.f, +1.f);
    const float Y = FMath::FRandRange(-1.f, +1.f);
    const FVector NormalizeDisplacement = FVector(X, Y, 0.f);

    // Apply a randomized scale on displacement
    const float Scale = FMath::FRandRange(0.f, MaxSpawnRange);
    const FVector Position = (NormalizeDisplacement * Scale);

    return Position;
}

void AMTD_FoeSpawner::SpawnFoe(TSubclassOf<AMTD_BaseFoeCharacter> FoeClass)
{
    check(FoeClass);
    
    const FTransform Transform = GetSpawnTransform();
    constexpr auto CollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    // Spawn the foe
    auto Foe = GetWorld()->SpawnActorDeferred<AMTD_BaseFoeCharacter>(
        FoeClass, Transform, nullptr, nullptr, CollisionHandlingMethod);
    
    if (!IsValid(Foe))
    {
        MTDS_WARN("Failed to spawn a foe of class [%s].", *FoeClass->GetName());
        return;
    }

    // Foes must have a controller
    Foe->SpawnDefaultController();

    // Finish spawning
    Foe->FinishSpawning(Transform);

    // Notify about spawn
    OnSpawnDelegate.Broadcast(Foe);
}

void AMTD_FoeSpawner::ListenForWaveEnd()
{
    const auto GameWavesSubsystem = UMTD_GameWavesSubsystem::Get(this);
    check(IsValid(GameWavesSubsystem));
    
    // Listen for wave end events
    GameWavesSubsystem->OnWaveEndDelegate.AddDynamic(this, &ThisClass::OnWaveEnd);
}

void AMTD_FoeSpawner::CacheLevelData()
{
    // Cache foe quantity data asset
    const auto GameDifficultySettings = UMTD_GameDifficultySettings::Get();
    FoeQuantityAsset = GameDifficultySettings->FoeQuantityAsset.LoadSynchronous();
    if (!IsValid(FoeQuantityAsset))
    {
        MTDS_WARN("Foe quantity data asset is invalid.");
        return;
    }
}

void AMTD_FoeSpawner::BuildCacheMap()
{
    if (!IsValid(FoeQuantityAsset))
    {
        MTDS_WARN("Foe quantity data asset is invalid.");
        return;
    }
    
    // Build cache map with spawnable foe classes
    for (const auto &[FoeClass, _] : FoeQuantityAsset->QuantityMap)
    {
        CachedFoesToSpawn.Add(FoeClass);
    }
}

void AMTD_FoeSpawner::PrepareForNextWave()
{
    const auto GameWavesSubsystem = UMTD_GameWavesSubsystem::Get(this);
    check(IsValid(GameWavesSubsystem));
    
    const auto GameDifficultySubsystem = UMTD_GameDifficultySubsystem::Get(this);
    check(IsValid(GameDifficultySubsystem));

    // Get main information about current wave
    const float TotalWaveTime = GameDifficultySubsystem->GetTotalCurrentWaveTime();
    const int32 CurrentWave = GameWavesSubsystem->GetCurrentWave();
    GlobalQuantityRate = GameDifficultySubsystem->GetScaledQuantity();
    
    // Cache foe spawns that have to take place along the whole wave
    CacheFoeSpawns(TotalWaveTime, CurrentWave);

    // Reset ticks counter
    TicksSinceWaveStart = 0;

    // Notify about the preparation if there are listeners
    if (OnPreparedForNewWaveDelegate.IsBound())
    {
        const int32 TotalFoesToSpawn = GetTotalFoesToSpawn();
        OnPreparedForNewWaveDelegate.Broadcast(TotalFoesToSpawn);
    }
}

void AMTD_FoeSpawner::CacheFoeSpawns(float WaveTotalTime, int32 Wave)
{
    if (!IsValid(FoeQuantityAsset))
    {
        MTDS_WARN("Foe quantity asset is invalid.");
        return;
    }
    
    if (!IsValid(LevelDiffDefinition))
    {
        MTDS_WARN("Level difficulty definition is invalid.");
        return;
    }

    // Get foe quantity rate that has to be used for this wave
    const UMTD_FoeRateDefinition *RateDef = LevelDiffDefinition->GetCharacterRateDefinition(Wave);
    if (!IsValid(RateDef))
    {
        MTDS_WARN("Foe rate definition for wave [%d] on difficulty [%s] is invalid.", Wave,
            *LevelDiffDefinition->GetName());
        return;
    }

    // Get intensivity curve that has to be used for this wave
    const UCurveFloat *IntensivityCurve = LevelDiffDefinition->GetIntensivityCurve(Wave);
    if (!IsValid(IntensivityCurve))
    {
        MTDS_WARN("Intensivity float curve for wave [%d] on difficulty [%s] is invalid.", Wave, 
            *IntensivityCurve->GetName());
        return;
    }
    
    for (auto &[FoeClass, CachedSpawnValues] : CachedFoesToSpawn)
    {
        // Compute total ticks for next wave to reserve all the required memory for that;
        // +1 because it computes spawn times also for time 0
        TotalTicksCurrentWave = (static_cast<int32>(WaveTotalTime * TicksPerSecond) + 1);
        
        // Erase previously cached data, allocate enough memory for the next wave
        CachedSpawnValues.Empty(TotalTicksCurrentWave);

        // Find how much the quantity should be multiplied by for the current foe class
        const auto FoundRate = RateDef->RateMap.Find(FoeClass);
        if (!FoundRate)
        {
            MTDS_WARN("Failed to find foe rate for foe [%s] in [%s].", *FoeClass->GetName(),
                *RateDef->GetName());
            continue;
        }

        // Dereference the pointer and store the value
        const float CurrentFoeQuantityMultiplier = *FoundRate;

        // Keep track of quantity over frames
        float Quantity = 0.f;
        
        // Simulate updates at a fixed delta time
        for (int32 TickNumber = 0; ; TickNumber++)
        {
            // Compute simulated value since wave start
            const float TimeSinceWaveStart = (TickInterval * TickNumber);

            // Finish simulation if time is out the total wave time
            if (TimeSinceWaveStart > WaveTotalTime)
            {
                break;
            }

            // Compute normalized time value
            const float Ratio = (TimeSinceWaveStart / WaveTotalTime);
            
            // Find intensivity on graph
            const float Intensivity = IntensivityCurve->GetFloatValue(Ratio);

            // Scale the intensivity value by some multipliers
            const float AdditionalQuantity = (Intensivity * GlobalQuantityRate * CurrentFoeQuantityMultiplier);

            // Keep track of current quantity amount over frames
            Quantity += AdditionalQuantity;

            // Get how much quantity is required to spawn a single entity of given foe class
            const float QuantityThreshold = *FoeQuantityAsset->QuantityMap.Find(FoeClass);

            // Add a new entry
            CachedSpawnValues.Add(0);
            
            // Avoid using the little reference hack every time, but only when it may be needed
            if (Quantity >= QuantityThreshold)
            {
                // Get a reference to the spawn times value instead of accessing it from the container in a loop
                int32 &SpawnTimesAtCurrentTick = CachedSpawnValues.Last();

                // Increase spawn amount at current tick until the current quality is above the threshold
                for (; Quantity >= QuantityThreshold; Quantity -= QuantityThreshold)
                {
                    // Increase spawn entity number by one for a specific class at a specific tick
                    SpawnTimesAtCurrentTick++;
                }
            }
        }
    }
}

void AMTD_FoeSpawner::SetIsSpawning(bool bFlag)
{
    // Cache state only if it's different
    if (bIsSpawning != bFlag)
    {
        bIsSpawning = bFlag;
        SetActorTickEnabled(bIsSpawning);
    }
}

void AMTD_FoeSpawner::OnWaveEnd(float WaveDuration)
{
    PrepareForNextWave();
}
