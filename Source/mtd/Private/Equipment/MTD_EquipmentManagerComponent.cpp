#include "Equipment/MTD_EquipmentManagerComponent.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystem/MTD_AbilitySet.h"
#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "AbilitySystem/MTD_GameplayTags.h"
#include "AbilitySystem/Attributes/MTD_PlayerSet.h"
#include "Equipment/MTD_EquipmentDefinition.h"
#include "Equipment/MTD_EquipmentInstance.h"
#include "Inventory/Items/MTD_ArmorItemData.h"
#include "Inventory/Items/MTD_BaseInventoryItemData.h"
#include "Inventory/Items/MTD_EquippableItemData.h"
#include "Inventory/Items/MTD_InventoryBlueprintFunctionLibrary.h"
#include "Player/MTD_PlayerController.h"
#include "Player/MTD_PlayerState.h"

UMTD_EquipmentManagerComponent::UMTD_EquipmentManagerComponent(const FObjectInitializer &ObjectInitializer)
    : UPawnComponent(ObjectInitializer)
{
    PrimaryComponentTick.bStartWithTickEnabled = false;
    PrimaryComponentTick.bCanEverTick = false;
}

void UMTD_EquipmentManagerComponent::InitializeComponent()
{
    Super::InitializeComponent();
}

void UMTD_EquipmentManagerComponent::UninitializeComponent()
{
    UnequipAll();
    Super::UninitializeComponent();
}

UMTD_EquipmentInstance *UMTD_EquipmentManagerComponent::EquipItem(UMTD_BaseInventoryItemData *ItemData)
{
    AActor *Owner = GetOwner();
    check(IsValid(ItemData));
    check(IsValid(Owner));
    
    // Check in-game logical conditions
    if (!CanEquipItem(ItemData))
    {
        MTDS_LOG("Owner [%s] is unable equip [%s].", *Owner->GetName(), *ItemData->GetName());
        return nullptr;
    }

    // Check if item can be equipped
    const EMTD_InventoryItemType ItemType = ItemData->ItemType;
    if (!(ItemType & EMTD_InventoryItemType::III_Equippable))
    {
        MTDS_WARN("Item [%s] is not equippable.", *ItemData->GetName());
        return nullptr;
    }
    
    const EMTD_EquipmentType EquipmentType = UMTD_InventoryBlueprintFunctionLibrary::GetEquipmentType(ItemData);

    // Check for invalidness
    if (EquipmentType == EMTD_EquipmentType::Invalid)
    {
        MTDS_WARN("Equipment is of invalid type.");
        return nullptr;
    }
    
    // Check if already equipment is already equipped
    if (EquipmentInstances.Contains(EquipmentType))
    {
        MTDS_WARN("Equipment [%s] is already equipped.", *UEnum::GetValueAsString(EquipmentType));
        return nullptr;
    }

    // Access equipment item data
    auto EquipItemData = Cast<UMTD_EquippableItemData>(ItemData);
    check(EquipItemData);

    // Save the ability system component given data
    FMTD_AbilitySet_GrantedHandles GrantedHandles;

    // Use the accessed data
    TSubclassOf<UMTD_EquipmentInstance> EquipmentInstanceClass = UMTD_EquipmentInstance::StaticClass();
    const UMTD_EquipmentDefinitionAsset *EquipDefAsset = EquipItemData->EquipmentDefinitionAsset;
    if (!IsValid(EquipDefAsset))
    {
        MTDS_LOG("Equip Item Data's [%s] Equipment Definition Asset is invalid.", *EquipItemData->GetName());
        EquipDefAsset = nullptr;
    }
    else
    {
        // Use a different equipment instance class if given any
        if (EquipDefAsset->EquipmentInstanceClass)
        {
            EquipmentInstanceClass = EquipDefAsset->EquipmentInstanceClass;
        }
        
        // Grant abilities if any
        if (IsValid(EquipDefAsset->AbilitySetToGrant))
        {
            auto MtdPs = Cast<AMTD_PlayerState>(Owner);
            check(MtdPs);
            
            auto MtdAsc = MtdPs->GetMtdAbilitySystemComponent();
            check(MtdAsc);

            EquipDefAsset->AbilitySetToGrant->GiveToAbilitySystem(MtdAsc, &GrantedHandles, Owner);
        }
    }

    // Create and initialize equipment instance
    auto EquipmentInstance = NewObject<UMTD_EquipmentInstance>(Owner, EquipmentInstanceClass);
    EquipmentInstance->SetOwner(Owner);
    EquipmentInstance->SetEquipmentType(EquipmentType);
    EquipmentInstance->SetItemData(EquipItemData);
    EquipmentInstance->SetGrantedHandles(GrantedHandles);
    
    if (EquipDefAsset)
    {
        EquipmentInstance->SpawnEquipmentActor(EquipDefAsset->ActorToSpawnDefinition);
    }
    
    EquipmentInstance->OnEquipped();
    EquipmentInstances.Add({ EquipmentType, EquipmentInstance });

    // Notify about the equipment
    OnItemEquipmentDelegate.Broadcast(EquipmentInstance);

    MTDS_LOG("Hero [%s] has equipped [%s].", *Owner->GetName(), *UEnum::GetValueAsString(EquipmentType));
    return EquipmentInstance;
}

UMTD_EquippableItemData *UMTD_EquipmentManagerComponent::UnequipItemType(EMTD_EquipmentType EquipmentType)
{
    const auto EquipmentInstance = EquipmentInstances.Find(EquipmentType);
    if (!EquipmentInstance)
    {
        MTDS_WARN("Equipment Type [%s] is not equipped.", *UEnum::GetValueAsString(EquipmentType));
        return nullptr;
    }
    
    UMTD_EquippableItemData *EquipItemData = UnequipItemInstance(*EquipmentInstance);
    return EquipItemData;
}

UMTD_EquippableItemData *UMTD_EquipmentManagerComponent::UnequipItemInstance(UMTD_EquipmentInstance *EquipmentInstance)
{
    if (!IsValid(EquipmentInstance))
    {
        MTDS_WARN("Equipment Instance is invalid.");
        return nullptr;
    }

    const EMTD_EquipmentType EquipmentType = EquipmentInstance->GetEquipmentType();
    EquipmentInstance->OnUnequipped();
    EquipmentInstances.Remove(EquipmentType);

    // Notify about the un-equipment
    OnItemUnEquipmentDelegate.Broadcast(EquipmentInstance);

    MTDS_LOG("Hero [%s] has unequipped [%s].", *GetNameSafe(GetOwner()), *UEnum::GetValueAsString(EquipmentType));
    auto EquipItemData = Cast<UMTD_EquippableItemData>(EquipmentInstance->ItemData);
    return EquipItemData;
}

void UMTD_EquipmentManagerComponent::UnequipAll()
{
    const int32 n = static_cast<int32>(EMTD_EquipmentType::Count);
    for (int32 i = 0; i < n; i++)
    {
        if (n != static_cast<int32>(EMTD_EquipmentType::Invalid))
        {
            UnequipItemType(static_cast<EMTD_EquipmentType>(i));
        }
    }
}

bool UMTD_EquipmentManagerComponent::CanEquipItem(UMTD_BaseInventoryItemData *ItemData) const
{
    if (!IsValid(ItemData))
    {
        return false;
    }

    if (!UMTD_InventoryBlueprintFunctionLibrary::IsEquippable(ItemData))
    {
        return false;
    }
    
    AActor *Owner = GetOwner();
    if (!IsValid(Owner))
    {
        return false;
    }

    // Access equipment item data
    const auto EquipItemData = Cast<UMTD_EquippableItemData>(ItemData);
    if (!EquipItemData)
    {
        return false;
    }
    
    const auto MtdPs = Cast<AMTD_PlayerState>(Owner);
    check(MtdPs);

    // Bots can equip anything regardless
    if (MtdPs->IsABot())
    {
        return true;
    }
    
    const auto MtdAsc = MtdPs->GetMtdAbilitySystemComponent();
    check(MtdAsc);

    // Check if trying to equip an item above player's level
    const FGameplayAttribute LevelStatAttribute = UMTD_PlayerSet::GetLevelStatAttribute();
    const float PlayerLevel = MtdAsc->GetNumericAttribute(LevelStatAttribute);
    if (EquipItemData->LevelRequired > PlayerLevel)
    {
        return false;
    }

    // Check player's class against equipment's classes that are allowed to use it
    const FMTD_GameplayTags GameplayTags = FMTD_GameplayTags::Get();
    const FGameplayTagContainer &EquipmentHeroClasses = EquipItemData->HeroClasses;
    const FGameplayTagContainer &CharacterHeroClasses = MtdPs->GetHeroClasses();
    
    // Should never happen
    if (EquipmentHeroClasses.IsEmpty())
    {
        return false;
    }
    
    // Check if item can be used by anyone
    if (FMTD_GameplayTags::IsForAllHeroClasses(EquipmentHeroClasses))
    {
        return true;
    }

    // Check if hero is marked as every class
    if (FMTD_GameplayTags::IsForAllHeroClasses(CharacterHeroClasses))
    {
        return true;
    }

    // Regular comparison
    if (!EquipmentHeroClasses.HasAny(CharacterHeroClasses))
    {
        return false;
    }

    return true;
}

bool UMTD_EquipmentManagerComponent::IsItemTypeEquipped(EMTD_EquipmentType EquipmentType) const
{
    const bool bContains = EquipmentInstances.Contains(EquipmentType);
    return bContains;
}
