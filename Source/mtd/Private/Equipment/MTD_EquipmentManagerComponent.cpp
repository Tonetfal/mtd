#include "Equipment/MTD_EquipmentManagerComponent.h"

#include "AbilitySystem/Attributes/MTD_BuilderSet.h"
#include "AbilitySystem/Attributes/MTD_PlayerSet.h"
#include "AbilitySystem/MTD_AbilitySet.h"
#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "AbilitySystem/MTD_GameplayTags.h"
#include "AbilitySystemGlobals.h"
#include "Equipment/MTD_EquipmentDefinition.h"
#include "Equipment/MTD_EquipmentInstance.h"
#include "Inventory/Items/MTD_ArmorItemData.h"
#include "Inventory/Items/MTD_BaseInventoryItemData.h"
#include "Inventory/Items/MTD_EquippableItemData.h"
#include "Inventory/Items/MTD_InventoryBlueprintFunctionLibrary.h"
#include "Inventory/MTD_InventoryManagerComponent.h"
#include "Player/MTD_PlayerState.h"

namespace
{

struct FMTD_PlayerStats
{
public:
    float PlayerHealth = 0.f;
    float PlayerDamage = 0.f;
    float PlayerSpeed = 0.f;
    float TowerHealth = 0.f;
    float TowerDamage = 0.f;
    float TowerRange = 0.f;
    float TowerSpeed = 0.f;
};

FMTD_PlayerStats ComputeStatDifference(const UMTD_EquippableItemData *OldItemData,
    const UMTD_EquippableItemData *NewItemData)
{
    check(IsValid(OldItemData));
    check(IsValid(NewItemData));

    FMTD_PlayerStats Result;

#define FIND_DIFFERENCE(x) Result. ## x = (NewItemData-> ## x - OldItemData-> ## x);
    FIND_DIFFERENCE(PlayerHealth);
    FIND_DIFFERENCE(PlayerDamage);
    FIND_DIFFERENCE(PlayerSpeed);
    
    FIND_DIFFERENCE(TowerHealth);
    FIND_DIFFERENCE(TowerDamage);
    FIND_DIFFERENCE(TowerRange);
    FIND_DIFFERENCE(TowerSpeed);
#undef FIND_DIFFERENCE

    return Result;
}

void ApplyStatDifference(UAbilitySystemComponent *Asc, const FMTD_PlayerStats &StatsDiff)
{
    check(IsValid(Asc));

    constexpr float Multiplier = 1.f;
    MOD_ATTRIBUTE_BASE(Player, HealthStat, StatsDiff.PlayerHealth);
    MOD_ATTRIBUTE_BASE(Player, DamageStat, StatsDiff.PlayerDamage);
    MOD_ATTRIBUTE_BASE(Player, SpeedStat, StatsDiff.PlayerDamage);
    
    MOD_ATTRIBUTE_BASE(Builder, HealthStat, StatsDiff.TowerHealth);
    MOD_ATTRIBUTE_BASE(Builder, DamageStat, StatsDiff.TowerDamage);
    MOD_ATTRIBUTE_BASE(Builder, RangeStat, StatsDiff.TowerRange);
    MOD_ATTRIBUTE_BASE(Builder, SpeedStat, StatsDiff.TowerSpeed);
}

}

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
    if (!IsValid(ItemData))
    {
        MTDS_WARN("Item Data is invalid.");
        return nullptr;
    }
    
    AActor *Owner = GetOwner();
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

    // Get equipment type
    const EMTD_EquipmentType EquipmentType = UMTD_InventoryBlueprintFunctionLibrary::GetEquipmentType(ItemData);

    // Check for invalidness
    if (EquipmentType == EMTD_EquipmentType::Invalid)
    {
        MTDS_WARN("Equipment is of invalid type.");
        return nullptr;
    }

    // Access equipment item data
    auto EquipItemData = Cast<UMTD_EquippableItemData>(ItemData);
    check(IsValid(EquipItemData));

    // Try to swap items. Avoid calling GrantStats if previously unequipped an item
    const bool bModStats = !(SwapItem(EquipmentType, EquipItemData));

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
            check(IsValid(MtdPs));
            
            auto MtdAsc = MtdPs->GetMtdAbilitySystemComponent();
            check(IsValid(MtdAsc));
            
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

    EquipmentInstance->OnEquipped(/* bModStats */ bModStats);
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

bool UMTD_EquipmentManagerComponent::SwapItem(const EMTD_EquipmentType EquipmentType,
    UMTD_EquippableItemData *NewEquipmentItemData)
{
    // Check if equipment type is already occupied
    const TObjectPtr<UMTD_EquipmentInstance> *CurrentEquipmentInstance = EquipmentInstances.Find(EquipmentType);
    if (!((CurrentEquipmentInstance) && (IsValid(*CurrentEquipmentInstance))))
    {
        // Item wasn't found or invalid
        return false;
    }
    
    MTDS_LOG("Equipment [%s] is already equipped, unequipping...", *UEnum::GetValueAsString(EquipmentType));

    AActor *Owner = GetOwner();
    check(IsValid(Owner));
    
    auto MtdPs = Cast<AMTD_PlayerState>(Owner);
    check(IsValid(MtdPs));
    
    auto MtdAsc = MtdPs->GetMtdAbilitySystemComponent();
    check(IsValid(MtdAsc));
    
    // Shorthand some expressions
    UMTD_EquippableItemData *OldItemData = (*CurrentEquipmentInstance)->ItemData;
    check(IsValid(OldItemData));
    
    // Find difference between new and old item
    const FMTD_PlayerStats StatsDiff = ComputeStatDifference(OldItemData, NewEquipmentItemData);
    
    // Apply stat difference
    ApplyStatDifference(MtdAsc, StatsDiff);
    
    // Unequip item, don't call TakeBackStats on old item
    (*CurrentEquipmentInstance)->OnUnequipped(/* bModStats */ false);
    
    // Move item to the inventory if possible
    UMTD_InventoryManagerComponent *InventoryManagerComponent = MtdPs->GetInventoryManagerComponent();
    check(IsValid(InventoryManagerComponent));
    InventoryManagerComponent->AddItem(OldItemData, true);

    return true;
}
