#include "Gameplay/Spawner/MTD_FoeSpawnSubsystem.h"

#include "Character/Components/MTD_HealthComponent.h"
#include "Character/MTD_BaseFoeCharacter.h"
#include "Gameplay/Difficulty/MTD_GameDifficultySubsystem.h"
#include "Gameplay/Spawner/MTD_FoeSpawner.h"
#include "Gameplay/Wave/MTD_GameWavesSubsystem.h"
#include "Kismet/GameplayStatics.h"

UMTD_FoeSpawnSubsystem *UMTD_FoeSpawnSubsystem::Get(const UObject *WorldContextObject)
{
    if (!IsValid(WorldContextObject))
    {
        MTD_WARN("World context object is invalid.");
        return nullptr;
    }
    
    const UWorld *World = WorldContextObject->GetWorld();
    if (!IsValid(World))
    {
        MTD_WARN("World is invalid.");
        return nullptr;
    }
    
    const auto Subsystem = World->GetSubsystem<UMTD_FoeSpawnSubsystem>();
    if (!IsValid(Subsystem))
    {
        MTD_WARN("Game difficulty subsystem is invalid.");
        return nullptr;
    }
    
    return Subsystem;
}

TStatId UMTD_FoeSpawnSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UMTD_FoeSPawnSubsystem, STATGROUP_Tickables);
}

void UMTD_FoeSpawnSubsystem::PostInitialize()
{
    Super::PostInitialize();
    
    const auto GameWavesSubsystem = UMTD_GameWavesSubsystem::Get(this);
    check(IsValid(GameWavesSubsystem));

    // Listen for wave start and end events
    GameWavesSubsystem->OnWaveStartDelegate.AddDynamic(this, &ThisClass::OnWaveStarted);
    GameWavesSubsystem->OnWaveEndDelegate.AddDynamic(this, &ThisClass::OnWaveEnded);

    const auto GameDifficultySubsystem = UMTD_GameDifficultySubsystem::Get(this);
    check(IsValid(GameDifficultySubsystem));

    // Listen for difficulty initialization
    UMTD_GameDifficultySubsystem::FOnDifficultySubsystemInitializedSignature::FDelegate Delegate;
    Delegate.BindUObject(this, &ThisClass::OnDifficultySubsystemInitialized);
    GameDifficultySubsystem->OnSubsystemInitialized_RegisterAndCall(Delegate);
}

void UMTD_FoeSpawnSubsystem::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    DispatchCachedData();
}

void UMTD_FoeSpawnSubsystem::DispatchCachedData()
{
    if (!SpawnedFoes.IsEmpty())
    {
        OnFoeSpawnedDelegate.Broadcast(SpawnedFoes);
        SpawnedFoes.Empty();
    }
    
    if (!KilledFoes.IsEmpty())
    {
        OnFoeKilledDelegate.Broadcast(KilledFoes);
        KilledFoes.Empty();
    }
}

void UMTD_FoeSpawnSubsystem::OnDifficultySubsystemInitialized(const UMTD_LevelDefinition *SelectedLevelDefinition,
    const UMTD_LevelDifficultyDefinition *SelectedLevelDifficultyDefinition)
{
    InitializeSpawners(SelectedLevelDefinition, SelectedLevelDifficultyDefinition);
}

void UMTD_FoeSpawnSubsystem::InitializeSpawners(const UMTD_LevelDefinition *SelectedLevelDefinition,
    const UMTD_LevelDifficultyDefinition *SelectedLevelDifficultyDefinition)
{
    check(SelectedLevelDefinition);
    check(SelectedLevelDifficultyDefinition);
    
    // Cache the spawners we'll be managing
    CacheSpawners();
    
    for (AMTD_FoeSpawner *Spawner : Spawners)
    {
        Spawner->Initialize(SelectedLevelDefinition, SelectedLevelDifficultyDefinition);
    }
}

void UMTD_FoeSpawnSubsystem::CacheSpawners()
{
    // Get all foe spawners
    TArray<AActor *> OutActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMTD_FoeSpawner::StaticClass(), OutActors);

    // Iterate through all found spawners
    for (AActor *Actor : OutActors)
    {
        auto Spawner = Cast<AMTD_FoeSpawner>(Actor);

        // Listen for foe spawn and preparation events
        Spawner->OnSpawnDelegate.AddDynamic(this, &ThisClass::OnFoeSpawn);
        Spawner->OnPreparedForNewWaveDelegate.AddDynamic(this, &ThisClass::OnSpawnerPrepared);

        // Cache a spawner
        Spawners.Add(Spawner);
    }
}

void UMTD_FoeSpawnSubsystem::OnWaveStarted(int32 WaveNumber, float RemainingForceStartTime)
{
    for (AMTD_FoeSpawner *Spawner : Spawners)
    {
        Spawner->StartSpawning();
    }

    // Reset counter for this wave
    KilledFoesOnCurrentWave = 0;
}

void UMTD_FoeSpawnSubsystem::OnWaveEnded(float WaveDuration)
{
    for (AMTD_FoeSpawner *Spawner : Spawners)
    {
        Spawner->StopSpawning();
    }

    // Reset counter for next wave
    TotalFoesToSpawnOnCurrentWave = 0;
}

void UMTD_FoeSpawnSubsystem::OnFoeSpawn(AMTD_BaseFoeCharacter *ForCharacter)
{
    MTD_VERBOSE("[%s] has spawned.", *ForCharacter->GetName());

    // Save information about spawn event
    SpawnedFoes.Push(ForCharacter);
    SpawnedFoesOnCurrentWave++;

    auto HealthComponent = UMTD_HealthComponent::FindHealthComponent(ForCharacter);
    check(IsValid(HealthComponent));

    // Listen for death event
    HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnFoeKilled);
}

void UMTD_FoeSpawnSubsystem::OnFoeKilled(AActor *FoeActor)
{
    MTD_VERBOSE("[%s] has died.", *FoeActor->GetName());

    // Save information about death event
    KilledFoes.Push(FoeActor);
    KilledFoesOnCurrentWave++;
    
    ensure(KilledFoesOnCurrentWave <= TotalFoesToSpawnOnCurrentWave);
    if (KilledFoesOnCurrentWave == TotalFoesToSpawnOnCurrentWave)
    {
        // Notify about the fact that every foe spawned during current wave has been killed
        OnAllFoesKilledDelegate.Broadcast(TotalFoesToSpawnOnCurrentWave);
    }
}

void UMTD_FoeSpawnSubsystem::OnSpawnerPrepared(int32 TotalFoesToSpawn)
{
    TotalFoesToSpawnOnCurrentWave += TotalFoesToSpawn;
}
