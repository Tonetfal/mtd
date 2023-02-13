#pragma once

#include "Components/PawnComponent.h"
#include "Equipment/MTD_EquipmentCoreTypes.h"
#include "mtd.h"

#include "MTD_EquipmentManagerComponent.generated.h"

class AMTD_InventoryItemInstance;
class UMTD_BaseInventoryItemData;
class UMTD_EquipItemData;
class UMTD_EquipmentInstance;

/**
 * Component to manager owner's equipment.
 *
 * It handles stat granting and retrieval, as well as spawning equipment actors in the world.
 */
UCLASS(BlueprintType, Const, ClassGroup="Pawn", meta=(BlueprintSpawnableComponent))
class UMTD_EquipmentManagerComponent
    : public UPawnComponent
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnItemEquippedSignature,
        UMTD_EquipmentInstance*, EquipmentInstance);
    
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnItemUnequippedSignature,
        UMTD_EquipmentInstance*, EquipmentInstance);

public:
    UMTD_EquipmentManagerComponent(const FObjectInitializer &ObjectInitializer);

    /**
     * Find equipment manager component on a given actor.
     * @param   Actor: actor to search for equipment manager component in.
     * @result  Equipment manager component residing on the given actor. Can be nullptr.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Pawn")
    static UMTD_EquipmentManagerComponent *FindEquipmentManagerComponent(const AActor *Actor);

    //~UActorComponent interface
    virtual void InitializeComponent() override;
    virtual void UninitializeComponent() override;
    //~End of UActorComponent interface

    /**
     * Equip a given item.
     * @param   ItemData: item to equip.
     * @return  Equipped item instance. May be nullptr.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Equipment", DisplayName="Equip Item")
    UMTD_EquipmentInstance *K2_EquipItem(UMTD_BaseInventoryItemData *ItemData);

    /**
     * Equip a given item.
     * @param   EquipItemData: item to equip.
     * @return  Equipped item instance. May be nullptr.
     */
    UMTD_EquipmentInstance *EquipItem(UMTD_EquipItemData *EquipItemData);

    /**
     * Unequip a given item type.
     * @param   EquipmentType: equipment type to unequip.
     * @param   bWarnIfNotEquipped: if true, it will log a message about equipment type not being equipped, if false,
     * it will not.
     * @return  Item data of the unequipped equipment instance. May be nullptr.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Equipment")
    UMTD_EquipItemData *UnequipItemType(EMTD_EquipmentType EquipmentType, bool bWarnIfNotEquipped = true);

    /**
     * Unequip a given equipment instance.
     * @param   EquipmentInstance: equipment instance to uneequip.
     * @return  Item data of the unequipped equipment instance. May be nullptr.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Equipment")
    UMTD_EquipItemData *UnequipItemInstance(UMTD_EquipmentInstance *EquipmentInstance);
    
    /**
     * Unequip all the equipment currently equipped.
     * @param   OutItemData: output param. Item data of all the unequipped items.
     */
    void UnequipAll(TArray<UMTD_EquipItemData *> *OutItemData);

    /**
     * Unequip all the equipment currently equipped.
     * @param   OutItemData: output param. Item data of all the unequipped items.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Equipment", DisplayName="Unequip All")
    void K2_UnequipAll(TArray<UMTD_EquipItemData *> &OutItemData);

    /**
     * Check whether the character is able to equip an item. 
     * @return  If true, owner is allowed to equip given item, false otherwise.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Equipment")
    bool CanEquipItem(const UMTD_BaseInventoryItemData *ItemData) const;

    /**
     * Check whether a given item type is equipped.
     * @param   EquipmentType: equipment type to check.
     * @return  If true, given item type is equipped, false otherwise.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Equipment")
    bool IsItemTypeEquipped(EMTD_EquipmentType EquipmentType) const;

    /**
     * Get equipment instance of a given equipment type.
     * @param   EquipmentType: equipment type to get instance of.
     * @return  Equipment instance of a given equipment type. May be nullptr.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Equipment")
    const UMTD_EquipmentInstance *GetEquipmentInstance(EMTD_EquipmentType EquipmentType) const;

private:
    /**
     * Smoothly swap two equipment items.
     *
     * For instance, if a character has 50/50 health, and an equipment gives him 10 bonus health, the character will be
     * at 40/40 health if that item is unequipped, and will be 40/50 if equipped once again.
     *
     * However, if the item has to be swapped, there has to be a smooth transition. If the new item gives 5 health, the
     * character's health has to transition from 50/50 to 45/45, and not 50/50 -> 40/40 -> 40/45
     *
     * @param   EquipmentType: equipment type to swap.
     * @param   NewEquipmentItemData: equipment item data to swap the given equipment type with.
     * @return  If true, items were successfully swapped, false otherwise.
     */
    bool SwapItem(const EMTD_EquipmentType EquipmentType, const UMTD_EquipItemData *NewEquipmentItemData);

public:
    /** Delegate to fire when an item is equipped. */
    UPROPERTY(BlueprintAssignable)
    FOnItemEquippedSignature OnItemEquippedDelegate;
    
    /** Delegate to fire when an item is unequipped. */
    UPROPERTY(BlueprintAssignable)
    FOnItemUnequippedSignature OnItemUnequippedDelegate;

private:
    /** Currently equipped equipment instances. */
    UPROPERTY(BlueprintReadOnly, Category="MTD|Equipment", meta=(AllowPrivateAccess="true"))
    TMap<EMTD_EquipmentType, TObjectPtr<UMTD_EquipmentInstance>> EquipmentInstances;
};

inline UMTD_EquipmentManagerComponent *UMTD_EquipmentManagerComponent::FindEquipmentManagerComponent(
    const AActor *Actor)
{
    return ((IsValid(Actor)) ? (Actor->FindComponentByClass<UMTD_EquipmentManagerComponent>()) : (nullptr));
}

inline const UMTD_EquipmentInstance *UMTD_EquipmentManagerComponent::GetEquipmentInstance(
    EMTD_EquipmentType EquipmentType) const
{
    const TObjectPtr<UMTD_EquipmentInstance> *EquipmentInstance = EquipmentInstances.Find(EquipmentType);
    return ((EquipmentInstance) ? (*EquipmentInstance) : (nullptr));
}
