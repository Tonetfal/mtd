#pragma once

#include "GameplayTagContainer.h"
#include "MTD_BaseInventoryItemData.h"

#include "MTD_EquippableItemData.generated.h"

class UMTD_EquipmentDefinitionAsset;

UCLASS(Abstract)
class MTD_API UMTD_EquippableItemData
    :  public UMTD_BaseInventoryItemData
{
   GENERATED_BODY()

public:
    /**
     * Main config.
     */

    /** Minimal level a character is required to equip the item. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config")
    int32 LevelRequired = 0;

    /** Hero classes that can use the item. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config")
    FGameplayTagContainer HeroClasses;

    /**
     * Visuals.
     */

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Preview")
    TObjectPtr<UMTD_EquipmentDefinitionAsset> EquipmentDefinitionAsset = nullptr;

    /**
     * Player attributes.
     */
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes|Player")
    float PlayerHealth = 0.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes|Player")
    float PlayerDamage = 0.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes|Player")
    float PlayerSpeed = 0.f;

    /**
     * Tower attributes.
     */
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes|Tower")
    float TowerHealth = 0.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes|Tower")
    float TowerDamage = 0.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes|Tower")
    float TowerRange = 0.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes|Tower")
    float TowerSpeed = 0.f;
};
