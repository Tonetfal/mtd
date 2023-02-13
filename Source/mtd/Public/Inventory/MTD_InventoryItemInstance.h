#pragma once

#include "Interaction/MTD_InteractActor.h"
#include "Inventory/Items/MTD_BaseInventoryItemData.h"
#include "mtd.h"

#include "MTD_InventoryItemInstance.generated.h"

class UBoxComponent;

/**
 * Actor instance of an existing item in world.
 * 
 * It's a frontend means to use items that have been dropped on the floor.
 */
UCLASS()
class MTD_API AMTD_InventoryItemInstance
    : public AMTD_InteractActor
{
    GENERATED_BODY()

public:
    inline static FName EquipInteractionID = "Equip";

public:
    AMTD_InventoryItemInstance();

    /**
     * Set item data the instance has to visualize.
     * @param   InItemData: item data to set.
     */
    void SetItemData(UMTD_BaseInventoryItemData *InItemData);

    //~IMTD_Interactle Interface
    virtual bool CanInteract_Implementation(AMTD_BaseCharacter *Character, const FName InteractionID) override;

protected:
    virtual bool Interact_Implementation(AMTD_BaseCharacter *Character, const FName InteractionID) override;
    //~End of IMTD_Interactle Interface
    
    //~End of AActor Interface
    virtual void OnConstruction(const FTransform &Transform) override;
    virtual void BeginPlay() override;
    //~End of AActor Interface

private:
    /** Object mesh. */
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UStaticMeshComponent> StaticMeshComponent = nullptr;

    /** Object collision box. */
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UBoxComponent> BoxComponent = nullptr;

    /** Backend item. */
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true", ExposeOnSpawn))
    TObjectPtr<UMTD_BaseInventoryItemData> ItemData = nullptr;
};

inline void AMTD_InventoryItemInstance::SetItemData(UMTD_BaseInventoryItemData *InItemData)
{
    ItemData = InItemData;
}
