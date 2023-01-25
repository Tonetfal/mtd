#pragma once

#include "AbilitySystem/Abilities/MTD_GameplayAbility.h"
#include "mtd.h"

#include "MTD_GameplayAbility_Interact.generated.h"

class AMTD_BaseCharacter;

UCLASS()
class MTD_API UMTD_GameplayAbility_Interact
    : public UMTD_GameplayAbility
{
    GENERATED_BODY()
    
public:
    UMTD_GameplayAbility_Interact();
    
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo *ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData *TriggerEventData) override;

    UFUNCTION(BlueprintImplementableEvent, DisplayName="OnInteraction")
    void K2_OnInteraction(AActor *InteractActor, bool bSuccess);

private:
    bool PerformTrace(FHitResult &OutHitResult) const;

private:
    UPROPERTY(EditDefaultsOnly, Category="MTD|Interaction")
    FName InteractionID;
    
    UPROPERTY(EditDefaultsOnly)
    TEnumAsByte<ECollisionChannel> TraceCollisionChannel = ECC_Visibility;
    
    UPROPERTY(EditDefaultsOnly, meta=(ClampMin="1.0"))
    float TraceLength = 1000.f;
    
    UPROPERTY(EditDefaultsOnly)
    bool bTraceComplex = true;

    UPROPERTY(EditDefaultsOnly)
    bool bDebugTrace = false;
};
