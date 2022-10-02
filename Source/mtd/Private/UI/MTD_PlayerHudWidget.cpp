#include "UI/MTD_PlayerHudWidget.h"

#include "Components/MTD_HealthComponent.h"
#include "Controllers/MTD_PlayerController.h"
#include "Utilities/MTD_Utilities.h"

#include "AIController.h"

bool UMTD_PlayerHudWidget::Initialize()
{
	if (IsValid(GetOwningPlayerPawn()))
	{
		UMTD_HealthComponent *Health =
			FMTD_Utilities::GetActorComponent<UMTD_HealthComponent>(
				GetOwningPlayerPawn());
		if (IsValid(Health))
		{
			Health->OnHealthChangedDelegate.AddUObject(
				this, &UMTD_PlayerHudWidget::OnHealthChanged);
		}
		else
		{
			MTD_WARN("%s has an invalid health component",
				*GetOwningPlayerPawn()->GetName());
		}
	}
	else
	{
		MTD_WARN("Player is invalid");
	}
	
	return Super::Initialize();
}

float UMTD_PlayerHudWidget::GetHealthRatio() const
{
	const UMTD_HealthComponent *Health =
		FMTD_Utilities::GetActorComponent<UMTD_HealthComponent>(
			GetOwningPlayerPawn());
	return IsValid(Health) ? Health->GetHealthRatio() : 0.f;
}

bool UMTD_PlayerHudWidget::IsPlayerAlive() const
{
	const UMTD_HealthComponent *Health =
		FMTD_Utilities::GetActorComponent<UMTD_HealthComponent>(
			GetOwningPlayerPawn());
	return IsValid(Health) && !Health->IsDead();
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
	TraceData.HitDistanceRatio = 0.f;

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
		FMTD_Utilities::GetMtdGenericTeamId(HitResult.GetActor());
	if (OtherTeam.GetId() == FGenericTeamId::NoTeam)
		return TraceData;

	const auto OurTeam =
		FMTD_Utilities::GetActorComponent<UMTD_TeamComponent>(Pc);
	if (!IsValid(OurTeam))
		return TraceData;

	TraceData.AttitudeTowardsHittingObject =
		FGenericTeamId::GetAttitude(OurTeam->GetGenericTeamId(), OtherTeam);

	return TraceData;
}

void UMTD_PlayerHudWidget::OnHealthChanged(int32 NewHealth, int32 HealthDelta)
{
	if (HealthDelta < 0)
	{
		OnTakeDamage();
	}
}
