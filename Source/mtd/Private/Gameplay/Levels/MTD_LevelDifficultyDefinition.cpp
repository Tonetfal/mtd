#include "Gameplay/Levels/MTD_LevelDifficultyDefinition.h"

const UCurveFloat *UMTD_LevelDifficultyDefinition::GetIntensivityCurve(int32 Wave) const
{
    const auto Found = SpecificIntensivityCurve.Find(Wave);
    return ((Found) ? (*Found) : (DefaultIntensivityCurve));
}

const UMTD_CharacterRateDefinition *UMTD_LevelDifficultyDefinition::GetCharacterRateDefinition(int32 Wave) const
{
    const auto Found = SpecificEnemyRateData.Find(Wave);
    return ((Found) ? (*Found) : (DefaultEnemyRateData));
}
