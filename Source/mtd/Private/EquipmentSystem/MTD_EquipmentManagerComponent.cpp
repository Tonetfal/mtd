#include "EquipmentSystem/MTD_EquipmentManagerComponent.h"

#include "AbilitySystem/Attributes/MTD_BuilderSet.h"
#include "AbilitySystem/Attributes/MTD_PlayerSet.h"
#include "AbilitySystem/MTD_AbilitySet.h"
#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "AbilitySystem/MTD_GameplayTags.h"
#include "AbilitySystemGlobals.h"
#include "EquipmentSystem/MTD_EquipmentDefinition.h"
#include "EquipmentSystem/MTD_EquipmentInstance.h"
#include "InventorySystem/Items/MTD_EquipItemData.h"
#include "InventorySystem/Items/MTD_InventoryBlueprintFunctionLibrary.h"
#include "InventorySystem/MTD_InventoryManagerComponent.h"
#include "Player/MTD_PlayerState.h"

namespace
{

/**
 * Simple helper struct containing player and tower stats.
 */
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

/**
 * Compute stat difference between two items.
 *
 * Example: If Lhs' health is 0, and Rhs' is 10, the health difference will be -10.
 * 
 * @param   Lhs: left hand side equip item data.
 * @param   Rhs: right hand side equip item data.
 * @return  Difference between two equip item data.
 */
FMTD_PlayerStats ComputeStatDifference(const UMTD_EquipItemData *Lhs, const UMTD_EquipItemData *Rhs)
{
    check(IsValid(Lhs));
    check(IsValid(Rhs));

    FMTD_PlayerStats Result;

    // Shorthand computitions
#define FIND_DIFFERENCE(x) Result. ## x = (Rhs->EquippableItemParameters. ## x - Lhs->EquippableItemParameters. ## x);
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

/**
 * Apply stat difference on a given ability system.
 * @param   AbilitySystemComponent: ability system to apply the difference on.
 * @param   StatsDiff: stat difference to apply.
 */
void ApplyStatDifference(UAbilitySystemComponent *AbilitySystemComponent, const FMTD_PlayerStats &StatsDiff)
{
    check(IsValid(AbilitySystemComponent));

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
    // Nothing to tick for
    PrimaryComponentTick.bStartWithTickEnabled = false;
    PrimaryComponentTick.bCanEverTick = false;

    // Demand initialization so that UninitializeComponent will be called when actor dies
    bWantsInitializeComponent = true;
}

void UMTD_EquipmentManagerComponent::InitializeComponent()
{
    Super::InitializeComponent();
}

void UMTD_EquipmentManagerComponent::UninitializeComponent()
{
    // Clean up spawned equipment instances from world
    UnequipAll(nullptr);
    Super::UninitializeComponent();
}

UMTD_EquipmentInstance *UMTD_EquipmentManagerComponent::K2_EquipItem(UMTD_BaseInventoryItemData *ItemData)
{
    if (!IsValid(ItemData))
    {
        MTDS_WARN("Item data is invalid.");
        return nullptr;
    }

    // Check if item can be equipped
    const EMTD_InventoryItemType ItemType = ItemData->ItemType;
    if (!(ItemType & EMTD_InventoryItemType::III_Equippable))
    {
        MTDS_WARN("Item [%s] is not equippable.", *ItemData->GetName());
        return nullptr;
    }

    // Equip item
    const auto EquipItemData = CastChecked<UMTD_EquipItemData>(ItemData);
    UMTD_EquipmentInstance *EquipmentInstance = EquipItem(EquipItemData);
    return EquipmentInstance;
}

UMTD_EquipmentInstance *UMTD_EquipmentManagerComponent::EquipItem(UMTD_EquipItemData *EquipItemData)
{
    if (!IsValid(EquipItemData))
    {
        MTDS_WARN("Equip item data is invalid.");
        return nullptr;
    }
    
    AActor *Owner = GetOwner();
    check(IsValid(Owner));
    
    // Check in-game logical conditions
    if (!CanEquipItem(EquipItemData))
    {
        MTDS_LOG("Owner [%s] is unable to equip [%s].", *Owner->GetName(), *EquipItemData->GetName());
        return nullptr;
    }
    
    // Get equipment type
    const EMTD_EquipmentType EquipmentType = UMTD_InventoryBlueprintFunctionLibrary::GetEquipmentType(EquipItemData);

    // Check for invalidness
    if (EquipmentType == EMTD_EquipmentType::Invalid)
    {
        MTDS_WARN("Equipment is of invalid type.");
        return nullptr;
    }

    // Try to swap items. Avoid calling GrantStats if previously unequipped an item
    const bool bModStats = !(SwapItem(EquipmentType, EquipItemData));

    // Save the ability system component given data
    FMTD_AbilitySet_GrantedHandles GrantedHandles;

    // Use the accessed data
    TSubclassOf<UMTD_EquipmentInstance> EquipmentInstanceClass = UMTD_EquipmentInstance::StaticClass();
    const UMTD_EquipmentDefinitionAsset *EquipDefAsset = EquipItemData->EquipmentDefinitionAsset;
    if (!IsValid(EquipDefAsset))
    {
        MTDS_LOG("Equip item data's [%s] equipment definition asset is invalid.", *EquipItemData->GetName());
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
            auto MtdAbilitySystemComponent = CastChecked<UMTD_AbilitySystemComponent>(
                UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner));
            EquipDefAsset->AbilitySetToGrant->GiveToAbilitySystem(MtdAbilitySystemComponent, &GrantedHandles, Owner);
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
        // Spawn an equipment instance actor if any
        EquipmentInstance->SpawnEquipmentActor(EquipDefAsset->ActorToSpawnDefinition);
    }

    // Notify equipment instance about being equipped
    EquipmentInstance->OnEquipped(bModStats);

    // Add new equipment instance to the list
    EquipmentInstances.Add({ EquipmentType, EquipmentInstance });

    // Notify about item begin equipped
    OnItemEquippedDelegate.Broadcast(EquipmentInstance);

    MTDS_LOG("Hero [%s] has equipped [%s].", *Owner->GetName(), *UEnum::GetValueAsString(EquipmentType));
    return EquipmentInstance;
}

UMTD_EquipItemData *UMTD_EquipmentManagerComponent::UnequipItemType(EMTD_EquipmentType EquipmentType,
    bool bWarnIfNotEquipped /* = true*/)
{
    const auto EquipmentInstance = EquipmentInstances.Find(EquipmentType);
    if (!EquipmentInstance)
    {
        if (bWarnIfNotEquipped)
        {
            MTDS_WARN("Equipment Type [%s] is not equipped.", *UEnum::GetValueAsString(EquipmentType));
        }
        return nullptr;
    }
    
    UMTD_EquipItemData *EquipItemData = UnequipItemInstance(*EquipmentInstance);
    return EquipItemData;
}

UMTD_EquipItemData *UMTD_EquipmentManagerComponent::UnequipItemInstance(UMTD_EquipmentInstance *EquipmentInstance)
{
    if (!IsValid(EquipmentInstance))
    {
        MTDS_WARN("Equipment instance is invalid.");
        return nullptr;
    }

    const EMTD_EquipmentType EquipmentType = EquipmentInstance->GetEquipmentType();
    EquipmentInstance->OnUnequipped();
    EquipmentInstances.Remove(EquipmentType);

    // Notify about item being unequipped
    OnItemUnequippedDelegate.Broadcast(EquipmentInstance);

    MTDS_LOG("Hero [%s] has unequipped [%s].", *GetNameSafe(GetOwner()), *UEnum::GetValueAsString(EquipmentType));
    auto EquipItemData = Cast<UMTD_EquipItemData>(EquipmentInstance->ItemData);
    return EquipItemData;
}

void UMTD_EquipmentManagerComponent::UnequipAll(TArray<UMTD_EquipItemData *> *OutItemData)
{
    // Iterate through each possible equipment type, and try to unequip it
    const int32 Count = static_cast<int32>(EMTD_EquipmentType::Count);
    for (int32 It = 0; It < Count; It++)
    {
        if (Count != static_cast<int32>(EMTD_EquipmentType::Invalid))
        {
            UMTD_EquipItemData *EquipData = UnequipItemType(static_cast<EMTD_EquipmentType>(It),
                false /* bWarnIfNotEquipped */);

            // Save item data if requested
            if (((OutItemData) && (IsValid(EquipData))))
            {
                OutItemData->Add(EquipData);
            }
        }
    }
}

void UMTD_EquipmentManagerComponent::K2_UnequipAll(TArray<UMTD_EquipItemData *> &OutItemData)
{
    // Wrap C++ call
    UnequipAll(&OutItemData);
}

bool UMTD_EquipmentManagerComponent::CanEquipItem(const UMTD_BaseInventoryItemData *ItemData) const
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

    // Bots can equip anything regardless
    const auto MtdPlayerState = CastChecked<AMTD_PlayerState>(Owner);
    if (MtdPlayerState->IsABot())
    {
        return true;
    }
    
    const auto MtdAbilitySystemComponent = MtdPlayerState->GetMtdAbilitySystemComponent();
    check(IsValid(MtdAbilitySystemComponent));

    // Check if trying to equip an item above player's level
    const FGameplayAttribute &LevelAttribute = UMTD_PlayerSet::GetLevelAttribute();
    const float PlayerLevel = MtdAbilitySystemComponent->GetNumericAttribute(LevelAttribute);
    
    // Access equipment item data
    const auto EquipItemData = CastChecked<UMTD_EquipItemData>(ItemData);
    if (EquipItemData->EquippableItemParameters.LevelRequired > PlayerLevel)
    {
        return false;
    }

    // Check player's class against equipment's classes that are allowed to use it
    const FGameplayTagContainer &EquipmentHeroClasses = EquipItemData->HeroClasses;
    const FGameplayTagContainer &CharacterHeroClasses = MtdPlayerState->GetHeroClasses();
    
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

bool UMTD_EquipmentManagerComponent::SwapItem(
    const EMTD_EquipmentType EquipmentType, const UMTD_EquipItemData *NewEquipmentItemData)
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
    
    auto MtdPlayerState = CastChecked<AMTD_PlayerState>(Owner);
    auto MtdAbilitySystemComponent = MtdPlayerState->GetMtdAbilitySystemComponent();
    check(IsValid(MtdAbilitySystemComponent));
    
    // Shorthand some expressions
    UMTD_EquipItemData *OldItemData = (*CurrentEquipmentInstance)->ItemData;
    check(IsValid(OldItemData));
    
    // Find difference between new and old item
    const FMTD_PlayerStats StatsDiff = ComputeStatDifference(OldItemData, NewEquipmentItemData);
    
    // Apply stat difference
    ApplyStatDifference(MtdAbilitySystemComponent, StatsDiff);
    
    // Unequip item, don't call TakeBackStats on old item
    (*CurrentEquipmentInstance)->OnUnequipped(/* bModStats */ false);

    UMTD_InventoryManagerComponent *InventoryManagerComponent = MtdPlayerState->GetInventoryManagerComponent();
    check(IsValid(InventoryManagerComponent));
    
    // Move item to the inventory if possible
    InventoryManagerComponent->AddItem(OldItemData, true);
    
    return true;
}
