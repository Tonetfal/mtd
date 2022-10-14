#pragma once

#include "AbilitySystem/Abilities/MTD_GameplayAbility.h"
#include "mtd.h"

#include "MTD_GameplayAbility_Death.generated.h"

UCLASS()
class MTD_API UMTD_GameplayAbility_Death : public UMTD_GameplayAbility
{
    GENERATED_BODY()
    
public:
    UMTD_GameplayAbility_Death();
    
protected:
    void OnDoneAddingNativeTags();
    
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