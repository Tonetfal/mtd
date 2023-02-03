#pragma once

#include "GameFramework/Actor.h"
#include "mtd.h"

#include "MTD_InventoryItemSpawner.generated.h"

class AMTD_InventoryItemInstance;
class UMTD_ArmorItemData;
class UMTD_BaseInventoryItemData;
class UMTD_EquippableItemData;
class UMTD_WeaponItemData;

USTRUCT()
struct FMTD_EquippableItemParameters
{
    GENERATED_BODY()

public:
    /**
     * Main config.
     */
    
    UPROPERTY(EditAnywhere, Category="Config")
    int32 LevelRequired = 0;

    /**
     * Player attributes.
     */
    
    UPROPERTY(EditAnywhere, Category="Attributes|Player")
    float PlayerHealth = 0.f;
    
    UPROPERTY(EditAnywhere, Category="Attributes|Player")
    float PlayerDamage = 0.f;
    
    UPROPERTY(EditAnywhere, Category="Attributes|Player")
    float PlayerSpeed = 0.f;

    /**
     * Tower attributes.
     */
    
    UPROPERTY(EditAnywhere, Category="Attributes|Tower")
    float TowerHealth = 0.f;
    
    UPROPERTY(EditAnywhere, Category="Attributes|Tower")
    float TowerDamage = 0.f;
    
    UPROPERTY(EditAnywhere, Category="Attributes|Tower")
    float TowerRange = 0.f;
    
    UPROPERTY(EditAnywhere, Category="Attributes|Tower")
    float TowerSpeed = 0.f;
};

USTRUCT()
struct FMTD_WeaponItemParameters
{
    GENERATED_BODY()

public:
    /**
     * Weapon attributes.
     */

    UPROPERTY(EditAnywhere, Category="Attributes|Weapon")
    int32 MeleeDamage = 0;
    
    UPROPERTY(EditAnywhere, Category="Attributes|Weapon")
    float BalanceDamage = 7.5f;

    UPROPERTY(EditAnywhere, Category="Attributes|Weapon")
    int32 RangedDamage = 0;

    UPROPERTY(EditAnywhere, Category="Attributes|Weapon")
    int32 Projectiles = 0;
    
    UPROPERTY(EditAnywhere, Category="Attributes|Weapon")
    float ProjectileSpeed = 0.f;
    
    UPROPERTY(EditAnywhere, Category="Attributes|Weapon")
    bool bRadial = false;
    
    UPROPERTY(EditAnywhere, Category="Attributes|Weapon")
    float RadialRange = 0.f;
};

USTRUCT()
struct FMTD_ArmorItemParameters
{
    GENERATED_BODY()

public:
    /**
     * Armor attributes.
     */

    UPROPERTY(EditAnywhere, Category="Attributes|Armor")
    int32 PhysicalResistance = 0;
    
    UPROPERTY(EditAnywhere, Category="Attributes|Armor")
    int32 FireResistance = 0;

    UPROPERTY(EditAnywhere, Category="Attributes|Armor")
    int32 DarkResistance = 0;

    UPROPERTY(EditAnywhere, Category="Attributes|Armor")
    int32 StatusResistance = 0;
};



/**
 * Actor that is able to spawn a specific item with specified parameters. It's supposed to be used in specific cases;
 * generally items should be spawned through InventoryBlueprintFunctionLibrary.
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
    virtual void PreInitializeComponents() override;
    virtual void BeginPlay() override;
    //~End of AActor Interface

private:
    void DispatchItemParameters(UMTD_BaseInventoryItemData *ItemData);
    void DispatchEquippableParameters(UMTD_BaseInventoryItemData *ItemData);
    void DispatchArmorParameters(UMTD_BaseInventoryItemData *ItemData);
    void DispatchWeaponParameters(UMTD_BaseInventoryItemData *ItemData);

private:
    UPROPERTY(EditInstanceOnly, Category="MTD|Inventory Item Spawner")
    int32 ItemID = 0;

    UPROPERTY(EditAnywhere, Category="MTD|Inventory Item Spawner")
    TSubclassOf<AMTD_InventoryItemInstance> InventoryItemInstanceClass = nullptr;

    UPROPERTY(EditInstanceOnly, Category="MTD|Inventory Item Spawner")
    FMTD_EquippableItemParameters EquippableItemParameters;

    UPROPERTY(EditInstanceOnly, Category="MTD|Inventory Item Spawner")
    FMTD_WeaponItemParameters WeaponItemParameters;
    
    UPROPERTY(EditInstanceOnly, Category="MTD|Inventory Item Spawner")
    FMTD_ArmorItemParameters ArmorItemParameters;
};
