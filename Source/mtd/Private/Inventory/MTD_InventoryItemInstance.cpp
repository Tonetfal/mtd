#include "Inventory/MTD_InventoryItemInstance.h"

#include "Character/MTD_BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Equipment/MTD_EquipmentInstance.h"
#include "Equipment/MTD_EquipmentManagerComponent.h"
#include "GameFramework/PlayerState.h"
#include "Inventory/Items/MTD_EquippableItemData.h"
#include "Inventory/Items/MTD_InventoryBlueprintFunctionLibrary.h"
#include "Inventory/MTD_InventoryManagerComponent.h"
#include "System/MTD_Tags.h"

AMTD_InventoryItemInstance::AMTD_InventoryItemInstance()
{
    BoxComponent = CreateDefaultSubobject<UBoxComponent>("Box Component");
    SetRootComponent(BoxComponent);
    BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    BoxComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    BoxComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    BoxComponent->SetCollisionObjectType(ECC_WorldDynamic);
    BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BoxComponent->InitBoxExtent(FVector(32.f));
    
    StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh Component");
    StaticMeshComponent->SetupAttachment(GetRootComponent());
    StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    Tags.Add(FMTD_Tags::Interactable);
}

void AMTD_InventoryItemInstance::BeginPlay()
{
    Super::BeginPlay();
    
    if (!IsValid(ItemData))
    {
        MTDS_WARN("Item Data is invalid.");
        return;
    }

    UStaticMesh *StaticMesh = ItemData->StaticMesh.Get();
    if (IsValid(StaticMesh))
    {
        StaticMeshComponent->SetStaticMesh(StaticMesh);
    }
    else
    {
        MTDS_WARN("Item Data Static Mesh is invalid.");
    }

    // There should be no non-positive IDs
    ensure(ItemData->ItemID > 0);
}

bool AMTD_InventoryItemInstance::CanInteract_Implementation(AMTD_BaseCharacter *Character, const FName InteractionID)
{
    const bool bIsLootInteraction = (InteractionID == DefaultInteractionID);
    const bool bIsEquipInteraction = (InteractionID == EquipInteractionID);
    if (((!bIsLootInteraction) && (!bIsEquipInteraction)))
    {
        return false;
    }
    
    if (!IsValid(Character))
    {
        return false;
    }

    if (!IsValid(ItemData))
    {
        return false;
    }
    
    const APlayerState *Ps = Character->GetPlayerState();
    if (!IsValid(Ps))
    {
        return false;
    }
    
    const auto InventoryManagerComponent = UMTD_InventoryManagerComponent::FindInventoryManagerComponent(Ps);
    if (!IsValid(InventoryManagerComponent))
    {
        return false;
    }

    if (bIsLootInteraction)
    {
        return InventoryManagerComponent->CanAddItem(ItemData);
    }

    // -- End of Regular Interaction, the rest is Equip Interaction
    
    const auto EquipItemData = Cast<UMTD_EquippableItemData>(ItemData);
    if (!IsValid(EquipItemData))
    {
        return false;
    }
    
    const auto EquipmentManagerComponent = UMTD_EquipmentManagerComponent::FindEquipmentManagerComponent(Ps);
    if (!IsValid(EquipmentManagerComponent))
    {
        return false;
    }

    // If item can be equipped, then we always can interact this way with it. The currently used equipment type will be
    // transfered into inventory if it can, otherwise it will be dropped on the floor
    const bool bCanEquipItem = EquipmentManagerComponent->CanEquipItem(ItemData);
    return bCanEquipItem;
}

bool AMTD_InventoryItemInstance::Interact_Implementation(AMTD_BaseCharacter *Character, const FName InteractionID)
{
    const bool bIsLootInteraction = (InteractionID == DefaultInteractionID);
    const bool bIsEquipInteraction = (InteractionID == EquipInteractionID);
    if (((!bIsLootInteraction) && (!bIsEquipInteraction)))
    {
        MTDS_WARN("Interaction ID [%s] is unknown.", *InteractionID.ToString());
        return false;
    }
    
    check(IsValid(Character));
    check(IsValid(ItemData));

    APlayerState *Ps = Character->GetPlayerState();
    check(IsValid(Ps));
    
    bool bShouldDestroy;
    
    // Loot interaction
    if (bIsLootInteraction)
    {
        auto InventoryManagerComponent = UMTD_InventoryManagerComponent::FindInventoryManagerComponent(Ps);
        check(IsValid(InventoryManagerComponent));

        const bool bAdded = InventoryManagerComponent->AddItem(ItemData);
        bShouldDestroy = bAdded;
        ensureAlways(bAdded);
    }
    
    // Equip interaction
    else
    {
        auto EquipmentManagerComponent = UMTD_EquipmentManagerComponent::FindEquipmentManagerComponent(Ps);
        check(IsValid(EquipmentManagerComponent));

        // Equip item
        const UMTD_EquipmentInstance *EquipmentInstance = EquipmentManagerComponent->EquipItem(ItemData);
        const bool bEquipped = IsValid(EquipmentInstance);
        bShouldDestroy = bEquipped;
        ensureAlways(bEquipped);
        
        // ensure(EquipmentManagerComponent->CanEquipItem(ItemData));
        //
        // auto EquipItemData = Cast<UMTD_EquippableItemData>(ItemData);
        // check(IsValid(EquipItemData));
        //
        // const EMTD_EquipmentType EquipmentType = UMTD_InventoryBlueprintFunctionLibrary::GetEquipmentType(ItemData);
        // if (EquipmentManagerComponent->IsItemTypeEquipped(EquipmentType))
        // {
        //     // Remove the current equipment, try to add it to the inventory; the item may be dropped on the floor
        //     UMTD_EquippableItemData *UnequippedItemData = EquipmentManagerComponent->UnequipItemType(EquipmentType);
        //     check(IsValid(UnequippedItemData));
        //     
        //     InventoryManagerComponent->AddItem(UnequippedItemData, true);
        // }
        //
        // // Previously CanEquipItem was checked along other validness things, hence it should never fail
        // const UMTD_EquipmentInstance *EquipmentInstance =  EquipmentManagerComponent->EquipItem(ItemData);
        // const bool bEquipped = IsValid(EquipmentInstance);
        // bShouldDestroy = bEquipped;
    }

    // Something could fail due some errors prior to runtime, such as null or invalid fields in a database
    if (bShouldDestroy)
    {
        Destroy();
    }
    
    return bShouldDestroy;
}
