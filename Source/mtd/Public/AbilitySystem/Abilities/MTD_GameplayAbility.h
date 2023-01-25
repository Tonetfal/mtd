#pragma once

#include "Abilities/GameplayAbility.h"
#include "mtd.h"

#include "MTD_GameplayAbility.generated.h"

class UMTD_GameplayAbility;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FMtdAbilitySignature,
    const UMTD_GameplayAbility*, GameplayAbility);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnApplyCooldownSignature,
    const UMTD_GameplayAbility*, GameplayAbility,
    float, Duration);

UENUM(BlueprintType)
enum class EMTD_AbilityActivationPolicy : uint8
{
    /** Try to activate the ability when the input is triggered. */
    OnInputTriggered,

    /** Initially try to activate the ability while the input is active. */
    WhileInputActive,

    /** Try to activate the ability when an avatar is assigned. */
    OnSpawn
};

UCLASS(Abstract, HideCategories=Input, meta=(ShortTooltip="The base gameplay ability class used by this project."))
class MTD_API UMTD_GameplayAbility
    : public UGameplayAbility
    , public IGameplayTagAssetInterface
{
    GENERATED_BODY()

public:
    UMTD_GameplayAbility();

    virtual void OnGiveAbility(const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilitySpec &Spec) override;

    FGameplayTag GetMainAbilityTag() const;
    EMTD_AbilityActivationPolicy GetActivationPolicy() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Ability Animation")
    UAnimMontage *GetRandomAbilityAnimMontage() const;

    virtual void GetOwnedGameplayTags(FGameplayTagContainer &TagContainer) const override;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Ability Cooldown")
    float GetCooldownNormilized() const;

    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo *ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData *TriggerEventData) override;

    virtual const FGameplayTagContainer *GetCooldownTags() const override;
    virtual void ApplyCooldown(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo *ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo) const override;

    virtual void EndAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo *ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        bool bReplicateEndAbility,
        bool bWasCancelled) override;

    virtual void InputPressed(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo *ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
    virtual void OnDoneAddingNativeTags();

private:
    void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilitySpec &Spec) const;

public:
    /**
     * Note: A delegate on GameplayAbility can be used properly only if the ability instantiating policy doesn' t
     * foresees ability instantiating for each ability use.
     */
    UPROPERTY(BlueprintAssignable)
    FMtdAbilitySignature OnAbilityActivatedDelegate;

    /**
     * Note: A delegate on GameplayAbility can be used properly only if the ability instantiating policy doesn' t
     * foresees ability instantiating for each ability use.
     */
    UPROPERTY(BlueprintAssignable)
    FMtdAbilitySignature OnAbilityEndedDelegate;

    /**
     * Note: A delegate on GameplayAbility can be used properly only if the ability instantiating policy doesn' t
     * foresees ability instantiating for each ability use.
     */
    UPROPERTY(BlueprintAssignable)
    FOnApplyCooldownSignature OnApplyCooldownDelegate;

    /**
     * Note: A delegate on GameplayAbility can be used properly only if the ability instantiating policy doesn' t
     * foresees ability instantiating for each ability use.
     */
    UPROPERTY(BlueprintAssignable)
    FMtdAbilitySignature OnInputPressedDelegate;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MTD|Cooldown")
    FScalableFloat CooldownDuration;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MTD|Cost")
    FScalableFloat ManaCost;

private:
    /**
     * Gameplay Tag that is used to determine animation montage to play. The tag will be added to AbilityTags as well.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Ability",
        meta=(AllowPrivateAccess="true"))
    FGameplayTag MainAbilityTag = FGameplayTag::EmptyTag;

    UPROPERTY(EditDefaultsOnly, Category="MTD|Activation",
        meta=(AllowPrivateAccess="true"))
    EMTD_AbilityActivationPolicy ActivationPolicy = EMTD_AbilityActivationPolicy::OnInputTriggered;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Damage",
        meta=(AllowPrivateAccess="true"))
    float DamageAdditive = 0.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Damage",
        meta=(AllowPrivateAccess="true"))
    float DamageMultiplier = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MTD|Cooldown",
        meta=(AllowPrivateAccess="true"))
    FGameplayTagContainer CooldownTags;

    UPROPERTY(BlueprintReadOnly, Category="MTD|Event Data", meta=(AllowPrivateAccess="true"))
    FGameplayEventData GameplayEventData;

    UPROPERTY(Transient)
    FGameplayTagContainer TempCooldownTags;
};

inline FGameplayTag UMTD_GameplayAbility::GetMainAbilityTag() const
{
    return MainAbilityTag;
}

inline EMTD_AbilityActivationPolicy UMTD_GameplayAbility::GetActivationPolicy() const
{
    return ActivationPolicy;
}

