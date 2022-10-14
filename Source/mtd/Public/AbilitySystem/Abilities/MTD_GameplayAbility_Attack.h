#pragma once

#include "AbilitySystem/Abilities/MTD_GameplayAbility.h"
#include "mtd.h"

#include "MTD_GameplayAbility_Attack.generated.h"

UCLASS()
class MTD_API UMTD_GameplayAbility_Attack : public UMTD_GameplayAbility
{
    GENERATED_BODY()
    
public:
    UMTD_GameplayAbility_Attack();
    
protected:
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
	
	virtual void OnAttackMontageEnded(
		UAnimMontage *AnimMontage, bool bInterrupted);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="MTD|Ability",
		meta=(AllowPrivateAccess="true"))
	TArray<TObjectPtr<UAnimMontage>> AttackMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD|Ability",
		meta=(AllowPrivateAccess="true"))
	float Duration = 0.f;
};