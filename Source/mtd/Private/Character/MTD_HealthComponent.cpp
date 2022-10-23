#include "Character/MTD_HealthComponent.h"

#include "AbilitySystem/Attributes/MTD_HealthSet.h"
#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "AbilitySystem/MTD_GameplayTags.h"
#include "GameplayEffectExtension.h"

UMTD_HealthComponent::UMTD_HealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UMTD_HealthComponent::InitializeWithAbilitySystem(
	UMTD_AbilitySystemComponent *Asc)
{
	const AActor *Owner = GetOwner();
	check(Owner);

	if (AbilitySystemComponent)
	{
		MTDS_ERROR("Health component for owner [%s] has already been "
			"initilized with an ability system", *Owner->GetName());
		return;
	}

	AbilitySystemComponent = Asc;
	if (!AbilitySystemComponent)
	{
		MTDS_ERROR("Cannot initilize health component for owner [%s] with a "
			"NULL ability system", *Owner->GetName());
		return;
	}

	HealthSet = AbilitySystemComponent->GetSet<UMTD_HealthSet>();
	if (!HealthSet)
	{
		MTDS_ERROR("Cannot initialize health component with NULL "
			"combat set on the ability system");
		return;
	}

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		UMTD_HealthSet::GetHealthAttribute()).AddUObject(
			this, &ThisClass::OnHealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		UMTD_HealthSet::GetMaxHealthAttribute()).AddUObject(
			this, &ThisClass::OnMaxHealthChanged);
	
	HealthSet->OnOutOfHealthDelegate.AddUObject(
		this, &ThisClass::OnOutOfHealth);

	AbilitySystemComponent->SetNumericAttributeBase(
		UMTD_HealthSet::GetHealthAttribute(), HealthSet->GetMaxHealth());
	
	OnHealthChangedDelegate.Broadcast(
		this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
	OnMaxHealthChangedDelegate.Broadcast(
		this, HealthSet->GetMaxHealth(), HealthSet->GetMaxHealth(), nullptr);
}

void UMTD_HealthComponent::UninitializeFromAbilitySystem()
{
	if (HealthSet)
	{
		HealthSet->OnOutOfHealthDelegate.RemoveAll(this);
	}

	HealthSet = nullptr;
	AbilitySystemComponent = nullptr;
}

float UMTD_HealthComponent::GetHealth() const
{
	return IsValid(HealthSet) ? HealthSet->GetHealth() : 0.f;
}

float UMTD_HealthComponent::GetMaxHealth() const
{
	return IsValid(HealthSet) ? HealthSet->GetMaxHealth() : 0.f;
}

float UMTD_HealthComponent::GetHealthNormilized() const
{
	if (IsValid(HealthSet))
	{
		const float Health = HealthSet->GetHealth();
		const float MaxHealth = HealthSet->GetMaxHealth();

		return MaxHealth > 0.f ? (Health / MaxHealth) : 0.f;
	}
	return 0.f;
}

void UMTD_HealthComponent::StartDeath()
{
	if (DeathState != EMTD_DeathState::NotDead)
		return;

	DeathState = EMTD_DeathState::DeathStarted;

	AActor *Owner = GetOwner();
	check(IsValid(Owner));

	OnDeathStarted.Broadcast(Owner);
}

void UMTD_HealthComponent::FinishDeath()
{
	if (DeathState != EMTD_DeathState::DeathStarted)
		return;

	DeathState = EMTD_DeathState::DeathFinished;

	AActor *Owner = GetOwner();
	check(IsValid(Owner));

	OnDeathFinished.Broadcast(Owner);
}

void UMTD_HealthComponent::SelfDestruct(bool bFeelOutOfWorld)
{
	if (DeathState == EMTD_DeathState::NotDead &&
		IsValid(AbilitySystemComponent))
	{
		// TODO: Implement me
	}
}

void UMTD_HealthComponent::OnUnregister()
{
	UninitializeFromAbilitySystem();
	
	Super::OnUnregister();
}

void UMTD_HealthComponent::ClearGameplayTags()
{
	if (!IsValid(AbilitySystemComponent))
		return;

	const FMTD_GameplayTags &GameplayTags = FMTD_GameplayTags::Get();

	AbilitySystemComponent->SetLooseGameplayTagCount(
		GameplayTags.Status_Death_Dying, 0);
	AbilitySystemComponent->SetLooseGameplayTagCount(
		GameplayTags.Status_Death_Dead, 0);
}

static AActor* GetInstigatorFromAttrChangeData(
	const FOnAttributeChangeData &ChangeData)
{
	if (ChangeData.GEModData != nullptr)
	{
		const FGameplayEffectContextHandle& EffectContext =
			ChangeData.GEModData->EffectSpec.GetEffectContext();
		return EffectContext.GetOriginalInstigator();
	}
	
    return nullptr;
}

void UMTD_HealthComponent::OnHealthChanged(
	const FOnAttributeChangeData &ChangeData)
{
	OnHealthChangedDelegate.Broadcast(
		this,
		ChangeData.OldValue,
		ChangeData.NewValue,
		GetInstigatorFromAttrChangeData(ChangeData));
}

void UMTD_HealthComponent::OnMaxHealthChanged(
	const FOnAttributeChangeData &ChangeData)
{
	OnMaxHealthChangedDelegate.Broadcast(
		this,
		ChangeData.OldValue,
		ChangeData.NewValue,
		GetInstigatorFromAttrChangeData(ChangeData));
}

void UMTD_HealthComponent::OnOutOfHealth(
	AActor *DamageInstigator,
	AActor *DamageCauser,
	const FGameplayEffectSpec &DamageEffectSpec,
	float DamageMagnitude)
{
#if WITH_SERVER_CODE
	if (!IsValid(AbilitySystemComponent))
		return;
	
	// Send the "Gameplay.Event.Death" gameplay event through the owner's
	// ability system. This can be used to trigger a death gameplay ability.
	FGameplayEventData Payload;
	Payload.EventTag = FMTD_GameplayTags::Get().Gameplay_Event_Death;
	Payload.Instigator = DamageInstigator;
	Payload.Target = AbilitySystemComponent->GetAvatarActor();
	Payload.OptionalObject = DamageEffectSpec.Def;
	Payload.ContextHandle = DamageEffectSpec.GetEffectContext();
	Payload.InstigatorTags =
		*DamageEffectSpec.CapturedSourceTags.GetAggregatedTags();
	Payload.TargetTags =
		*DamageEffectSpec.CapturedTargetTags.GetAggregatedTags();
	Payload.EventMagnitude = DamageMagnitude;

	// FScopedPredictionWindow NewScopedWindow(AbilitySystemComponent, true);
	AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);

	// TODO: Send an event about death. This can be used to keep track of some
	// game statistics
	// ...

#endif // #if WITH_SERVER_CODE
}
