#pragma once

#include "AbilitySystemInterface.h"
#include "Character/MTD_GameResultInterface.h"
#include "GameFramework/Character.h"
#include "mtd.h"

#include "MTD_BaseCharacter.generated.h"

class AMTD_PlayerState;
class UMTD_AbilitySystemComponent;
class UMTD_BalanceComponent;
class UMTD_CombatComponent;
class UMTD_HealthComponent;
class UMTD_HeroComponent;
class UMTD_ManaComponent;
class UMTD_PawnExtensionComponent;

/**
 * Base character class used to create other characters.
 *
 * List of added behaviors:
 * - Ability system component
 * - Health
 * - Mana
 * - Balance
 * - Melee combat
 */
UCLASS()
class MTD_API AMTD_BaseCharacter :
    public ACharacter, 
    public IAbilitySystemInterface,
    public IMTD_GameResultInterface
{
    GENERATED_BODY()

public:
    AMTD_BaseCharacter();

protected:
    virtual void InitializeAttributes();

public:
    //~AActor interface
    virtual void PostInitializeComponents() override;
    
protected:
    virtual void BeginPlay() override;
    //~End of AActor interface

    //~APawn interface
    virtual void NotifyControllerChanged() override;
    //~End of APawn interface

    //~ACharacter interface
    virtual void SetupPlayerInputComponent(UInputComponent *PlayerInputComponent) override;
    //~End of ACharacter interface

protected:
    /**
     * Event to fire on ability system component initialization.
     */
    virtual void OnAbilitySystemInitialized();
    
    /**
     * Event to fire on ability system component uninitialization.
     */
    virtual void OnAbilitySystemUninitialized();

    /**
     * Event to fire on death finished.
     */
    virtual void DestroyDueToDeath();

    /**
     * Uninitialize the character.
     */
    virtual void Uninit();

public:
    /**
     * Get health component.
     * @return  Health component.
     */
    UMTD_HealthComponent *GetHealthComponent() const;
    
    /**
     * Get mana component.
     * @return  Mana component.
     */
    UMTD_ManaComponent *GetManaComponent() const;
    
    /**
     * Get balance component.
     * @return  Balance component.
     */
    UMTD_BalanceComponent *GetBalanceComponent() const;
    
    /**
     * Get combat component.
     * @return  Combat component.
     */
    UMTD_CombatComponent *GetCombatComponent() const;

    /**
     * Get MTD player state.
     * @return  MTD player state.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Character")
    AMTD_PlayerState *GetMtdPlayerState() const;

    /**
     * Get MTD ability system component.
     * @return  MTD ability system component.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Character")
    UMTD_AbilitySystemComponent *GetMtdAbilitySystemComponent() const;

    //~IAbilitySystemInterface interface
    virtual UAbilitySystemComponent *GetAbilitySystemComponent() const override;
    //~End IAbilitySystemInterface interface

    /**
     * Get movement direction angle relatively to camera direction.
     * @return  Movement direction angle relatively to camera direction.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Character")
    float GetMovementDirectionAngle() const;

protected:
    /**
     * Event to fire when our death has started.
     * @param   OwningActor: actor that has started dying.
     */
    UFUNCTION(BlueprintNativeEvent, Category="MTD|Character")
    void OnDeathStarted(AActor *OwningActor);
    virtual void OnDeathStarted_Implementation(AActor *OwningActor);

    /**
     * Event to fire when our death has finished.
     * @param   OwningActor: actor that has finished dying.
     */
    UFUNCTION(BlueprintNativeEvent, Category="MTD|Character")
    void OnDeathFinished(AActor *OwningActor);
    virtual void OnDeathFinished_Implementation(AActor *OwningActor);

    /**
     * Cause self destroy due out of bounds.
     * @param   DamageType: unused.
     */
    virtual void FellOutOfWorld(const UDamageType &DamageType) override;

private:
    /** Component to initialize this actor, and to store some additional data. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_PawnExtensionComponent> PawnExtentionComponent = nullptr;

    /** Component to initialize gameplay ability system of this actor. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_HeroComponent> HeroComponent = nullptr;

protected:
    /** Component to keep track of health and death. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_HealthComponent> HealthComponent = nullptr;

    /** Component to keep track of mana. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_ManaComponent> ManaComponent = nullptr;

    /** Component to keep track of balance. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_BalanceComponent> BalanceComponent = nullptr;
    
    /** Component to activate combat hitboxes. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_CombatComponent> CombatComponent = nullptr;

};

inline UMTD_HealthComponent *AMTD_BaseCharacter::GetHealthComponent() const
{
    return HealthComponent;
}

inline UMTD_ManaComponent *AMTD_BaseCharacter::GetManaComponent() const
{
    return ManaComponent;
}

inline UMTD_BalanceComponent *AMTD_BaseCharacter::GetBalanceComponent() const
{
    return BalanceComponent;
}

inline UMTD_CombatComponent *AMTD_BaseCharacter::GetCombatComponent() const
{
    return CombatComponent;
}
