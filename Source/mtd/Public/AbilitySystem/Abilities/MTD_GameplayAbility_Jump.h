#pragma once

#include "AbilitySystem/Abilities/MTD_GameplayAbility.h"
#include "mtd.h"

#include "MTD_GameplayAbility_Jump.generated.h"

UCLASS()
class MTD_API UMTD_GameplayAbility_Jump : public UMTD_GameplayAbility
{
	GENERATED_BODY()
	
public:
	UMTD_GameplayAbility_Jump();
	
	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo *ActorInfo,
		const FGameplayTagContainer *SourceTags,
		const FGameplayTagContainer *TargetTags,
		FGameplayTagContainer *OptionalRelevantTags) const override;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo *ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData *TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo *ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

	UFUNCTION()
	virtual void OnAnimMontageEnded(const FHitResult &Hit);
};
