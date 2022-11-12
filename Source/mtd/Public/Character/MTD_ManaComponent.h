#pragma once

#include "Character/MTD_PawnComponent.h"
#include "mtd.h"

#include "MTD_ManaComponent.generated.h"

class UMTD_ManaSet;
class UMTD_ManaComponent;
class UMTD_AbilitySystemComponent;
struct FOnAttributeChangeData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
    FManaAttributeChangedSignature,
    UMTD_ManaComponent*, ManaComponent,
    float, OldValue,
    float, NewValue,
    AActor*, Instigator);

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class MTD_API UMTD_ManaComponent : public UMTD_PawnComponent
{
    GENERATED_BODY()

public:
    UMTD_ManaComponent();

    UFUNCTION(BlueprintCallable, Category="MTD|Mana")
    static UMTD_ManaComponent *FindManaComponent(const AActor *Actor)
    {
        return (IsValid(Actor)) ? (Actor->FindComponentByClass<UMTD_ManaComponent>()) : (nullptr);
    }

    UFUNCTION(BlueprintCallable, Category="MTD|Mana")
    void InitializeWithAbilitySystem(UMTD_AbilitySystemComponent *Asc);

    UFUNCTION(BlueprintCallable, Category="MTD|Mana")
    void UninitializeFromAbilitySystem();

    UFUNCTION(BlueprintCallable, Category="MTD|Mana")
    float GetMana() const;

    UFUNCTION(BlueprintCallable, Category="MTD|Mana")
    float GetMaxMana() const;

    UFUNCTION(BlueprintCallable, Category="MTD|Mana")
    float GetManaNormilized() const;

protected:
    virtual void OnUnregister() override;

    virtual void OnManaChanged(const FOnAttributeChangeData &ChangeData);
    virtual void OnMaxManaChanged(const FOnAttributeChangeData &ChangeData);

public:
    UPROPERTY(BlueprintAssignable)
    FManaAttributeChangedSignature OnManaChangedDelegate;

    UPROPERTY(BlueprintAssignable)
    FManaAttributeChangedSignature OnMaxManaChangedDelegate;

private:
    UPROPERTY()
    TObjectPtr<UMTD_AbilitySystemComponent> AbilitySystemComponent = nullptr;

    UPROPERTY()
    TObjectPtr<const UMTD_ManaSet> ManaSet = nullptr;
};
