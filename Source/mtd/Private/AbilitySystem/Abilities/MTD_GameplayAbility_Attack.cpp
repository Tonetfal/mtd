#include "AbilitySystem/Abilities/MTD_GameplayAbility_Attack.h"

#include "AbilitySystem/Effects/MTD_GameplayEffect_Attack.h"
#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "AbilitySystem/MTD_GameplayTags.h"
#include "Character/MTD_BaseCharacter.h"
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

    CommitExecute(Handle, ActorInfo, ActivationInfo);

    const auto MtdAsc = CastChecked<UMTD_AbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());

    int32 AttackEffectLevel;
    HandleAttackGameplayEffect(MtdAsc, AttackEffectLevel);

    UAnimMontage *AnimMontage = GetRandomAbilityAnimMontage();
    PlayAttackAnimation(CastChecked<AMTD_BaseCharacter>(ActorInfo->AvatarActor.Get()), AnimMontage);

    FTimerHandle EndAbilityTimerHandle;
    const float RemainingCooldownTime = GetCooldownTimeRemaining();
    GetWorld()->GetTimerManager().SetTimer(
        EndAbilityTimerHandle, this, &ThisClass::K2_EndAbility, RemainingCooldownTime);

    SetCanBeCanceled(true);

    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UMTD_GameplayAbility_Attack::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo *ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    check(ActorInfo);

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UMTD_GameplayAbility_Attack::HandleAttackGameplayEffect(
    UMTD_AbilitySystemComponent *MtdAsc, int32 &OutAttackEffectLevel) const
{
    // Check if we have an active attack GE
    const FActiveGameplayEffectHandle AttackHandle = GetFirstActiveAttackGameplayEffect(MtdAsc);

    OutAttackEffectLevel = 0;
    if (!AttackHandle.IsValid())
    {
        CreateAndApplyAttackGameplayEffectOnSelf(MtdAsc);
    }
    else
    {
        // Increase GE level by one and reset its duration
        // ReSharper disable once CppExpressionWithoutSideEffects
        MtdAsc->IncreaseGameplayEffectLevelHandle(AttackHandle, 1.f);
        MtdAsc->SetGameplayEffectDurationHandle(AttackHandle, AttackGeDuration);

        CancelPreviousAttack(MtdAsc);

        const FActiveGameplayEffect *AttackGe = MtdAsc->GetActiveGameplayEffect(AttackHandle);
        OutAttackEffectLevel = static_cast<int32>(AttackGe->Spec.GetLevel());
    }
}

FActiveGameplayEffectHandle UMTD_GameplayAbility_Attack::GetFirstActiveAttackGameplayEffect(
    const UMTD_AbilitySystemComponent *MtdAsc) const
{
    FGameplayEffectQuery GeQuery;
    GeQuery.EffectDefinition = UMTD_GameplayEffect_Attack::StaticClass();
    TArray<FActiveGameplayEffectHandle> ActiveGeHandles = MtdAsc->GetActiveEffects(GeQuery);

    if (ActiveGeHandles.Num() > 1)
    {
        MTDS_WARN("Ability system [%s] has more than one UMTD_GameplayEffect_Attack. "
            "Only the first will be considered.", *MtdAsc->GetName());
    }

    return !ActiveGeHandles.IsEmpty() ? ActiveGeHandles[0] : FActiveGameplayEffectHandle();
}

int32 UMTD_GameplayAbility_Attack::GetAttackMontageIndexToPlay(
    const TArray<UAnimMontage *> Animations, int32 AttackEffectLevel) const
{
    const int32 Size = Animations.Num();
    return (Size != 0) ? (AttackEffectLevel % Size) : (-1);
}

UAnimMontage *UMTD_GameplayAbility_Attack::GetAbilityAnimMontage(
    TArray<UAnimMontage *> Animations, int32 AttackEffectLevel) const
{
    const int32 AttackMontageIndex = GetAttackMontageIndexToPlay(Animations, AttackEffectLevel);
    return (AttackMontageIndex != -1) ? (Animations[AttackMontageIndex]) : (nullptr);
}

void UMTD_GameplayAbility_Attack::CancelPreviousAttack(UMTD_AbilitySystemComponent *MtdAsc) const
{
    FGameplayTagContainer AbilityTypesToCancel;
    AbilityTypesToCancel.AddTag(FMTD_GameplayTags::Get().Gameplay_Ability_Attack_Melee);
    MtdAsc->CancelAbilities(&AbilityTypesToCancel, nullptr, nullptr);
}

void UMTD_GameplayAbility_Attack::CreateAndApplyAttackGameplayEffectOnSelf(UMTD_AbilitySystemComponent *MtdAsc) const
{
    const FGameplayEffectSpecHandle SpecHandle =
        MtdAsc->MakeOutgoingSpec(UMTD_GameplayEffect_Attack::StaticClass(), 1.f, MtdAsc->MakeEffectContext());

    const FActiveGameplayEffectHandle EffectHandle = MtdAsc->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
    MtdAsc->SetGameplayEffectDurationHandle(EffectHandle, AttackGeDuration);
}

void UMTD_GameplayAbility_Attack::PlayAttackAnimation(const ACharacter *PlayOn, UAnimMontage *AbilityAnimMontage)
{
    UAnimInstance *AnimInstance = PlayOn->GetMesh()->GetAnimInstance();
    AnimInstance->Montage_Play(AbilityAnimMontage);
}
