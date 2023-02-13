#pragma once

#include "AbilitySystem/Abilities/MTD_GameplayAbility.h"
#include "mtd.h"

#include "MTD_GameplayAbility_Death.generated.h"

/**
 * Gameplay ability that allows a character to die. Is fired automatically when Gameplay.Event.Death event is fired.
 *
 * Note: This is an abstract class because the default implementation lacks EndAbility condition, it should be defined
 * inside blueprints instead.
 */
UCLASS(Abstract)
class MTD_API UMTD_GameplayAbility_Death
    : public UMTD_GameplayAbility
{
    GENERATED_BODY()
    
public:
    UMTD_GameplayAbility_Death();
    
protected:
    virtual void OnDoneAddingNativeTags() override;
    
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

	UFUNCTION(BlueprintCallable, Category = "MTD|Ability")
	void StartDeath();

	UFUNCTION(BlueprintCallable, Category = "MTD|Ability")
	void FinishDeath();
    
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MTD|Death")
    bool bAutoStartDeath = true;
};
