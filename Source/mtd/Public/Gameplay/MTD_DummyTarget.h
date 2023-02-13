#pragma once

#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Actor.h"
#include "mtd.h"

#include "MTD_DummyTarget.generated.h"

class UCapsuleComponent;
class UMTD_AbilitySystemComponent;

/**
 * Dummy target actor for testing purposes, both development and the end result.
 *
 * It cannot die, and it knows how much damage has been dealt to it.
 *
 * Players can attack the dummy target to see own DPS.
 *
 * Developers can use it as a static target to test features on it.
 */
UCLASS()
class MTD_API AMTD_DummyTarget
    : public AActor
    , public IAbilitySystemInterface
    , public IGenericTeamAgentInterface
{
    GENERATED_BODY()

public:
    AMTD_DummyTarget();

    //~AActor Interface
    virtual void PostInitializeComponents() override;
    //~End of AActor Interface

    //~IAbilitySystemInterface Interface
    virtual UAbilitySystemComponent *GetAbilitySystemComponent() const override;
    //~End of IAbilitySystemInterface Interface
    
    //~IGenericTeamAgentInterface Interface
    virtual FGenericTeamId GetGenericTeamId() const override;
    //~End of IGenericTeamAgentInterface Interface

protected:
    /**
     * Event to fire when health decreases.
     * @param   LostHealth: amount of health lost.
     */
    UFUNCTION(BlueprintNativeEvent, DisplayName="OnHealthDecrease")
    void K2_OnHealthDecrease(float LostHealth);

private:
    /**
     * Event to fire when health changes.
     * @param   ChangeData: data about attribute change.
     */
    void OnHealthChanged(const FOnAttributeChangeData &ChangeData);

private:
    /** Ability system component to manage basic gameplay interactions. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_AbilitySystemComponent> AbilitySystemComponent = nullptr;
};
