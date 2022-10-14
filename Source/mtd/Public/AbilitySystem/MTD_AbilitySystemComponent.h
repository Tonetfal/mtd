#pragma once

#include "AbilitySystemComponent.h"
#include "mtd.h"

#include "MTD_AbilitySystemComponent.generated.h"

UCLASS()
class MTD_API UMTD_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	bool SetGameplayEffectDurationHandle(
		FActiveGameplayEffectHandle Handle, float NewDuration);
	
	bool IncreaseGameplayEffectLevelHandle(
		FActiveGameplayEffectHandle Handle, float IncreaseBy);
};
