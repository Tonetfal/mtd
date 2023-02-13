#pragma once

#include "Character/MTD_AscComponent.h"
#include "mtd.h"

#include "MTD_ManaComponent.generated.h"

class UMTD_AbilitySystemComponent;
class UMTD_ManaComponent;
class UMTD_ManaSet;
struct FOnAttributeChangeData;

/**
 * Component that handles and notifies about mana related events, also makes it easier to read related mana data.
 */
UCLASS(Blueprintable, ClassGroup="Character", meta=(BlueprintSpawnableComponent))
class MTD_API UMTD_ManaComponent
    : public UMTD_AscComponent
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
        FManaAttributeChangedSignature,
        UMTD_ManaComponent *, ManaComponent,
        float, OldValue,
        float, NewValue,
        AActor *, Instigator);

public:
    UMTD_ManaComponent();

    /**
     * Find mana component on a given actor.
     * @param   Actor: actor to search for mana extension component in.
     * @result  Mana component residing on the given actor. Can be nullptr.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Mana Component")
    static UMTD_ManaComponent *FindManaComponent(const AActor *Actor);

    //~UMTD_AscComponent Interface
    virtual void InitializeWithAbilitySystem(UMTD_AbilitySystemComponent *InAbilitySystemComponent) override;
    virtual void UninitializeFromAbilitySystem() override;
    //~End of UMTD_AscComponent Interface

    /**
     * Get current mana amount.
     * @return  Current mana amount.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Mana Component")
    float GetMana() const;

    /**
     * Get max mana amount.
     * @return  Max mana amount.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Mana Component")
    float GetMaxMana() const;

    /**
     * Get mana amount in normalized form [0.0, 1.0].
     * @return  Mana amount in normalized form [0.0, 1.0].
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Mana Component")
    float GetManaNormilized() const;

    /**
     * Check whether mana pool is full or not.
     * @return  If true, mana pool is full, false otherwise.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Mana Component")
    bool IsManaFull() const;

protected:
    /**
     * Event to fire when mana attribute is changed.
     * @param   ChangeData: data about attribute change.
     */
    virtual void OnManaChanged(const FOnAttributeChangeData &ChangeData);
    
    /**
     * Event to fire when max mana attribute is changed.
     * @param   ChangeData: data about attribute change.
     */
    virtual void OnMaxManaChanged(const FOnAttributeChangeData &ChangeData);

public:
    /** Delegate to fire when mana attribute is changed. */
    UPROPERTY(BlueprintAssignable)
    FManaAttributeChangedSignature OnManaChangedDelegate;

    /** Delegate to fire when max mana attribute is changed. */
    UPROPERTY(BlueprintAssignable)
    FManaAttributeChangedSignature OnMaxManaChangedDelegate;

private:
    /** Mana set defining all mana related data to use. */
    UPROPERTY()
    TObjectPtr<const UMTD_ManaSet> ManaSet = nullptr;
};

inline UMTD_ManaComponent *UMTD_ManaComponent::FindManaComponent(const AActor *Actor)
{
    return ((IsValid(Actor)) ? (Actor->FindComponentByClass<UMTD_ManaComponent>()) : (nullptr));
}
