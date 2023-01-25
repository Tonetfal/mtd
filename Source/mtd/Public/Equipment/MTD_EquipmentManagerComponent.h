#pragma once

#include "Components/PawnComponent.h"
#include "Equipment/MTD_EquipmentCoreTypes.h"
#include "mtd.h"

#include "MTD_EquipmentManagerComponent.generated.h"

class AMTD_InventoryItemInstance;
class UMTD_BaseInventoryItemData;
class UMTD_EquippableItemData;
class UMTD_EquipmentInstance;

/**
 * Manager of pawn's equipment items used by it.
 */
UCLASS(BlueprintType, Const, ClassGroup="Pawn", meta=(BlueprintSpawnableComponent))
class UMTD_EquipmentManagerComponent
    : public UPawnComponent
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnItemEquipmentSignature,
        UMTD_EquipmentInstance*, EquipmentInstance);
    
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnItemUnEquipmentSignature,
        UMTD_EquipmentInstance*, EquipmentInstance);

public:
    UMTD_EquipmentManagerComponent(const FObjectInitializer &ObjectInitializer);

    UFUNCTION(BlueprintPure, Category="MTD|Pawn")
    static UMTD_EquipmentManagerComponent *FindEquipmentManagerComponent(const AActor *Actor);

    //~UActorComponent interface
    virtual void InitializeComponent() override;
    virtual void UninitializeComponent() override;
    //~End of UActorComponent interface

    UFUNCTION(BlueprintCallable, Category="MTD|Equipment")
    UMTD_EquipmentInstance *EquipItem(UMTD_BaseInventoryItemData *ItemData);

    UFUNCTION(BlueprintCallable, Category="MTD|Equipment")
    UMTD_EquippableItemData *UnequipItemType(EMTD_EquipmentType EquipmentType);
    
    UFUNCTION(BlueprintCallable, Category="MTD|Equipment")
    UMTD_EquippableItemData *UnequipItemInstance(UMTD_EquipmentInstance *EquipmentInstance);
    
    UFUNCTION(BlueprintCallable, Category="MTD|Equipment")
    void UnequipAll();

    /**
     * Check game logical conditions of whether the character is able to equip an item. It checks things like level
     * required and character classes.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Equipment")
    bool CanEquipItem(UMTD_BaseInventoryItemData *ItemData) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Equipment")
    bool IsItemTypeEquipped(EMTD_EquipmentType EquipmentType) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Equipment")
    const UMTD_EquipmentInstance *GetEquipmentInstance(EMTD_EquipmentType EquipmentType) const;

public:
    UPROPERTY(BlueprintAssignable)
    FOnItemEquipmentSignature OnItemEquipmentDelegate;
    
    UPROPERTY(BlueprintAssignable)
    FOnItemUnEquipmentSignature OnItemUnEquipmentDelegate;

private:
    UPROPERTY(BlueprintReadOnly, Category="MTD|Equipment", meta=(AllowPrivateAccess="true"))
    TMap<EMTD_EquipmentType, TObjectPtr<UMTD_EquipmentInstance>> EquipmentInstances;
};

inline UMTD_EquipmentManagerComponent *UMTD_EquipmentManagerComponent::FindEquipmentManagerComponent(
    const AActor *Actor)
{
    return ((IsValid(Actor)) ? (Actor->FindComponentByClass<UMTD_EquipmentManagerComponent>()) : (nullptr));
}

inline const UMTD_EquipmentInstance *UMTD_EquipmentManagerComponent::GetEquipmentInstance(
    EMTD_EquipmentType EquipmentType) const
{
    const TObjectPtr<UMTD_EquipmentInstance> *EquipmentInstance = EquipmentInstances.Find(EquipmentType);
    return (EquipmentInstance) ? (*EquipmentInstance) : (nullptr);
}
