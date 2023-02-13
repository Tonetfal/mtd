#include "Inventory/Items/MTD_InventoryBlueprintFunctionLibrary.h"

#include "Character/MTD_BaseFoeCharacter.h"
#include "GameModes/MTD_TowerDefenseMode.h"
#include "Inventory/Items/MTD_ArmorItemData.h"
#include "Inventory/Items/MTD_BaseInventoryItemData.h"
#include "Inventory/Items/MTD_ItemDataManager.h"
#include "Inventory/Items/MTD_ItemDropManager.h"
#include "Inventory/Items/MTD_MaterialItemData.h"
#include "Inventory/Items/MTD_WeaponItemData.h"
#include "Inventory/MTD_InventoryItemInstance.h"
#include "Kismet/GameplayStatics.h"

/**
 * Generation parameters to pass around.
 */
struct FMTD_GenerationParameters
{
public:
    UObject *WorldContextObject = nullptr;
    int32 ItemID = 0;
    FName RowID;
    float Difficulty = 0.f;
    UMTD_BaseInventoryItemData *BaseItemData = nullptr;
    FMTD_BaseInventoryItemDataRow *BaseItemDataRow = nullptr;
};



/**
 * Equippable item generation.
 */

/**
 * Generate randomizable stats
 */
static void RandomizeEquippableItemData(FMTD_GenerationParameters &Params, UMTD_EquipItemData &EquipItemData)
{
    // @todo make use of Difficulty parameter
    EquipItemData.EquippableItemParameters.PlayerHealth = FMath::RandRange(-100, 100);
    EquipItemData.EquippableItemParameters.PlayerDamage = FMath::RandRange(-100, 100);
    EquipItemData.EquippableItemParameters.PlayerSpeed = FMath::RandRange(-100, 100);
    
    EquipItemData.EquippableItemParameters.TowerHealth = FMath::RandRange(-100, 100);
    EquipItemData.EquippableItemParameters.TowerDamage = FMath::RandRange(-100, 100);
    EquipItemData.EquippableItemParameters.TowerRange = FMath::RandRange(-100, 100);
    EquipItemData.EquippableItemParameters.TowerSpeed = FMath::RandRange(-100, 100);
}



/**
 * Armor item generation.
 */

/**
 * Dispatch armor templated data.
 */
static void DispatchArmorItemData(FMTD_GenerationParameters &Params, UMTD_ArmorItemData &ArmorItemData,
    const FMTD_ArmorItemDataRow &ArmorItemDataRow)
{
    ArmorItemData.ArmorType = ArmorItemDataRow.ArmorType;
    ArmorItemData.HeroClasses = ArmorItemDataRow.HeroClasses;
}

/**
 * Generate randomizable stats
 */
static void RandomizeArmorItemData(FMTD_GenerationParameters &Params, UMTD_ArmorItemData &ArmorItemData,
    FMTD_ArmorItemDataRow &ArmorItemDataRow)
{
    RandomizeEquippableItemData(Params, ArmorItemData);
    
    // @todo make use of Difficulty parameter
    ArmorItemData.ArmorItemParameters.PhysicalResistance = FMath::RandRange(-10, 10);
    ArmorItemData.ArmorItemParameters.FireResistance = FMath::RandRange(-10, 10);
    ArmorItemData.ArmorItemParameters.DarkResistance = FMath::RandRange(-10, 10);
    ArmorItemData.ArmorItemParameters.StatusResistance = FMath::RandRange(-10, 10);
}

/**
 * Dispatch and generate armor related data.
 */
static void GenerateArmorItemData(FMTD_GenerationParameters &Params, bool bRandomizeStats)
{
    auto ArmorItemData = CastChecked<UMTD_ArmorItemData>(Params.BaseItemData);

    const UDataTable *ArmorDataTable = UMTD_ItemDataManager::GetArmorDataTable();
    if (!IsValid(ArmorDataTable))
    {
        MTD_WARN("Armor data table is invalid.");
        return;
    }

    // Find template item data
    const auto Row = ArmorDataTable->FindRow<FMTD_ArmorItemDataRow>(Params.RowID, nullptr, false);
    if (!Row)
    {
        MTD_WARN("Item ID [%s] could not be found in armor item data table.", *Params.RowID.GetPlainNameString());
        return;
    }

    // Dispatch templated data
    DispatchArmorItemData(Params, *ArmorItemData, *Row);

    if (bRandomizeStats)
    {
        // Generate randomizable stats
        RandomizeArmorItemData(Params, *ArmorItemData, *Row);
    }
}



/**
 * Weapon item generation.
 */

/**
 * Dispatch templated data
 */
static void DispatchWeaponItemData(FMTD_GenerationParameters &Params, UMTD_WeaponItemData &WeaponItemData,
    const FMTD_WeaponItemDataRow &WeaponItemDataRow)
{
    WeaponItemData.WeaponType = WeaponItemDataRow.WeaponType;
    WeaponItemData.HeroClasses = WeaponItemDataRow.HeroClasses;
    WeaponItemData.EquipmentDefinitionAsset = WeaponItemDataRow.EquipmentDefinitionAsset;
}

/**
 * Generate randomizable stats
 */
static void RandomizeWeaponItemData(FMTD_GenerationParameters &Params, UMTD_WeaponItemData &WeaponItemData,
    const FMTD_WeaponItemDataRow &FMTD_WeaponItemDataRow)
{
    RandomizeEquippableItemData(Params, WeaponItemData);
    
    // @todo make use of Difficulty parameter
    if (WeaponItemData.WeaponType & IWT_Melee)
    {
        // Randomize melee item stats
        WeaponItemData.WeaponItemParameters.MeleeDamage = FMath::RandRange(0, 100);
    }

    if (WeaponItemData.WeaponType & IWT_Ranged)
    {
        // Randomize ranged item stats
        WeaponItemData.WeaponItemParameters.RangedDamage = FMath::RandRange(0, 100);
        
        if (FMTD_WeaponItemDataRow.ProjectilesLimit > 0)
        {
            WeaponItemData.WeaponItemParameters.Projectiles =
                FMath::RandRange(1, FMTD_WeaponItemDataRow.ProjectilesLimit);
        }
        
        if (FMTD_WeaponItemDataRow.ProjectileSpeedLimit > 0)
        {
            WeaponItemData.WeaponItemParameters.ProjectileSpeed =
                (FMath::FRand() * FMTD_WeaponItemDataRow.ProjectileSpeedLimit);
        }
        
        if (FMTD_WeaponItemDataRow.bRadial)
        {
            WeaponItemData.WeaponItemParameters.bRadial = true;
            WeaponItemData.WeaponItemParameters.RadialRange =
                (FMath::FRand() * FMTD_WeaponItemDataRow.RadialRangeLimit);
        }
    }
}

/**
 * Dispatch and generate weapon related data.
 */
static void GenerateWeaponItemData(FMTD_GenerationParameters &Params, bool bRandomizeStats)
{
    auto WeaponItemData = CastChecked<UMTD_WeaponItemData>(Params.BaseItemData);

    const UDataTable *WeaponDataTable = UMTD_ItemDataManager::GetWeaponDataTable();
    if (!IsValid(WeaponDataTable))
    {
        MTD_WARN("Weapon data table is invalid.");
        return;
    }

    // Find template item data
    const auto Row = WeaponDataTable->FindRow<FMTD_WeaponItemDataRow>(Params.RowID, nullptr, false);
    if (!Row)
    {
        MTD_WARN("Item ID [%s] could not be found in weapon item data table.", *Params.RowID.GetPlainNameString());
        return;
    }

    // Dispatch templated data
    DispatchWeaponItemData(Params, *WeaponItemData, *Row);

    if (bRandomizeStats)
    {
        // Generate randomizable stats
        RandomizeWeaponItemData(Params, *WeaponItemData, *Row);
    }
}



/**
 * Material item generation.
 */

/**
 * Dispatch templated data
 */
static void DispatchMaterialItemData(FMTD_GenerationParameters &Params, UMTD_MaterialItemData &MaterialItemData,
    const FMTD_MaterialItemDataRow &MaterialItemDataRow)
{
    MaterialItemData.MaxAmount = MaterialItemDataRow.MaxAmount;
}

/**
 * Generate randomizable stats
 */
static void RandomizeMaterialItemData(FMTD_GenerationParameters &Params, UMTD_MaterialItemData &MaterialItemData,
    const FMTD_MaterialItemDataRow &FMTD_MaterialItemDataRow)
{
    // Nothing at the moment
}

/**
 * Dispatch and generate material related data.
 */
static void GenerateMaterialItemData(FMTD_GenerationParameters &Params, bool bRandomizeStats)
{
    auto MaterialItemData = CastChecked<UMTD_MaterialItemData>(Params.BaseItemData);

    const UDataTable *MaterialDataTable = UMTD_ItemDataManager::GetMaterialDataTable();
    if (!IsValid(MaterialDataTable))
    {
        MTD_WARN("Material data table is invalid.");
        return;
    }

    // Find template item data
    const auto Row = MaterialDataTable->FindRow<FMTD_MaterialItemDataRow>(Params.RowID, nullptr, false);
    if (!Row)
    {
        MTD_WARN("Item ID [%s] could not be found in material item data table.", *Params.RowID.GetPlainNameString());
        return;
    }

    // Dispatch templated data
    DispatchMaterialItemData(Params, *MaterialItemData, *Row);

    if (bRandomizeStats)
    {
        // Generate randomizable stats
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

/**
 * Simple generation function selector.
 */
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

/**
 * Dispatch templated data.
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

/**
 * Prepare generation parameters to use to create and generate an inventory item.
 */
static bool CreateGenerationParameters(FMTD_GenerationParameters &OutParams, UObject *WorldContextObject,
    int32 ItemID, float Difficulty)
{
    if (!IsValid(WorldContextObject))
    {
        MTD_WARN("World context object is invalid.");
        return false;
    }
    
    const UWorld *World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (!IsValid(World))
    {
        MTD_WARN("World is invalid.");
        return false;
    }

    const UDataTable *BaseItemDataTable = UMTD_ItemDataManager::GetBaseItemDataTable();
    if (!IsValid(BaseItemDataTable))
    {
        MTD_WARN("Base item data table is invalid.");
        return false;
    }

    const FName RowID = *FString::FormatAsNumber(ItemID);
    const auto Row = BaseItemDataTable->FindRow<FMTD_BaseInventoryItemDataRow>(RowID, nullptr, false);
    if (!Row)
    {
        MTD_WARN("Item ID [%s] could not be found in base item data table.", *RowID.GetPlainNameString());
        return false;
    }

    const TSubclassOf<UMTD_BaseInventoryItemData> &ItemDataType = GetItemDataType(Row->ItemType);
    if (!ItemDataType)
    {
        MTD_WARN("Item data type is invalid.");
        return false;
    }

    auto ItemData = NewObject<UMTD_BaseInventoryItemData>(WorldContextObject, ItemDataType);
    if (!IsValid(ItemData))
    {
        MTD_WARN("Failed to create a item data.");
        return false;
    }

    OutParams = { WorldContextObject, ItemID, RowID, 0.f, ItemData, Row };
    return true;
}

static UMTD_BaseInventoryItemData *GenerateInventoryItemData_Implementation(
    UObject *WorldContextObject, int32 ItemID, float Difficulty, bool bRandomizeStats)
{
    FMTD_GenerationParameters GenerationParams;
    if (!CreateGenerationParameters(GenerationParams, WorldContextObject, ItemID, Difficulty))
    {
        MTD_WARN("Failed to create generation parameters with item ID [%d].", ItemID);
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

UMTD_BaseInventoryItemData *UMTD_InventoryBlueprintFunctionLibrary::GenerateDropItem(
    AMTD_BaseFoeCharacter *WorldContextObject)
{
    if (!IsValid(WorldContextObject))
    {
        MTD_WARN("World context object is invalid.");
        return nullptr;
    }

    const UWorld *World = WorldContextObject->GetWorld();
    if (!IsValid(World))
    {
        MTD_WARN("World is invalid.");
        return nullptr;
    }

    const auto TowerDefenseMode = Cast<AMTD_TowerDefenseMode>(UGameplayStatics::GetGameMode(World));
    if (!IsValid(TowerDefenseMode))
    {
        MTD_WARN("Tower defense mode is invalid.");
        return nullptr;
    }
    
    const UMTD_ItemDropManager *ItemDrops = UMTD_ItemDropManager::Get();
    if (!IsValid(ItemDrops))
    {
        MTD_WARN("Item drops is invalid.");
        return nullptr;
    }

    // Randomize item ID for this class
    int32 ItemID = 0;
    const TSubclassOf<AMTD_BaseFoeCharacter> &CharacterClass = WorldContextObject->GetClass();
    if (!ItemDrops->GetRandomItemID(CharacterClass, ItemID))
    {
        return nullptr;
    }

    // Get scale value
    const float Difficulty = TowerDefenseMode->GetScaledDifficulty();

    // Randomize item
    UMTD_BaseInventoryItemData *ItemData = UMTD_InventoryBlueprintFunctionLibrary::GenerateInventoryItemData(
        WorldContextObject, ItemID, Difficulty);

    return ItemData;
}

AMTD_InventoryItemInstance *UMTD_InventoryBlueprintFunctionLibrary::GenerateDropItemInstance(
    AMTD_BaseFoeCharacter *WorldContextObject, bool bWarnIfFailed)
{
    UMTD_BaseInventoryItemData *ItemData = GenerateDropItem(WorldContextObject);
    if (!IsValid(ItemData))
    {
        if (bWarnIfFailed)
        {
            MTD_WARN("Item data is invalid.");
        }
        
        return nullptr;
    }

    AMTD_InventoryItemInstance *ItemInstance = UMTD_InventoryBlueprintFunctionLibrary::CreateItemInstance(
        WorldContextObject, ItemData);

    return ItemInstance;
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
        MTD_WARN("Item data is invalid.");
        return nullptr;
    }

    UWorld *World = WorldContextObject->GetWorld();
    if (!IsValid(World))
    {
        MTD_WARN("World is invalid.");
        return nullptr;
    }

    const FTransform Transform = WorldContextObject->GetTransform();
    constexpr auto CollsiionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    
    // Spawn item
    auto ItemInstance = World->SpawnActorDeferred<AMTD_InventoryItemInstance>(
        AMTD_InventoryItemInstance::StaticClass(), Transform, nullptr, nullptr, CollsiionHandlingMethod);

    if (!IsValid(ItemInstance))
    {
        MTD_WARN("Failed to spawn item instance.");
        return nullptr;
    }

    // Set item data before running initialization on item itself
    ItemInstance->SetItemData(ItemData);

    // Finish spawning
    ItemInstance->FinishSpawning(Transform);

    return ItemInstance;
}

EMTD_EquipmentType UMTD_InventoryBlueprintFunctionLibrary::GetEquipmentType(const UMTD_BaseInventoryItemData *ItemData)
{
    EMTD_EquipmentType EquipmentType = EMTD_EquipmentType::Invalid;

    if (ItemData->IsA(UMTD_ArmorItemData::StaticClass()))
    {
        const auto ArmorItemData = Cast<UMTD_ArmorItemData>(ItemData);
        EquipmentType = static_cast<EMTD_EquipmentType>(ArmorItemData->ArmorType.GetValue());
    }

    if (ItemData->IsA(UMTD_WeaponItemData::StaticClass()))
    {
        EquipmentType = EMTD_EquipmentType::Weapon;
    }

    return EquipmentType;
}

bool UMTD_InventoryBlueprintFunctionLibrary::IsEquippable(const UMTD_BaseInventoryItemData *ItemData)
{
    const auto EquipItemData = Cast<UMTD_EquipItemData>(ItemData);
    if (!IsValid(EquipItemData))
    {
        return false;
    }
    
    const EMTD_EquipmentType EquipmentType = GetEquipmentType(ItemData);
    const bool bResult = (EquipmentType != EMTD_EquipmentType::Invalid);
    return bResult;
}

int32 UMTD_InventoryBlueprintFunctionLibrary::GetItemAmount(const UMTD_BaseInventoryItemData *ItemData)
{
    if (!IsValid(ItemData))
    {
        MTD_WARN("Item data is invalid.");
        return -1;
    }

    const auto MaterialItemData = Cast<UMTD_MaterialItemData>(ItemData);
    if (!IsValid(MaterialItemData))
    {
        return -1;
    }

    return MaterialItemData->CurrentAmount;
}

int32 UMTD_InventoryBlueprintFunctionLibrary::GetMaxItemAmount(const UMTD_BaseInventoryItemData *ItemData)
{
    if (!IsValid(ItemData))
    {
        MTD_WARN("Item data is invalid.");
        return -1;
    }

    const auto MaterialItemData = Cast<UMTD_MaterialItemData>(ItemData);
    if (!IsValid(MaterialItemData))
    {
        return -1;
    }

    return MaterialItemData->MaxAmount;
}
