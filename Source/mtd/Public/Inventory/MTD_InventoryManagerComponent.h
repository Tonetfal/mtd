#pragma once

#include "Components/ActorComponent.h"
#include "mtd.h"

#include "MTD_InventoryManagerComponent.generated.h"

class AMTD_InventoryItemInstance;
class UMTD_BaseInventoryItemData;
class UMTD_MaterialItemData;

/**
 * Component that holds all the items, as well as manages their addition and removal.
 */
UCLASS(BlueprintType, ClassGroup=("Character"), meta=(BlueprintSpawnableComponent))
class MTD_API UMTD_InventoryManagerComponent
    : public UActorComponent
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnItemAddedSignature,
        UMTD_BaseInventoryItemData*, ItemData);
    
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnItemRemovedSignature,
        UMTD_BaseInventoryItemData*, ItemData);

public:
    UMTD_InventoryManagerComponent();
    
    UFUNCTION(BlueprintPure, Category="MTD|Player")
    static UMTD_InventoryManagerComponent *FindInventoryManagerComponent(const AActor *Actor);

    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool CanAddItem(const UMTD_BaseInventoryItemData *ItemData) const;
    
    UFUNCTION(BlueprintCallable)
    bool AddItem(UMTD_BaseInventoryItemData *ItemData, bool bDropIfNotEnoughSpace = false);
    
    UFUNCTION(BlueprintCallable)
    bool RemoveItem(UMTD_BaseInventoryItemData *ItemData);

    UFUNCTION(BlueprintCallable)
    AMTD_InventoryItemInstance *DropItem(UMTD_BaseInventoryItemData *ItemData);

    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetMaxSlots() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetUsedSlots() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetRemainingSlots() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsFull() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsEmpty() const;

private:
    void AddWeaponItem(UMTD_BaseInventoryItemData *ItemData);
    void AddArmorItem(UMTD_BaseInventoryItemData *ItemData);
    void AddMaterialItem(UMTD_BaseInventoryItemData *ItemData);
    
    TArray<const UMTD_BaseInventoryItemData*> GetAllInstancesItemID(int32 ItemID) const;
    TArray<UMTD_BaseInventoryItemData*> GetAllInstancesItemID(int32 ItemID);
    const UMTD_MaterialItemData *GetStackableItemEntry(int32 ItemID) const;
    UMTD_MaterialItemData *GetStackableItemEntry(int32 ItemID);
    const UMTD_BaseInventoryItemData *FindItemID(int32 ItemID) const;
    UMTD_BaseInventoryItemData *FindItemID(int32 ItemID);
    bool ContainsItemID(int32 ItemID) const;
    bool CanStackItem(int32 ItemID) const;

public:
    UPROPERTY(BlueprintAssignable)
    FOnItemAddedSignature OnItemAddedDelegate;
    
    UPROPERTY(BlueprintAssignable)
    FOnItemRemovedSignature OnItemRemovedDelegate;

private:
    UPROPERTY(EditDefaultsOnly, Category="MTD|Inventory Manager Component", meta=(ClampMin="0.0"))
    int32 MaxSlots = 25;

    UPROPERTY(BlueprintReadOnly, Category="MTD|Inventory Manager Component", meta=(AllowPrivateAccess="true"))
    TArray<TObjectPtr<UMTD_BaseInventoryItemData>> InventoryEntries;
};

inline UMTD_InventoryManagerComponent *UMTD_InventoryManagerComponent::FindInventoryManagerComponent(
    const AActor *Actor)
{
    return ((IsValid(Actor) ? (Actor->FindComponentByClass<UMTD_InventoryManagerComponent>()) : (nullptr)));
}
