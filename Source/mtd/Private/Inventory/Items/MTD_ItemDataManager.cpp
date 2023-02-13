#include "Inventory/Items/MTD_ItemDataManager.h"

#include "Inventory/Items/MTD_ArmorItemData.h"
#include "Inventory/Items/MTD_MaterialItemData.h"
#include "Inventory/Items/MTD_WeaponItemData.h"

UMTD_ItemDataManager *UMTD_ItemDataManager::Construct()
{
    if (!ensureMsgf(!IsValid(ItemDataManager), TEXT("Item data manager is already instantiated.")))
    {
        return nullptr;
    }
    
    // Get the class name defined in config
    FString ClassName;
    GConfig->GetString(TEXT("ItemDataManager"), TEXT("DefaultItemDataManagerClass"), ClassName, GGameIni);

    // Get the actual class pointer
    const UClass *DefaultClass = LoadClass<UMTD_ItemDataManager>(nullptr, *ClassName);
    if (!DefaultClass)
    {
        MTD_WARN("Default item data manager class is NULL.");
        return nullptr;
    }
    
    // Create a new instance, and disallow default garbage collecting
    constexpr EObjectFlags Flags = (EObjectFlags::RF_Transient | EObjectFlags::RF_MarkAsRootSet);
    ItemDataManager = NewObject<UMTD_ItemDataManager>(GetTransientPackage(), DefaultClass, NAME_None, Flags);

    if (!IsValid(ItemDataManager))
    {
        MTD_WARN("Failed to create item data manager.");
        return nullptr;
    }

    ItemDataManager->CheckDataValidness();
    
    MTD_LOG("Item data manager has been successfully created.");
    return ItemDataManager;
}

UMTD_ItemDataManager *UMTD_ItemDataManager::Get()
{
    if (!ItemDataManager)
    {
        ItemDataManager = Construct();
    }

    return ItemDataManager;
}

void UMTD_ItemDataManager::CheckDataValidness() const
{
    VerifyBaseItemDataTable();
    VerifyArmorDataTable();
    VerifyWeaponDataTable();
    VerifyMaterialDataTable();
}

void UMTD_ItemDataManager::VerifyBaseItemDataTable() const
{
    if (!IsValid(BaseItemDataTable))
    {
        MTDS_WARN("Base item data table is invalid.");
        return;
    }
}

void UMTD_ItemDataManager::VerifyArmorDataTable() const
{
    if (!IsValid(ArmorDataTable))
    {
        MTDS_WARN("Armor data table is invalid.");
        return;
    }

    const TMap<FName, uint8*> &DataTable = ArmorDataTable->GetRowMap();
    for (auto [Name, Pointer] : DataTable)
    {
        const auto ArmorItemDataRow = reinterpret_cast<FMTD_ArmorItemDataRow *>(Pointer);
        if (ArmorItemDataRow)
        {
            if (ArmorItemDataRow->HeroClasses.IsEmpty())
            {
                MTDS_WARN("Armor item data [%s] has no hero class that can use it.", *Name.ToString());
            }
            
            if (ArmorItemDataRow->ArmorType == EMTD_ArmorType::Invalid)
            {
                MTDS_WARN("Armor item data [%s] has an invalid armor type.", *Name.ToString());
            }
        }
    }
}

void UMTD_ItemDataManager::VerifyWeaponDataTable() const
{
    if (!IsValid(WeaponDataTable))
    {
        MTDS_WARN("Weapon Data Table is invalid.");
        return;
    }

    const TMap<FName, uint8*> &DataTable = WeaponDataTable->GetRowMap();
    for (auto [Name, Pointer] : DataTable)
    {
        const auto WeaponItemDataRow = reinterpret_cast<FMTD_WeaponItemDataRow *>(Pointer);
        if (WeaponItemDataRow)
        {
            if (WeaponItemDataRow->HeroClasses.IsEmpty())
            {
                MTDS_WARN("Weapon Item Data [%s] has no hero class that can use it.", *Name.ToString());
            }
            
            if (WeaponItemDataRow->WeaponType == EMTD_WeaponType::IWT_Invalid)
            {
                MTDS_WARN("Weapon item data [%s] has an invalid weapon type.", *Name.ToString());
            }
        }
    }
}

void UMTD_ItemDataManager::VerifyMaterialDataTable() const
{
    if (!IsValid(MaterialDataTable))
    {
        MTDS_WARN("Material Data Table is invalid.");
        return;
    }

    const TMap<FName, uint8 *> &DataTable = MaterialDataTable->GetRowMap();
    for (auto [Name, Pointer] : DataTable)
    {
        const auto MaterialItemDataRow = reinterpret_cast<FMTD_MaterialItemDataRow *>(Pointer);
        if (MaterialItemDataRow)
        {
            if (MaterialItemDataRow->MaxAmount < 1)
            {
                MTDS_WARN("Material item data [%s] has a non-positive number [%d] for max amount.",
                    *Name.ToString(), MaterialItemDataRow->MaxAmount);
            }
        }
    }
}

