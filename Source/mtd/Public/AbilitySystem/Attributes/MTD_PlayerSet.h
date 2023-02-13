#pragma once

#include "AbilitySystem/Attributes/MTD_AttributeSet.h"
#include "mtd.h"

#include "MTD_PlayerSet.generated.h"

/**
 * Attribute set containing all the player stat attributes which are given by equipment and by leveling up. Also
 * contains level related data.
 */
UCLASS()
class MTD_API UMTD_PlayerSet
    : public UMTD_AttributeSet
{
    GENERATED_BODY()

public:
    DECLARE_MULTICAST_DELEGATE(FOnMaxLevelSignature);

public:
    ATTRIBUTE_ACCESSORS(UMTD_PlayerSet, Level);
    ATTRIBUTE_ACCESSORS(UMTD_PlayerSet, LevelExperience);
    
    ATTRIBUTE_ACCESSORS(UMTD_PlayerSet, HealthStat);
    ATTRIBUTE_ACCESSORS(UMTD_PlayerSet, DamageStat);
    ATTRIBUTE_ACCESSORS(UMTD_PlayerSet, SpeedStat);
    
    ATTRIBUTE_ACCESSORS(UMTD_PlayerSet, HealthStat_Bonus);
    ATTRIBUTE_ACCESSORS(UMTD_PlayerSet, DamageStat_Bonus);
    ATTRIBUTE_ACCESSORS(UMTD_PlayerSet, SpeedStat_Bonus);

public:
    UMTD_PlayerSet();
    
    virtual void PreAttributeChange(const FGameplayAttribute &Attribute, float &NewValue) override;
    virtual void PostAttributeChange(const FGameplayAttribute &Attribute, float OldValue, float NewValue) override;

private:
    /**
     * Try to level up given new total EXP. It will consider all the levels between the current one and the one that can
     * be reached with the new EXP.
     * @param   TotalExp: new EXP value.
     */
    void TryLevelUp(int32 TotalExp);

    /**
     * Lazy cache operation.
     * @return  If true, cache has been successful or already did, false otherwise.
     */
    bool CacheExpRows();

public:
    /** Delegate to fire when max level has been reached. */
    FOnMaxLevelSignature OnMaxLevelDelegate;

protected:
    /** Current level value. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData Level;

    /** Current experience value. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData LevelExperience;

    /** Base health scale value. Modified by equipment. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData HealthStat;

    /** Base damage scale value. Modified by equipment. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData DamageStat;

    /** Base speed scale value. Modified by equipment. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData SpeedStat;
    
    /** Base health scale value. Modified by leveling up. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData HealthStat_Bonus;

    /** Base damage scale value. Modified by leveling up. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData DamageStat_Bonus;

    /** Base speed scale value. Modified by leveling up. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData SpeedStat_Bonus;

    /** Bonus player attributes to regular attributes mapping. Does NOT have anything to do with networking. */
    TMap<FGameplayAttribute, FGameplayAttribute> BonusAttributeReplicationMapping;

    /** Cached row containing EXP amount for each level. */
    const FRealCurve *ExpLevelRow = nullptr;

    /** Cached row containing total EXP amount for each level. */
    const FRealCurve *TotalExpLevelRow = nullptr;

    /** Cached max EXP value to level to max level. */
    int32 MaxTotalExp = 0;

    /** Cached max reachable level. */
    int32 MaxLevel = 0;

    /** If true, max level has been hit, false otherwise. */
    bool bIsMaxLevel = false;
};
