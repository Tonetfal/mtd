#include "Utilities/MTD_Utilities.h"

#include "Components/MTD_TeamComponent.h"

#include "AIController.h"

FGenericTeamId FMTD_Utilities::GetMtdGenericTeamId(const AActor *InActor)
{
	const FGenericTeamId InvalidTeamId = FGenericTeamId::NoTeam;
	if (!IsValid(InActor))
		return InvalidTeamId;
	
	const APawn *Pawn = Cast<APawn>(InActor);
	if (!IsValid(Pawn))
		return InvalidTeamId;

	const AController *Controller = Pawn->GetController();
	if (!IsValid(Controller))
		return InvalidTeamId;

	const UMTD_TeamComponent *Team =
		FMTD_Utilities::GetActorComponent<UMTD_TeamComponent>(Controller);
	if (!IsValid(Team))
		return InvalidTeamId;

	return Team->GetGenericTeamId();
}

ETeamAttitude::Type FMTD_Utilities::GetMtdTeamAttitudeBetween(
	const AActor *Lhs, const AActor *Rhs)
{
	const FGenericTeamId LhsId = FMTD_Utilities::GetMtdGenericTeamId(Lhs);
	if (LhsId == FGenericTeamId::NoTeam)
		return ETeamAttitude::Type::Neutral;
	
	const FGenericTeamId RhsId = FMTD_Utilities::GetMtdGenericTeamId(Rhs);
	if (RhsId == FGenericTeamId::NoTeam)
		return ETeamAttitude::Type::Neutral;

	return FGenericTeamId::GetAttitude(LhsId, RhsId);
}

EMTD_TeamId FMTD_Utilities::GenericToMtdTeamId(FGenericTeamId GenericId)
{
	const EMTD_TeamId MtdId = static_cast<EMTD_TeamId>(GenericId.GetId());
	return MtdId < EMTD_TeamId::Count ? MtdId : EMTD_TeamId::Unknown;
}
