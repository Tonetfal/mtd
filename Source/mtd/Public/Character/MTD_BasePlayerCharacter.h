#pragma once

#include "Character/MTD_BaseCharacter.h"
#include "GameplayTagContainer.h"
#include "mtd.h"

#include "MTD_BasePlayerCharacter.generated.h"

class UCameraComponent;
class UMTD_AbilitiesUiData;
class UMTD_InventoryManagerComponent;
class UMTD_LevelComponent;
class UMTD_PlayerData;
class USpringArmComponent;

/**
 * Base player character class used to create specific ones.
 *
 * List of added behaviors:
 * - Ability system component
 * - Health
 * - Mana
 * - Balance
 * - Melee combat
 * - Level
 * - Inventory
 * - Camera
 * - Hero class
 */
UCLASS()
class MTD_API AMTD_BasePlayerCharacter
    : public AMTD_BaseCharacter
{
    GENERATED_BODY()

public:
    AMTD_BasePlayerCharacter();

    /**
     * Get hero classes this character is associated with.
     * @return  Hero classes this character is associated with.
     */
    FGameplayTagContainer GetHeroClasses() const;

    /**
     * Get camera component.
     * @return  Camera component.
     */
    UCameraComponent *GetCameraComponent() const;

    /**
     * Get player data.
     * @return  Player data.
     */
    const UMTD_PlayerData *GetPlayerData() const;

protected:
    //~AActor Interface
    virtual void BeginPlay() override;

public:
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    //~End of AActor Interface

protected:
    //~AMTD_BaseCharacter Interface
    virtual void InitializeAttributes() override;

    virtual void OnDeathStarted_Implementation(AActor *OwningActor) override;
    //~End of AMTD_BaseCharacter Interface
    
    /**
     * Event to fire on level up.
     * @param   LevelComponent: level component level has changed on.
     * @param   OldValue: previous level value.
     * @param   NewValue: new level value.
     * @param   InInstigator: logical instigator that started whole chain.
     */
    virtual void OnLevelUp(UMTD_LevelComponent *LevelComponent, float OldValue, float NewValue, AActor *InInstigator);

    //~IMTD_GameResultInterface Interface
    virtual void OnGameTerminated_Implementation(EMTD_GameResult GameResult) override;
    //~End of IMTD_GameResultInterface Interface
    
private:
    /**
     * Start listening for level up event.
     */
    void ListenForLevelUp();
    
    /**
     * Dispatch input mapping contexts from player data.
     */
    void InitializeInput();

    /**
     * Make inputs be ignored.
     */
    void DisableControllerInput();

    /**
     * Disable movement so that the movement component doesn't do anything.
     */
    void DisableMovement();

    /**
     * Make collision components ignore everything.
     */
    void DisableCollision();

private:
    /** Camera boom to move camera around. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<USpringArmComponent> SpringArmComponent = nullptr;
    
    /** Camera component to allow player to see the world. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UCameraComponent> CameraComponent = nullptr;

    /** Player related data. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Player", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_PlayerData> PlayerData = nullptr;

    /** UI related data to describe abilities. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|UI", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_AbilitiesUiData> UiData = nullptr;
};

inline UCameraComponent *AMTD_BasePlayerCharacter::GetCameraComponent() const
{
    return CameraComponent;
}

inline const UMTD_PlayerData *AMTD_BasePlayerCharacter::GetPlayerData() const
{
    return PlayerData;
}
