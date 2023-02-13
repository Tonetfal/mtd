#include "Gameplay/Levels/MTD_LevelDifficultyDefinition.h"

const UCurveFloat *UMTD_LevelDifficultyDefinition::GetIntensivityCurve(int32 Wave) const
{
    const auto Found = SpecificIntensivityCurve.Find(Wave);
    return ((Found) ? (*Found) : (DefaultIntensivityCurve));
}

const UMTD_FoeRateDefinition *UMTD_LevelDifficultyDefinition::GetCharacterRateDefinition(int32 Wave) const
{
    const auto Found = SpecificFoeRateData.Find(Wave);
    return ((Found) ? (*Found) : (DefaultFoeRateData));
}
