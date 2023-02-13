#include "GameModes/MTD_TowerDefenseMode.h"

#include "Character/MTD_BaseCharacter.h"
#include "Character/MTD_HealthComponent.h"
#include "Gameplay/Levels/MTD_LevelDefinition.h"
#include "Gameplay/Levels/MTD_LevelDifficultyDefinition.h"
#include "Gameplay/MTD_WaveManager.h"
#include "Gameplay/Objective/Core/MTD_Core.h"
#include "Gameplay/Spawner/MTD_FoeSpawnerManager.h"
#include "Kismet/GameplayStatics.h"
#include "System/MTD_GameInstance.h"
#include "Utility/MTD_Utility.h"

AMTD_TowerDefenseMode::AMTD_TowerDefenseMode()
{
    // We have to dispatch spawner manager's dispatched data
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    WaveManager = CreateDefaultSubobject<UMTD_WaveManager>("MTD Wave Manager");
    SpawnerManager = CreateDefaultSubobject<UMTD_FoeSpawnerManager>("MTD Spawner Manager");
}

void AMTD_TowerDefenseMode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    check(IsValid(SpawnerManager));
    SpawnerManager->DispatchCachedData();
}

void AMTD_TowerDefenseMode::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    // Listen for wave start and end events
    WaveManager->OnWaveStartDelegate.AddDynamic(this, &ThisClass::OnWaveStarted);
    WaveManager->OnWaveEndDelegate.AddDynamic(this, &ThisClass::OnWaveEnded);
    
    const auto GameInstance = Cast<UMTD_GameInstance>(GetGameInstance());
    check(IsValid(GameInstance));

    // @todo pick a level with a widget instead of hardcoding the first level be always be the case
    
    // This is test purpose code...
    const TArray<TObjectPtr<UMTD_LevelDefinition>> GameLevels = GameInstance->GetGameLevels();
    const UMTD_LevelDefinition *FirstGameLevel = GameLevels[0];
    check(IsValid(FirstGameLevel))
    
    const UMTD_LevelDifficultyDefinition *EasyDifficulty = FirstGameLevel->Difficulties[EMTD_LevelDifficulty::Easy];
    check(IsValid(EasyDifficulty));

    SelectedLevelDefinition = FirstGameLevel;
    SelectedLevelDifficultyDefinition = EasyDifficulty;

    // Initialize wave and spawner manages AFTER the level information has been retrieved
    WaveManager->Initialize();
    SpawnerManager->Initialize();
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

void AMTD_TowerDefenseMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    // Manually delete wave manager
    if (IsValid(WaveManager))
    {
        WaveManager->MarkAsGarbage();
    }
    
    // Manually delete spawner manager
    if (IsValid(SpawnerManager))
    {
        SpawnerManager->MarkAsGarbage();
    }
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

float AMTD_TowerDefenseMode::GetScaledDifficulty() const
{
    if (!IsValid(SelectedLevelDifficultyDefinition))
    {
        MTDS_WARN("Selected level difficulty definition is invalid.");
        return 0.f;
    }

    // @todo actually compute the difficulty by applying a formula and using current wave as the scalar
    return SelectedLevelDifficultyDefinition->BaseDifficulty;
}

float AMTD_TowerDefenseMode::GetScaledQuantity() const
{
    if (!IsValid(SelectedLevelDifficultyDefinition))
    {
        MTDS_WARN("Selected level difficulty definition is invalid.");
        return 0.f;
    }

    // @todo actually compute the quantity by applying a formula and using current wave as the scalar
    return SelectedLevelDifficultyDefinition->BaseQuantity;
}

float AMTD_TowerDefenseMode::GetTotalCurrentWaveTime() const
{
    if (!IsValid(SelectedLevelDifficultyDefinition))
    {
        MTDS_WARN("Selected level difficulty definition is invalid.");
        return 0.f;
    }

    // @todo actually compute the time by applying a formula and using current wave as the scalar
    return SelectedLevelDifficultyDefinition->BaseTime;
}

int32 AMTD_TowerDefenseMode::GetCurrentWave() const
{
    return ((IsValid(WaveManager)) ? (WaveManager->GetCurrentWave()) : (-1));
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
