#include "System/MTD_GameInstance.h"

#include "Engine/CurveTable.h"
#include "Engine/DataTable.h"
#include "Inventory/Items/MTD_ArmorItemData.h"
#include "Inventory/Items/MTD_MaterialItemData.h"
#include "Inventory/Items/MTD_WeaponItemData.h"
#include "MTD_CoreTypes.h"

void UMTD_GameInstance::Init()
{
    Super::Init();
    FGenericTeamId::SetAttitudeSolver(&SolveTeamAttitude);

    VerifyBaseItemDataTable();
    VerifyArmorDataTable();
    VerifyWeaponDataTable();
    VerifyMaterialDataTable();
    VerifyLevelCurveTable();
}

void UMTD_GameInstance::VerifyBaseItemDataTable() const
{
    if (!IsValid(BaseItemDataTable))
    {
        MTDS_WARN("Base Item Data Table is invalid.");
        return;
    }
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
        const auto WeaponItemDataRow = reinterpret_cast<FMTD_WeaponItemDataRow *>(Pointer);
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
        const auto MaterialItemDataRow = reinterpret_cast<FMTD_MaterialItemDataRow *>(Pointer);
        if (MaterialItemDataRow)
        {
            if (MaterialItemDataRow->MaxAmount < 1)
            {
                MTDS_WARN("Material Item Data [%s] is not a positive number.", *Name.ToString());
            }
        }
    }
}

void UMTD_GameInstance::VerifyLevelCurveTable() const
{
    if (!IsValid(LevelExperienceCurveTable))
    {
        MTDS_WARN("Level Curve Table is invalid.");
        return;
    }
}
