#pragma once

#include "AbilitySystem/Abilities/MTD_GameplayAbility.h"
#include "mtd.h"

#include "MTD_GameplayAbility_Attack.generated.h"

class UMTD_AbilitySystemComponent;
struct FMTD_AbilityAnimations;

/**
 * Gameplay ability to perform an attack, can be both melee and ranged. Handles attack GE application and increases its
 * level as the attacks happen.
 * 
 * Note: This is an abstract class because C++ implementation is only about triggering new/continuing previous GE
 * attacks for combos, animation and end ability condition. Damage related code has to be handled inside blueprints
 * instead.
 */
UCLASS(Abstract)
class MTD_API UMTD_GameplayAbility_Attack
    : public UMTD_GameplayAbility
{
    GENERATED_BODY()

public:
    UMTD_GameplayAbility_Attack();

protected:
    virtual void OnDoneAddingNativeTags() override;
    
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

private:
    /**
     * Create attack gameplay effect if none, otherwise increase its level.
     * @param   MtdAsc: MTD ability system component to apply the gameplay effect on.
     * @param   OutAttackEffectLevel: attack gameplay effect level.
     */
    void HandleAttackGameplayEffect(UMTD_AbilitySystemComponent *MtdAsc, int32 &OutAttackEffectLevel) const;

    /**
     * Get first active attack gameplay effect active on given ability system component.
     * @param   MtdAsc: MTD abilty system component to search for gameplay effect on.
     * @return  Active attack gameplay effect handle.
     */
    FActiveGameplayEffectHandle GetFirstActiveAttackGameplayEffect(const UMTD_AbilitySystemComponent *MtdAsc) const;

    /**
     * Create and apply attack gameplay effect on self. Should be only called if none is active yet.
     * @param   MtdAsc: MTD ability system component to create and apply the attack gameplay effect on.
     */
    void CreateAndApplyAttackGameplayEffectOnSelf(UMTD_AbilitySystemComponent *MtdAsc) const;

    /**
     * Get attack animation montage index to play for current attack.
     * @param   Animations: animations container with all the attack animations.
     * @param   AttackEffectLevel: current attack gameplay effect level, is used to determine the index.
     * @return  Attack animation montage index for the given animations container to play.
     */
    int32 GetAttackMontageIndexToPlay(const TArray<UAnimMontage *> Animations, int32 AttackEffectLevel) const;

    /**
     * Get attack animation montage to play for current attack.
     * @param   Animations: animations container with all the attack animations.
     * @param   AttackEffectLevel: current attack gameplay effect level, is used to determine the index.
     * @return  Attack animation montage to play for current attack.
     */
    UAnimMontage *GetAbilityAnimMontage(TArray<UAnimMontage *> Animations, int32 AttackEffectLevel) const;

    /**
     * Play attack animation montage on character.
     * @param   Character: character to play the animation on.
     * @param   AbilityAnimMontage: attack animation montage to play.
     */
    void PlayAttackAnimation(const ACharacter *Character, UAnimMontage *AbilityAnimMontage);

protected:
    /** Time in seconds attack gameplay effect lasts for. The timer is updated each time an attack ability is used. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD|Ability",
        meta=(AllowPrivateAccess="true", DisplayName="Attack Gameplay Effect Duration"))
    float AttackGeDuration = 0.f;
};
