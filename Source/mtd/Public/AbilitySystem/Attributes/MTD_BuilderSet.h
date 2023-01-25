#pragma once

#include "AbilitySystem/Attributes/MTD_AttributeSet.h"
#include "mtd.h"

#include "MTD_BuilderSet.generated.h"

/**
 * Attribute set that defines all the builder statistics data which is given by its equipment and by leveling up.
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

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData HealthStat;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData DamageStat;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData RangeStat;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttributeData SpeedStat;
};
