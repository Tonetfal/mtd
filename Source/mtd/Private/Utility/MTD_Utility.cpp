#include "Utility/MTD_Utility.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "AI/NavigationSystemBase.h"
#include "Character/Components/MTD_TeamComponent.h"

FGenericTeamId FMTD_Utility::GetMtdGenericTeamId(const AActor *InActor)
{
    const FGenericTeamId InvalidTeamId = FGenericTeamId::NoTeam;
    if (!IsValid(InActor))
    {
        return InvalidTeamId;
    }

    const auto Pawn = Cast<APawn>(InActor);
    if (!IsValid(Pawn))
    {
        return InvalidTeamId;
    }

    const AController *Controller = Pawn->GetController();
    if (!IsValid(Controller))
    {
        return InvalidTeamId;
    }

    const auto Team = GetActorComponent<UMTD_TeamComponent>(Controller);
    if (!IsValid(Team))
    {
        return InvalidTeamId;
    }

    return Team->GetGenericTeamId();
}

ETeamAttitude::Type FMTD_Utility::GetMtdTeamAttitudeBetween(const AActor *Lhs, const AActor *Rhs)
{
    const FGenericTeamId LhsId = GetMtdGenericTeamId(Lhs);
    if (LhsId == FGenericTeamId::NoTeam)
    {
        return ETeamAttitude::Type::Neutral;
    }

    const FGenericTeamId RhsId = GetMtdGenericTeamId(Rhs);
    if (RhsId == FGenericTeamId::NoTeam)
    {
        return ETeamAttitude::Type::Neutral;
    }

    return FGenericTeamId::GetAttitude(LhsId, RhsId);
}

EMTD_TeamId FMTD_Utility::GenericToMtdTeamId(FGenericTeamId GenericId)
{
    const EMTD_TeamId MtdId = static_cast<EMTD_TeamId>(GenericId.GetId());
    return (MtdId < EMTD_TeamId::Count) ? (MtdId) : (EMTD_TeamId::Unknown);
}

ENavigationQueryResult::Type FMTD_Utility::ComputePathTo(const AActor *Other, float &Cost,
    const FMTD_PathFindingContext &Context)
{
    if (!Context.IsValid())
    {
        return ENavigationQueryResult::Invalid;
    }
    
    const FVector TargetPosition = Other->GetActorLocation();
    const ENavigationQueryResult::Type PathResult = Context.NavigationSystem->GetPathCost(
        Context.World,
        Context.StartPosition,
        TargetPosition,
        Cost,
        Context.NavigationData,
        Context.NavQueryFilter);

    return PathResult;
}

FMTD_PathFindingContext FMTD_PathFindingContext::Create(const APawn *Pawn)
{
    check(Pawn);
    
    FMTD_PathFindingContext Context;
    auto AiController = Pawn->GetController<AAIController>();
    if (!(::IsValid(AiController)))
    {
        return Context;
    }
    
    UWorld *World = Pawn->GetWorld();
    if (!(::IsValid(World)))
    {
        return Context;
    }
    
    const auto NavigationSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
    if (!::IsValid(NavigationSystem))
    {
        return Context;
    }
    
    const FVector StartPosition = Pawn->GetNavAgentLocation();
    const FNavAgentProperties &NavAgentProps = AiController->GetNavAgentPropertiesRef();
    ANavigationData *NavigationData = NavigationSystem->GetNavDataForProps(NavAgentProps, StartPosition);
    if (!(::IsValid(NavigationData)))
    {
        return Context;
    }

    const TSubclassOf<UNavigationQueryFilter> NavQueryFilter = AiController->GetDefaultNavigationFilterClass();
    if (!(::IsValid(NavQueryFilter)))
    {
        return Context;
    }

    Context.World = World;
    Context.AiController = AiController;
    Context.NavQueryFilter = NavQueryFilter;
    Context.StartPosition = StartPosition;
    Context.NavigationSystem = NavigationSystem;
    Context.NavigationData = NavigationData;
    Context.bIsValid = true;

    return Context;
}

bool FMTD_PathFindingContext::IsValid() const
{
    return bIsValid;
}
