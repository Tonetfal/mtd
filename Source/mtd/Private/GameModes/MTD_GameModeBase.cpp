#include "GameModes/MTD_GameModeBase.h"

#include "Character/MTD_LevelComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/MTD_PlayerState.h"

AMTD_GameModeBase::AMTD_GameModeBase()
{
}

void AMTD_GameModeBase::AddExp(int32 Exp, int32 PlayerIndex)
{
    const APlayerController *PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), PlayerIndex);
    if (!IsValid(PlayerController))
    {
        MTDS_WARN("There is no Player Controller at Index [%d].", PlayerIndex);
        return;
    }
    
    const auto Ps = PlayerController->GetPlayerState<AMTD_PlayerState>();
    if (!IsValid(Ps))
    {
        MTDS_WARN("MTD Player State on [%s] is invalid.", *PlayerController->GetName());
        return;
    }
    
    UMTD_LevelComponent *LevelComponent = Ps->GetLevelComponent();
    check(LevelComponent);
    LevelComponent->AddExp(Exp);
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
