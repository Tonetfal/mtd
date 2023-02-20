#pragma once

#include "mtd.h"
#include "UObject/Object.h"

#include "MTD_ItemDataManager.generated.h"

class UDataTable;

/**
 * Singleton structure containing all item data tables.
 */
UCLASS(BlueprintType, Blueprintable)
class MTD_API UMTD_ItemDataManager
    : public UObject
{
    GENERATED_BODY()

private:
    /**
     * Create item data manager using class specified in the config.
     * @return  Created item data manager. May be nullptr;
     */
    static UMTD_ItemDataManager *Construct();

public:
    /**
     * Get singleton instance of item data manager.
     * @return  Singleton instance of item data manager.
     */
    static UMTD_ItemDataManager *Get();

    /**
     * Get base item data table.
     * @return  Base item data table.
     */
    static const UDataTable *GetBaseItemDataTable();

    /**
     * Get armor data table.
     * @return  Armor data table.
     */
    static const UDataTable *GetArmorDataTable();
    
    /**
     * Get weapon data table.
     * @return  Weapon data table.
     */
    static const UDataTable *GetWeaponDataTable();
    
    /**
     * Get material data table.
     * @return  Material data table.
     */
    static const UDataTable *GetMaterialDataTable();

public:
    /**
     * Check whether data tables are valid both memory wise and logic wise.
     */
    void CheckDataValidness() const;

private:
    /**
     * Check whether base item data table is valid.
     */
    void VerifyBaseItemDataTable() const;
    
    /**
     * Check whether armor data table is valid.
     */
    void VerifyArmorDataTable() const;
    
    /**
     * Check whether weapon data table is valid.
     */
    void VerifyWeaponDataTable() const;
    
    /**
     * Check whether material data table is valid.
     */
    void VerifyMaterialDataTable() const;

private:
    /** Singleton instance of the item data manager. */
    inline static TObjectPtr<UMTD_ItemDataManager> ItemDataManager = nullptr;

    /** Base item data table containing items templated information. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Item Data Manager", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UDataTable> BaseItemDataTable = nullptr;
    
    /** Armor data table containing armor templated information. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Item Data Manager", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UDataTable> ArmorDataTable = nullptr;
    
    /** Weapon data table containing weapon templated information. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Item Data Manager", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UDataTable> WeaponDataTable = nullptr;
    
    /** Material data table containing material templated information. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Item Data Manager", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UDataTable> MaterialDataTable = nullptr;
};

inline const UDataTable *UMTD_ItemDataManager::GetBaseItemDataTable()
{
    const UMTD_ItemDataManager *Manager = Get();
    return ((IsValid(Manager)) ? (Manager->BaseItemDataTable) : (nullptr));
}

inline const UDataTable *UMTD_ItemDataManager::GetArmorDataTable()
{
    const UMTD_ItemDataManager *Manager = Get();
    return ((IsValid(Manager)) ? (Manager->ArmorDataTable) : (nullptr));
}

inline const UDataTable *UMTD_ItemDataManager::GetWeaponDataTable()
{
    const UMTD_ItemDataManager *Manager = Get();
    return ((IsValid(Manager)) ? (Manager->WeaponDataTable) : (nullptr));
}

inline const UDataTable *UMTD_ItemDataManager::GetMaterialDataTable()
{
    const UMTD_ItemDataManager *Manager = Get();
    return ((IsValid(Manager)) ? (Manager->MaterialDataTable) : (nullptr));
}
