#include "AbilitySystem/Attributes/MTD_PlayerSet.h"

#include "Character/MTD_LevelComponent.h"
#include "System/MTD_GameInstance.h"

void UMTD_PlayerSet::PreAttributeChange(const FGameplayAttribute &Attribute, float &NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

    // Avoid using MaxTotalExp if cache didn't successfully
    if (CacheExpRows())
    {
        // Disallow EXP go beyond the max value
        NewValue = FMath::Clamp(NewValue, 0.f, MaxTotalExp);
    }
}

void UMTD_PlayerSet::PostAttributeChange(const FGameplayAttribute &Attribute, float OldValue, float NewValue)
{
    Super::PostAttributeChange(Attribute, OldValue, NewValue);

    if (Attribute == GetExperienceStatAttribute())
    {
        // Avoid running level up logic at max level
        if (!bIsMaxLevel)
        {
            if (CacheExpRows())
            {
                TryLevelUp(NewValue);
            }
        }
    }

    else if (Attribute == GetLevelStatAttribute())
    {
        if (!bIsMaxLevel)
        {
            // Avoid using MaxLevel if cache didn't successfully
            if (CacheExpRows())
            {
                if (NewValue == MaxLevel)
                {
                    bIsMaxLevel = true;
                    OnMaxLevelDelegate.Broadcast();
                }
            }
        }
    }
}

void UMTD_PlayerSet::TryLevelUp(int32 TotalExp)
{
    check(TotalExpLevelRow);
    ensure(!bIsMaxLevel);
    
    const int32 OriginalLevel = LevelStat.GetCurrentValue();
    int32 NewLevel = OriginalLevel;

    // Try to level up as many times as it's possible
    do
    {
        const int32 NextLevel = (NewLevel + 1);
        if (!TotalExpLevelRow->KeyExistsAtTime(NextLevel))
        {
            // There is no next level, hence current one is the max one
            break;
        }
        
        const int32 NextLevelTotalExp = TotalExpLevelRow->Eval(NextLevel);
        if (TotalExp < NextLevelTotalExp)
        {
            // There is not enough EXP to level up to next level
            break;
        }
        
        NewLevel = NextLevel;
    } while(true);

    // Set the value only if it changed
    if (NewLevel > OriginalLevel)
    {
        SetLevelStat(NewLevel);
    }
}

bool UMTD_PlayerSet::CacheExpRows()
{
    if (((ExpLevelRow) && (TotalExpLevelRow)))
    {
        // Already cached
        return true;
    }
    
    const AActor *Owner = GetOwningActor();
    check(IsValid(Owner));

    const auto GameInstance = Cast<UMTD_GameInstance>(Owner->GetGameInstance());
    check(IsValid(GameInstance));

    const UCurveTable *ExperienceLevelTable = GameInstance->GetLevelExpCurveTable();
    if (!IsValid(ExperienceLevelTable))
    {
        MTDS_WARN("Experience Level Curve Table is invalid.");
        return false;
    }

    const FName ExpRowName = UMTD_LevelComponent::ExpRowName;
    const FRealCurve *ExpFoundRow = ExperienceLevelTable->FindCurve(ExpRowName, {});
    if (!ExpFoundRow)
    {
        MTDS_WARN("Could not find Row [%s] in Experience Level Curve Table.", *ExpRowName.ToString());
        return false;
    }
    
    const FName TotalExpRowName = UMTD_LevelComponent::TotalExpRowName;
    const FRealCurve *TotalExpFoundRow = ExperienceLevelTable->FindCurve(TotalExpRowName, {});
    if (!TotalExpFoundRow)
    {
        MTDS_WARN("Could not find Row [%s] in Experience Level Curve Table.", *TotalExpRowName.ToString());
        return false;
    }

    ExpLevelRow = ExpFoundRow;
    TotalExpLevelRow = TotalExpFoundRow;

    MaxLevel = TotalExpFoundRow->GetNumKeys();
    MaxTotalExp = TotalExpFoundRow->Eval(MaxLevel);
    
    return true;
}
