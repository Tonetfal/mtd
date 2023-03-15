#pragma once

#include "Character/Components/MTD_PawnComponent.h"
#include "mtd.h"

#include "MTD_PawnExtensionComponent.generated.h"

class UMTD_AbilityAnimationSet;
class UMTD_AbilitySystemComponent;
class UMTD_PawnData;
struct FGameplayTag;
struct FMTD_AbilityAnimations;

/**
 * Pawn Extension Component is responsible for initializing owner pawn. It's the first step in this custom project
 * gameplay initialization.
 *
 * It informs about the fact that the owner pawn's components are ready to initialize, as well
 * as notify about the fact that Ability System Component was initialized or uninitialized. It also contains some pawn
 * related data.
 *
 * In order to initialize the Ability System Component, it has to be passed to it from outside. In this project it's
 * meant to be done through Hero Component, which is also responsible for initialization process, but a more specific
 * one. The Ability System Component will only be initialized after the pawn itself is ready to do so.
 *
 * Other components or anything else can listen for the events using these functions:
 * - OnPawnReadyToInitialize_RegisterAndCall
 * - OnAbilitySystemInitialized_RegisterAndCall
 * - OnAbilitySystemUninitialized_Register
 *
 * The pawn may become ready to initialize for different reasons, but they have to be notified about from outside.
 * The main reasons may be:
 * - Pawn data has been initialized (InitializePawnData function)
 * - Controller has been changed (HandleControllerChanged function)
 * - Character's player input component setup has been called (SetupPlayerInputComponent function)
 *
 * @see UMTD_HeroComponent
 */
UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class MTD_API UMTD_PawnExtensionComponent
    : public UMTD_PawnComponent
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynamicMulticastSignature);

public:
    UMTD_PawnExtensionComponent();

protected:
    //~AActor Interface
    virtual void OnRegister() override;
    //~End of AActor Interface

public:
    /**
     * Find pawn extension component on a given actor.
     * @param   Actor: actor to search for pawn extension component in.
     * @result  Pawn extension component residing on the given actor. Can be nullptr.
     */
    UFUNCTION(BlueprintPure, Category="MTD|Pawn")
    static UMTD_PawnExtensionComponent *FindPawnExtensionComponent(const AActor *Actor);

    /**
     * Initialize pawn data residing on this component.
     * @param   InPawnData: pawn data to initialize this component with.
     */
    void InitializePawnData(const UMTD_PawnData *InPawnData);
    
    /**
     * Get typed pawn data.
     * @return  Typed pawn data.
     */
    template <class T>
    const T *GetPawnData() const;

    /**
     * Initialize the ability system component.
     * @param   InAbilitySystemComponent: ability system component to initialize the component with.
     * @param   InOwnerActor: actor that will be the owner the initialize ability system.
     */
    void InitializeAbilitySystem(UMTD_AbilitySystemComponent *InAbilitySystemComponent, AActor *InOwnerActor);
    
    /**
     * Remove the ability system component, and its related data from the component.
     */
    void UninitializeAbilitySystem();

    /**
     * Handle controller change situation. Might insigate ability system component initialization if not already, as
     * well as its uninitialization if required.
     */
    void HandleControllerChanged();
    
    /**
     * Setup player input component. Might insigate ability system component initialization if not already.
     */
    void SetupPlayerInputComponent();

    /**
     * Check if owner pawn is ready to initialize. May run all the check logic to determine the result
     * @return  If true, owner pawn is ready to initialize, false otherwise.
     */
    bool CheckPawnReadyToInitialize();

    /**
     * Check whether owner pawn is ready to initialize. Doesn't run check logic.
     * @return  If true, owner pawn is ready to initialize, false otherwise.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Pawn", meta=(ExpandBoolAsExecs="ReturnValue"))
    bool IsPawnReadyToInitialize() const;

    /**
     * Register a delegate to call when pawn is ready to initialize. Will be immediately fired if it already is.
     * @param   Delegate: delegate to register for the event.
     */
    void OnPawnReadyToInitialize_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);
    
    /**
     * Register a delegate to call when ability system is initialized. Will be immediately fired if it already is.
     * @param   Delegate: delegate to register for the event.
     */
    void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);
    
    /**
     * Register a delegate to call when ability system is uninitialized.
     * @param   Delegate: delegate to register for the event.
     */
    void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate);

    /**
     * Get ability animation montages container for a given gameplay ability tag.
     * @param   AbilityTag: gameplay ability tag to search an animation montage for.
     * @param   OutAbilityAnimations: output parameter. Animation montages container associated with the given gameplay
     * ability tag.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetAbilityAnimMontages(const FGameplayTag &AbilityTag, FMTD_AbilityAnimations &OutAbilityAnimations) const;

    /**
     * Get ability animation montage for a given gameplay ability tag.
     * @param   AbilityTag: gameplay ability tag to search an animation montage for.
     * @return  Animation montage associated with the given gameplay ability tag to play.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure)
    const UAnimMontage *GetRandomAnimMontage(const FGameplayTag &AbilityTag) const;
    
    /**
     * Shorthander for MTD ability system component getter.
     * @return  MTD ability system component. May be nullptr.
     */
    UFUNCTION(BlueprintPure, Category = "MTD|Pawn")
    UMTD_AbilitySystemComponent *GetMtdAbilitySystemComponent() const;

protected:
    /** Delegate to fire when pawn is ready to initialize. */
    FSimpleMulticastDelegate OnPawnReadyToInitialize;

    /** Delegate to fire when pawn is ready to initialize. It's fired after C++ version. */
    UPROPERTY(BlueprintAssignable, meta=(DisplayName="On Pawn Ready To Initialize"))
    FDynamicMulticastSignature BP_OnPawnReadyToInitialize;

    /** Delegate to fire when ability system is initialized. */
    FSimpleMulticastDelegate OnAbilitySystemInitialized;
    
    /** Delegate to fire when ability system is uninitialized. */
    FSimpleMulticastDelegate OnAbilitySystemUninitialized;

private:
    /** Ability system component this component is initialized with. */
    UPROPERTY()
    TObjectPtr<UMTD_AbilitySystemComponent> AbilitySystemComponent = nullptr;

    /** Pawn data used by the owner. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MTD|Pawn", meta=(AllowPrivateAccess="true"))
    TObjectPtr<const UMTD_PawnData> PawnData = nullptr;

    /** Animation sets associated with MTD abilities main gameplay tags. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MTD|Pawn", meta=(AllowPrivateAccess="true"))
    TObjectPtr<const UMTD_AbilityAnimationSet> AnimationSet = nullptr;

    /** If true, pawn is ready to initialize, false otherwise. */
    bool bPawnReadyToInitialize = false;
};

inline UMTD_PawnExtensionComponent *UMTD_PawnExtensionComponent::FindPawnExtensionComponent(const AActor *Actor)
{
    return ((IsValid(Actor)) ? (Actor->FindComponentByClass<UMTD_PawnExtensionComponent>()) : (nullptr));
}

inline bool UMTD_PawnExtensionComponent::IsPawnReadyToInitialize() const
{
    return bPawnReadyToInitialize;
}

template <class T>
inline const T *UMTD_PawnExtensionComponent::GetPawnData() const
{
    return Cast<T>(PawnData);
}

inline UMTD_AbilitySystemComponent *UMTD_PawnExtensionComponent::GetMtdAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}
