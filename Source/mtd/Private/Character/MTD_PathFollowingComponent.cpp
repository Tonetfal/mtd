#include "Character/MTD_PathFollowingComponent.h"

#include "GameModes/MTD_GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "System/MTD_LevelPathManager.h"

UMTD_PathFollowingComponent::UMTD_PathFollowingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UMTD_PathFollowingComponent::BeginPlay()
{
	Super::BeginPlay();
	
	const UWorld *World = GetWorld();
	if (!IsValid(World))
	{
		MTDS_WARN("World is invalid");
		return;
	}

	const AActor *ActorOwner = GetOwner();
	if (!IsValid(ActorOwner))
	{
		MTDS_WARN("Owner is invalid");
		return;
	}
	
	AGameModeBase *GM = UGameplayStatics::GetGameMode(GetWorld());
	if (!IsValid(GM))
	{
		MTDS_WARN("Game mode is invalid");
		return;
	}

	AMTD_GameModeBase *GameMode = Cast<AMTD_GameModeBase>(GM);
	if (!IsValid(GameMode))
	{
		MTDS_WARN("Failed to cast [%s] to MTD GM", *GM->GetName());
		return;
	}

	PathManager = GameMode->GetLevelPathManager();
	if (!IsValid(PathManager))
	{
		MTDS_WARN("[%s] has an invalid path manager", *GM->GetName());
		return;
	}
}

void UMTD_PathFollowingComponent::EndPlay(
    const EEndPlayReason::Type EndPlayReason)
{
	if (AllowToPrepareTimerHandle.IsValid())
		GetWorld()->GetTimerManager().ClearTimer(AllowToPrepareTimerHandle);
	
	Super::EndPlay(EndPlayReason);
}

void UMTD_PathFollowingComponent::PreparePath()
{
	if (!bIsAllowedToPrepare)
		return;
	
	const AActor *ActorOwner = GetOwner();
	if (!IsValid(ActorOwner))
	{
		MTDS_WARN("Owner is invalid");
		return;
	}
	
	MTDS_VVERBOSE("Preparing a level path...");

	Path = PathManager->GetNavigationPath(
		ActorOwner->GetActorLocation(), PathChannel);
	
	PathIndex = 0;

	// Disallow too frequent prepation requests
	GetWorld()->GetTimerManager().SetTimer(AllowToPrepareTimerHandle,this,
		&UMTD_PathFollowingComponent::AllowToPrepare, 1.f);
	bIsAllowedToPrepare = false;
}

bool UMTD_PathFollowingComponent::SelectNextPathPoint()
{
	PathIndex++;
	
	if (Path.Num() <= PathIndex)
	{
		MTDS_VVERBOSE("Path is terminated", *GetName());
		return false;
	}
	else
	{	
        MTDS_VVERBOSE("Moving on next path point");
        return true;
	}
}

void UMTD_PathFollowingComponent::OnMoveToFinished(bool bSuccess)
{
	bFailedToGetFollowPoint = !bSuccess;
	OnDestinationDelegate.Broadcast(bSuccess);
}

void UMTD_PathFollowingComponent::AllowToPrepare()
{
	bIsAllowedToPrepare = true;
}
