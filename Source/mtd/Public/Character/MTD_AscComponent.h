#pragma once

#include "Character/MTD_PawnComponent.h"
#include "mtd.h"

#include "MTD_AscComponent.generated.h"

class UMTD_AbilitySystemComponent;

/**
 * Pawn component that makes use of ability system component. Handles the base ability system component initialization
 * process.
 */
UCLASS()
class MTD_API UMTD_AscComponent
    : public UMTD_PawnComponent
{
    GENERATED_BODY()

public:
    UMTD_AscComponent();
    
    /**
     * Initialize the component with ability system component.
     * @param   InAbilitySystemComponent: ability system component to initialize the component with.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Balance")
    virtual void InitializeWithAbilitySystem(UMTD_AbilitySystemComponent *InAbilitySystemComponent);

    /**
     * Remove the ability system component, and its related data from the component.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Balance")
    virtual void UninitializeFromAbilitySystem();

protected:
    //~UActorComponent Interface
    virtual void OnUnregister() override;
    //~End of UActorComponent Interface

protected:
    /** Ability system component this component is initialized with. */
    UPROPERTY()
    TObjectPtr<UMTD_AbilitySystemComponent> AbilitySystemComponent = nullptr;
};
