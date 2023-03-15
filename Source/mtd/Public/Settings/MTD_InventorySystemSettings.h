#pragma once

#include "Engine/DeveloperSettings.h"
#include "mtd.h"

#include "MTD_InventorySystemSettings.generated.h"

class UDataTable;

/**
 * Settings class for inventory system that can be easily accessed in project settings in the editor.
 */
UCLASS(Config="Game", DefaultConfig, DisplayName="Inventory System")
class MTD_API UMTD_InventorySystemSettings
    : public UDeveloperSettings
{
    GENERATED_BODY()

public: 
    /**
     * Get singleton instance of inventory system settings.
     * @return  Singleton instance of inventory system settings.
     */
	static UMTD_InventorySystemSettings *Get();

    //~UDeveloperSettings Interface
    virtual FName GetCategoryName() const override;
    //~End of UDeveloperSettings Interface

private:
    /** Singleton instance of the inventory system settings. */
    inline static TObjectPtr<UMTD_InventorySystemSettings> DefaultSettings = nullptr;

public:
    /** Base item data table containing items templated information. */
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="General")
    TSoftObjectPtr<UDataTable> BaseItemDataTable = nullptr;
    
    /** Armor data table containing armor templated information. */
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="General")
    TSoftObjectPtr<UDataTable> ArmorDataTable = nullptr;
    
    /** Weapon data table containing weapon templated information. */
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="General")
    TSoftObjectPtr<UDataTable> WeaponDataTable = nullptr;
    
    /** Material data table containing material templated information. */
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="General")
    TSoftObjectPtr<UDataTable> MaterialDataTable = nullptr;
};

inline FName UMTD_InventorySystemSettings::GetCategoryName() const
{
    return TEXT("MTD");
}
