#include "GameModes/MTD_GameModeBase.h"

#include "Character/MTD_LevelComponent.h"
#include "Inventory/Items/MTD_ItemDropManager.h"
#include "Kismet/GameplayStatics.h"
#include "Player/MTD_PlayerState.h"

AMTD_GameModeBase::AMTD_GameModeBase()
{
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;
}

void AMTD_GameModeBase::PostActorCreated()
{
    Super::PostActorCreated();
    StartListeningForPlayers();
}

void AMTD_GameModeBase::AddExp(int32 Exp, int32 PlayerIndex)
{
    const int32 Num = Players.Num();
    if (!((PlayerIndex >= 0) && (PlayerIndex < Num)))
    {
        MTDS_WARN("Player Index [%d] is invalid.", PlayerIndex);
        return;
    }
    
    const AMTD_PlayerState *Ps = Players[PlayerIndex];
    if (!IsValid(Ps))
    {
        MTDS_WARN("MTD Player State on index [%d] is invalid.", PlayerIndex);
        return;
    }
    
    UMTD_LevelComponent *LevelComponent = Ps->GetLevelComponent();
    check(IsValid(LevelComponent));
    
    LevelComponent->AddExp(Exp);
}

void AMTD_GameModeBase::BroadcastExp(int32 Exp)
{
    int32 Index = 0;
    for (const AMTD_PlayerState *Ps : Players)
    {
        if (!IsValid(Ps))
        {
            MTDS_WARN("MTD Player State on index [%d] is invalid.", Index);
            Index++;
            return;
        }
    
        UMTD_LevelComponent *LevelComponent = Ps->GetLevelComponent();
        check(IsValid(LevelComponent));
    
        LevelComponent->AddExp(Exp);
        Index++;
    }
}

AActor *AMTD_GameModeBase::GetGameTarget(APawn *Client) const
{
    return nullptr;
}

void AMTD_GameModeBase::TerminateGame(EMTD_GameResult Reason)
{
    // Avoid dispatching if already did
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
    
    bGameOver = true;
    OnGameTerminatedDelegate.Broadcast(Reason);
}

void AMTD_GameModeBase::StartListeningForPlayers()
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

    OnAddPlayer(Actor);
}

void AMTD_GameModeBase::OnAddPlayer(AActor *Actor)
{
    check(IsValid(Actor));
    check(Actor->IsA(AMTD_PlayerState::StaticClass()));
    
    auto PlayerState = Cast<AMTD_PlayerState>(Actor);
    check(!PlayerState->IsABot());
    
    Players.Add(PlayerState);
    PlayerState->OnDestroyed.AddDynamic(this, &ThisClass::OnRemovePlayer);
    PlayerState->OnHeroClassesSetDelegate.AddUObject(this, &ThisClass::OnHeroClassesChanged);

    MTDS_LOG("Player State [%s] has been added to players list.", *PlayerState->GetName());
}

void AMTD_GameModeBase::OnRemovePlayer(AActor *Actor)
{
    auto Ps = Cast<AMTD_PlayerState>(Actor);
    Players.Remove(Ps);
    
    MTDS_LOG("Player State [%s] has been removed from players list.", *Ps->GetName());
}

void AMTD_GameModeBase::OnHeroClassesChanged(const FGameplayTagContainer &HeroClasses)
{
    UMTD_ItemDropManager *ItemDropManager = UMTD_ItemDropManager::Get();

    // May be invalid prior to play-time, in BPs viewport for intance
    if (IsValid(ItemDropManager))
    {
        ItemDropManager->AddHeroClasses(HeroClasses);
    }
}
