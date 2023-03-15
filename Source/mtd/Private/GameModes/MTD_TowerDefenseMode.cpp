#include "GameModes/MTD_TowerDefenseMode.h"

#include "Character/MTD_BaseCharacter.h"
#include "Gameplay/Objective/Core/MTD_Core.h"
#include "Gameplay/Wave/MTD_GameWavesSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "System/MTD_GameInstance.h"
#include "Utility/MTD_Utility.h"

AMTD_TowerDefenseMode::AMTD_TowerDefenseMode()
{
    // Nothing to tick for
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;
}

void AMTD_TowerDefenseMode::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    auto GameWavesSubsystem = UMTD_GameWavesSubsystem::Get(this);
    check(IsValid(GameWavesSubsystem));

    // Listen for wave start and end events
    GameWavesSubsystem->OnWaveStartDelegate.AddDynamic(this, &ThisClass::OnWaveStarted);
    GameWavesSubsystem->OnWaveEndDelegate.AddDynamic(this, &ThisClass::OnWaveEnded);
}

void AMTD_TowerDefenseMode::BeginPlay()
{
    Super::BeginPlay();

    // Cache cores before enemies move towards them
    CacheCores();

    // @todo ask for GM phase on individual actors instead. this is useful for future networking because a player
    // may connect to the game at any point, and it should be able to get the state regardless
    
    // Some elements will spawn after GM, and they may be interested in this event as well, hence delay it for a tick
    GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda(
        [this] () { OnPhaseChangedDelegate.Broadcast(EMTD_GamePhase::Build); }));
}

void AMTD_TowerDefenseMode::OnWaveStarted(int32 WaveNumber, float RemainingTime)
{
    // Enter in combat phase after a wave has started
    SetPhase(EMTD_GamePhase::Combat);
}

void AMTD_TowerDefenseMode::OnWaveEnded(float WaveDuration)
{
    // Enter in build phase after a wave has finished
    SetPhase(EMTD_GamePhase::Build);
}

void AMTD_TowerDefenseMode::OnCoreDestroyed(AMTD_Core *Core)
{
    // Losing a core loses the game
    TerminateGame(EMTD_GameResult::Lose);
}

void AMTD_TowerDefenseMode::SetPhase(EMTD_GamePhase InGamePhase)
{
    if (CurrentGamePhase == InGamePhase)
    {
        // Avoid resetting if it's the same state
        return;
    }
    
    if (PhaseSwitchDelayTimerHandle.IsValid())
    {
        MTDS_WARN("Time phase switch delay timer handle is still valid. Phase switch is dropped.");
        return;
    }

    // Save the state
    CurrentGamePhase = InGamePhase;

    // Payload code
    auto Payload = [this, InGamePhase] ()
        {
            // Notify about phase change
            OnPhaseChangedDelegate.Broadcast(InGamePhase);
            PhaseSwitchDelayTimerHandle.Invalidate();
        };

    // Delay the payload execution
    GetWorldTimerManager().SetTimer(PhaseSwitchDelayTimerHandle, Payload, TimePhaseSwitchDelay, false);
}

AActor *AMTD_TowerDefenseMode::GetGameTarget(APawn *Client) const
{
    if (!IsValid(Client))
    {
        MTDS_WARN("World context object is invalid.");
        return nullptr;
    }
    
    const FMTD_PathFindingContext Context = FMTD_PathFindingContext::Create(Client);
    if (!Context.IsValid())
    {
        MTDS_WARN("Path finding context on owner [%s] couldn't be created correctly.", *GetName());
        return nullptr;
    }
    
    // Store final result in here
    AActor *Result = nullptr;
    float LowestCost = 0.f;

    for (AMTD_Core *Core : Cores)
    {
        float Cost;
        const ENavigationQueryResult::Type PathResult = FMTD_Utility::ComputePathTo(Core, Cost, Context);

        // If the core is reachable check whether its the most optimal path cost wise
        if (PathResult == ENavigationQueryResult::Success)
        {
            if ((!Result) || (LowestCost > Cost))
            {
                LowestCost = Cost;
                Result = Core;
            }
        }
        // Should never happen
        else
        {
            MTD_WARN("Actor [%s] at [%s] is unable to get to core [%s].",
                *Client->GetName(), *Client->GetActorLocation().ToString(), *Core->GetName());
        }
    }

    return Result;
}

int32 AMTD_TowerDefenseMode::GetCurrentWave() const
{
    const auto GameWavesSubsystem = UMTD_GameWavesSubsystem::Get(this);
    check(IsValid(GameWavesSubsystem));

    const int32 CurrentWave = GameWavesSubsystem->GetCurrentWave();
    return CurrentWave;
}

void AMTD_TowerDefenseMode::CacheCores()
{
    // Find all cores placed on current level
    TArray<AActor *> OutActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMTD_Core::StaticClass(), OutActors);

    for (AActor *Actor : OutActors)
    {
        auto Core = Cast<AMTD_Core>(Actor);

        // Listen for core destruction events
        Core->OnCoreDestroyedDelegate.AddDynamic(this, &ThisClass::OnCoreDestroyed);

        // Cache core
        Cores.Add(Core);
    }
}
