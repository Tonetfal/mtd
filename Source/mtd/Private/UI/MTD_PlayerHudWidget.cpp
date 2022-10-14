#include "UI/MTD_PlayerHudWidget.h"

#include "AIController.h"
#include "Character/MTD_HealthComponent.h"
#include "Player/MTD_PlayerController.h"
#include "Utility/MTD_Utility.h"

bool UMTD_PlayerHudWidget::Initialize()
{
	return Super::Initialize();
}

bool UMTD_PlayerHudWidget::IsPlayerAlive() const
{
	const auto Health =
		UMTD_HealthComponent::FindHealthComponent(GetOwningPlayer());
	return IsValid(Health) && !Health->IsDeadOrDying();
}

bool UMTD_PlayerHudWidget::IsPlayerSpectating() const
{
	const APlayerController *Pc = GetOwningPlayer();
	return IsValid(Pc) && Pc->GetStateName() == NAME_Spectating;
}

FMTD_TraceData UMTD_PlayerHudWidget::GetTraceData() const
{
	FMTD_TraceData TraceData;
	TraceData.AttitudeTowardsHittingObject = ETeamAttitude::Neutral;
	TraceData.HitDistanceRatio = 1.f;

	const UWorld *World = GetWorld();
	if (!IsValid(World))
	{
		MTD_WARN("World is invalid");
		return TraceData;
	}
	
	const APlayerController *Pc = GetOwningPlayer();
	if (!IsValid(Pc))
	{
		MTD_WARN("Player controller is invalid");
		return TraceData;
	}

	FVector Location;
	FRotator Rotation;
	Pc->GetPlayerViewPoint(Location, Rotation);
	
	const FVector Direction = Rotation.Vector();
	
	const FVector TraceStart = Location;
	const FVector TraceEnd = TraceStart + Direction * TraceLineLength;
	
	FHitResult HitResult;
	const bool Hit = World->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		ECollisionChannel::ECC_Visibility,
		FCollisionQueryParams(TEXT(""), true, GetOwningPlayerPawn()));

	if (!Hit)
		return TraceData;

	TraceData.HitDistanceRatio =
		FMath::Min(HitResult.Distance / MaxScaleDownLength, 1.f);

	const FGenericTeamId OtherTeam =
		FMTD_Utility::GetMtdGenericTeamId(HitResult.GetActor());
	if (OtherTeam.GetId() == FGenericTeamId::NoTeam)
		return TraceData;

	const auto OurTeam =
		FMTD_Utility::GetActorComponent<UMTD_TeamComponent>(Pc);
	if (!IsValid(OurTeam))
		return TraceData;

	TraceData.AttitudeTowardsHittingObject =
		FGenericTeamId::GetAttitude(OurTeam->GetGenericTeamId(), OtherTeam);

	return TraceData;
}
