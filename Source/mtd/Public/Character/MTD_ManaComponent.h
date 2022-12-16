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

    UFUNCTION(BlueprintCallable, Category="MTD|Mana Component")
    static UMTD_ManaComponent *FindManaComponent(const AActor *Actor);

    UFUNCTION(BlueprintCallable, Category="MTD|Mana Component")
    void InitializeWithAbilitySystem(UMTD_AbilitySystemComponent *Asc);

    UFUNCTION(BlueprintCallable, Category="MTD|Mana Component")
    void UninitializeFromAbilitySystem();

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Mana Component")
    float GetMana() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Mana Component")
    float GetMaxMana() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Mana Component")
    float GetManaNormilized() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Mana Component")
    bool IsManaFull() const;

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

inline UMTD_ManaComponent *UMTD_ManaComponent::FindManaComponent(const AActor *Actor)
{
    return (IsValid(Actor)) ? (Actor->FindComponentByClass<UMTD_ManaComponent>()) : (nullptr);
}
