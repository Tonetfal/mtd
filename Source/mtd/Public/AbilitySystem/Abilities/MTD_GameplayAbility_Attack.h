#pragma once

#include "AbilitySystem/Abilities/MTD_GameplayAbility.h"
#include "mtd.h"

#include "MTD_GameplayAbility_Attack.generated.h"

struct FMTD_AbilityAnimations;
class UMTD_AbilitySystemComponent;

/*
 * Note: C++ implementation is only about triggering new/continuing previous GE attacks for combos, animation and end
 * ability condition. Damage related code has to be handled inside BPs instead. To know if someone was hit, fire events
 * from collision handling code, and listen for them.
 */

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
    void HandleAttackGameplayEffect(UMTD_AbilitySystemComponent *MtdAsc, int32 &OutAttackEffectLevel) const;
    FActiveGameplayEffectHandle GetFirstActiveAttackGameplayEffect(const UMTD_AbilitySystemComponent *MtdAsc) const;
    int32 GetAttackMontageIndexToPlay(const TArray<UAnimMontage *> Animations, int32 AttackEffectLevel) const;
    UAnimMontage *GetAbilityAnimMontage(TArray<UAnimMontage *> Animations, int32 AttackEffectLevel) const;
    void CancelPreviousAttack(UMTD_AbilitySystemComponent *MtdAsc) const;
    void CreateAndApplyAttackGameplayEffectOnSelf(UMTD_AbilitySystemComponent *MtdAsc) const;
    void PlayAttackAnimation(const ACharacter *PlayOn, UAnimMontage *AbilityAnimMontage);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD|Ability",
        meta=(AllowPrivateAccess="true", DisplayName="Attack Gameplay Effect Duration"))
    float AttackGeDuration = 0.f;
};
