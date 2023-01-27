#include "Gameplay/Spawner/MTD_CharacterSpawner.h"

#include "Character/MTD_BaseCharacter.h"
#include "Engine/DataTable.h"
#include "GameModes/MTD_TowerDefenseMode.h"
#include "Gameplay/MTD_WaveManager.h"
#include "Gameplay/Levels/MTD_LevelDifficultyDefinition.h"
#include "Gameplay/Spawner/MTD_SpawnerCoreTypes.h"
#include "Kismet/GameplayStatics.h"
#include "System/MTD_GameInstance.h"

AMTD_CharacterSpawner::AMTD_CharacterSpawner()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;

    // Compute fixed delta seconds
    TickInterval = (1.f / TicksPerSecond);

    // Set tickrate to a fixed amount
    SetActorTickInterval(TickInterval);
}

void AMTD_CharacterSpawner::StartSpawning()
{
    // Avoid starting spawning if there is nothing to spawn
    const int32 Total = GetTotalCharactersToSpawn();
    if (Total <= 0)
    {
        MTDS_WARN("There is no character to spawn. Avoid starting spawning.");
        return;
    }
    
    SetIsSpawning(true);
}

void AMTD_CharacterSpawner::StopSpawning()
{
    SetIsSpawning(false);
}

int32 AMTD_CharacterSpawner::GetTotalCharactersOfClassToSpawn(TSubclassOf<AMTD_BaseCharacter> CharacterClass) const
{
    if (!CharacterClass)
    {
        MTDS_WARN("Character Class is invalid.");
        return 0;
    }

    const auto Found = CachedCharactersToSpawn.Find(CharacterClass);
    if (!Found)
    {
        MTDS_WARN("No characters of Character Class [%s] will be spawned.", *CharacterClass->GetName());
        return 0;
    }

    int32 TotalTimes = 0;
    for (const int32 Times : *Found)
    {
        TotalTimes += Times;
    }
    return TotalTimes;
}

TMap<TSubclassOf<AMTD_BaseCharacter>, int32> AMTD_CharacterSpawner::GetTotalCharactersOfClassesToSpawn() const
{
    TMap<TSubclassOf<AMTD_BaseCharacter>, int32> TotalTimesMap;
    for (const auto &[CharacterClass, CachedSpawnValues] : CachedCharactersToSpawn)
    {
        const int32 TotalTimes = GetTotalCharactersOfClassToSpawn(CharacterClass);
        TotalTimesMap.Add(CharacterClass, TotalTimes);
    }

    return TotalTimesMap;
}

int32 AMTD_CharacterSpawner::GetTotalCharactersToSpawn() const
{
    int32 OverallTotalTimes = 0;
    const TMap<TSubclassOf<AMTD_BaseCharacter>, int32> TotalCharacterToSpawnList = GetTotalCharactersOfClassesToSpawn();
    for (const auto &[_, SpecificTotalTimes] : TotalCharacterToSpawnList)
    {
        OverallTotalTimes += SpecificTotalTimes;
    }

    return OverallTotalTimes;
}

void AMTD_CharacterSpawner::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    ensure(TicksSinceWaveStart < TotalTicksCurrentWave);

    // Use the cached values to check how many entities of particular class has to be spawned at current tick
    for (const auto &[CharacterClass, CachedSpawnValues] : CachedCharactersToSpawn)
    {
        const int32 SpawnTimes = CachedSpawnValues[TicksSinceWaveStart];
        for (int32 i = 0; i < SpawnTimes; i++)
        {
            SpawnCharacter(CharacterClass);
        }
    }

    TicksSinceWaveStart++;
    if (TicksSinceWaveStart >= TotalTicksCurrentWave)
    {
        // All characters have been spawned at this point, stop spawning
        SetIsSpawning(false);
    }
}

void AMTD_CharacterSpawner::BeginPlay()
{
    Super::BeginPlay();
    
}

void AMTD_CharacterSpawner::Initialize()
{
    // Cache tower defense mode because it will be used often
    TowerDefenseMode = Cast<AMTD_TowerDefenseMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (!IsValid(TowerDefenseMode))
    {
        MTDS_WARN("Tower Defense Mode is invalid.");
        return;
    }

    ListenForWaveEnd();
    CacheLevelData();
    BuildCacheMap();
    PrepareForNewWave();
}

FTransform AMTD_CharacterSpawner::GetSpawnTransform() const
{
    FTransform Transform = GetActorTransform();
    const FVector Offset = GetRandomPointOnSpawnArea();
    Transform.AddToTranslation(Offset);

    return Transform;
}

FVector AMTD_CharacterSpawner::GetRandomPointOnSpawnArea() const
{
    const float X = FMath::FRandRange(-1.f, +1.f);
    const float Y = (((FMath::RandBool()) ? (+1.f) : (-1.f)) * (1.f - FMath::Abs(X)));
    const FVector Direction = FVector(X, Y, 0.f);
    const FVector Position = (Direction * SpawnRange);

    return Position;
}

void AMTD_CharacterSpawner::SpawnCharacter(TSubclassOf<AMTD_BaseCharacter> CharacterClass)
{
    check(CharacterClass);
    const FTransform Transform = GetSpawnTransform();

    // Spawn the character
    auto Character = GetWorld()->SpawnActorDeferred<AMTD_BaseCharacter>(CharacterClass, Transform, nullptr, nullptr,
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
    check(Character);
    
    Character->SpawnDefaultController();
    UGameplayStatics::FinishSpawningActor(Character, Transform);

    // Notify about spawn
    OnSpawnDelegate.Broadcast(Character);
}

void AMTD_CharacterSpawner::ListenForWaveEnd()
{
    if (!IsValid(TowerDefenseMode))
    {
        MTDS_WARN("Tower Defense Mode is invalid.");
        return;
    }
    
    UMTD_WaveManager *WaveManager = TowerDefenseMode->GetWaveManager();
    check(IsValid(WaveManager));
    
    // Listen for wave end events
    WaveManager->OnWaveEndDelegate.AddDynamic(this, &ThisClass::OnWaveEnd);
}

void AMTD_CharacterSpawner::CacheLevelData()
{
    if (!IsValid(TowerDefenseMode))
    {
        MTDS_WARN("Tower Defense Mode is invalid.");
        return;
    }
    
    const auto GameInstance = Cast<UMTD_GameInstance>(TowerDefenseMode->GetGameInstance());
    check(IsValid(GameInstance));

    // Cache enemy quantity data asset
    EnemyQuantityAsset = GameInstance->GetEnemyQuantityAsset();
    if (!IsValid(EnemyQuantityAsset))
    {
        MTDS_WARN("Enemy Quantity Data Asset is invalid.");
        return;
    }
    
    // Cache selected level difficulty definition
    LevelDiffDefinition = TowerDefenseMode->GetSelectedLevelDifficultyDefinition();
    if (!IsValid(LevelDiffDefinition))
    {
        MTDS_WARN("Selected Level Difficulty Definition is invalid.");
        return;
    }
}

void AMTD_CharacterSpawner::BuildCacheMap()
{
    if (!IsValid(EnemyQuantityAsset))
    {
        MTDS_WARN("Enemy Quantity Data Asset is invalid.");
        return;
    }
    
    // Build cache map with spawnable character classes
    for (const auto &[CharacterClass, _] : EnemyQuantityAsset->QuantityMap)
    {
        CachedCharactersToSpawn.Add(CharacterClass);
    }
}

void AMTD_CharacterSpawner::PrepareForNewWave()
{
    if (!IsValid(TowerDefenseMode))
    {
        MTDS_WARN("Tower Defense Mode is invalid.");
        return;
    }

    // Get main information about current wave
    const float TotalWaveTime = TowerDefenseMode->GetTotalCurrentWaveTime();
    const int32 CurrentWave = TowerDefenseMode->GetCurrentWave();
    GlobalQuantityRate = TowerDefenseMode->GetScaledQuantity();
    
    // Cache enemy spawns that have to take place along the whole wave
    ComputeEnemySpawns(TotalWaveTime, CurrentWave);

    // Reset ticks counter
    TicksSinceWaveStart = 0;

    // Notify about the preparation if there are listeners
    if (OnPreparedForNewWaveDelegate.IsBound())
    {
        const int32 TotalCharactersToSpawn = GetTotalCharactersToSpawn();
        OnPreparedForNewWaveDelegate.Broadcast(TotalCharactersToSpawn);
    }
}

void AMTD_CharacterSpawner::ComputeEnemySpawns(float WaveTotalTime, int32 CurrentWave)
{
    if (!IsValid(LevelDiffDefinition))
    {
        MTDS_WARN("Level Difficulty Definition is invalid.");
        return;
    }

    // Get character quantity rate that has to be used for this wave
    const UMTD_CharacterRateDefinition *RateDef = LevelDiffDefinition->GetCharacterRateDefinition(CurrentWave);
    if (!IsValid(RateDef))
    {
        MTDS_WARN("Character Rate Definition for Wave [%d] on Difficulty [%s] is invalid.", CurrentWave,
            *LevelDiffDefinition->GetName());
        return;
    }

    // Get intensivity curve that has to be used for this wave
    const UCurveFloat *IntensivityCurve = LevelDiffDefinition->GetIntensivityCurve(CurrentWave);
    if (!IsValid(IntensivityCurve))
    {
        MTDS_WARN("Intensivity Float Curve for Wave [%d] on Difficulty [%s] is invalid.", CurrentWave, 
            *IntensivityCurve->GetName());
        return;
    }
    
    for (auto &[CharacterClass, CachedSpawnValues] : CachedCharactersToSpawn)
    {
        // Compute total ticks for next wave to reserve all the required memory for that;
        // +1 because it computes spawn times also for time 0
        TotalTicksCurrentWave = (static_cast<int32>(WaveTotalTime * TicksPerSecond) + 1);
        
        // Erase previously cached data, allocate enough memory for the next wave
        CachedSpawnValues.Empty(TotalTicksCurrentWave);

        // Find how much the quantity should be multiplied by for the current character class
        const auto FoundRate = RateDef->RateMap.Find(CharacterClass);
        if (!FoundRate)
        {
            MTDS_WARN("Failed to find Character Rate for Character [%s] in [%s].", *CharacterClass->GetName(),
                *RateDef->GetName());
            continue;
        }

        // Dereference the pointer and store the value
        const float CurrentCharacterQuantityMultiplier = *FoundRate;

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
            const float AdditionalQuantity = (Intensivity * GlobalQuantityRate * CurrentCharacterQuantityMultiplier);

            // Keep track of current quantity amount over frames
            Quantity += AdditionalQuantity;

            // Get how much quantity is required to spawn a single entity of given character class
            const float QuantityThreshold = *EnemyQuantityAsset->QuantityMap.Find(CharacterClass);

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

void AMTD_CharacterSpawner::SetIsSpawning(bool bFlag)
{
    // Cache state only if it's different
    if (bIsSpawning != bFlag)
    {
        bIsSpawning = bFlag;
        SetActorTickEnabled(bIsSpawning);
    }
}

void AMTD_CharacterSpawner::OnWaveEnd(float WaveDuration)
{
    PrepareForNewWave();
}
