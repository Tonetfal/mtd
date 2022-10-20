#pragma once

#include "AbilitySystem/Attributes/MTD_AttributeSet.h"
#include "mtd.h"

#include "MTD_PlayerSet.generated.h"

UCLASS()
class MTD_API UMTD_PlayerSet : public UMTD_AttributeSet
{
	GENERATED_BODY()

public:
	ATTRIBUTE_ACCESSORS(UMTD_PlayerSet, HealthStat);
	ATTRIBUTE_ACCESSORS(UMTD_PlayerSet, DamageStat);
	ATTRIBUTE_ACCESSORS(UMTD_PlayerSet, SpeedStat);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData HealthStat;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData DamageStat;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData SpeedStat;
};
