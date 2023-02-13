#pragma once

#include "AbilitySystemInterface.h"
#include "Character/MTD_GameResultInterface.h"
#include "GameFramework/Actor.h"
#include "mtd.h"

#include "MTD_Core.generated.h"

class UMTD_AbilitySystemComponent;
class UMTD_HealthComponent;
class USphereComponent;

/**
 * Core is a default foes objective in tower defense mode.
 *
 * It represents the main object players have to defend from foes in order to win.
 */
UCLASS()
class MTD_API AMTD_Core 
    : public AActor
    , public IAbilitySystemInterface
    , public IMTD_GameResultInterface
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
        FOnCoreHealthChangedSignature, 
        float, OldHealth, 
        float, NewHealth);
        
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnCoreDestroyedSignature,
        AMTD_Core *, Core);

public:
    AMTD_Core();

protected:
    //~AActor Interface
    virtual void BeginPlay() override;
    //~End of AActor Interface

public:
    /**
     * Get MTD ability system component.
     * @return  MTD ability system component.
     */
    UMTD_AbilitySystemComponent *GetMtdAbilitySystemComponent() const;

    //~IAbilitySystemInterface interface
    virtual UAbilitySystemComponent *GetAbilitySystemComponent() const override;
    //~End IAbilitySystemInterface interface

protected:
    /**
     * Event to fire when a core's health changes.
     * @param   InHealthComponent: core's health component.
     * @param   OldValue: previous health value.
     * @param   NewValue: new health value.
     * @param   InInstigator: logical actor that has started the whole chain.
     */
    UFUNCTION(BlueprintImplementableEvent, Category="MTD|Core")
    void OnCoreHealthChanged(UMTD_HealthComponent *InHealthComponent, float OldValue, float NewValue,
        AActor *InInstigator);
    void OnCoreHealthChanged_Implementation(UMTD_HealthComponent *InHealthComponent, float OldValue, float NewValue,
        AActor *InInstigator);

    /**
     * Event to fire when a core is destroyed.
     * @param   CoreActor: core that has been destroyed.
     */
    UFUNCTION(BlueprintImplementableEvent, Category="MTD|Core")
    void OnCoreDestroyed(AActor *CoreActor);
    void OnCoreDestroyed_Implementation(AActor *CoreActor);

public:
    /** Delegate to fire when core health changes. */
    UPROPERTY(BlueprintCallable, BlueprintAssignable)
    FOnCoreHealthChangedSignature OnCoreHealthChangedDelegate;

    /** Delegate to fire when core is destroyed. */
    UPROPERTY(BlueprintCallable, BlueprintAssignable)
    FOnCoreDestroyedSignature OnCoreDestroyedDelegate;

private:
    /** Core collision sphere. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Core", meta=(AllowPrivateAccess="true"))
    TObjectPtr<USphereComponent> CollisionComponent = nullptr;

    /** Health component to check health and death events. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Core", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_HealthComponent> HealthComponent = nullptr;

    /** Ability system component for health set, death ability, and gameplay cues. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Core", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_AbilitySystemComponent> MtdAbilitySystemComponent = nullptr;
};
