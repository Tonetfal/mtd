#pragma once

#include "mtd.h"
#include "GameFramework/Pawn.h"

#include "MTD_EquipmentInstance.generated.h"

struct FMTD_EquipmentActorToSpawn;

UCLASS(BlueprintType, Blueprintable)
class UMTD_EquipmentInstance : public UObject
{
	GENERATED_BODY()

public:
	UMTD_EquipmentInstance();

	//~UObject interface
	virtual bool IsSupportedForNetworking() const override
		{ return true; }
	virtual UWorld *GetWorld() const override final;
	//~End of UObject interface

	UFUNCTION(BlueprintPure, Category="Equipment")
	UObject *GetInstigator() const
		{ return Instigator; }

	void SetInstigator(UObject *InInstigator)
		{ Instigator = InInstigator; }

	UFUNCTION(BlueprintPure, Category="Equipment")
	APawn *GetPawn() const;

	UFUNCTION(BlueprintPure, Category="Equipment",
		meta=(DeterminesOutputType=PawnType))
	APawn *GetTypedPawn(TSubclassOf<APawn> PawnType) const;

	UFUNCTION(BlueprintPure, Category="Equipment")
	AActor *GetSpawnedActor() const
		{ return SpawnedActor; }

	virtual void SpawnEquipmentActor(
		const FMTD_EquipmentActorToSpawn &ActorToSpawn);
	virtual void DestroyEquipmentActor();

	virtual void OnEquipped();
	virtual void OnUnequipped();

protected:
	UFUNCTION(BlueprintImplementableEvent, Category="Equipment",
		meta=(DisplayName="OnEquipped"))
	void K2_OnEquipped();

	UFUNCTION(BlueprintImplementableEvent, Category="Equipment",
		meta=(DisplayName="OnUnequipped"))
	void K2_OnUnequipped();

private:
	UPROPERTY()
	TObjectPtr<UObject> Instigator;

	UPROPERTY(Replicated)
	TObjectPtr<AActor> SpawnedActor;
};
