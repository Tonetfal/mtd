#include "AbilitySystem/Abilities/MTD_GameplayAbility_AttackMelee.h"

#include "AbilitySystem/Attributes/MTD_BalanceSet.h"
#include "AbilitySystem/MTD_GameplayTags.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/MTD_CombatSet.h"

void UMTD_GameplayAbility_AttackMelee::OnHit(const FGameplayEventData &Payload)
{
    const UAbilitySystemComponent *AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
    if (!IsValid(AbilitySystemComponent))
    {
        MTDS_WARN("Ability System Component is invalid.");
        return;
    }

    const int32 Num = Payload.TargetData.Num();
    if (Num == 0)
    {
        MTDS_WARN("Target Data is empty.");
        return;
    }

    FGameplayAbilityTargetData *TargetData = Payload.TargetData.Data[0].Get();
    if (!TargetData)
    {
        MTDS_WARN("Target Data is NULL.");
        return;
    }
    
    const FHitResult *HitResult = TargetData->GetHitResult();
    if (!HitResult)
    {
        MTDS_WARN("Hit Result is NULL.");
        return;
    }
    
    const AActor *TargetActor = Payload.Target;
    if (!IsValid(TargetActor))
    {
        MTDS_WARN("Target is invalid.");
        return;
    }

    // Create a common effect context to use on gameplay effects
    FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
    EffectContext.AddHitResult(*HitResult, true);

    // Create the gameplay effects spec handles
    const FVector KnockbackDirection = ComputeKnockbackDirection(TargetActor);
    const FGameplayEffectSpecHandle &BalanceDamageSpec = GetBalanceDamageSpecHandle(KnockbackDirection, EffectContext);
    const FGameplayEffectSpecHandle &DamageSpec = GetDamageSpecHandle(EffectContext);

    // Apply balance set beforehand because it may play some animations that are less important than death animation
    if (BalanceDamageSpec.Data)
    {
        TargetData->ApplyGameplayEffectSpec(*BalanceDamageSpec.Data);
    }

    // Apply damage spec the last because it may play death animation, which is the most important one
    if (DamageSpec.Data)
    {
        TargetData->ApplyGameplayEffectSpec(*DamageSpec.Data);
    }
}

FGameplayEffectSpecHandle UMTD_GameplayAbility_AttackMelee::GetBalanceDamageSpecHandle(const FVector KnockbackDirection,
    const FGameplayEffectContextHandle &EffectContext) const
{
    FGameplayEffectSpecHandle SpecHandle;
    if (!GameplayEffectBalanceDamageInstantClass)
    {
        MTDS_WARN("Balance Damage Instant Gameplay Effect Class is invalid.");
        return SpecHandle;
    }

    const UAbilitySystemComponent *AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo_Checked();
    SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffectBalanceDamageInstantClass, 1.f, EffectContext);
    const float BalanceDamage = AbilitySystemComponent->GetNumericAttribute(UMTD_BalanceSet::GetDamageAttribute());
    
    const FMTD_GameplayTags &Tags = FMTD_GameplayTags::Get();
    SpecHandle.Data->SetSetByCallerMagnitude(Tags.SetByCaller_KnockbackDirection_X, KnockbackDirection.X);
    SpecHandle.Data->SetSetByCallerMagnitude(Tags.SetByCaller_KnockbackDirection_Y, KnockbackDirection.Y);
    SpecHandle.Data->SetSetByCallerMagnitude(Tags.SetByCaller_KnockbackDirection_Z, KnockbackDirection.Z);
    SpecHandle.Data->SetSetByCallerMagnitude(Tags.SetByCaller_Balance_Damage, BalanceDamage);

    return SpecHandle;
}

FGameplayEffectSpecHandle UMTD_GameplayAbility_AttackMelee::GetDamageSpecHandle(
    const FGameplayEffectContextHandle &EffectContext) const
{
    FGameplayEffectSpecHandle SpecHandle;
    if (!GameplayEffectDamageInstantClass)
    {
        MTDS_WARN("Damage Instant Gameplay Effect Class is invalid.");
        return SpecHandle;
    }

    const UAbilitySystemComponent *AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo_Checked();
    SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffectDamageInstantClass, 1.f, EffectContext);
    const float DamageBase = AbilitySystemComponent->GetNumericAttribute(UMTD_CombatSet::GetDamageBaseAttribute());

    const FMTD_GameplayTags &Tags = FMTD_GameplayTags::Get();
    SpecHandle.Data->SetSetByCallerMagnitude(Tags.SetByCaller_Damage_Base, DamageBase);
    SpecHandle.Data->SetSetByCallerMagnitude(Tags.SetByCaller_Damage_Additive, DamageAdditive);
    SpecHandle.Data->SetSetByCallerMagnitude(Tags.SetByCaller_Damage_Multiplier, DamageMultiplier);

    return SpecHandle;
}

FVector UMTD_GameplayAbility_AttackMelee::ComputeKnockbackDirection(const AActor *TargetActor) const
{
    check(IsValid(TargetActor));
    
    const AActor *AvatarActor = GetAvatarActorFromActorInfo();
    check(IsValid(AvatarActor));

    const FVector A = TargetActor->GetActorLocation();
    const FVector B = AvatarActor->GetActorLocation();
    const FVector Displacement = (A - B);
    const FVector Direction = Displacement.GetSafeNormal();

    return Direction;
}
