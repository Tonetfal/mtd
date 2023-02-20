#pragma once

#include "AbilitySystem/Abilities/MTD_GameplayAbility.h"
#include "mtd.h"

#include "MTD_GameplayAbility_Interact.generated.h"

class AMTD_BaseCharacter;

/**
 * Interaction ability that is intended to be used with MTD_Interactable objects.
 */
UCLASS()
class MTD_API UMTD_GameplayAbility_Interact
    : public UMTD_GameplayAbility
{
    GENERATED_BODY()
    
public:
    UMTD_GameplayAbility_Interact();

    //~UMTD_GameplayAbility Interface
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo *ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData *TriggerEventData) override;
    //~End of UMTD_GameplayAbility Interface

    /**
     * Event to fire when an interaction happens.
     * @param   InteractActor: actor that is trying to interact with us.
     * @param   bSuccess: if true, the interaction has been successful, false otherwise.
     */
    UFUNCTION(BlueprintImplementableEvent, DisplayName="OnInteraction")
    void K2_OnInteraction(AActor *InteractActor, bool bSuccess);

private:
    /**
     * Check for interactable actor in front of camera.
     * @param   OutHitResult: output parameter. Hit result.
     * @return  If true, there is an interaction actor, false otherwise.
     */
    bool PerformTrace(FHitResult &OutHitResult) const;

private:
    /** Interaction ID to use when interacting. */
    UPROPERTY(EditDefaultsOnly, Category="MTD|Interaction")
    FName InteractionID;

    /** Trace channel to use when tracing for interactable objects. */
    UPROPERTY(EditDefaultsOnly)
    TEnumAsByte<ECollisionChannel> TraceCollisionChannel = ECC_Visibility;

    /** Length of the trace used to search for interactable objects. */
    UPROPERTY(EditDefaultsOnly, meta=(ClampMin="1.0"))
    float TraceLength = 1000.f;

    /** Use complex trace? */
    UPROPERTY(EditDefaultsOnly)
    bool bTraceComplex = true;

    /** Draw traces? */
    UPROPERTY(EditDefaultsOnly)
    bool bDebugTrace = false;
};
