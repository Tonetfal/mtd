#include "InventorySystem/MTD_InventoryItemInstance.h"

#include "Character/MTD_BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "EquipmentSystem/MTD_EquipmentInstance.h"
#include "EquipmentSystem/MTD_EquipmentManagerComponent.h"
#include "GameFramework/PlayerState.h"
#include "InventorySystem/Items/MTD_EquipItemData.h"
#include "InventorySystem/Items/MTD_InventoryBlueprintFunctionLibrary.h"
#include "InventorySystem/MTD_InventoryManagerComponent.h"
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
    
}

void AMTD_InventoryItemInstance::OnConstruction(const FTransform &Transform)
{
    Super::OnConstruction(Transform);
    
    if (!IsValid(ItemData))
    {
        MTDS_WARN("Item data is invalid.");
        return;
    }

    UStaticMesh *StaticMesh = ItemData->StaticMesh.Get();
    if (IsValid(StaticMesh))
    {
        StaticMeshComponent->SetStaticMesh(StaticMesh);
    }
    else
    {
        MTDS_WARN("Item data static mesh is invalid.");
    }
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
    
    const APlayerState *PlayerState = Character->GetPlayerState();
    if (!IsValid(PlayerState))
    {
        return false;
    }
    
    const auto InventoryManagerComponent = UMTD_InventoryManagerComponent::FindInventoryManagerComponent(PlayerState);
    if (!IsValid(InventoryManagerComponent))
    {
        return false;
    }

    if (bIsLootInteraction)
    {
        return InventoryManagerComponent->CanAddItem(ItemData);
    }

    // -- End of Regular Interaction, the rest is Equip Interaction
    
    const auto EquipItemData = Cast<UMTD_EquipItemData>(ItemData);
    if (!IsValid(EquipItemData))
    {
        return false;
    }
    
    const auto EquipmentManagerComponent = UMTD_EquipmentManagerComponent::FindEquipmentManagerComponent(PlayerState);
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

    APlayerState *PlayerState = Character->GetPlayerState();
    check(IsValid(PlayerState));
    
    bool bShouldDestroy;
    
    // Loot interaction
    if (bIsLootInteraction)
    {
        auto InventoryManagerComponent = UMTD_InventoryManagerComponent::FindInventoryManagerComponent(PlayerState);
        check(IsValid(InventoryManagerComponent));

        const EMTD_InventoryResult AddResult = InventoryManagerComponent->AddItem(ItemData);
        const bool bSuccess = (AddResult != EMTD_InventoryResult::Failed);;
        
        bShouldDestroy = bSuccess;
        ensureAlways(bSuccess);
    }
    
    // Equip interaction
    else
    {
        auto EquipmentManagerComponent = UMTD_EquipmentManagerComponent::FindEquipmentManagerComponent(PlayerState);
        check(IsValid(EquipmentManagerComponent));

        // Equip item
        const UMTD_EquipmentInstance *EquipmentInstance = EquipmentManagerComponent->K2_EquipItem(ItemData);
        const bool bEquipped = IsValid(EquipmentInstance);
        
        bShouldDestroy = bEquipped;
        ensureAlways(bEquipped);
    }

    // Something could fail due some errors prior to runtime, such as null or invalid fields in a database
    if (bShouldDestroy)
    {
        Destroy();
    }
    
    return bShouldDestroy;
}
