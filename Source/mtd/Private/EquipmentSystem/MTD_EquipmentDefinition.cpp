#include "EquipmentSystem/MTD_EquipmentDefinition.h"

#include "EquipmentSystem/MTD_EquipmentInstance.h"

UMTD_EquipmentDefinitionAsset::UMTD_EquipmentDefinitionAsset()
{
    // Use the base one by default
    EquipmentInstanceClass = UMTD_EquipmentInstance::StaticClass();
}
