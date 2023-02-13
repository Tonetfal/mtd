#include "Gameplay/Spawner/MTD_FoeSpawnerManager.h"

#include "Character/MTD_BaseFoeCharacter.h"
#include "Character/MTD_HealthComponent.h"
#include "GameModes/MTD_TowerDefenseMode.h"
#include "Gameplay/MTD_WaveManager.h"
#include "Gameplay/Spawner/MTD_FoeSpawner.h"
#include "Kismet/GameplayStatics.h"

void UMTD_FoeSpawnerManager::Initialize()
{
    UWorld *World = GetWorld();
    auto TowerDefenseMode = Cast<AMTD_TowerDefenseMode>(UGameplayStatics::GetGameMode(World));
    if (!IsValid(TowerDefenseMode))
    {
        MTDS_WARN("Tower defense mode is invaid.");
        return;
    }

    UMTD_WaveManager *WaveManager = TowerDefenseMode->GetWaveManager();
    if (!IsValid(WaveManager))
    {
        MTDS_WARN("Wave manager is invalid.");
        return;
    }

    // Listen for wave start and end events
    WaveManager->OnWaveStartDelegate.AddDynamic(this, &ThisClass::OnWaveStarted);
    WaveManager->OnWaveEndDelegate.AddDynamic(this, &ThisClass::OnWaveEnded);

    // Cache the spawners we'll be managing
    CacheSpawners();

    // Initialize them
    InitializeSpawners();

    if (Spawners.IsEmpty())
    {
        MTDS_WARN("There are no spawners on the map.");
    }
}

void UMTD_FoeSpawnerManager::DispatchCachedData()
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

void UMTD_FoeSpawnerManager::CacheSpawners()
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

void UMTD_FoeSpawnerManager::InitializeSpawners()
{
    for (AMTD_FoeSpawner *Spawner : Spawners)
    {
        Spawner->Initialize();
    }
}

void UMTD_FoeSpawnerManager::OnWaveStarted(int32 WaveNumber, float RemainingForceStartTime)
{
    for (AMTD_FoeSpawner *Spawner : Spawners)
    {
        Spawner->StartSpawning();
    }

    // Reset counter for this wave
    KilledFoesOnCurrentWave = 0;
}

void UMTD_FoeSpawnerManager::OnWaveEnded(float WaveDuration)
{
    for (AMTD_FoeSpawner *Spawner : Spawners)
    {
        Spawner->StopSpawning();
    }

    // Reset counter for next wave
    TotalFoesToSpawnOnCurrentWave = 0;
}

void UMTD_FoeSpawnerManager::OnFoeSpawn(AMTD_BaseFoeCharacter *ForCharacter)
{
    MTD_VERBOSE("[%s] has spawned.", *ForCharacter->GetName());

    // Save information about spawn event
    SpawnedFoes.Push(ForCharacter);
    SpawnedFoesOnCurrentWave++;

    auto HealthComponent = UMTD_HealthComponent::FindHealthComponent(ForCharacter);
    check(IsValid(HealthComponent));

    // Listen for death event
    HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnCharacterKilled);
}

void UMTD_FoeSpawnerManager::OnCharacterKilled(AActor *FoeActor)
{
    MTD_VERBOSE("[%s] has died.", *FoeActor->GetName());

    // Save information about death event
    KilledFoes.Push(FoeActor);
    KilledFoesOnCurrentWave++;
    
    ensure(KilledFoesOnCurrentWave <= TotalFoesToSpawnOnCurrentWave);
    if (KilledFoesOnCurrentWave == TotalFoesToSpawnOnCurrentWave)
    {
        // Notify about the fact that everything has been spawned
        OnAllFoesKilledDelegate.Broadcast(TotalFoesToSpawnOnCurrentWave);
    }
}

void UMTD_FoeSpawnerManager::OnSpawnerPrepared(int32 TotalFoesToSpawn)
{
    TotalFoesToSpawnOnCurrentWave += TotalFoesToSpawn;
}
