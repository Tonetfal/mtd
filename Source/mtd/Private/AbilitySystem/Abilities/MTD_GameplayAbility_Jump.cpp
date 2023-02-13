#include "AbilitySystem/Abilities/MTD_GameplayAbility_Jump.h"

#include "Character/MTD_BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

bool UMTD_GameplayAbility_Jump::CanActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo *ActorInfo,
    const FGameplayTagContainer *SourceTags,
    const FGameplayTagContainer *TargetTags,
    FGameplayTagContainer *OptionalRelevantTags) const
{
    check(ActorInfo);

    // Avoid jumping if character isn't allowed to do so
    const auto Character = CastChecked<AMTD_BaseCharacter>(ActorInfo->AvatarActor);
    if (!Character->GetCharacterMovement()->CanAttemptJump())
    {
        return false;
    }

    return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UMTD_GameplayAbility_Jump::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo *ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData *TriggerEventData)
{
    check(ActorInfo);

    // Get the character
    const auto Character = CastChecked<AMTD_BaseCharacter>(ActorInfo->AvatarActor);

    // Start jumping
    Character->Jump();

    // Listen for landed delegate to end the ability
    auto &Delegate = Character->LandedDelegate;
    if (!Delegate.IsAlreadyBound(this, &ThisClass::OnAnimMontageEnded))
    {
        Delegate.AddDynamic(this, &ThisClass::OnAnimMontageEnded);
    }

    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UMTD_GameplayAbility_Jump::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo *ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UMTD_GameplayAbility_Jump::OnAnimMontageEnded(const FHitResult &Hit)
{
    // End the ability when character lands
    K2_EndAbility();
}
