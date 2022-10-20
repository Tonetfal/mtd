#include "Character/MTD_TeamComponent.h"

#include "Utility/MTD_Utility.h"

UMTD_TeamComponent::UMTD_TeamComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void UMTD_TeamComponent::SetMtdTeamId(EMTD_TeamId Id)
{
	uint8 IdInt = static_cast<int>(Id);
	if (IdInt >= static_cast<int>(EMTD_TeamId::Count))
		IdInt = static_cast<int>(EMTD_TeamId::Unknown);

	TeamId = Id;
	SetGenericTeamId(FGenericTeamId(IdInt));
}

EMTD_TeamId UMTD_TeamComponent::GetMtdTeamId() const
{
	return TeamId;
}

FGenericTeamId UMTD_TeamComponent::GetGenericTeamId() const
{
	return FGenericTeamId(static_cast<uint8>(TeamId));
}

void UMTD_TeamComponent::SetGenericTeamId(const FGenericTeamId &Id)
{
	AActor *Owner = GetOwner();
	if (!IsValid(Owner))
	{
		MTDS_WARN("Owner is invalid");
		return;
	}
	
	auto *OwnerTeamAgent = Cast<IGenericTeamAgentInterface>(Owner);
	if (!OwnerTeamAgent)
	{
		MTDS_WARN("[%s] does not inherit IGenericTeamAgentInterface",
			*Owner->GetName());
		return;
	}
	OwnerTeamAgent->SetGenericTeamId(Id);
}

ETeamAttitude::Type UMTD_TeamComponent::GetTeamAttitudeTowards(
	const AActor &Other) const
{
	const auto *OtherTeam =
		FMTD_Utility::GetActorComponent<UMTD_TeamComponent>(&Other);
	if (IsValid(OtherTeam))
	{
		return FGenericTeamId::GetAttitude(
			GetGenericTeamId(), OtherTeam->GetGenericTeamId());
	}

	return IGenericTeamAgentInterface::GetTeamAttitudeTowards(Other);
}

void UMTD_TeamComponent::BeginPlay()
{
	Super::BeginPlay();

	if (TeamId == EMTD_TeamId::Unknown)
	{
		MTDS_WARN("[%s] does not belong to any team", *GetNameSafe(GetOwner()));
	}

	SetMtdTeamId(TeamId);
}
