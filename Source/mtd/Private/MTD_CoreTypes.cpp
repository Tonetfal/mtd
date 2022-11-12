#include "MTD_CoreTypes.h"

ETeamAttitude::Type AllyAttitude(FGenericTeamId, FGenericTeamId Other)
{
    const auto Id = static_cast<EMTD_TeamId>(Other.GetId());
    switch (Id)
    {
    case EMTD_TeamId::Player:
    case EMTD_TeamId::Tower:
        return ETeamAttitude::Type::Friendly;
    case EMTD_TeamId::Enemy:
        return ETeamAttitude::Type::Hostile;
    case EMTD_TeamId::Neutral:
        return ETeamAttitude::Type::Neutral;
    default:
        break;
    }
    return ETeamAttitude::Neutral;
}

ETeamAttitude::Type TowerAttitude(FGenericTeamId, FGenericTeamId Other)
{
    const auto Id = static_cast<EMTD_TeamId>(Other.GetId());
    switch (Id)
    {
    case EMTD_TeamId::Player:
    case EMTD_TeamId::Tower:
        return ETeamAttitude::Type::Friendly;
    case EMTD_TeamId::Enemy:
        return ETeamAttitude::Type::Hostile;
    case EMTD_TeamId::Neutral:
        return ETeamAttitude::Type::Neutral;
    default:
        break;
    }
    return ETeamAttitude::Neutral;
}

ETeamAttitude::Type CoreAttitude(FGenericTeamId, FGenericTeamId Other)
{
    const auto Id = static_cast<EMTD_TeamId>(Other.GetId());
    switch (Id)
    {
    case EMTD_TeamId::Player:
    case EMTD_TeamId::Tower:
        return ETeamAttitude::Type::Friendly;
    case EMTD_TeamId::Enemy:
        return ETeamAttitude::Type::Hostile;
    case EMTD_TeamId::Neutral:
        return ETeamAttitude::Type::Neutral;
    default:
        break;
    }
    return ETeamAttitude::Neutral;
}

ETeamAttitude::Type EnemyAttitude(FGenericTeamId, FGenericTeamId Other)
{
    const auto Id = static_cast<EMTD_TeamId>(Other.GetId());
    switch (Id)
    {
    case EMTD_TeamId::Player:
    case EMTD_TeamId::Tower:
        return ETeamAttitude::Type::Hostile;
    case EMTD_TeamId::Enemy:
        return ETeamAttitude::Type::Friendly;
    case EMTD_TeamId::Neutral:
        return ETeamAttitude::Type::Neutral;
    default:
        break;
    }
    return ETeamAttitude::Neutral;
}

ETeamAttitude::Type NeutralAttitude(FGenericTeamId, FGenericTeamId Other)
{
    return ETeamAttitude::Neutral;
}

ETeamAttitude::Type InvalidAttitude(FGenericTeamId, FGenericTeamId Other)
{
    return ETeamAttitude::Neutral;
}

ETeamAttitude::Type SolveTeamAttitude(FGenericTeamId Lhs, FGenericTeamId Rhs)
{
    const auto Id = static_cast<EMTD_TeamId>(Lhs.GetId());
    switch (Id) // Lhs is always the case type
    {
    case EMTD_TeamId::Player:
        return AllyAttitude(Lhs, Rhs);
    case EMTD_TeamId::Tower:
        return TowerAttitude(Lhs, Rhs);
    case EMTD_TeamId::Core:
        return CoreAttitude(Lhs, Rhs);
    case EMTD_TeamId::Enemy:
        return EnemyAttitude(Lhs, Rhs);
    case EMTD_TeamId::Neutral:
        return NeutralAttitude(Lhs, Rhs);
    default:
        break;
    }

    return InvalidAttitude(Lhs, Rhs);
}
