#pragma once

#include "GameFramework/Actor.h"
#include "InventorySystem/Items/MTD_ArmorItemData.h"
#include "InventorySystem/Items/MTD_WeaponItemData.h"
#include "mtd.h"

#include "MTD_InventoryItemSpawner.generated.h"

class AMTD_InventoryItemInstance;
class UMTD_ArmorItemData;
class UMTD_BaseInventoryItemData;
class UMTD_EquipItemData;
class UMTD_WeaponItemData;

/**
 * Simple item spawn wrapper. It spawns a specific item with specified stats prior to playtime, and destroy ownself as
 * soon as playtime begins.
 * 
 * Note that generally items should be spawned through InventoryBlueprintFunctionLibrary.
 */
UCLASS()
class MTD_API AMTD_InventoryItemSpawner
    : public AActor
{
    GENERATED_BODY()

public:
    AMTD_InventoryItemSpawner();

protected:
    //~AActor Interface

    /**
     * Create an item instance prior to playtime. Delete it if already created to create a new one.
     * @param   Transform: transform this actor has.
     */
    virtual void OnConstruction(const FTransform &Transform) override;

    /**
     * Destroy ownself.
     */
    virtual void BeginPlay() override;
    //~End of AActor Interface

private:
    /**
     * Dispatch all item related data.
     * @param   ItemData: item object to dispatch data on.
     */
    void DispatchItemParameters(UMTD_BaseInventoryItemData *ItemData) const;
    
    /**
     * Dispatch equip item related data.
     * @param   ItemData: item object to dispatch data on.
     */
    void DispatchEquipParameters(UMTD_BaseInventoryItemData *ItemData) const;
    
    /**
     * Dispatch armor item related data.
     * @param   ItemData: item object to dispatch data on.
     */
    void DispatchArmorParameters(UMTD_BaseInventoryItemData *ItemData) const;
    
    /**
     * Dispatch weapon item related data.
     * @param   ItemData: item object to dispatch data on.
     */
    void DispatchWeaponParameters(UMTD_BaseInventoryItemData *ItemData) const;

private:
    /** Item ID to spawn. */
    UPROPERTY(EditInstanceOnly, Category="MTD|Inventory Item Spawner")
    int32 ItemID = 0;

    /** Inventory item instance class to spawn. */
    UPROPERTY(EditAnywhere, Category="MTD|Inventory Item Spawner")
    TSubclassOf<AMTD_InventoryItemInstance> InventoryItemInstanceClass = nullptr;

    /** Equip item stat parameters to spawn the item with. */
    UPROPERTY(EditInstanceOnly, Category="MTD|Inventory Item Spawner")
    FMTD_EquipItemParameters EquipItemParameters;

    /** Weapon item stat parameters to spawn the item with. */
    UPROPERTY(EditInstanceOnly, Category="MTD|Inventory Item Spawner")
    FMTD_WeaponItemParameters WeaponItemParameters;
    
    /** Armor item stat parameters to spawn the item with. */
    UPROPERTY(EditInstanceOnly, Category="MTD|Inventory Item Spawner")
    FMTD_ArmorItemParameters ArmorItemParameters;

    /** Spawned item instance. */
    UPROPERTY()
    TObjectPtr<AMTD_InventoryItemInstance> ItemInstance = nullptr;
};
