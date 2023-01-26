#pragma once

#include "AbilitySystem/Attributes/MTD_AttributeSet.h"
#include "mtd.h"

#include "MTD_PlayerSet.generated.h"

/**
 * Attribute set that defines all the player statistics data which is given by its equipment and by leveling up. It
 * also contains data related to the level.
 */
UCLASS()
class MTD_API UMTD_PlayerSet
    : public UMTD_AttributeSet
{
    GENERATED_BODY()

public:
    DECLARE_MULTICAST_DELEGATE(FOnMaxLevelSignature);

public:
    ATTRIBUTE_ACCESSORS(UMTD_PlayerSet, LevelStat);
    ATTRIBUTE_ACCESSORS(UMTD_PlayerSet, ExperienceStat);
    
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
    void TryLevelUp(int32 TotalExp);

    /**
     * Lazy cache operation.
     */
    bool CacheExpRows();

public:
    FOnMaxLevelSignature OnMaxLevelDelegate;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData LevelStat;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData ExperienceStat;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData HealthStat;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData DamageStat;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData SpeedStat;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData HealthStat_Bonus;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData DamageStat_Bonus;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData SpeedStat_Bonus;

    TMap<FGameplayAttribute, FGameplayAttribute> BonusAttributeReplicationMapping;

    const FRealCurve *ExpLevelRow = nullptr;
    const FRealCurve *TotalExpLevelRow = nullptr;
    int32 MaxTotalExp = 0;
    int32 MaxLevel = 0;
    
    bool bIsMaxLevel = false;
};
