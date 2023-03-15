#include "AbilitySystem/Abilities/MTD_GameplayAbility.h"

#include "AbilitySystem/Effects/MTD_GameplayEffect_Cooldown.h"
#include "AbilitySystem/Effects/MTD_GameplayEffect_Cost.h"
#include "AbilitySystem/MTD_GameplayTags.h"
#include "AbilitySystemComponent.h"
#include "Character/Components/MTD_PawnExtensionComponent.h"
#include "GameplayTagsManager.h"

UMTD_GameplayAbility::UMTD_GameplayAbility()
{
    CostGameplayEffectClass = UMTD_GameplayEffect_Cost::StaticClass();
    CooldownGameplayEffectClass = UMTD_GameplayEffect_Cooldown::StaticClass();

    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    
    UGameplayTagsManager::Get().CallOrRegister_OnDoneAddingNativeTagsDelegate(
        FSimpleDelegate::CreateUObject(this, &ThisClass::OnDoneAddingNativeTags));
}

void UMTD_GameplayAbility::OnDoneAddingNativeTags()
{
    // Add the main ability tag to ability tag container
    AbilityTags.AddTag(MainAbilityTag);
}

void UMTD_GameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilitySpec &Spec)
{
    Super::OnGiveAbility(ActorInfo, Spec);

    // Handle OnSpawn activation policy
    TryActivateAbilityOnSpawn(ActorInfo, Spec);
}

const UAnimMontage *UMTD_GameplayAbility::GetRandomAbilityAnimMontage() const
{
    if (!MainAbilityTag.IsValid())
    {
        MTDS_WARN("Main Ability Tag is invalid.");
        return nullptr;
    }

    // Pawn extension component contains the animations a pawn can play for a certain ability tag
    const AActor *AvatarActor = GetActorInfo().AvatarActor.Get();
    const auto PawnExtComponent = UMTD_PawnExtensionComponent::FindPawnExtensionComponent(AvatarActor);
    check(IsValid(PawnExtComponent));

    // Get the animation montage and check for validness
    const UAnimMontage *AnimMontage = PawnExtComponent->GetRandomAnimMontage(MainAbilityTag);
    if (!IsValid(AnimMontage))
    {
        MTDS_WARN("Owner [%s] has no Anim Montage to play with Gameplay Tag [%s].", *MainAbilityTag.ToString());
        return nullptr;
    }

    return AnimMontage;
}

void UMTD_GameplayAbility::GetOwnedGameplayTags(FGameplayTagContainer &TagContainer) const
{
    TagContainer = AbilityTags;
}

float UMTD_GameplayAbility::GetCooldownNormilized() const
{
    const float Remaining = GetCooldownTimeRemaining();

    // Avoid dividing by 0
    return ((CooldownDuration.Value != 0.f) ? (Remaining / CooldownDuration.Value) : (0.f));
}

void UMTD_GameplayAbility::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo *ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData *TriggerEventData)
{
    // Always save passed event data
    GameplayEventData = ((TriggerEventData) ? (*TriggerEventData) : (FGameplayEventData()));
    
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    // Notify about ability activation
    OnAbilityActivatedDelegate.Broadcast(this);
}

const FGameplayTagContainer *UMTD_GameplayAbility::GetCooldownTags() const
{
    auto MutableTags = const_cast<FGameplayTagContainer *>(&TempCooldownTags);
    MutableTags->Reset();

    // Add our cooldown tags to tags that are currently on cooldown
    const FGameplayTagContainer *ParentTags = Super::GetCooldownTags();
    if (ParentTags)
    {
        MutableTags->AppendTags(*ParentTags);
    }
    MutableTags->AppendTags(CooldownTags);

    return MutableTags;
}

void UMTD_GameplayAbility::ApplyCooldown(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo *ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo) const
{
    // Avoid applying cooldown time if cooldown is a non-positive value
    if (CooldownDuration.GetValue() <= 0.f)
    {
        return;
    }

    if (!CooldownTags.IsValid())
    {
        // Notify about invalid cooldown tags only if cooldown duration is a positive value
        MTDS_WARN("Cooldown tags are invalid.");
        return;
    }
    
    // Get GE class used to define a cooldown
    const UGameplayEffect *CooldownGe = GetCooldownGameplayEffect();
    if (!IsValid(CooldownGe))
    {
        return;
    }

    // Create GE to apply the cooldown
    const float AbilityLevel = GetAbilityLevel();
    FGameplayEffectSpecHandle GeSpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGe->GetClass(), AbilityLevel);
    GeSpecHandle.Data->DynamicGrantedTags.AppendTags(CooldownTags);

    // Apply cooldown
    // ReSharper disable once CppExpressionWithoutSideEffects
    ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, GeSpecHandle);

    // Notify about ability cooldown application
    OnApplyCooldownDelegate.Broadcast(this, CooldownDuration.GetValueAtLevel(AbilityLevel));
}

void UMTD_GameplayAbility::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo *ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

    // Notify about ability end
    OnAbilityEndedDelegate.Broadcast(this);
}

void UMTD_GameplayAbility::InputPressed(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo *ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo)
{
    // Notify about ability input press
    OnInputPressedDelegate.Broadcast(this);
}

void UMTD_GameplayAbility::TryActivateAbilityOnSpawn(
    const FGameplayAbilityActorInfo *ActorInfo,
    const FGameplayAbilitySpec &Spec) const
{
    // Check if activation policy supposes to activate the ability right now, and check if it is already active or not
    if (((ActivationPolicy != EMTD_AbilityActivationPolicy::OnSpawn) || (Spec.IsActive())))
    {
        return;
    }

    check(ActorInfo);
    UAbilitySystemComponent *Asc = ActorInfo->AbilitySystemComponent.Get();
    check(IsValid(Asc));

    // Activate ability on spawn
    Asc->TryActivateAbility(Spec.Handle);
}
