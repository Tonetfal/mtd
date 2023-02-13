#pragma once

#include "AbilitySystem/MTD_AbilitySet.h"
#include "Equipment/MTD_EquipmentCoreTypes.h"
#include "GameFramework/Pawn.h"
#include "mtd.h"

#include "MTD_EquipmentInstance.generated.h"

class UAbilitySystemComponent;
class UMTD_AbilitySystemComponent;
class UMTD_CombatSet;
class UMTD_EquipmentManagerComponent;
class UMTD_EquipItemData;
struct FMTD_EquipmentActorDefinition;

/** Shorthand modification of an attribute. */
#define MOD_ATTRIBUTE_BASE(SET, ATTRIBUTE, DATA) \
    do \
    { \
        const float Base = AbilitySystemComponent->GetNumericAttributeBase( \
            UMTD_ ## SET ## Set::Get ## ATTRIBUTE ## Attribute()); \
        const float Delta = (DATA * Multiplier); \
        const float NewBase = (Base + Delta); \
        AbilitySystemComponent->SetNumericAttributeBase( \
            UMTD_ ## SET ## Set::Get ## ATTRIBUTE ## Attribute(), NewBase); \
    } while (0)

/**
 * Equipment instance that manages main logical and visual data.
 *
 * Logic wise, it manages owner's stats by granting them on equip, and by retrieving them on unequip.
 *
 * Visual wise, it manages the equipment actor spawn in the world.
 */
UCLASS(BlueprintType, Blueprintable)
class UMTD_EquipmentInstance
    : public UObject
{
    GENERATED_BODY()

    friend UMTD_EquipmentManagerComponent;

public:
    //~UObject interface
    virtual UWorld *GetWorld() const override final;
    //~End of UObject interface

    /**
     * Get logical equipment owner.
     * @return  Logical equipment owner.
     */
    UFUNCTION(BlueprintPure, Category="MTD|Equipment")
    AActor *GetOwner() const;

    /**
     * Get physical equipment owner.
     * @return  Physical equipment owner.
     */
    UFUNCTION(BlueprintPure, Category="MTD|Equipment")
    APawn *GetPawn() const;

    /**
     * Get typed physical equipment owner.
     * @return  Typed physical equipment owner.
     */
    template <class T>
    T *GetPawn() const;

    /**
     * Get typed physical equipment owner.
     * @return  Typed physical equipment owner.
     */
    UFUNCTION(BlueprintPure, Category="MTD|Equipment", meta=(DeterminesOutputType=PawnType))
    APawn *GetTypedPawn(TSubclassOf<APawn> PawnType) const;

    /**
     * Get spawned equipment actor.
     * @return  Spawned equipment actor. May be nullptr.
     */
    UFUNCTION(BlueprintPure, Category="MTD|Equipment")
    AActor *GetSpawnedActor() const;

    /**
     * Get ability system component.
     * @return  Ability system component.
     */
    UFUNCTION(BlueprintPure, Category="MTD|Equipment")
    UAbilitySystemComponent *GetAbilitySystemComponent() const;

    /**
     * Get MTD ability system component.
     * @return  MTD ability system component.
     */
    UFUNCTION(BlueprintPure, Category="MTD|Equipment")
    UMTD_AbilitySystemComponent *GetMtdAbilitySystemComponent() const;

    /**
     * Get equipment type.
     * @return  Equipment type.
     */
    EMTD_EquipmentType GetEquipmentType() const;

    /**
     * Get equip item data.
     * @return  Equip item data.
     */
    const UMTD_EquipItemData *GetItemData() const;

    /**
     * Spawn equipment actor. Supports only 1 actor.
     * @param   ActorToSpawn: equipment actor to spawn definition.
     */
    virtual void SpawnEquipmentActor(const FMTD_EquipmentActorDefinition &ActorToSpawn);

    /**
     * Destroy equipment actor.
     */
    virtual void DestroyEquipmentActor();

    /**
     * Event to fire when instance is equipped.
     * @param   bModStats: if true, equipment stats will alter the owner's attributes, false otherwise.
     */
    virtual void OnEquipped(bool bModStats = true);
    
    /**
     * Event to fire when instance is unequipped.
     * @param   bModStats: if true, equipment stats will alter the owner's attributes, false otherwise.
     */
    virtual void OnUnequipped(bool bModStats = true);

protected:
    /**
     * Event to fire when instance is equipped.
     */
    UFUNCTION(BlueprintImplementableEvent, Category="MTD|Equipment", DisplayName="OnEquipped")
    void K2_OnEquipped();

    /**
     * Event to fire when instance is unequipped.
     */
    UFUNCTION(BlueprintImplementableEvent, Category="MTD|Equipment", DisplayName="OnUnequipped")
    void K2_OnUnequipped();

    /**
     * Alter owner's attributes using item data.
     * @param   Multiplier: value to multiply each stat by.
     */
    void ModStats(float Multiplier);

    /**
     * Alter owner's attributes using item data. Is called only when everything is valid, and only on players.
     * @param   Multiplier: value to multiply each stat by.
     * @param   AbilitySystemComponent: ability system to perform the action on.
     */
    virtual void ModStats_Internal(float Multiplier, UAbilitySystemComponent *AbilitySystemComponent);

    /**
     * Check whether the owner is a player or not.
     * @return  If true, owner is a player, false otherwise.
     */
    bool IsPlayer() const;

private:
    /**
     * Set logical owner.
     * @param   InOwner: actor to set as a new owner.
     */
    void SetOwner(AActor *InOwner);

    /**
     * Set equipment type.
     * @param   InEquipmentType: equipment type to set.
     */
    void SetEquipmentType(EMTD_EquipmentType InEquipmentType);

    /**
     * Set item data.
     * @param   InItemData: item data to set.
     */
    void SetItemData(UMTD_EquipItemData *InItemData);

    /**
     * Set granted handles. They are intended to be the handles gotten from granting all ability sets of an equipment.
     * @param   InGrantedHandles: granted handles to set.
     */
    void SetGrantedHandles(const FMTD_AbilitySet_GrantedHandles &InGrantedHandles);

    /**
     * Grant stats to owner using item data.
     */
    void GrantStats();
    
    /**
     * Retrieve stats from owner using item data.
     */
    void RetrieveStats();

protected:
    /** Type of this equipment. */
    UPROPERTY(BlueprintReadOnly, Category="MTD|Equipment", meta=(AllowPrivateAccess="true"))
    EMTD_EquipmentType EquipmentType = EMTD_EquipmentType::Invalid;

    /** Item data defining the equipment. */
    UPROPERTY(BlueprintReadOnly, Category="MTD|Equipment", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_EquipItemData> ItemData = nullptr;

    /** Granted handles yielded from granting this equipment's ability sets */
    UPROPERTY()
    FMTD_AbilitySet_GrantedHandles GrantedHandles;

private:
    /** Logical owner. */
    UPROPERTY()
    TObjectPtr<AActor> Owner = nullptr;

    /** Spawned actor. Equipment visual representation. */
    UPROPERTY()
    TObjectPtr<AActor> SpawnedActor = nullptr;
};

template <class T>
T *UMTD_EquipmentInstance::GetPawn() const
{
    return Cast<T>(GetPawn());
}

inline AActor *UMTD_EquipmentInstance::GetOwner() const
{
    return Owner;
}

inline AActor *UMTD_EquipmentInstance::GetSpawnedActor() const
{
    return SpawnedActor;
}

inline void UMTD_EquipmentInstance::SetOwner(AActor *InOwner)
{
    Owner = InOwner;
}

inline void UMTD_EquipmentInstance::SetEquipmentType(EMTD_EquipmentType InEquipmentType)
{
    EquipmentType = InEquipmentType;
}

inline void UMTD_EquipmentInstance::SetItemData(UMTD_EquipItemData *InItemData)
{
    ItemData = InItemData;
}

inline const UMTD_EquipItemData *UMTD_EquipmentInstance::GetItemData() const
{
    return ItemData;
}

inline void UMTD_EquipmentInstance::SetGrantedHandles(const FMTD_AbilitySet_GrantedHandles &InGrantedHandles)
{
    GrantedHandles = InGrantedHandles;
}
