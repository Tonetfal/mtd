#include "Components/MTD_HealthComponent.h"

UMTD_HealthComponent::UMTD_HealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMTD_HealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Health = MaxHealth;

	AActor *ActorOwner = GetOwner();
	if (IsValid(ActorOwner))
	{
		ActorOwner->OnTakeAnyDamage.AddDynamic(
			this, &UMTD_HealthComponent::OnTakeAnyDamage);
	}
}

void UMTD_HealthComponent::IncreaseHealth(int32 Value)
{
	SetHealth(Health + Value);
}

void UMTD_HealthComponent::DecreaseHealth(int32 Value)
{
	SetHealth(Health - Value);
}

void UMTD_HealthComponent::SetHealth(int32 Value)
{
	if (bImmortal && Value < Health)
		return;
	
	const int32 OldHealth = Health; 
	Health = FMath::Clamp(Value, 0.f, MaxHealth);
	
	OnHealthChangedDelegate.Broadcast(Health, Health - OldHealth);
}

void UMTD_HealthComponent::OnTakeAnyDamage(
	AActor *DamagedActor,
	float Damage,
	const UDamageType *DamageType,
	AController *IntigatedBy,
	AActor *DamageCauser)
{
	if (Damage <= 0.f || IsDead())
		return;

	const int32 OldHealth = Health;
	DecreaseHealth(Damage);

	if (IsDead())
	{
		OnDeathDelegate.Broadcast();
	}

	if (Damage > 0.f)
		OnDamagedDelegate.Broadcast(DamageCauser, OldHealth - Health);
}
