#include "System/MTD_GameInstance.h"

#include "Engine/DataTable.h"
#include "Inventory/Items/MTD_ArmorItemData.h"
#include "Inventory/Items/MTD_MaterialItemData.h"
#include "Inventory/Items/MTD_WeaponItemData.h"

void UMTD_GameInstance::Init()
{
    Super::Init();

    VerifyArmorDataTable();
    VerifyWeaponDataTable();
    VerifyMaterialDataTable();
}

void UMTD_GameInstance::VerifyArmorDataTable() const
{
    if (!IsValid(ArmorDataTable))
    {
        MTDS_WARN("Armor Data Table is invalid.");
        return;
    }

    const TMap<FName, uint8*> &DataTable = ArmorDataTable->GetRowMap();
    for (auto [Name, Pointer] : DataTable)
    {
        const auto ArmorItemDataRow = (FMTD_ArmorItemDataRow*) (Pointer);
        if (ArmorItemDataRow)
        {
            if (ArmorItemDataRow->HeroClasses.IsEmpty())
            {
                MTDS_WARN("Armor Item Data [%s] has no Hero Class that can use it.", *Name.ToString());
            }
        }
    }
    
}

void UMTD_GameInstance::VerifyWeaponDataTable() const
{
    if (!IsValid(WeaponDataTable))
    {
        MTDS_WARN("Weapon Data Table is invalid.");
        return;
    }

    const TMap<FName, uint8*> &DataTable = WeaponDataTable->GetRowMap();
    for (auto [Name, Pointer] : DataTable)
    {
        const auto WeaponItemDataRow = (FMTD_WeaponItemDataRow*) (Pointer);
        if (WeaponItemDataRow)
        {
            if (WeaponItemDataRow->HeroClasses.IsEmpty())
            {
                MTDS_WARN("Weapon Item Data [%s] has no Hero Class that can use it.", *Name.ToString());
            }
        }
    }
}

void UMTD_GameInstance::VerifyMaterialDataTable() const
{
    if (!IsValid(MaterialDataTable))
    {
        MTDS_WARN("Material Data Table is invalid.");
        return;
    }

    const TMap<FName, uint8*> &DataTable = MaterialDataTable->GetRowMap();
    for (auto [Name, Pointer] : DataTable)
    {
        const auto MaterialItemDataRow = (FMTD_MaterialItemDataRow*) (Pointer);
        if (MaterialItemDataRow)
        {
            if (MaterialItemDataRow->MaxAmount < 1)
            {
                MTDS_WARN("Material Item Data [%s] is not a positive number.", *Name.ToString());
            }
        }
    }
}
