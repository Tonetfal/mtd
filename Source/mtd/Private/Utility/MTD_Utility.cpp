#include "Utility/MTD_Utility.h"

#include "AIController.h"
#include "Character/MTD_TeamComponent.h"

FGenericTeamId FMTD_Utility::GetMtdGenericTeamId(const AActor *InActor)
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
		FMTD_Utility::GetActorComponent<UMTD_TeamComponent>(Controller);
	if (!IsValid(Team))
		return InvalidTeamId;

	return Team->GetGenericTeamId();
}

ETeamAttitude::Type FMTD_Utility::GetMtdTeamAttitudeBetween(
	const AActor *Lhs, const AActor *Rhs)
{
	const FGenericTeamId LhsId = FMTD_Utility::GetMtdGenericTeamId(Lhs);
	if (LhsId == FGenericTeamId::NoTeam)
		return ETeamAttitude::Type::Neutral;
	
	const FGenericTeamId RhsId = FMTD_Utility::GetMtdGenericTeamId(Rhs);
	if (RhsId == FGenericTeamId::NoTeam)
		return ETeamAttitude::Type::Neutral;

	return FGenericTeamId::GetAttitude(LhsId, RhsId);
}

EMTD_TeamId FMTD_Utility::GenericToMtdTeamId(FGenericTeamId GenericId)
{
	const EMTD_TeamId MtdId = static_cast<EMTD_TeamId>(GenericId.GetId());
	return MtdId < EMTD_TeamId::Count ? MtdId : EMTD_TeamId::Unknown;
}
