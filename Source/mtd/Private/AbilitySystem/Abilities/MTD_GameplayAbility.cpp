#include "AbilitySystem/Abilities/MTD_GameplayAbility.h"

#include "AbilitySystem/Effects/MTD_GameplayEffect_Cooldown.h"
#include "AbilitySystem/Effects/MTD_GameplayEffect_Cost.h"
#include "AbilitySystem/MTD_GameplayTags.h"
#include "AbilitySystemComponent.h"
#include "Character/MTD_PawnExtensionComponent.h"
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
    AbilityTags.AddTag(MainAbilityTag);
}

void UMTD_GameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilitySpec &Spec)
{
    Super::OnGiveAbility(ActorInfo, Spec);

    TryActivateAbilityOnSpawn(ActorInfo, Spec);
}

UAnimMontage *UMTD_GameplayAbility::GetRandomAbilityAnimMontage() const
{
    if (!MainAbilityTag.IsValid())
    {
        MTDS_WARN("Main Ability Tag is invalid.");
        return nullptr;
    }
    
    const AActor *AvatarActor = GetActorInfo().AvatarActor.Get();
    const auto PawnExtComponent = UMTD_PawnExtensionComponent::FindPawnExtensionComponent(AvatarActor);
    check(PawnExtComponent);

    UAnimMontage *AnimMontage = PawnExtComponent->GetRandomAnimMontage(MainAbilityTag);
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
    return (CooldownDuration.Value != 0.f) ? (Remaining / CooldownDuration.Value) : (0.f);
}

void UMTD_GameplayAbility::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo *ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData *TriggerEventData)
{
    // Always save given event data
    GameplayEventData = (TriggerEventData) ? (*TriggerEventData) : (FGameplayEventData());
    
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    OnAbilityActivatedDelegate.Broadcast(this);
}

const FGameplayTagContainer *UMTD_GameplayAbility::GetCooldownTags() const
{
    FGameplayTagContainer *MutableTags = const_cast<FGameplayTagContainer *>(&TempCooldownTags);
    MutableTags->Reset();

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
    if (!CooldownTags.IsValid() || CooldownDuration.GetValue() <= 0.f)
    {
        return;
    }

    const UGameplayEffect *CooldownGe = GetCooldownGameplayEffect();
    if (!IsValid(CooldownGe))
    {
        return;
    }

    const float AbilityLevel = GetAbilityLevel();
    FGameplayEffectSpecHandle GeSpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGe->GetClass(), AbilityLevel);

    GeSpecHandle.Data->DynamicGrantedTags.AppendTags(CooldownTags);

    // ReSharper disable once CppExpressionWithoutSideEffects
    ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, GeSpecHandle);

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

    OnAbilityEndedDelegate.Broadcast(this);
}

void UMTD_GameplayAbility::InputPressed(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo *ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo)
{
    OnInputPressedDelegate.Broadcast(this);
}

void UMTD_GameplayAbility::TryActivateAbilityOnSpawn(
    const FGameplayAbilityActorInfo *ActorInfo,
    const FGameplayAbilitySpec &Spec) const
{
    check(ActorInfo);

    UAbilitySystemComponent *Asc = ActorInfo->AbilitySystemComponent.Get();

    if ((Spec.IsActive()) || (ActivationPolicy != EMTD_AbilityActivationPolicy::OnSpawn) || (!IsValid(Asc)))
    {
        return;
    }

    Asc->TryActivateAbility(Spec.Handle);
}
