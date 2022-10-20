#pragma once

#include "AbilitySystem/Abilities/MTD_GameplayAbility.h"
#include "mtd.h"

#include "MTD_GameplayAbility_Attack.generated.h"

class UMTD_AbilitySystemComponent;

// Note: C++ implementation is only about triggering new/continuing
// previous GE attacks for combos, animation and end ability condition.
// Damage related code has to be handled inside BPs instead.
// To know if someone was hit, fire events from collision handling code, and
// listen for them.

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

private:
	void HandleAttackGameplayEffect(
		UMTD_AbilitySystemComponent *MtdAsc,
		int32 &OutAttackMontageIndex) const;

	FActiveGameplayEffectHandle GetFirstActiveAttackGameplayEffect(
		const UMTD_AbilitySystemComponent *MtdAsc) const;

	int32 GetAttackMontageIndexToPlay(
		FActiveGameplayEffectHandle AttackGeHandle,
		const UMTD_AbilitySystemComponent *MtdAsc) const;

	void CancelPreviousAttack(UMTD_AbilitySystemComponent *MtdAsc) const;

	void CreateAndApplyAttackGameplayEffectOnSelf(
		UMTD_AbilitySystemComponent *MtdAsc) const;

	void PlayAttackAnimation(const ACharacter *PlayOn, int32 AnimMontageIndex);

protected:
	// Instead of having a vector of anim montages inside the GA put them in the
	// character instead since different character will be able to use the same
	// weapon which will have the same ability, so we must not give some anim
	// montages for an ability, we'd create many same abilities with just
	// different montages that way.
	// Instead, the weapon should hold its type (Sword, Axe, Knife etc) inside,
	// and the attack ability (though this applies not only to attacks) will
	// know what weapon type it's animating, so the ability will ask the avatar
	// to play a certain animation at a given index for a certain weapon type.
	// The anim montage index is got from computing from
	// - AnimMontages.Num() % AttackGEs.Num()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="MTD|Ability",
		meta=(AllowPrivateAccess="true"))
	TArray<TObjectPtr<UAnimMontage>> AttackAnimMontages;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD|Ability",
		meta=(AllowPrivateAccess="true", DisplayName="Attack Gameplay Effect Duration"))
	float AttackGeDuration = 0.f;
};