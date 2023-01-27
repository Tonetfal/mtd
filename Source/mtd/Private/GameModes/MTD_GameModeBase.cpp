#include "GameModes/MTD_GameModeBase.h"

#include "Character/MTD_LevelComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/MTD_PlayerState.h"

AMTD_GameModeBase::AMTD_GameModeBase()
{
}

void AMTD_GameModeBase::AddExp(int32 Exp, int32 PlayerIndex)
{
    if (!Players.Contains(PlayerIndex))
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

void AMTD_GameModeBase::PreInitializeComponents()
{
    Super::PreInitializeComponents();

    UWorld *World = GetWorld();
    check(IsValid(World));

    auto Lambda = [this] (AActor *Actor)
        {
            if (Actor->IsA(AMTD_PlayerState::StaticClass()))
            {
                auto Ps = Cast<AMTD_PlayerState>(Actor);
                if (!Ps->IsABot())
                {
                    Players.Add(Ps);
                }
            }
        };
    
    World->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateLambda(Lambda));
}

void AMTD_GameModeBase::BeginPlay()
{
    Super::BeginPlay();
}

void AMTD_GameModeBase::StartPlay()
{
    Super::StartPlay();
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
