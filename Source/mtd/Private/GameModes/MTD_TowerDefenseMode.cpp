#include "GameModes/MTD_TowerDefenseMode.h"

#include "AbilitySystem/MTD_AbilitySet.h"
#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "AIController.h"
#include "Character/MTD_BaseCharacter.h"
#include "Character/MTD_CharacterSpawner.h"
#include "Character/MTD_HealthComponent.h"
#include "GameModes/MTD_Core.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"

AMTD_TowerDefenseMode::AMTD_TowerDefenseMode()
{
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;
}

void AMTD_TowerDefenseMode::BeginPlay()
{
    Super::BeginPlay();

    CacheCores();
    CacheSpawners();
    DispatchAbilities();
    SetupForceWaveStartTimer(SecondsToWaveForceStart);

    // Some elements will spawn after GM, and they may be interested in this event as well, hence delay it for a tick
    GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda(
        [this] () { OnPhaseChangedDelegate.Broadcast(EMTD_GamePhase::Build); }));
}

void AMTD_TowerDefenseMode::OnWaveForceStart()
{
    OnWaveForceStartDelegate.Broadcast();
    StartWave();
}

void AMTD_TowerDefenseMode::StartWave()
{
    if (bWaveRunning)
    {
        return;
    }

    if (MobsToSpawn <= 0)
    {
        MTDS_WARN("Mobs to spawn counter (%d) is now valid.", MobsToSpawn);
        return;
    }

    MTD_LOG("Wave [%d] has started.", CurrentWave);

    OnWaveStartDelegate.Broadcast(CurrentWave, RemainingSecondsToWaveForceStart);
    OnPhaseChangedDelegate.Broadcast(EMTD_GamePhase::Combat);
    
    StartSpawning();
    
    bWaveRunning = true;
}

void AMTD_TowerDefenseMode::EndWave()
{
    MTD_LOG("Wave [%d] has ended.", CurrentWave);
    
    CurrentWave++;

    SpawnedMobsOnCurrentWave = 0;
    KilledMobsOnCurrentWave = 0;

    OnWaveEndDelegate.Broadcast(WaveDuration);
    OnPhaseChangedDelegate.Broadcast(EMTD_GamePhase::Build);
    
    bWaveRunning = false;

    SetupForceWaveStartTimer(SecondsToWaveForceStart);
}

void AMTD_TowerDefenseMode::StartSpawning()
{
    for (AMTD_CharacterSpawner *Spawner : Spawners)
    {
        Spawner->StartSpawning();
    }
}

void AMTD_TowerDefenseMode::StopSpawning()
{
    for (AMTD_CharacterSpawner *Spawner : Spawners)
    {
        Spawner->StopSpawning();
    }
}

AActor *AMTD_TowerDefenseMode::GetGameTarget(APawn *Client) const
{
    check(IsValid(Client));
    
    // Store final result in here
    AActor *Result = nullptr;
    float LowestCost = 0.f;

    // Cache calculation data
    UWorld *World = GetWorld();
    const FVector ClientPosition = Client->GetNavAgentLocation();
    const auto ClientController = CastChecked<AAIController>(Client->GetController());
    const auto NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
    ANavigationData *NavData = NavSys->GetNavDataForProps(ClientController->GetNavAgentPropertiesRef(), ClientPosition);
    const TSubclassOf<UNavigationQueryFilter> FilterClass = ClientController->GetDefaultNavigationFilterClass();

    for (AMTD_Core *Core : Cores)
    {
        // Shorthand calculation data in variables
        float Cost;
        const FVector CorePosition = Core->GetActorLocation();

        // Check path cost a core
        const ENavigationQueryResult::Type PathResult = NavSys->GetPathCost(
            World, ClientPosition, CorePosition, Cost, NavData, FilterClass);

        // Path result must always be valid in terms of passed data validness
        check(PathResult != ENavigationQueryResult::Error);
        check(PathResult != ENavigationQueryResult::Invalid);

        // If the core is reachable check whether its the most optimal path cost wise
        if (PathResult == ENavigationQueryResult::Success)
        {
            if ((!Result) || (LowestCost > Cost))
            {
                LowestCost = Cost;
                Result = Core;
            }
        }
        // Should never happen. The only reason must be a bad level design that doesn't allow an AI to get all the cores
        else
        {
            MTD_WARN("An actor on [%s] cannot get core [%s]", *ClientPosition.ToString(), *Core->GetName());
        }
    }

    return Result;
}

void AMTD_TowerDefenseMode::CacheCores()
{
    TArray<AActor *> OutActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMTD_Core::StaticClass(), OutActors);

    for (AActor *Actor : OutActors)
    {
        auto Core = Cast<AMTD_Core>(Actor);

        // Setup a core and add it to the list
        Core->OnCoreDestroyedDelegate.AddDynamic(this, &ThisClass::OnCoreDestroyed);
        Cores.Add(Core);
    }
}

void AMTD_TowerDefenseMode::CacheSpawners()
{
    TArray<AActor *> OutActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMTD_CharacterSpawner::StaticClass(), OutActors);

    for (AActor *Actor : OutActors)
    {
        auto Spawner = Cast<AMTD_CharacterSpawner>(Actor);

        Spawner->OnSpawnDelegate.AddDynamic(this, &ThisClass::OnMobSpawn);
        Spawners.Add(Spawner);
    }
}

void AMTD_TowerDefenseMode::DispatchAbilities()
{
    if (!PlayerClass)
    {
        MTD_WARN("Player class is not specified.");
        return;
    }

    if (AbilitySets.IsEmpty())
    {
        MTD_WARN("Ability set is empty.");
        return;
    }
    
    TArray<AActor *> OutActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), PlayerClass, OutActors);

    for (AActor *Actor : OutActors)
    {
        auto Player = Cast<AMTD_BaseCharacter>(Actor);
        UMTD_AbilitySystemComponent *MtdAsc = Player->GetMtdAbilitySystemComponent();

        for (const UMTD_AbilitySet *Set : AbilitySets)
        {
            Set->GiveToAbilitySystem(MtdAsc, nullptr, nullptr);
        }
        
        MTD_LOG("Player [%s] has been granted with Tower Defense Mode abilities.", *Player->GetName());
    }
}

void AMTD_TowerDefenseMode::SetupForceWaveStartTimer(float Seconds)
{
    if (!bShouldSetupForceWaveStartCountdown)
    {
        return;
    }

    RemainingSecondsToWaveForceStart = SecondsToWaveForceStart;

    // Call each second
    GetWorld()->GetTimerManager().SetTimer(ForceWaveStartTimerTickTimerHandle, this,
        &ThisClass::WaveForceStartTimerTick, 1.f, true);
}

void AMTD_TowerDefenseMode::WaveForceStartTimerTick()
{
    RemainingSecondsToWaveForceStart -= 1.f;
    
    if (RemainingSecondsToWaveForceStart > 0.f)
    {
        OnWaveForceStartTimerTickDelegate.Broadcast(RemainingSecondsToWaveForceStart);
    }
    else
    {
        RemainingSecondsToWaveForceStart = 0.f;
        OnWaveForceStart();
        GetWorld()->GetTimerManager().ClearTimer(ForceWaveStartTimerTickTimerHandle);
    }
}

void AMTD_TowerDefenseMode::OnCoreDestroyed()
{
    TerminateGame(EMTD_GameResult::Lose);
}

void AMTD_TowerDefenseMode::OnMobSpawn(AActor *Actor)
{
    MTD_VERBOSE("[%s] has spawned.", *Actor->GetName());
    SpawnedMobsOnCurrentWave++;

    auto HealthComponent = UMTD_HealthComponent::FindHealthComponent(Actor);
    HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnMobDied);

    if (SpawnedMobsOnCurrentWave >= MobsToSpawn)
    {
        StopSpawning();
    }
}

void AMTD_TowerDefenseMode::OnMobDied(AActor *Actor)
{
    MTD_VERBOSE("[%s] has died.", *Actor->GetName());
    KilledMobsOnCurrentWave++;
    
    if (KilledMobsOnCurrentWave >= MobsToSpawn)
    {
        EndWave();
    }
}
