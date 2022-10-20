#pragma once

#include "AbilitySystem/Attributes/MTD_AttributeSet.h"
#include "mtd.h"

#include "MTD_CombatSet.generated.h"

UCLASS()
class MTD_API UMTD_CombatSet : public UMTD_AttributeSet
{
	GENERATED_BODY()

public:
	ATTRIBUTE_ACCESSORS(UMTD_CombatSet, BaseDamage);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData BaseDamage;
};
