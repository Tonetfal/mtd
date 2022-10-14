#include "AbilitySystem/Abilities/MTD_GameplayAbility_Attack.h"

#include "AbilitySystem/Effects/MTD_GameplayEffect_Attack.h"
#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "AbilitySystem/MTD_GameplayTags.h"
#include "Character/MTD_BaseCharacter.h"
#include "GameplayTagsManager.h"

UMTD_GameplayAbility_Attack::UMTD_GameplayAbility_Attack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	AbilityTags.AddTag(FMTD_GameplayTags::Get().GameplayAbility_AttackMelee);
}

bool UMTD_GameplayAbility_Attack::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo *ActorInfo,
	const FGameplayTagContainer *SourceTags,
	const FGameplayTagContainer *TargetTags,
	FGameplayTagContainer *OptionalRelevantTags) const
{
	if (AttackMontage.IsEmpty())
	{
		MTDS_WARN("No attack montage to play!");
		return false;
	}

	return Super::CanActivateAbility(
		Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UMTD_GameplayAbility_Attack::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo *ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, 
	const FGameplayEventData *TriggerEventData)
{
	check(ActorInfo);
	
	const auto MtdAsc = CastChecked<UMTD_AbilitySystemComponent>(
		ActorInfo->AbilitySystemComponent.Get());
	
	FGameplayEffectQuery GeQuery;
	GeQuery.EffectDefinition = UMTD_GameplayEffect_Attack::StaticClass();
	TArray<FActiveGameplayEffectHandle> ActiveGeHandles = 
		MtdAsc->GetActiveEffects(GeQuery);

	if (ActiveGeHandles.Num() > 1)
	{
		MTDS_WARN("Ability system [%s] has more than one "
			"UMTD_GameplayEffect_Attack. Only the first will be considered",
			*GetNameSafe(MtdAsc));
	}

	int32 AttackMontageIndex = 0;
	if (!ActiveGeHandles.IsEmpty())
	{
		const FActiveGameplayEffectHandle AttackHandle = ActiveGeHandles[0];
		const FActiveGameplayEffect *AttackGe =
			MtdAsc->GetActiveGameplayEffect(AttackHandle);
		
		// Select a valid attack montage index
		const int32 Level = static_cast<int32>(AttackGe->Spec.GetLevel());
		AttackMontageIndex = Level % AttackMontage.Num();

		// Increase GE level and reset its duration
		MtdAsc->IncreaseGameplayEffectLevelHandle(AttackHandle, 1.f);
		MtdAsc->SetGameplayEffectDurationHandle(AttackHandle, Duration);
		
		// Cancel previous attack
		FGameplayTagContainer AbilityTypesToCancel;
		AbilityTypesToCancel.AddTag(
			FMTD_GameplayTags::Get().GameplayAbility_AttackMelee);
		MtdAsc->CancelAbilities(&AbilityTypesToCancel, nullptr, this);
	}
	else
	{
		// Create and apply on self gameplay effect
		const FGameplayEffectSpecHandle SpecHandle = MtdAsc->MakeOutgoingSpec(
			UMTD_GameplayEffect_Attack::StaticClass(),
			1.f,
			MtdAsc->MakeEffectContext());

		const FActiveGameplayEffectHandle EffectHandle =
			MtdAsc->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
		MtdAsc->SetGameplayEffectDurationHandle(EffectHandle, Duration);
	}
	
	// Activate attack animation
	auto Actor =
		CastChecked<AMTD_BaseCharacter>(ActorInfo->AvatarActor.Get());
	UAnimInstance *AnimInstance = Actor->GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(AttackMontage[AttackMontageIndex]);

	// Call end ability on montage ended
	// FOnMontageEnded OnMontageEnded;
	// OnMontageEnded.BindUObject(this, &ThisClass::OnAttackMontageEnded);
	// AnimInstance->Montage_SetEndDelegate(OnMontageEnded);
	K2_CancelAbility();

	// Note: Wait Gameplay Event and Apply GE To Target is in BP
	
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
	
	const auto MtdAsc = CastChecked<UMTD_AbilitySystemComponent>(
		ActorInfo->AbilitySystemComponent.Get());

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility,
		bWasCancelled);
}

void UMTD_GameplayAbility_Attack::OnAttackMontageEnded(
	UAnimMontage*AnimMontage,bool bInterrupted)
{
	K2_EndAbility();
}
