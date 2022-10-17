#include "Equipment/MTD_EquipmentManagerComponent.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "Equipment/MTD_EquipmentDefinition.h"
#include "Equipment/MTD_EquipmentInstance.h"
#include "Net/UnrealNetwork.h"

UMTD_EquipmentManagerComponent::UMTD_EquipmentManagerComponent(
	const FObjectInitializer &ObjectInitializer)
: UPawnComponent(ObjectInitializer)
{
}

void UMTD_EquipmentManagerComponent::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquipmentInstance);
}

UMTD_EquipmentInstance *UMTD_EquipmentManagerComponent::EquipItem(
	TSubclassOf<UMTD_EquipmentDefinition> EquipmentDefinition)
{
	AActor *Owner = GetOwner();
	check(EquipmentDefinition);
	check(Owner);

	const auto Equipment =
		GetDefault<UMTD_EquipmentDefinition>(EquipmentDefinition);

	auto MtdAsc = Cast<UMTD_AbilitySystemComponent>(
		UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner));

	TSubclassOf<UMTD_EquipmentInstance> InstanceType = Equipment->InstanceType;
	if (!InstanceType)
	{
		InstanceType = UMTD_EquipmentInstance::StaticClass();
	}

	Equipment->AbilitySetToGrant->GiveToAbilitySystem(MtdAsc, nullptr, Owner);
	EquipmentInstance = NewObject<UMTD_EquipmentInstance>(Owner, InstanceType);
	EquipmentInstance->SpawnEquipmentActor(Equipment->ActorToSpawn);
	EquipmentInstance->OnEquipped();

	return EquipmentInstance;
}

void UMTD_EquipmentManagerComponent::UnequipItem()
{
	if (!IsValid(EquipmentInstance))
		return;
	
	EquipmentInstance->OnUnequipped();
}

void UMTD_EquipmentManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UMTD_EquipmentManagerComponent::UninitializeComponent()
{
	UnequipItem();
	
	Super::UninitializeComponent();
}
