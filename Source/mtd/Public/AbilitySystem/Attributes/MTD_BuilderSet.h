#pragma once

#include "AbilitySystem/Attributes/MTD_AttributeSet.h"
#include "mtd.h"

#include "MTD_BuilderSet.generated.h"

/**
* Attribute set containing all the builder stat attributes which are given by equipment and by leveling up.
 *
 * Note that all the stat attributes are values to compute the final scale of their relative attribute.
 */
UCLASS()
class MTD_API UMTD_BuilderSet
    : public UMTD_AttributeSet
{
    GENERATED_BODY()

public:
    ATTRIBUTE_ACCESSORS(UMTD_BuilderSet, HealthStat);
    ATTRIBUTE_ACCESSORS(UMTD_BuilderSet, DamageStat);
    ATTRIBUTE_ACCESSORS(UMTD_BuilderSet, RangeStat);
    ATTRIBUTE_ACCESSORS(UMTD_BuilderSet, SpeedStat);
    
    ATTRIBUTE_ACCESSORS(UMTD_BuilderSet, HealthStat_Bonus);
    ATTRIBUTE_ACCESSORS(UMTD_BuilderSet, DamageStat_Bonus);
    ATTRIBUTE_ACCESSORS(UMTD_BuilderSet, RangeStat_Bonus);
    ATTRIBUTE_ACCESSORS(UMTD_BuilderSet, SpeedStat_Bonus);

public:
    UMTD_BuilderSet();

    virtual void PostAttributeChange(const FGameplayAttribute &Attribute, float OldValue, float NewValue) override;

protected:
    /** Base health scale value. Modified by equipment. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData HealthStat;

    /** Base damage scale value. Modified by equipment. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData DamageStat;
    
    /** Base range scale value. Modified by equipment. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData RangeStat;

    /** Base speed scale value. Modified by equipment. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData SpeedStat;
    
    /** Base health scale value. Modified by leveling up. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData HealthStat_Bonus;

    /** Base damage scale value. Modified by leveling up. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData DamageStat_Bonus;
    
    /** Base range scale value. Modified by leveling up. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData RangeStat_Bonus;

    /** Base speed scale value. Modified by leveling up. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData SpeedStat_Bonus;

    /** Bonus builder attributes to regular attributes mapping. Does NOT have anything to do with networking. */
    TMap<FGameplayAttribute, FGameplayAttribute> BonusAttributeReplicationMapping;
};
