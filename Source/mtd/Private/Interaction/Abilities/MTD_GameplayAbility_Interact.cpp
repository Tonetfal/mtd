#include "Interaction/Abilities/MTD_GameplayAbility_Interact.h"

#include "Camera/CameraComponent.h"
#include "Character/MTD_BasePlayerCharacter.h"
#include "Interaction/MTD_Interactable.h"

UMTD_GameplayAbility_Interact::UMTD_GameplayAbility_Interact()
{
    InteractionID = IMTD_Interactable::DefaultInteractionID;
}

void UMTD_GameplayAbility_Interact::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo *ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData *TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    FHitResult HitResult;
    PerformTrace(HitResult);
    if (HitResult.bBlockingHit)
    {
        AActor *HitActor = HitResult.GetActor();
        if (HitActor->Implements<UMTD_Interactable>())
        {
            auto PlayerCharacter = Cast<AMTD_BasePlayerCharacter>(GetAvatarActorFromActorInfo());
            if (IsValid(PlayerCharacter))
            {
                // Call the actual interaction function
                const bool bSuccess = IMTD_Interactable::Execute_TryInteract(HitActor, PlayerCharacter, InteractionID);

                // Notify BPs about the interaction
                K2_OnInteraction(HitActor, bSuccess);
            }
        }
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}

bool UMTD_GameplayAbility_Interact::PerformTrace(FHitResult &OutHitResult) const
{
    AActor *AvatarActor = GetAvatarActorFromActorInfo();
    if (!IsValid(AvatarActor))
    {
        MTDS_WARN("Avatar Actor is invalid.");
        return false;
    }
    
    auto PlayerCharacter = Cast<AMTD_BasePlayerCharacter>(AvatarActor);
    if (!IsValid(PlayerCharacter))
    {
        MTDS_WARN("Avatar Actor [%s] is not of type AMTD_BasePlayerCharacter.", *AvatarActor->GetName());
        return false;
    }
    
    const UCameraComponent *CameraComponent = PlayerCharacter->GetCameraComponent();
    if (!IsValid(CameraComponent))
    {
        MTDS_WARN("Camera Component is invalid.");
        return false;
    }
            
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

    OutHitResult = HitResult;
    return bHit;
}
