#include "Components/MTD_PathFollowingComponent.h"

#include "Core/MTD_GameModeBase.h"
#include "Core/MTD_LevelPathManager.h"

#include "Kismet/GameplayStatics.h"

UMTD_PathFollowingComponent::UMTD_PathFollowingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMTD_PathFollowingComponent::BeginPlay()
{
	Super::BeginPlay();
	
	const UWorld *World = GetWorld();
	if (!IsValid(World))
	{
		MTD_WARN("World is invalid");
		return;
	}

	const AActor *ActorOwner = GetOwner();
	if (!IsValid(ActorOwner))
	{
		MTD_WARN("Owner is invalid");
		return;
	}
	
	AGameModeBase *GM = UGameplayStatics::GetGameMode(GetWorld());
	if (!IsValid(GM))
	{
		MTD_WARN("Game mode is invalid");
		return;
	}

	AMTD_GameModeBase *GameMode = Cast<AMTD_GameModeBase>(GM);
	if (!IsValid(GameMode))
	{
		MTD_WARN("MTD Game mode is invalid");
		return;
	}

	PathManager = GameMode->GetLevelPathManager();
	if (!IsValid(PathManager))
	{
		MTD_WARN("Path manager is invalid");
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
		MTD_WARN("Owner is invalid");
		return;
	}
	
	MTD_VVERBOSE("Preparing a level path...");

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
		MTD_VVERBOSE("%s has reached terminated", *GetName());
		return false;
	}
	
	MTD_VVERBOSE("Moving on next path point");
	return true;
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
