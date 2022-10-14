#pragma once

#include "Abilities/GameplayAbility.h"
#include "mtd.h"

#include "MTD_GameplayAbility.generated.h"

UCLASS()
class MTD_API UMTD_GameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UMTD_GameplayAbility();
	
	virtual const FGameplayTagContainer *GetCooldownTags() const override;
	virtual void ApplyCooldown(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo *ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) const override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MTD|Cooldown")
	FScalableFloat CooldownDuration;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MTD|Cooldown",
		meta=(AllowPrivateAccess="true"))
	FGameplayTagContainer CooldownTags;

	UPROPERTY(Transient)
	FGameplayTagContainer TempCooldownTags;
};
