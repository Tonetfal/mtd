#pragma once

#include "Abilities/GameplayAbility.h"
#include "mtd.h"

#include "MTD_GameplayAbility.generated.h"

class UMTD_GameplayAbility;

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

/**
 * Base gameplay ability class used by this project.
 */
UCLASS(Abstract, HideCategories="Input")
class MTD_API UMTD_GameplayAbility
    : public UGameplayAbility
    , public IGameplayTagAssetInterface
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FMtdAbilitySignature,
        const UMTD_GameplayAbility *, GameplayAbility);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
        FOnApplyCooldownSignature,
        const UMTD_GameplayAbility *, GameplayAbility,
        float, Duration);

public:
    UMTD_GameplayAbility();

    virtual void OnGiveAbility(const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilitySpec &Spec) override;

    /**
     * Get main ability tag.
     * @return  Main ability tag.
     */
    FGameplayTag GetMainAbilityTag() const;

    /**
     * Get ability activation policy.
     * @return  Ability activation policy.
     */
    EMTD_AbilityActivationPolicy GetActivationPolicy() const;

    /**
     * Get random ability animation montage.
     * @return  Random ability animation montage.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Ability Animation")
    const UAnimMontage *GetRandomAbilityAnimMontage() const;
    
    virtual void GetOwnedGameplayTags(FGameplayTagContainer &TagContainer) const override;

    /**
     * Get cooldown time in normilized form [0.0, 1.0].
     * @return  Cooldown time in normilized form [0.0, 1.0].
     */
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
    /**
     * Add main ability tag to ability tags container.
     */
    virtual void OnDoneAddingNativeTags();

private:
    void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilitySpec &Spec) const;

public:
    /**
     * Note: A delegate on GameplayAbility can be used properly only if the ability instantiating policy doesn't
     * foresees ability instantiating for each ability use.
     */
    UPROPERTY(BlueprintAssignable)
    FMtdAbilitySignature OnAbilityActivatedDelegate;

    /**
     * Note: A delegate on GameplayAbility can be used properly only if the ability instantiating policy doesn't
     * foresees ability instantiating for each ability use.
     */
    UPROPERTY(BlueprintAssignable)
    FMtdAbilitySignature OnAbilityEndedDelegate;

    /**
     * Note: A delegate on GameplayAbility can be used properly only if the ability instantiating policy doesn't
     * foresees ability instantiating for each ability use.
     */
    UPROPERTY(BlueprintAssignable)
    FOnApplyCooldownSignature OnApplyCooldownDelegate;

    /**
     * Note: A delegate on GameplayAbility can be used properly only if the ability instantiating policy doesn't
     * foresees ability instantiating for each ability use.
     */
    UPROPERTY(BlueprintAssignable)
    FMtdAbilitySignature OnInputPressedDelegate;

    /** Ability cooldown duration time in seconds. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MTD|Cooldown")
    FScalableFloat CooldownDuration;

    /** Ability application mana cost. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MTD|Cost")
    FScalableFloat ManaCost;

protected:
    /**
     * Main gameplay tag determening this ability. Is be added to AbilityTags after initialization. Is used to determine
     * animation montage to play.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Ability", meta=(AllowPrivateAccess="true"))
    FGameplayTag MainAbilityTag = FGameplayTag::EmptyTag;

    /** Ability activation policy determening when the ability has to be used. */
    UPROPERTY(EditDefaultsOnly, Category="MTD|Activation", meta=(AllowPrivateAccess="true"))
    EMTD_AbilityActivationPolicy ActivationPolicy = EMTD_AbilityActivationPolicy::OnInputTriggered;

    /** Flat damage amount to add on hit. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Damage", meta=(AllowPrivateAccess="true"))
    float DamageAdditive = 0.f;

    /** Damage multiplier that can be used by further gameplay features. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Damage", meta=(AllowPrivateAccess="true"))
    float DamageMultiplier = 1.f;

    /** Ability cooldown tags to determine the cooldown. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MTD|Cooldown", meta=(AllowPrivateAccess="true"))
    FGameplayTagContainer CooldownTags;

    /** The most recent activated ability's event data. */
    UPROPERTY(BlueprintReadOnly, Category="MTD|Event Data", meta=(AllowPrivateAccess="true"))
    FGameplayEventData GameplayEventData;

    /** A temporary container used to move cooldown tags around. */
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
