#pragma once

#include "Components/ActorComponent.h"
#include "mtd.h"

#include "MTD_BalanceComponent.generated.h"

struct FGameplayEffectSpec;
struct FOnAttributeChangeData;
class UMTD_AbilitySystemComponent;
class UMTD_BalanceSet;

UCLASS(BlueprintType)
class UMTD_BalanceHitData : public UObject
{
    GENERATED_BODY()
    
public:
    UPROPERTY(BlueprintReadOnly)
    float BalanceDamage = 0.f;
    
    UPROPERTY(BlueprintReadOnly)
    FVector KnockbackDirection = FVector::ZeroVector;
};

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class MTD_API UMTD_BalanceComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBalanceDownSignature, const UMTD_BalanceHitData*, HitData);

public:
    UMTD_BalanceComponent();

    UFUNCTION(BlueprintCallable, Category="MTD|Balance")
    static UMTD_BalanceComponent *FindBalanceComponent(const AActor *Actor);

    UFUNCTION(BlueprintCallable, Category="MTD|Balance")
    void InitializeWithAbilitySystem(UMTD_AbilitySystemComponent *Asc);

    UFUNCTION(BlueprintCallable, Category="MTD|Balance")
    void UninitializeFromAbilitySystem();

protected:
    virtual void OnUnregister() override;

private:
    void OnBalanceDown(
        AActor *EffectInstigator,
        AActor *EffectCauser,
        const FGameplayEffectSpec &EffectSpec,
        float EffectMagnitude);

public:
    UPROPERTY(BlueprintAssignable)
    FOnBalanceDownSignature OnBalanceDownDelegate;

private:
    UPROPERTY()
    TObjectPtr<UMTD_AbilitySystemComponent> AbilitySystemComponent = nullptr;

    UPROPERTY()
    TObjectPtr<const UMTD_BalanceSet> BalanceSet = nullptr;
};

inline UMTD_BalanceComponent *UMTD_BalanceComponent::FindBalanceComponent(const AActor *Actor)
{
    return (IsValid(Actor)) ? (Actor->FindComponentByClass<UMTD_BalanceComponent>()) : (nullptr);
}
