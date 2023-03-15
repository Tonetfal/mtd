#include "GameModes/MTD_GameModeBase.h"

#include "AbilitySystem/Attributes/MTD_HealthSet.h"
#include "AbilitySystem/Attributes/MTD_ManaSet.h"
#include "AbilitySystemComponent.h"
#include "Character/Components/MTD_LevelComponent.h"
#include "InventorySystem/MTD_ItemDropSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Player/MTD_PlayerState.h"

AMTD_GameModeBase::AMTD_GameModeBase()
{
    // Nothing to tick for
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;
}

void AMTD_GameModeBase::PostActorCreated()
{
    Super::PostActorCreated();

    // Listen for players as soon as possible
    ListenForPlayers();
}

void AMTD_GameModeBase::AddExp(int32 Exp, int32 PlayerIndex)
{
    const AMTD_PlayerState *PlayerState = GetPlayerState(PlayerIndex);
    if (!PlayerState)
    {
        return;
    }
    
    UMTD_LevelComponent *LevelComponent = PlayerState->GetLevelComponent();
    check(IsValid(LevelComponent));
    
    LevelComponent->AddExp(Exp);
}

void AMTD_GameModeBase::AddHealth(int32 Health, int32 PlayerIndex)
{
    const AMTD_PlayerState *PlayerState = GetPlayerState(PlayerIndex);
    if (!PlayerState)
    {
        return;
    }

    UAbilitySystemComponent *AbilitySystemComponent = PlayerState->GetAbilitySystemComponent();
    check(IsValid(AbilitySystemComponent));

    AbilitySystemComponent->ApplyModToAttribute(UMTD_HealthSet::GetHealthAttribute(), EGameplayModOp::Additive, Health);
}

void AMTD_GameModeBase::AddMana(int32 Mana, int32 PlayerIndex)
{
    const AMTD_PlayerState *PlayerState = GetPlayerState(PlayerIndex);
    if (!PlayerState)
    {
        return;
    }

    UAbilitySystemComponent *AbilitySystemComponent = PlayerState->GetAbilitySystemComponent();
    check(IsValid(AbilitySystemComponent));

    AbilitySystemComponent->ApplyModToAttribute(UMTD_ManaSet::GetManaAttribute(), EGameplayModOp::Additive, Mana);
}

void AMTD_GameModeBase::BroadcastExp(int32 Exp)
{
    int32 Index = 0;
    for (const AMTD_PlayerState *PlayerState : Players)
    {
        if (!IsValid(PlayerState))
        {
            MTDS_WARN("Player (%d) is invalid.", Index);
            Index++;
            return;
        }
    
        UMTD_LevelComponent *LevelComponent = PlayerState->GetLevelComponent();
        check(IsValid(LevelComponent));
    
        LevelComponent->AddExp(Exp);
        Index++;
    }
}

AActor *AMTD_GameModeBase::GetGameTarget(APawn *Client) const
{
    // To override
    return nullptr;
}

void AMTD_GameModeBase::TerminateGame(EMTD_GameResult Reason)
{
    // Avoid terminating if already did
    if (bGameOver)
    {
        return;
    }
        
    if (Reason == EMTD_GameResult::Win)
    {
        MTD_LOG("Terminate game due victory.");
    }
    else
    {
        MTD_LOG("Terminate game due lose");
    }

    // Save game state
    bGameOver = true;

    // Notify about game termination
    OnGameTerminatedDelegate.Broadcast(Reason);
}

void AMTD_GameModeBase::ListenForPlayers()
{
    const UWorld *World = GetWorld();
    check(IsValid(World));

    World->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateUObject(this, &ThisClass::OnActorSpawned));
}

void AMTD_GameModeBase::OnActorSpawned(AActor *Actor)
{
    if (!IsValid(Actor))
    {
        return;
    }
    
    if (!Actor->IsA(AMTD_PlayerState::StaticClass()))
    {
        return;
    }
    
    const auto PlayerState = Cast<AMTD_PlayerState>(Actor);
    if (PlayerState->IsABot())
    {
        return;
    }

    OnAddPlayer(PlayerState);
}

void AMTD_GameModeBase::OnAddPlayer(AMTD_PlayerState *InPlayerState)
{
    check(IsValid(InPlayerState));
    check(!InPlayerState->IsABot());

    // Add to players list
    Players.Add(InPlayerState);

    // Listen for destroy event and for class change
    InPlayerState->OnDestroyed.AddDynamic(this, &ThisClass::OnRemovePlayer);
    InPlayerState->OnHeroClassesSetDelegate.AddUObject(this, &ThisClass::OnHeroClassesChanged);

    MTDS_LOG("Player State [%s] has been added to players list.", *InPlayerState->GetName());
}

void AMTD_GameModeBase::OnRemovePlayer(AActor *Actor)
{
    auto PlayerState = CastChecked<AMTD_PlayerState>(Actor);

    // Remove from players list
    Players.Remove(PlayerState);
    
    MTDS_LOG("Player [%s] has been removed from players list.", *GetNameSafe(PlayerState));
}

void AMTD_GameModeBase::OnHeroClassesChanged(const FGameplayTagContainer &HeroClasses)
{
    // May be invalid prior to play-time, for instance, in BPs viewport
    auto ItemDropSubsystem = UMTD_ItemDropSubsystem::Get(this);
    check(IsValid(ItemDropSubsystem));
    
    ItemDropSubsystem->AddHeroClasses(HeroClasses);
}

AMTD_PlayerState *AMTD_GameModeBase::GetPlayerState(int32 PlayerIndex)
{
    const int32 Num = Players.Num();
    if (!((PlayerIndex >= 0) && (PlayerIndex < Num)))
    {
        MTDS_WARN("Player index [%d] is invalid.", PlayerIndex);
        return nullptr;
    }
    
    AMTD_PlayerState *PlayerState = Players[PlayerIndex];
    if (!IsValid(PlayerState))
    {
        MTDS_WARN("Player (%d) is invalid.", PlayerIndex);
        return nullptr;
    }

    return PlayerState;
}
