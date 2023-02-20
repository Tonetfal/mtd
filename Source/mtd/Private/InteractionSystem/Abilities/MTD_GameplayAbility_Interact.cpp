#include "InteractionSystem/Abilities/MTD_GameplayAbility_Interact.h"

#include "Camera/CameraComponent.h"
#include "Character/MTD_BasePlayerCharacter.h"
#include "InteractionSystem/MTD_Interactable.h"

UMTD_GameplayAbility_Interact::UMTD_GameplayAbility_Interact()
{
    // Use default interaction ID by default
    InteractionID = IMTD_Interactable::DefaultInteractionID;
}

void UMTD_GameplayAbility_Interact::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo *ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData *TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    // Search for interactable objects
    FHitResult HitResult;
    PerformTrace(HitResult);
    
    if (HitResult.bBlockingHit)
    {
        // Check whether the hit actor is an interactable actor
        AActor *HitActor = HitResult.GetActor();
        if (HitActor->Implements<UMTD_Interactable>())
        {
            auto PlayerCharacter = CastChecked<AMTD_BasePlayerCharacter>(GetAvatarActorFromActorInfo());
            
            // Call the actual interaction function
            const bool bSuccess = IMTD_Interactable::Execute_TryInteract(HitActor, PlayerCharacter, InteractionID);

            // Notify blueprints about the interaction
            K2_OnInteraction(HitActor, bSuccess);
        }
    }

    // End ability regardless
    EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}

bool UMTD_GameplayAbility_Interact::PerformTrace(FHitResult &OutHitResult) const
{
    AActor *AvatarActor = GetAvatarActorFromActorInfo();
    if (!IsValid(AvatarActor))
    {
        MTDS_WARN("Avatar actor is invalid.");
        return false;
    }
    
    auto PlayerCharacter = Cast<AMTD_BasePlayerCharacter>(AvatarActor);
    if (!IsValid(PlayerCharacter))
    {
        MTDS_WARN("Failed to cast [%s] to MTD base player character.", *AvatarActor->GetName());
        return false;
    }
    
    const UCameraComponent *CameraComponent = PlayerCharacter->GetCameraComponent();
    if (!IsValid(CameraComponent))
    {
        MTDS_WARN("Camera component is invalid.");
        return false;
    }

    // Prepare trace data
    const FVector Forward = CameraComponent->GetForwardVector();
    const FVector TraceStart = CameraComponent->GetComponentLocation();
    const FVector TraceEnd = (TraceStart + (Forward * TraceLength));

    const UWorld *World = GetWorld();
    FCollisionQueryParams QueryParams;
    QueryParams.bTraceComplex = bTraceComplex;
    QueryParams.bDebugQuery = bDebugTrace;
    QueryParams.AddIgnoredActor(AvatarActor);

    FHitResult HitResult;
    const bool bHit = World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, TraceCollisionChannel, 
        QueryParams);

    // Output hit result
    OutHitResult = HitResult;
    return bHit;
}
