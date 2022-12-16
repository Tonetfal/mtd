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

    UFUNCTION(BlueprintPure, Category="MTD|Pawn")
    static UMTD_EquipmentManagerComponent *FindEquipmentManagerComponent(const AActor *Actor);

    //~UActorComponent interface
    virtual void InitializeComponent() override;
    virtual void UninitializeComponent() override;
    //~End of UActorComponent interface

    UFUNCTION(BlueprintCallable, Category="MTD|Equipment")
    UMTD_EquipmentInstance *EquipItem(TSubclassOf<UMTD_EquipmentDefinition> EquipmentDefinition);

    UFUNCTION(BlueprintCallable, Category="MTD|Equipment")
    void UnequipItem();

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Equipment")
    const UMTD_EquipmentInstance *GetEquipmentInstance() const;

private:
    UPROPERTY()
    TObjectPtr<UMTD_EquipmentInstance> EquipmentInstance;
};

inline UMTD_EquipmentManagerComponent *UMTD_EquipmentManagerComponent::FindEquipmentManagerComponent(
    const AActor *Actor)
{
    return (IsValid(Actor)) ? (Actor->FindComponentByClass<UMTD_EquipmentManagerComponent>()) : (nullptr);
}

inline const UMTD_EquipmentInstance *UMTD_EquipmentManagerComponent::GetEquipmentInstance() const
{
    return EquipmentInstance;
}
