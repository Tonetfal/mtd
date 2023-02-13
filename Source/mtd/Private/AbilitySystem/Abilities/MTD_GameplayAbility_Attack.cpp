#include "AbilitySystem/Abilities/MTD_GameplayAbility_Attack.h"

#include "AbilitySystem/Effects/MTD_GameplayEffect_Attack.h"
#include "AbilitySystem/MTD_AbilityAnimationSet.h"
#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "AbilitySystem/MTD_GameplayTags.h"
#include "Character/MTD_BaseCharacter.h"
#include "Character/MTD_PawnExtensionComponent.h"
#include "GameplayTagsManager.h"

UMTD_GameplayAbility_Attack::UMTD_GameplayAbility_Attack()
{
    UGameplayTagsManager::Get().CallOrRegister_OnDoneAddingNativeTagsDelegate(
        FSimpleDelegate::CreateUObject(this, &ThisClass::OnDoneAddingNativeTags));
}

void UMTD_GameplayAbility_Attack::OnDoneAddingNativeTags()
{
    MainAbilityTag = FMTD_GameplayTags::Get().Gameplay_Ability_Attack_Melee;
    Super::OnDoneAddingNativeTags();
}

bool UMTD_GameplayAbility_Attack::CanActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo *ActorInfo,
    const FGameplayTagContainer *SourceTags,
    const FGameplayTagContainer *TargetTags,
    FGameplayTagContainer *OptionalRelevantTags) const
{
    return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UMTD_GameplayAbility_Attack::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo *ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData *TriggerEventData)
{
    check(ActorInfo);

    // Apply cooldown
    CommitExecute(Handle, ActorInfo, ActivationInfo);

    // Get MTD ASC
    const auto MtdAsc = CastChecked<UMTD_AbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());
    const auto Character = CastChecked<AMTD_BaseCharacter>(ActorInfo->AvatarActor.Get());

    // Spawn attack GE, or keep increasing its level
    int32 AttackEffectLevel;
    HandleAttackGameplayEffect(MtdAsc, AttackEffectLevel);

    // Get attack animation. Attack GE level is used as an index
    const auto ExtenstionComponent = UMTD_PawnExtensionComponent::FindPawnExtensionComponent(Character);
    FMTD_AbilityAnimations AttackAnimations;
    ExtenstionComponent->GetAbilityAnimMontages(MainAbilityTag, AttackAnimations);
    UAnimMontage *AnimMontage = GetAbilityAnimMontage(AttackAnimations.Animations, AttackEffectLevel);
    check(IsValid(AnimMontage));
    
    // Play attack animation
    PlayAttackAnimation(Character, AnimMontage);

    // Keep ability active until a new one can be activated. It's needed to allow WaitEvent node in BPs actually wait
    FTimerHandle EndAbilityTimerHandle;
    const float RemainingCooldownTime = GetCooldownTimeRemaining();
    GetWorld()->GetTimerManager().SetTimer(
        EndAbilityTimerHandle, this, &ThisClass::K2_EndAbility, RemainingCooldownTime);

    // Allow to cancel the ability
    SetCanBeCanceled(true);

    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UMTD_GameplayAbility_Attack::HandleAttackGameplayEffect(
    UMTD_AbilitySystemComponent *MtdAsc, int32 &OutAttackEffectLevel) const
{
    check(IsValid(MtdAsc));
    
    // Check if we have an active attack GE
    const FActiveGameplayEffectHandle AttackHandle = GetFirstActiveAttackGameplayEffect(MtdAsc);

    // This is the first attack in the serie
    if (!AttackHandle.IsValid())
    {
        // Create a new one if there is none
        CreateAndApplyAttackGameplayEffectOnSelf(MtdAsc);
        OutAttackEffectLevel = 0;
    }

    // This attack is a follower
    else
    {
        // Increase GE level by one and reset its duration
        // ReSharper disable once CppExpressionWithoutSideEffects
        MtdAsc->ChangeGameplayEffectLevelHandle(AttackHandle, 1);
        MtdAsc->SetGameplayEffectDurationHandle(AttackHandle, AttackGeDuration);

        // Save the attack effect level to return
        const FActiveGameplayEffect *AttackGe = MtdAsc->GetActiveGameplayEffect(AttackHandle);
        OutAttackEffectLevel = static_cast<int32>(AttackGe->Spec.GetLevel());
    }
}

FActiveGameplayEffectHandle UMTD_GameplayAbility_Attack::GetFirstActiveAttackGameplayEffect(
    const UMTD_AbilitySystemComponent *MtdAsc) const
{
    check(IsValid(MtdAsc));
    
    // Find all attack GEs
    FGameplayEffectQuery GeQuery;
    GeQuery.EffectDefinition = UMTD_GameplayEffect_Attack::StaticClass();
    TArray<FActiveGameplayEffectHandle> ActiveGeHandles = MtdAsc->GetActiveEffects(GeQuery);

    // Only one should be active
    const int32 Num = ActiveGeHandles.Num();
    if (Num > 1)
    {
        MTDS_WARN("Ability system [%s] has more than one UMTD_GameplayEffect_Attack. "
            "Only the first will be considered.", *MtdAsc->GetName());
    }

    return ((!ActiveGeHandles.IsEmpty()) ? (ActiveGeHandles[0]) : (FActiveGameplayEffectHandle()));
}

void UMTD_GameplayAbility_Attack::CreateAndApplyAttackGameplayEffectOnSelf(UMTD_AbilitySystemComponent *MtdAsc) const
{
    check(IsValid(MtdAsc));
    
    // Create the GE spec
    const FGameplayEffectSpecHandle SpecHandle =
        MtdAsc->MakeOutgoingSpec(UMTD_GameplayEffect_Attack::StaticClass(), 1.f, MtdAsc->MakeEffectContext());

    // Change its duration
    SpecHandle.Data->Duration = AttackGeDuration;

    // Apply it on self
    MtdAsc->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
}

int32 UMTD_GameplayAbility_Attack::GetAttackMontageIndexToPlay(
    const TArray<UAnimMontage *> Animations, int32 AttackEffectLevel) const
{
    const int32 Num = Animations.Num();
    return ((Num != 0) ? (AttackEffectLevel % Num) : (-1));
}

UAnimMontage *UMTD_GameplayAbility_Attack::GetAbilityAnimMontage(
    TArray<UAnimMontage *> Animations, int32 AttackEffectLevel) const
{
    const int32 AttackMontageIndex = GetAttackMontageIndexToPlay(Animations, AttackEffectLevel);
    return ((AttackMontageIndex != -1) ? (Animations[AttackMontageIndex]) : (nullptr));
}

void UMTD_GameplayAbility_Attack::PlayAttackAnimation(const ACharacter *Character, UAnimMontage *AbilityAnimMontage)
{
    check(IsValid(Character));
    check(IsValid(AbilityAnimMontage));
    
    UAnimInstance *AnimInstance = Character->GetMesh()->GetAnimInstance();
    check(IsValid(AnimInstance));

    AnimInstance->Montage_Play(AbilityAnimMontage);
}
