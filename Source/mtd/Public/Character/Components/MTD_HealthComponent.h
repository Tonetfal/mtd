#pragma once

#include "Character/Components/MTD_AscComponent.h"
#include "mtd.h"

#include "MTD_HealthComponent.generated.h"

class UMTD_AbilitySystemComponent;
class UMTD_HealthComponent;
class UMTD_HealthSet;
struct FGameplayEffectSpec;
struct FOnAttributeChangeData;

UENUM(BlueprintType)
enum class EMTD_DeathState : uint8
{
    NotDead = 0,
    DeathStarted,
    DeathFinished
};

/**
 * Component that handles and notifies about health and death related events, also makes it easier to read related
 * health data.
 */
UCLASS(Blueprintable, ClassGroup="Character", meta=(BlueprintSpawnableComponent))
class MTD_API UMTD_HealthComponent
    : public UMTD_AscComponent
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FDeathEventSignature,
        AActor *, OwningActor);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
        FHealthAttributeChangedSignature,
        UMTD_HealthComponent *, HealthComponent,
        float, OldValue,
        float, NewValue,
        AActor *, Instigator);

public:
    UMTD_HealthComponent();

    /**
     * Find health component on a given actor.
     * @param   Actor: actor to search for health component in.
     * @result  Health component residing on the given actor. Can be nullptr.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Health")
    static UMTD_HealthComponent *FindHealthComponent(const AActor *Actor);

    //~UMTD_AscComponent Interface
    virtual void InitializeWithAbilitySystem(UMTD_AbilitySystemComponent *InAbilitySystemComponent) override;
    virtual void UninitializeFromAbilitySystem() override;
    //~End of UMTD_AscComponent Interface

    /**
     * Get current health amount.
     * @return  Current health amount.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Health Component")
    float GetHealth() const;

    /**
     * Get max health amount.
     * @return  Max health amount.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Health Component")
    float GetMaxHealth() const;

    /**
     * Get health amount in normalized form [0.0, 1.0].
     * @return  Health amount in normalized form [0.0, 1.0].
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Health Component")
    float GetHealthNormilized() const;

    /**
     * Get current death state.
     * @return  Current death state.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Health Component")
    EMTD_DeathState GetDeathState() const;

    /**
     * Check whether the owner is dead or dying.
     * @return  If true, the owner is dead or dying, false otherwise.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure=false, Category="MTD|Health Component",
        meta=(ExpandBoolAsExecs="ReturnValue"))
    bool IsDeadOrDying() const;

    /**
     * Start death logic.
     */
    virtual void StartDeath();
    
    /**
     * End death logic.
     */
    virtual void FinishDeath();

    /**
     * Kill own self.
     * @param   bFeelOutOfWorld: if true, death is due the fact that owner is outside the world, false otherwise.
     */
    virtual void SelfDestruct(bool bFeelOutOfWorld = false);

protected:
    /**
     * Event to fire when health attribute is changed.
     * @param   ChangeData: data about attribute change.
     */
    virtual void OnHealthChanged(const FOnAttributeChangeData &ChangeData);
    
    /**
     * Event to fire when max health attribute is changed.
     * @param   ChangeData: data about attribute change.
     */
    virtual void OnMaxHealthChanged(const FOnAttributeChangeData &ChangeData);

    /**
     * Event to fire when health has reached 0.
     * @param   DamageInstigator: actor started the whole chain.
     * @param   DamageCauser: physical actor tied to the application of the damage.
     * @param   DamageEffectSpec: damage gameplay effect spec.
     * @param   DamageMagnitude: amount of damage dealt.
     */
    virtual void OnOutOfHealth(AActor *DamageInstigator, AActor *DamageCauser,
        const FGameplayEffectSpec &DamageEffectSpec, float DamageMagnitude);

public:
    /** Delegate to fire when death logic has started. */
    UPROPERTY(BlueprintAssignable)
    FDeathEventSignature OnDeathStarted;

    /** Delegate to fire when death logic has finished. */
    UPROPERTY(BlueprintAssignable)
    FDeathEventSignature OnDeathFinished;

    /** Delegate to fire when health attribute has changed. */
    UPROPERTY(BlueprintAssignable)
    FHealthAttributeChangedSignature OnHealthChangedDelegate;

    /** Delegate to fire when max health attribute has changed. */
    UPROPERTY(BlueprintAssignable)
    FHealthAttributeChangedSignature OnMaxHealthChangedDelegate;

private:
    /** Health set defining all health related data to use. */
    UPROPERTY()
    TObjectPtr<const UMTD_HealthSet> HealthSet = nullptr;

    /** Current death state that the owner is in. */
    UPROPERTY()
    EMTD_DeathState DeathState = EMTD_DeathState::NotDead;
};

inline UMTD_HealthComponent *UMTD_HealthComponent::FindHealthComponent(const AActor *Actor)
{
    return ((IsValid(Actor)) ? (Actor->FindComponentByClass<UMTD_HealthComponent>()) : (nullptr));
}

inline EMTD_DeathState UMTD_HealthComponent::GetDeathState() const
{
    return DeathState;
}

inline bool UMTD_HealthComponent::IsDeadOrDying() const
{
    return (DeathState > EMTD_DeathState::NotDead);
}
