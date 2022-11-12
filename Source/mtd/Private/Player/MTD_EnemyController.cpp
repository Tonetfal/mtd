#include "Player/MTD_EnemyController.h"

#include "Character/MTD_TeamComponent.h"

AMTD_EnemyController::AMTD_EnemyController()
{
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;

    Team = CreateDefaultSubobject<UMTD_TeamComponent>(TEXT("MTD Team Component"));

    bWantsPlayerState = true;
}

FPathFollowingRequestResult AMTD_EnemyController::MoveTo(const FAIMoveRequest &MoveRequest, FNavPathSharedPtr *OutPath)
{
    bIsMoving = true;
    return Super::MoveTo(MoveRequest, OutPath);
}

void AMTD_EnemyController::BeginPlay()
{
    Super::BeginPlay();

    UPathFollowingComponent *PathFollowing = GetPathFollowingComponent();
    if (IsValid(PathFollowing))
    {
        PathFollowing->OnRequestFinished.AddUObject(this, &ThisClass::OnMoveFinished);
    }
}

void AMTD_EnemyController::OnMoveFinished(FAIRequestID RequestID, const FPathFollowingResult &Result)
{
    bIsMoving = false;
}
