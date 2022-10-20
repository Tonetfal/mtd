#include "Player/MTD_EnemyController.h"

#include "Character/MTD_TeamComponent.h"

AMTD_EnemyController::AMTD_EnemyController()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	Team =
		CreateDefaultSubobject<UMTD_TeamComponent>(TEXT("MTD Team Component"));

	bWantsPlayerState = true;
}

bool AMTD_EnemyController::CanBeSeenFrom(
	const FVector &ObserverLocation,
	FVector &OutSeenLocation,
	int32 &NumberOfLoSChecksPerformed,
	float &OutSightStrength,
	const AActor *IgnoreActor,
	const bool *bWasVisible,
	int32 *UserData) const
{
	if (!GetWorld() || !IsValid(GetPawn()))
		return false;

	FHitResult HitResult;
	const FVector OurLocation = GetPawn()->GetActorLocation();
	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		ObserverLocation,
		OurLocation,
		EnemyTraceChannel);

	NumberOfLoSChecksPerformed++;

	const AActor *Actor = HitResult.GetActor();
	const bool bCanBeSeen = IsValid(Actor) && Actor == GetPawn();
	
	if (bCanBeSeen)
	{
		OutSeenLocation = OurLocation;
		OutSightStrength = 1.f;
	}
	else
	{
		OutSeenLocation = FVector::ZeroVector;
		OutSightStrength = 0.f;
	}

	return bCanBeSeen;
}

FPathFollowingRequestResult AMTD_EnemyController::MoveTo(
	const FAIMoveRequest &MoveRequest, FNavPathSharedPtr *OutPath)
{
	bIsMoving = true;
	return Super::MoveTo(MoveRequest,OutPath);
}

void AMTD_EnemyController::BeginPlay()
{
	Super::BeginPlay();

	UPathFollowingComponent *PathFollowing = GetPathFollowingComponent();
	if (IsValid(PathFollowing))
	{
		PathFollowing->OnRequestFinished.AddUObject(
			this, &ThisClass::OnMoveFinished);
	}
}

void AMTD_EnemyController::OnMoveFinished(
	FAIRequestID RequestID, const FPathFollowingResult &Result)
{
	bIsMoving = false;
}
