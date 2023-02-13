#pragma once

#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerState.h"
#include "mtd.h"

#include "MTD_PlayerState.generated.h"

class AMTD_BasePlayerCharacter;
class AMTD_PlayerController;
class UGameplayAbility;
class UMTD_AbilitySystemComponent;
class UMTD_EquipmentManagerComponent;
class UMTD_InventoryManagerComponent;
class UMTD_LevelComponent;

/**
 * Default player state used in this project.
 *
 * Manages components that have to persist throughout the whole game. If managed on the character himself, they would
 * be destroyed as soon as he does. The components are:
 * - Ability system
 * - Equipment system
 * - Inventory system
 */
UCLASS()
class MTD_API AMTD_PlayerState
    : public APlayerState
    , public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    DECLARE_MULTICAST_DELEGATE_OneParam(
        FOnHeroClassesSetSignature,
        const FGameplayTagContainer & /* HeroClasses */);

public:
    AMTD_PlayerState();

    /**
     * Get MTD player controller.
     * @return  MTD player controller.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Player State")
    AMTD_PlayerController *GetMtdPlayerController() const;
    
    /**
     * Get MTD player character.
     * @return  MTD player character.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Player State")
    AMTD_BasePlayerCharacter *GetMtdPlayerCharacter() const;

    /**
     * Get MTD ability system component.
     * @return  MTD ability system component.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Player State")
    UMTD_AbilitySystemComponent *GetMtdAbilitySystemComponent() const;

    //~IAbilitySystemInterface Interface
    virtual UAbilitySystemComponent *GetAbilitySystemComponent() const override;
    //~End of IAbilitySystemInterface Interface

    /**
     * Get MTD equipment manager component.
     * @return  MTD equipment manager component.
     */
    UMTD_EquipmentManagerComponent *GetEquipmentManagerComponent() const;
    
    /**
     * Get MTD inventory manager component.
     * @return  MTD inventory manager component.
     */
    UMTD_InventoryManagerComponent *GetInventoryManagerComponent() const;
    
    /**
     * Get MTD level component.
     * @return  MTD level component.
     */
    UMTD_LevelComponent *GetLevelComponent() const;

    //~AActor interface
    virtual void PostInitializeComponents() override;
    //~End of AActor interface

    /**
     * Set hero classes that player character is associated with.
     * @param   InHeroClasses: hero classes container.
     */
    void SetHeroClasses(const FGameplayTagContainer &InHeroClasses);

    /**
     * Get hero classes that player character is associated with.
     * @return  Hero classes that player character is associated with.
     */
    const FGameplayTagContainer &GetHeroClasses() const;

    /**
     * Clear all hero classes that player character is associated with.
     */
    void ClearHeroClasses();
    
private:
    /**
     * Event to fire when controller is possessed.
     * @param   PlayerCharacter: player character that possessed the controller.
     */
    UFUNCTION()
    void OnControllerPossessed(AMTD_BasePlayerCharacter *PlayerCharacter);

    /**
     * Event to fire when ability system is initialized.
     */
    void OnAbilitySystemInitialized();

public:
    /** Delegate to fire when hero classes are set. */
    FOnHeroClassesSetSignature OnHeroClassesSetDelegate;

protected:
    /** Our ability system component, which will be used to initialize character's components. */
    UPROPERTY(VisibleAnywhere, Category="MTD|Components")
    TObjectPtr<UMTD_AbilitySystemComponent> AbilitySystemComponent = nullptr;

    /** Equipment that character is using. Have it on PS so that its granted stats don't go away once player dies. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_EquipmentManagerComponent> EquipmentManagerComponent = nullptr;
    
    /** Inventory that character is has. Have it on PS so that it doesn't get destroyed after character dies. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_InventoryManagerComponent> InventoryManagerComponent = nullptr;

    /**
     * Level component to keep track of character level. Have it on PS so that it doesn't get destroyed after character
     * dies.
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_LevelComponent> LevelComponent = nullptr;

    /** Hero classes character is associated with. */
    UPROPERTY()
    FGameplayTagContainer HeroClasses;
};

inline UMTD_AbilitySystemComponent *AMTD_PlayerState::GetMtdAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

inline UMTD_EquipmentManagerComponent *AMTD_PlayerState::GetEquipmentManagerComponent() const
{
    return EquipmentManagerComponent;
}

inline UMTD_InventoryManagerComponent *AMTD_PlayerState::GetInventoryManagerComponent() const
{
    return InventoryManagerComponent;
}

inline UMTD_LevelComponent *AMTD_PlayerState::GetLevelComponent() const
{
    return LevelComponent;
}

inline void AMTD_PlayerState::SetHeroClasses(const FGameplayTagContainer &InHeroClasses)
{
    HeroClasses = InHeroClasses;

    // Notify about hero classes set
    OnHeroClassesSetDelegate.Broadcast(HeroClasses);
}

inline const FGameplayTagContainer &AMTD_PlayerState::GetHeroClasses() const
{
    return HeroClasses;
}

inline void AMTD_PlayerState::ClearHeroClasses()
{
    HeroClasses = FGameplayTagContainer::EmptyContainer;
}
