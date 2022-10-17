#include "Equipment/MTD_EquipmentInstance.h"

#include "Equipment/MTD_EquipmentDefinition.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

UMTD_EquipmentInstance::UMTD_EquipmentInstance()
{
}

UWorld *UMTD_EquipmentInstance::GetWorld() const
{
	APawn *OwningPawn = GetPawn();
	return IsValid(OwningPawn) ? OwningPawn->GetWorld() : nullptr;
}

void UMTD_EquipmentInstance::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Instigator);
	DOREPLIFETIME(ThisClass, SpawnedActor);
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

void UMTD_EquipmentInstance::SpawnEquipmentActor(
	const FMTD_EquipmentActorToSpawn &ActorToSpawn)
{
	APawn *OwningPawn = GetPawn();
	if (!IsValid(OwningPawn))
		return;

	USceneComponent *AttachTarget = OwningPawn->GetRootComponent();
	
	ACharacter *Char = Cast<ACharacter>(OwningPawn);
	if (IsValid(Char))
		AttachTarget = Char->GetMesh();

	AActor *NewActor = GetWorld()->SpawnActorDeferred<AActor>(
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
}

void UMTD_EquipmentInstance::OnUnequipped()
{
	K2_OnUnequipped();
}
