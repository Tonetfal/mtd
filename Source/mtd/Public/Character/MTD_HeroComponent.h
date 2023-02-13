#pragma once

#include "Character/MTD_PawnComponent.h"
#include "mtd.h"

#include "MTD_HeroComponent.generated.h"

class UMTD_AbilitySystemComponent;
class UMTD_InputConfig;
class UMTD_PawnData;
class UMTD_PlayerData;
struct FGameplayTag;
struct FInputActionValue;

/**
 * Hero Component is responsible for initializing owner pawn. It's the second step in this project custom gameplay
 * initialization.
 *
 * When the Pawn Extension Component notifies us about the fact that the owner pawn is ready to
 * initialize, the Hero Component will carry on initializing the remaining data, such as Ability System Component,
 * gameplay abilities, gameplay effects, attribute sets, and inputs related to our owner.
 *
 * The crutial initialization data resides in Pawn Data and Player Data assets. If owner is not a locally controlled
 * player, then his inputs will not be processed.
 *
 * @see UMTD_PawnExtensionComponent
 */
UCLASS(Blueprintable, ClassGroup="Character", meta=(BlueprintSpawnableComponent))
class MTD_API UMTD_HeroComponent
    : public UMTD_PawnComponent
{
    GENERATED_BODY()

public:
    UMTD_HeroComponent();

    /**
     * Add an additional input config.
     * @param   InputConfig: config containing all actions with their relative input tags.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Input")
    void AddAdditionalInputConfig(const UMTD_InputConfig *InputConfig);

    //~IMTD_ReadyInterface Interface
    virtual bool IsPawnComponentReadyToInitialize() const override;
    //~End of IMTD_ReadyInterface Interface

    /**
     * Get MTD ability system component.
     * @return  MTD ability system component.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Hero Component")
    UMTD_AbilitySystemComponent *GetMtdAbilitySystemComponent() const;

    //~UActorComponent interface
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
    virtual void OnRegister() override;
    //~End of UActorComponent interface

    /**
     * Event to fire when pawn owner is ready to initialize.
     *
     * Initializes ability system component on pawn extension component, as well as dispatches pawn data, such as
     * input configs on locally controlled players, and ability sets on everyone.
     */
    void OnPawnReadyToInitialize();

    /**
     * Initialize player input using input config from player data.
     * @param   InputComponent: input component to bind new inputs on.
     */
    virtual void InitializePlayerInput(UInputComponent *InputComponent);

    /**
     * Event to fire when an input is pressed. Notifies MTD ability system component about the input state in order to
     * interact abilities.
     * @param   InputTag: input tag the input is associated with.
     */
    void Input_AbilityInputTagPressed(FGameplayTag InputTag);
    
    /**
     * Event to fire when an input is released. Notifies MTD ability system component about the input state in order to
     * interact abilities.
     * @param   InputTag: input tag the input is associated with.
     */
    void Input_AbilityInputTagReleased(FGameplayTag InputTag);

    /**
     * Check whether locally controlled player can move.
     * @return  If true, locally controlled player can move, false otherwise.
     */
    UFUNCTION(BlueprintImplementableEvent, Category="MTD|Hero Component|Input", DisplayName="Can Move")
    bool K2_CanMove();

    /**
     * Check whether locally controlled player can rotate camera.
     * @return  If true, locally controlled player can rotate camera, false otherwise.
     */
    UFUNCTION(BlueprintImplementableEvent, Category="MTD|Hero Component|Input", DisplayName="Can Look")
    bool K2_CanLook();

    /**
     * Handle locally controlled player movement inputs. Is intended to be binded as a native action.
     * @param   InputActionValue: data describing input.
     */
    void Input_Move(const FInputActionValue &InputActionValue);
    
    /**
     * Handle locally controlled player camera rotation inputs. Is intended to be binded as a native action.
     * @param   InputActionValue: data describing input.
     */
    void Input_LookMouse(const FInputActionValue &InputActionValue);

private:
    /**
     * Pawn data used to initialize pawns. It allows us to add gameplay abilities, gameplay effects, and attribute sets
     * on the owner pawn.
     */
    UPROPERTY()
    TObjectPtr<const UMTD_PawnData> PawnData = nullptr;

    /**
     * Player data used to initialize players. It allows us to bind inputs to locally controlled players.
     */
    UPROPERTY()
    TObjectPtr<const UMTD_PlayerData> PlayerData = nullptr;

    /** If true, the owner pawn has been initialized, false otherwise. */
    bool bPawnHasInitialized = false;
};
