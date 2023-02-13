#include "AbilitySystem/Attributes/MTD_PlayerSet.h"

#include "Character/MTD_LevelComponent.h"
#include "System/MTD_GameInstance.h"

UMTD_PlayerSet::UMTD_PlayerSet()
{
    // Bind corresponding attribute (Health -> BonusHealth, Damage -> BonusDamage, ...)
#define MAP(x) BonusAttributeReplicationMapping.Add(Get ## x ## Stat_BonusAttribute(), Get ## x ## StatAttribute())
    MAP(Damage);
    MAP(Health);
    MAP(Speed);
#undef MAP
}

void UMTD_PlayerSet::PreAttributeChange(const FGameplayAttribute &Attribute, float &NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

    if (Attribute == GetLevelExperienceAttribute())
    {
        if (CacheExpRows())
        {
            // EXP values range [0, MaxTotalExp]
            NewValue = FMath::Clamp(NewValue, 0.f, MaxTotalExp);
        }
    }
}

void UMTD_PlayerSet::PostAttributeChange(const FGameplayAttribute &Attribute, float OldValue, float NewValue)
{
    Super::PostAttributeChange(Attribute, OldValue, NewValue);

    if (Attribute == GetLevelExperienceAttribute())
    {
        // Avoid running level up logic at max level
        if (!bIsMaxLevel)
        {
            TryLevelUp(NewValue);
        }
    }

    else if (Attribute == GetLevelAttribute())
    {
        if (!bIsMaxLevel)
        {
            if (NewValue == MaxLevel)
            {
                bIsMaxLevel = true;

                // Notify about reaching to max level
                OnMaxLevelDelegate.Broadcast();
            }
        }
    }

    // Check if changed attribute is a bonus one
    else if (const auto &MappedValue = BonusAttributeReplicationMapping.Find(Attribute))
    {
        // Get the base attribute data, i.e. if HealthBonus has been changed, this will get Health attribute
        FGameplayAttributeData *AttributeData = MappedValue->GetGameplayAttributeData(this);
        check(AttributeData);
        
        // Modify the base attribute by the bonus attribute delta
        const float Delta = (NewValue - OldValue);
        const float CurrentValue = AttributeData->GetCurrentValue();
        const float FinalValue = (CurrentValue + Delta);
        AttributeData->SetCurrentValue(FinalValue);
    }
}

void UMTD_PlayerSet::TryLevelUp(int32 TotalExp)
{
    if (!CacheExpRows())
    {
        return;
    }
    
    check(TotalExpLevelRow);
    ensure(!bIsMaxLevel);

    // Save level before leveling up logic
    const int32 OriginalLevel = Level.GetCurrentValue();

    // Save current level in here. Might be changed throughout the function
    int32 NewLevel = OriginalLevel;

    // Try to level up as many times as it's possible
    do
    {
        const int32 NextLevel = (NewLevel + 1);
        if (!TotalExpLevelRow->KeyExistsAtTime(NextLevel))
        {
            // There is no next level, hence current one should be the max one
            break;
        }
        
        const int32 NextLevelTotalExp = TotalExpLevelRow->Eval(NextLevel);
        if (TotalExp < NextLevelTotalExp)
        {
            // There is not enough EXP to level up to next level
            break;
        }

        // Level up
        NewLevel = NextLevel;
    } while(true);

    // Set the value only if it changed
    if (NewLevel > OriginalLevel)
    {
        SetLevel(NewLevel);
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

    // Cache found values to avoid accessing everything above every time EXP changes
    ExpLevelRow = ExpFoundRow;
    TotalExpLevelRow = TotalExpFoundRow;
    MaxLevel = TotalExpFoundRow->GetNumKeys();
    MaxTotalExp = TotalExpFoundRow->Eval(MaxLevel);
    
    return true;
}
