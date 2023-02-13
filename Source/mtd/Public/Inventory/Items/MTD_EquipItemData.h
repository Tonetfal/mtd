#pragma once

#include "GameplayTagContainer.h"
#include "MTD_BaseInventoryItemData.h"

#include "MTD_EquipItemData.generated.h"

class UMTD_EquipmentDefinitionAsset;

/**
 * Simple structure containing all equipment generated data.
 */
USTRUCT(BlueprintType)
struct FMTD_EquipItemParameters
{
    GENERATED_BODY()

public:
    /**
     * Main config.
     */
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config")
    int32 LevelRequired = 0;

    /**
     * Player attributes.
     */

    /** Bonus player health stat attribute armor gives. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes|Player")
    float PlayerHealth = 0.f;
    
    /** Bonus player damage stat attribute armor gives. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes|Player")
    float PlayerDamage = 0.f;
    
    /** Bonus player speed stat attribute armor gives. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes|Player")
    float PlayerSpeed = 0.f;

    /**
     * Tower attributes.
     */
    
    /** Bonus tower health stat attribute armor gives. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes|Tower")
    float TowerHealth = 0.f;
    
    /** Bonus tower damage stat attribute armor gives. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes|Tower")
    float TowerDamage = 0.f;
    
    /** Bonus tower range stat attribute armor gives. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes|Tower")
    float TowerRange = 0.f;
    
    /** Bonus tower speed stat attribute armor gives. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes|Tower")
    float TowerSpeed = 0.f;
};

/**
 * Equip item data containing specific equip information.
 */
UCLASS(Abstract)
class MTD_API UMTD_EquipItemData
    :  public UMTD_BaseInventoryItemData
{
   GENERATED_BODY()

public:
    /**
     * Main config.
     */

    /** Hero classes that can use the item. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config")
    FGameplayTagContainer HeroClasses;

    /** Item generated parameters. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FMTD_EquipItemParameters EquippableItemParameters;

    /**
     * Visuals.
     */

    /** Visual representation of equipment on character. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Preview")
    TObjectPtr<UMTD_EquipmentDefinitionAsset> EquipmentDefinitionAsset = nullptr;
};
