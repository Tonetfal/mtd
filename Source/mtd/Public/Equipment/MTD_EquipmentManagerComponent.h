#pragma once

#include "mtd.h"
#include "Components/PawnComponent.h"
#include "MTD_EquipmentManagerComponent.generated.h"

class UMTD_EquipmentInstance;
class UMTD_EquipmentDefinition;

UCLASS(BlueprintType, Const, meta=(BlueprintSpawnableComponent))
class UMTD_EquipmentManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UMTD_EquipmentManagerComponent(const FObjectInitializer &ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category="MTD|Equipment")
	UMTD_EquipmentInstance *EquipItem(
		TSubclassOf<UMTD_EquipmentDefinition> EquipmentDefinition);
	
	UFUNCTION(BlueprintCallable, Category="MTD|Equipment")
	void UnequipItem();
	
	//~UActorComponent interface
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	//~End of UActorComponent interface

private:
	UPROPERTY()
	TObjectPtr<UMTD_EquipmentInstance> EquipmentInstance;
};
