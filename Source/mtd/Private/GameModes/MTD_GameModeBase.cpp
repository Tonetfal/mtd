#include "GameModes/MTD_GameModeBase.h"

#include "MTD_CoreTypes.h"
#include "Kismet/GameplayStatics.h"

AMTD_GameModeBase::AMTD_GameModeBase()
{
}

void AMTD_GameModeBase::BeginPlay()
{
    Super::BeginPlay();

    FGenericTeamId::SetAttitudeSolver(&SolveTeamAttitude);
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
