#include "Gameplay/Spawner/MTD_SpawnerManager.h"

#include "Character/MTD_HealthComponent.h"
#include "GameModes/MTD_TowerDefenseMode.h"
#include "Gameplay/MTD_WaveManager.h"
#include "Gameplay/Spawner/MTD_CharacterSpawner.h"
#include "Kismet/GameplayStatics.h"

void UMTD_SpawnerManager::Initialize()
{
    auto Tdm = Cast<AMTD_TowerDefenseMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (!IsValid(Tdm))
    {
        MTDS_WARN("Tower Defense Mode is invaid.");
        return;
    }

    UMTD_WaveManager *WaveManager = Tdm->GetWaveManager();
    if (!IsValid(WaveManager))
    {
        MTDS_WARN("Wave Manager is invalid.");
        return;
    }

    // Listen for wave start and end events
    WaveManager->OnWaveStartDelegate.AddDynamic(this, &ThisClass::OnWaveStarted);
    WaveManager->OnWaveEndDelegate.AddDynamic(this, &ThisClass::OnWaveEnded);

    // Cache the spawners it will be managing
    CacheSpawners();

    // Initialize them
    InitializeSpawners();

    if (Spawners.IsEmpty())
    {
        MTDS_WARN("There is no spawners on the map.");
    }
}

void UMTD_SpawnerManager::DispatchCachedData()
{
    if (!SpawnedCharacters.IsEmpty())
    {
        OnCharactersSpawnedDelegate.Broadcast(SpawnedCharacters);
        SpawnedCharacters.Empty();
    }
    
    if (!KilledCharacters.IsEmpty())
    {
        OnCharactersKilledDelegate.Broadcast(KilledCharacters);
        KilledCharacters.Empty();
    }
}

void UMTD_SpawnerManager::CacheSpawners()
{
    TArray<AActor *> OutActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMTD_CharacterSpawner::StaticClass(), OutActors);

    for (AActor *Actor : OutActors)
    {
        auto Spawner = Cast<AMTD_CharacterSpawner>(Actor);

        Spawner->OnSpawnDelegate.AddDynamic(this, &ThisClass::OnCharacterSpawn);
        Spawner->OnPreparedForNewWaveDelegate.AddDynamic(this, &ThisClass::OnSpawnedPrepared);
        Spawners.Add(Spawner);
    }
}

void UMTD_SpawnerManager::InitializeSpawners()
{
    for (AMTD_CharacterSpawner *Spawner : Spawners)
    {
        Spawner->Initialize();
    }
}

void UMTD_SpawnerManager::OnWaveStarted(int32 WaveNumber, float RemainingSeconds)
{
    for (AMTD_CharacterSpawner *Spawner : Spawners)
    {
        Spawner->StartSpawning();
    }
}

void UMTD_SpawnerManager::OnWaveEnded(float WaveDuration)
{
    for (AMTD_CharacterSpawner *Spawner : Spawners)
    {
        Spawner->StopSpawning();
    }
    
    TotalCharactersToSpawnOnCurrentWave = 0;
}

void UMTD_SpawnerManager::OnCharacterSpawn(AActor *Actor)
{
    MTD_VERBOSE("[%s] has spawned.", *Actor->GetName());

    // Save information about spawn event
    SpawnedCharacters.Push(Actor);
    SpawnedCharactersOnCurrentWave++;

    auto HealthComponent = UMTD_HealthComponent::FindHealthComponent(Actor);
    check(IsValid(HealthComponent));

    // Listen for death event
    HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnCharacterKilled);
}

void UMTD_SpawnerManager::OnCharacterKilled(AActor *Actor)
{
    MTD_VERBOSE("[%s] has died.", *Actor->GetName());

    // Save information about death event
    KilledCharacters.Push(Actor);
    KilledCharactersOnCurrentWave++;

    if (KilledCharactersOnCurrentWave > TotalCharactersToSpawnOnCurrentWave)
    {
        // Notify about the fact that everything has been spawned
        OnAllCharactersKilledDelegate.Broadcast(TotalCharactersToSpawnOnCurrentWave);
    }
}

void UMTD_SpawnerManager::OnSpawnedPrepared(int32 TotalCharactersToSpawn)
{
    TotalCharactersToSpawnOnCurrentWave += TotalCharactersToSpawn;
}
