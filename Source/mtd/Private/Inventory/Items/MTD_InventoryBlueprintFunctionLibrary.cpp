#include "Inventory/Items/MTD_InventoryBlueprintFunctionLibrary.h"

#include "Inventory/Items/MTD_ArmorItemData.h"
#include "Inventory/Items/MTD_BaseInventoryItemData.h"
#include "Inventory/Items/MTD_MaterialItemData.h"
#include "Inventory/Items/MTD_WeaponItemData.h"
#include "Inventory/MTD_InventoryItemInstance.h"
#include "Kismet/GameplayStatics.h"
#include "System/MTD_GameInstance.h"

struct FMTD_GenerationParameters
{
public:
    UObject *WorldContextObject = nullptr;
    int32 ItemID = 0;
    FName RowID;
    float Difficulty = 0.f;
    UMTD_BaseInventoryItemData *BaseItemData = nullptr;
    UMTD_GameInstance *GameInstance = nullptr;
    FMTD_BaseInventoryItemDataRow *BaseItemDataRow = nullptr;
};



/**
 * Equippable item generation.
 */

static void RandomizeEquippableItemData(FMTD_GenerationParameters &Params, UMTD_EquippableItemData &EquipItemData)
{
    // @todo make use of Difficulty parameter
    EquipItemData.PlayerHealth = FMath::RandRange(-100, 100);
    EquipItemData.PlayerDamage = FMath::RandRange(-100, 100);
    EquipItemData.PlayerSpeed = FMath::RandRange(-100, 100);
    
    EquipItemData.TowerHealth = FMath::RandRange(-100, 100);
    EquipItemData.TowerDamage = FMath::RandRange(-100, 100);
    EquipItemData.TowerRange = FMath::RandRange(-100, 100);
    EquipItemData.TowerSpeed = FMath::RandRange(-100, 100);
}



/**
 * Armor item generation.
 */

static void DispatchArmorItemData(FMTD_GenerationParameters &Params, UMTD_ArmorItemData &ArmorItemData,
    const FMTD_ArmorItemDataRow &ArmorItemDataRow)
{
    ArmorItemData.ArmorType = ArmorItemDataRow.ArmorType;
    ArmorItemData.HeroClasses = ArmorItemDataRow.HeroClasses;
}

static void RandomizeArmorItemData(FMTD_GenerationParameters &Params, UMTD_ArmorItemData &ArmorItemData,
    FMTD_ArmorItemDataRow &ArmorItemDataRow)
{
    RandomizeEquippableItemData(Params, ArmorItemData);
    
    // @todo make use of Difficulty parameter
    ArmorItemData.PhysicalResistance = FMath::RandRange(-10, 10);
    ArmorItemData.FireResistance = FMath::RandRange(-10, 10);
    ArmorItemData.DarkResistance = FMath::RandRange(-10, 10);
    ArmorItemData.StatusResistance = FMath::RandRange(-10, 10);
}

static void GenerateArmorItemData(FMTD_GenerationParameters &Params, bool bRandomizeStats)
{
    auto ArmorItemData = Cast<UMTD_ArmorItemData>(Params.BaseItemData);

    const UDataTable *ArmorDataTable = Params.GameInstance->GetArmorDataTable();
    if (!IsValid(ArmorDataTable))
    {
        MTD_WARN("Armor Data Table is invalid.");
        return;
    }

    const auto Row = ArmorDataTable->FindRow<FMTD_ArmorItemDataRow>(Params.RowID, nullptr, false);
    if (!Row)
    {
        MTD_WARN("Item ID [%s] could not be found in Armor Item Data Table.", *Params.RowID.GetPlainNameString());
        return;
    }

    DispatchArmorItemData(Params, *ArmorItemData, *Row);

    if (bRandomizeStats)
    {
        RandomizeArmorItemData(Params, *ArmorItemData, *Row);
    }
}



/**
 * Weapon item generation.
 */

static void DispatchWeaponItemData(FMTD_GenerationParameters &Params, UMTD_WeaponItemData &WeaponItemData,
    const FMTD_WeaponItemDataRow &WeaponItemDataRow)
{
    WeaponItemData.WeaponType = WeaponItemDataRow.WeaponType;
    WeaponItemData.HeroClasses = WeaponItemDataRow.HeroClasses;
    WeaponItemData.EquipmentDefinitionAsset = WeaponItemDataRow.EquipmentDefinitionAsset;
}

static void RandomizeWeaponItemData(FMTD_GenerationParameters &Params, UMTD_WeaponItemData &WeaponItemData,
    const FMTD_WeaponItemDataRow &FMTD_WeaponItemDataRow)
{
    RandomizeEquippableItemData(Params, WeaponItemData);
    
    // @todo make use of Difficulty parameter
    if (WeaponItemData.WeaponType & IWT_Sword)
    {
        WeaponItemData.MeleeDamage = FMath::RandRange(0, 100);
    }

    if (WeaponItemData.WeaponType & IWT_Staff)
    {
        WeaponItemData.RangedDamage = FMath::RandRange(0, 100);
        
        if (FMTD_WeaponItemDataRow.ProjectilesLimit > 0)
        {
            WeaponItemData.Projectiles = FMath::RandRange(1, FMTD_WeaponItemDataRow.ProjectilesLimit);
        }
        
        if (FMTD_WeaponItemDataRow.ProjectileSpeedLimit > 0)
        {
            WeaponItemData.ProjectileSpeed = (FMath::FRand() * FMTD_WeaponItemDataRow.ProjectileSpeedLimit);
        }
    }
}

static void GenerateWeaponItemData(FMTD_GenerationParameters &Params, bool bRandomizeStats)
{
    auto WeaponItemData = Cast<UMTD_WeaponItemData>(Params.BaseItemData);

    const UDataTable *WeaponDataTable = Params.GameInstance->GetWeaponDataTable();
    if (!IsValid(WeaponDataTable))
    {
        MTD_WARN("Weapon Data Table is invalid.");
        return;
    }

    const auto Row = WeaponDataTable->FindRow<FMTD_WeaponItemDataRow>(Params.RowID, nullptr, false);
    if (!Row)
    {
        MTD_WARN("Item ID [%s] could not be found in Weapon Item Data Table.", *Params.RowID.GetPlainNameString());
        return;
    }

    DispatchWeaponItemData(Params, *WeaponItemData, *Row);

    if (bRandomizeStats)
    {
        RandomizeWeaponItemData(Params, *WeaponItemData, *Row);
    }
}



/**
 * Material item generation.
 */

static void DispatchMaterialItemData(FMTD_GenerationParameters &Params, UMTD_MaterialItemData &MaterialItemData,
    const FMTD_MaterialItemDataRow &MaterialItemDataRow)
{
    MaterialItemData.MaxAmount = MaterialItemDataRow.MaxAmount;
}

static void RandomizeMaterialItemData(FMTD_GenerationParameters &Params, UMTD_MaterialItemData &MaterialItemData,
    const FMTD_MaterialItemDataRow &FMTD_MaterialItemDataRow)
{
}

static void GenerateMaterialItemData(FMTD_GenerationParameters &Params, bool bRandomizeStats)
{
    auto MaterialItemData = Cast<UMTD_MaterialItemData>(Params.BaseItemData);

    const UDataTable *MaterialDataTable = Params.GameInstance->GetMaterialDataTable();
    if (!IsValid(MaterialDataTable))
    {
        MTD_WARN("Material Data Table is invalid.");
        return;
    }

    const auto Row = MaterialDataTable->FindRow<FMTD_MaterialItemDataRow>(Params.RowID, nullptr, false);
    if (!Row)
    {
        MTD_WARN("Item ID [%s] could not be found in Material Item Data Table.", *Params.RowID.GetPlainNameString());
        return;
    }

    DispatchMaterialItemData(Params, *MaterialItemData, *Row);

    if (bRandomizeStats)
    {
        RandomizeMaterialItemData(Params, *MaterialItemData, *Row);
    }
}



/**
 * Helper functions.
 */

static TSubclassOf<UMTD_BaseInventoryItemData> GetItemDataType(EMTD_InventoryItemType InventoryItemType)
{
    switch (InventoryItemType)
    {
    case EMTD_InventoryItemType::III_Armor:
        return UMTD_ArmorItemData::StaticClass();
    case EMTD_InventoryItemType::III_Weapon:
        return UMTD_WeaponItemData::StaticClass();
    case EMTD_InventoryItemType::III_Material:
        return UMTD_MaterialItemData::StaticClass();
    default:
        return nullptr;
    }
}

static bool GenerateSpecificItemData(FMTD_GenerationParameters &Params, bool bRandomizeStats)
{
    switch (Params.BaseItemData->ItemType)
    {
    case EMTD_InventoryItemType::III_Armor:
        GenerateArmorItemData(Params, bRandomizeStats);
        break;
    case EMTD_InventoryItemType::III_Weapon:
        GenerateWeaponItemData(Params, bRandomizeStats);
        break;
    case EMTD_InventoryItemType::III_Material:
        GenerateMaterialItemData(Params, bRandomizeStats);
        break;
    default:
        MTD_WARN("Unknown item type [%s].", *UEnum::GetValueAsString(Params.BaseItemData->ItemType));
        return false;
    }

    return true;
}


/**
 * Base item generation.
 */

static bool DispatchBaseItemData(FMTD_GenerationParameters &Params)
{
    const FMTD_BaseInventoryItemDataRow &BaseItemDataRow = *Params.BaseItemDataRow;
    Params.BaseItemData->ItemID = FCString::Atoi(*Params.RowID.ToString());
    Params.BaseItemData->ItemType = BaseItemDataRow.ItemType;
    Params.BaseItemData->InventoryItemInstanceActorClass = BaseItemDataRow.InventoryItemInstanceActorClass;
    Params.BaseItemData->StaticMesh = BaseItemDataRow.StaticMesh;
    Params.BaseItemData->Thumbnail = BaseItemDataRow.Thumbnail;
    Params.BaseItemData->Name = BaseItemDataRow.Name;
    Params.BaseItemData->Description = BaseItemDataRow.Description;

    return true;
}

static bool CreateGenerationParameters(FMTD_GenerationParameters &OutParams, UObject *WorldContextObject,
    int32 ItemID, float Difficulty)
{
    if (!IsValid(WorldContextObject))
    {
        MTD_WARN("World Context Object is invalid.");
        return false;
    }
    
    const UWorld *World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (!IsValid(World))
    {
        MTD_WARN("World is invalid.");
        return false;
    }

    const auto MtdGameInstance = Cast<UMTD_GameInstance>(UGameplayStatics::GetGameInstance(World));
    if (!IsValid(MtdGameInstance))
    {
        MTD_WARN("MTD Game Instance is invalid.");
        return false;
    }

    const UDataTable *BasefalseTable = MtdGameInstance->GetBaseItemDataTable();
    if (!IsValid(BasefalseTable))
    {
        MTD_WARN("Base Item Data Table is invalid.");
        return false;
    }

    const FName RowID = *FString::FormatAsNumber(ItemID);
    const auto Row = BasefalseTable->FindRow<FMTD_BaseInventoryItemDataRow>(RowID, nullptr, false);
    if (!Row)
    {
        MTD_WARN("Item ID [%s] could not be found in Base Item Data Table.", *RowID.GetPlainNameString());
        return false;
    }

    const TSubclassOf<UMTD_BaseInventoryItemData> ItemDataType = GetItemDataType(Row->ItemType);
    if (!ItemDataType)
    {
        MTD_WARN("Item Data Type is invalid.");
        return false;
    }

    auto ItemData = NewObject<UMTD_BaseInventoryItemData>(WorldContextObject, ItemDataType);
    if (!IsValid(ItemData))
    {
        MTD_WARN("Failed to create a Inventory Item Data.");
        return false;
    }

    OutParams = { WorldContextObject, ItemID, RowID, 0.f, ItemData, MtdGameInstance, Row };
    return true;
}

static UMTD_BaseInventoryItemData *GenerateInventoryItemData_Implementation(
    UObject *WorldContextObject, int32 ItemID, float Difficulty, bool bRandomizeStats)
{
    FMTD_GenerationParameters GenerationParams;
    if (!CreateGenerationParameters(GenerationParams, WorldContextObject, ItemID, Difficulty))
    {
        MTD_WARN("Failed to create generation parameters with Item ID [%d].", ItemID);
        return nullptr;
    }

    DispatchBaseItemData(GenerationParams);
    GenerateSpecificItemData(GenerationParams, bRandomizeStats);

    return GenerationParams.BaseItemData;
}

UMTD_BaseInventoryItemData *UMTD_InventoryBlueprintFunctionLibrary::GenerateInventoryItemData(
    UObject *WorldContextObject, int32 ItemID, float Difficulty)
{
    return GenerateInventoryItemData_Implementation(WorldContextObject, ItemID, Difficulty, true);
}

UMTD_BaseInventoryItemData *UMTD_InventoryBlueprintFunctionLibrary::CreateBaseInventoryItemData(
    UObject *WorldContextObject, int32 ItemID)
{
    return GenerateInventoryItemData_Implementation(WorldContextObject, ItemID, 0.f, false);
}

AMTD_InventoryItemInstance *UMTD_InventoryBlueprintFunctionLibrary::CreateItemInstance(
    AActor *WorldContextObject, UMTD_BaseInventoryItemData *ItemData)
{
    if (!IsValid(ItemData))
    {
        MTD_WARN("Item Data is invalid.");
        return nullptr;
    }

    UWorld *World = WorldContextObject->GetWorld();
    if (!IsValid(World))
    {
        MTD_WARN("World is invalid.");
        return nullptr;
    }

    const FTransform Transform = WorldContextObject->GetTransform();
    auto ItemInstance = World->SpawnActorDeferred<AMTD_InventoryItemInstance>(
        AMTD_InventoryItemInstance::StaticClass(), Transform, nullptr, nullptr,
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

    if (!IsValid(ItemInstance))
    {
        MTD_WARN("Item Instance could not be spawned.");
        return nullptr;
    }

    ItemInstance->SetItemData(ItemData);
    UGameplayStatics::FinishSpawningActor(ItemInstance, Transform);

    return ItemInstance;
}

EMTD_EquipmentType UMTD_InventoryBlueprintFunctionLibrary::GetEquipmentType(UMTD_BaseInventoryItemData *ItemData)
{
    EMTD_EquipmentType EquipmentType = EMTD_EquipmentType::Invalid;

    if (ItemData->IsA(UMTD_ArmorItemData::StaticClass()))
    {
        const auto ArmorItemData = Cast<UMTD_ArmorItemData>(ItemData);
        EquipmentType = static_cast<EMTD_EquipmentType>(ArmorItemData->ArmorType);
    }

    if (ItemData->IsA(UMTD_WeaponItemData::StaticClass()))
    {
        EquipmentType = EMTD_EquipmentType::Weapon;
    }

    return EquipmentType;
}

bool UMTD_InventoryBlueprintFunctionLibrary::IsEquippable(UMTD_BaseInventoryItemData *ItemData)
{
    const auto EquipItemData = Cast<UMTD_EquippableItemData>(ItemData);
    if (!IsValid(EquipItemData))
    {
        return false;
    }
    
    const EMTD_EquipmentType EquipmentType = GetEquipmentType(ItemData);
    const bool bResult = (EquipmentType != EMTD_EquipmentType::Invalid);
    return bResult;
}

int32 UMTD_InventoryBlueprintFunctionLibrary::GetItemAmount(UMTD_BaseInventoryItemData *ItemData)
{
    if (!IsValid(ItemData))
    {
        MTD_WARN("Item Data is invalid.");
        return -1;
    }

    const auto MaterialItemData = Cast<UMTD_MaterialItemData>(ItemData);
    if (!IsValid(MaterialItemData))
    {
        return -1;
    }

    return MaterialItemData->CurrentAmount;
}

int32 UMTD_InventoryBlueprintFunctionLibrary::GetMaxItemAmount(UMTD_BaseInventoryItemData *ItemData)
{
    if (!IsValid(ItemData))
    {
        MTD_WARN("Item Data is invalid.");
        return -1;
    }

    const auto MaterialItemData = Cast<UMTD_MaterialItemData>(ItemData);
    if (!IsValid(MaterialItemData))
    {
        return -1;
    }

    return MaterialItemData->MaxAmount;
}
