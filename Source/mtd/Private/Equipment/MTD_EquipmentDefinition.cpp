#include "Equipment/MTD_EquipmentDefinition.h"

#include "Equipment/MTD_EquipmentInstance.h"

UMTD_EquipmentDefinitionAsset::UMTD_EquipmentDefinitionAsset()
{
    // Use the base one by default
    EquipmentInstanceClass = UMTD_EquipmentInstance::StaticClass();
}
