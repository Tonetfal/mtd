#include "Equipment/MTD_EquipmentInstance.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/MTD_PlayerSet.h"
#include "Equipment/MTD_EquipmentDefinition.h"
#include "GameFramework/Character.h"

UMTD_EquipmentInstance::UMTD_EquipmentInstance()
{
}

UWorld *UMTD_EquipmentInstance::GetWorld() const
{
	APawn *OwningPawn = GetPawn();
	return IsValid(OwningPawn) ? OwningPawn->GetWorld() : nullptr;
}

APawn *UMTD_EquipmentInstance::GetPawn() const
{
	return Cast<APawn>(GetOuter());
}

APawn *UMTD_EquipmentInstance::GetTypedPawn(TSubclassOf<APawn> PawnType) const
{
	UObject *Outer = GetOuter();
	return (PawnType && IsValid(Outer) && Outer->IsA(PawnType)) ?
		(Cast<APawn>(Outer)) : (nullptr);
}

UAbilitySystemComponent*
	UMTD_EquipmentInstance::GetAbilitySystemComponent() const
{
	const APawn *Pawn = GetPawn();
	return IsValid(Pawn) ?
		UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Pawn) : 
		nullptr;
}

UMTD_AbilitySystemComponent*
	UMTD_EquipmentInstance::GetMtdAbilitySystemComponent() const
{
	return
		CastChecked<UMTD_AbilitySystemComponent>(GetAbilitySystemComponent());
}

void UMTD_EquipmentInstance::SpawnEquipmentActor(
	const FMTD_EquipmentActorToSpawn &ActorToSpawn)
{
	APawn *OwningPawn = GetPawn();
	if (!IsValid(OwningPawn))
		return;

	USceneComponent *AttachTarget = OwningPawn->GetRootComponent();
	
	auto Char = Cast<ACharacter>(OwningPawn);
	if (IsValid(Char))
		AttachTarget = Char->GetMesh();

	auto NewActor = GetWorld()->SpawnActorDeferred<AActor>(
		ActorToSpawn.ActorToSpawn, FTransform::Identity, OwningPawn);
	
	NewActor->FinishSpawning(FTransform::Identity, true);
	NewActor->SetActorRelativeTransform(ActorToSpawn.AttachTransform);
	NewActor->AttachToComponent(
		AttachTarget,
		FAttachmentTransformRules::KeepRelativeTransform,
		ActorToSpawn.AttachToSocket);

	SpawnedActor = NewActor;
}

void UMTD_EquipmentInstance::DestroyEquipmentActor()
{
	SpawnedActor->Destroy();
}

void UMTD_EquipmentInstance::OnEquipped()
{
	K2_OnEquipped();

	GrantStats();
}

void UMTD_EquipmentInstance::OnUnequipped()
{
	K2_OnUnequipped();
	
	TakeBackStats();
}

void UMTD_EquipmentInstance::ModStats(float Multiplier)
{
	if (!IsPlayer())
		return;
	
	UAbilitySystemComponent *Asc = GetAbilitySystemComponent();
	if (!IsValid(Asc))
	{
		MTDS_ERROR("Cannot modify stats to owner [%s] with a NULL ability "
			"system", *GetNameSafe(GetOuter()));
		return;
	}
	
	check(Asc->GetAttributeSet(UMTD_PlayerSet::StaticClass()));

	Asc->ApplyModToAttribute(UMTD_PlayerSet::GetHealthStatAttribute(),
		EGameplayModOp::Additive, PlayerStats.HealthStat * Multiplier);
	Asc->ApplyModToAttribute(UMTD_PlayerSet::GetDamageStatAttribute(),
		EGameplayModOp::Additive, PlayerStats.DamageStat * Multiplier);
	Asc->ApplyModToAttribute(UMTD_PlayerSet::GetSpeedStatAttribute(),
		EGameplayModOp::Additive, PlayerStats.SpeedStat * Multiplier);
}

bool UMTD_EquipmentInstance::IsPlayer() const
{
	const APawn *Pawn = GetPawn();
	if (!IsValid(Pawn))
		return false;
	
	// Bots' equip must not have any stats, so we have nothing to mod
	return !Pawn->IsBotControlled();
}

void UMTD_EquipmentInstance::GrantStats()
{
	ModStats(+1.f);
}

void UMTD_EquipmentInstance::TakeBackStats()
{
	ModStats(-1.f);
}
