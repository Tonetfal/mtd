#pragma once

#include "Character/MTD_AscComponent.h"
#include "mtd.h"

#include "MTD_BalanceComponent.generated.h"

class UMTD_BalanceSet;
struct FGameplayEffectSpec;
struct FOnAttributeChangeData;

/**
 * Hit related data container.
 */
UCLASS(BlueprintType)
class UMTD_BalanceHitData
    : public UObject
{
    GENERATED_BODY()
    
public:
    /** Amount of balance damage dealt. */
    UPROPERTY(BlueprintReadOnly)
    float BalanceDamage = 0.f;

    /** Knock back direction a character has to knock back towards. */
    UPROPERTY(BlueprintReadOnly)
    FVector KnockBackDirection = FVector::ZeroVector;
};

/**
 * Component that handles and notifies about balance related events.
 */
UCLASS(Blueprintable, ClassGroup="Character", meta=(BlueprintSpawnableComponent))
class MTD_API UMTD_BalanceComponent
    : public UMTD_AscComponent
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnBalanceDownSignature,
        const UMTD_BalanceHitData *, HitData);

public:
    UMTD_BalanceComponent();

    /**
     * Find balance component on a given actor.
     * @param   Actor: actor to search for balance component in.
     * @result  Balance component residing on the given actor. Can be nullptr.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Balance Component")
    static UMTD_BalanceComponent *FindBalanceComponent(const AActor *Actor);

    //~UMTD_AscComponent Interface
    virtual void InitializeWithAbilitySystem(UMTD_AbilitySystemComponent *InAbilitySystemComponent) override;
    virtual void UninitializeFromAbilitySystem() override;
    //~End of UMTD_AscComponent Interface

private:
    /**
     * Event to fire when balance has been knocked off.
     * @param   EffectInstigator: actor started the whole chain.
     * @param   EffectCauser: physical actor tied to the application of this effect.
     * @param   EffectSpec: balance damage gameplay effect spec.
     * @param   EffectMagnitude: amount of balance damage dealt.
     */
    void OnBalanceDown(AActor *EffectInstigator, AActor *EffectCauser, const FGameplayEffectSpec &EffectSpec,
        float EffectMagnitude);

public:
    /** Delegate to fire when owner's balance is down.  */
    UPROPERTY(BlueprintAssignable)
    FOnBalanceDownSignature OnBalanceDownDelegate;

private:
    /** Balance set defining all balance related data to use. */
    UPROPERTY()
    TObjectPtr<const UMTD_BalanceSet> BalanceSet = nullptr;
};

inline UMTD_BalanceComponent *UMTD_BalanceComponent::FindBalanceComponent(const AActor *Actor)
{
    return ((IsValid(Actor)) ? (Actor->FindComponentByClass<UMTD_BalanceComponent>()) : (nullptr));
}
