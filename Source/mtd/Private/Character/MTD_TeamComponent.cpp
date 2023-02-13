#include "Character/MTD_TeamComponent.h"

#include "Utility/MTD_Utility.h"

UMTD_TeamComponent::UMTD_TeamComponent()
{
    // Nothing to tick for
    PrimaryComponentTick.bStartWithTickEnabled = false;
    PrimaryComponentTick.bCanEverTick = false;
}

void UMTD_TeamComponent::SetMtdTeamId(EMTD_TeamId Id)
{
    // Set team ID
    const uint8 IdInt = static_cast<int8>(Id);
    TeamId = Id;

    // Set generic team ID
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

    auto OwnerTeamAgent = Cast<IGenericTeamAgentInterface>(Owner);
    if (!OwnerTeamAgent)
    {
        MTDS_WARN("Failed to cast [%s] to IGenericTeamAgentInterface.", *Owner->GetName());
        return;
    }
    OwnerTeamAgent->SetGenericTeamId(Id);
}

ETeamAttitude::Type UMTD_TeamComponent::GetTeamAttitudeTowards(const AActor &OtherActor) const
{
    // Return neutral by default
    ETeamAttitude::Type TeamAttitude = ETeamAttitude::Neutral;

    // Try to get attitude towards the passed actor
    const auto OtherTeamAgent = Cast<IGenericTeamAgentInterface>(&OtherActor);
    if (OtherTeamAgent)
    {
        TeamAttitude = FGenericTeamId::GetAttitude(GetGenericTeamId(), OtherTeamAgent->GetGenericTeamId());
    }

    return TeamAttitude;
}

void UMTD_TeamComponent::BeginPlay()
{
    Super::BeginPlay();

    if (TeamId == EMTD_TeamId::Unknown)
    {
        MTDS_WARN("[%s] does not belong to any team.", *GetNameSafe(GetOwner()));
    }

    SetMtdTeamId(TeamId);
}
