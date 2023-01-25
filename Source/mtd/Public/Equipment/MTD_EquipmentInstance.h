#pragma once

#include "GameFramework/Pawn.h"
#include "mtd.h"
#include "MTD_EquipmentCoreTypes.h"
#include "AbilitySystem/MTD_AbilitySet.h"

#include "MTD_EquipmentInstance.generated.h"

class UAbilitySystemComponent;
class UMTD_AbilitySystemComponent;
class UMTD_CombatSet;
class UMTD_EquipmentManagerComponent;
class UMTD_EquippableItemData;
struct FMTD_EquipmentActorDefinition;

#define APPLY_MOD_TO_ATTRIBUTE(SET, ATTRIBUTE, DATA) \
    Asc->ApplyModToAttribute(UMTD_ ## SET ## Set::Get ## ATTRIBUTE ## Attribute(), \
        EGameplayModOp::Additive, (DATA * Multiplier))

/**
 * Equipment item spawned in the world and held by some character.
 */
UCLASS(BlueprintType, Blueprintable)
class UMTD_EquipmentInstance
    : public UObject
{
    GENERATED_BODY()

    friend UMTD_EquipmentManagerComponent;

public:
    UMTD_EquipmentInstance();

    //~UObject interface
    virtual UWorld *GetWorld() const override final;
    //~End of UObject interface

    UFUNCTION(BlueprintPure, Category="MTD|Equipment")
    AActor *GetOwner() const;

    UFUNCTION(BlueprintPure, Category="MTD|Equipment")
    APawn *GetPawn() const;

    template <class T>
    T *GetPawn() const;

    UFUNCTION(BlueprintPure, Category="MTD|Equipment", meta=(DeterminesOutputType=PawnType))
    APawn *GetTypedPawn(TSubclassOf<APawn> PawnType) const;

    UFUNCTION(BlueprintPure, Category="MTD|Equipment")
    AActor *GetSpawnedActor() const;

    UFUNCTION(BlueprintPure, Category="MTD|Equipment")
    UAbilitySystemComponent *GetAbilitySystemComponent() const;

    UFUNCTION(BlueprintPure, Category="MTD|Equipment")
    UMTD_AbilitySystemComponent *GetMtdAbilitySystemComponent() const;

    EMTD_EquipmentType GetEquipmentType() const;
    const UMTD_EquippableItemData *GetItemData() const;

    virtual void SpawnEquipmentActor(const FMTD_EquipmentActorDefinition &ActorToSpawn);
    virtual void DestroyEquipmentActor();

    virtual void OnEquipped();
    virtual void OnUnequipped();

protected:
    UFUNCTION(BlueprintImplementableEvent, Category="MTD|Equipment", meta=(DisplayName="OnEquipped"))
    void K2_OnEquipped();

    UFUNCTION(BlueprintImplementableEvent, Category="MTD|Equipment", meta=(DisplayName="OnUnequipped"))
    void K2_OnUnequipped();

    virtual void ModStats(float Multiplier);
    virtual void ModStats_Internal(float Multiplier, UAbilitySystemComponent *Asc);
    bool IsPlayer() const;

private:
    void SetOwner(AActor *InOwner);
    void SetEquipmentType(EMTD_EquipmentType InEquipmentType);
    void SetItemData(UMTD_EquippableItemData *InItemData);
    void SetGrantedHandles(FMTD_AbilitySet_GrantedHandles &InGrantedHandles);

    void GrantStats();
    void TakeBackStats();

protected:
    UPROPERTY(BlueprintReadOnly, Category="MTD|Equipment", meta=(AllowPrivateAccess="true"))
    EMTD_EquipmentType EquipmentType = EMTD_EquipmentType::Invalid;
    
    UPROPERTY(BlueprintReadOnly, Category="MTD|Equipment", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_EquippableItemData> ItemData = nullptr;

    UPROPERTY()
    FMTD_AbilitySet_GrantedHandles GrantedHandles;

private:
    UPROPERTY()
    TObjectPtr<AActor> Owner = nullptr;

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

inline void UMTD_EquipmentInstance::SetItemData(UMTD_EquippableItemData *InItemData)
{
    ItemData = InItemData;
}

inline const UMTD_EquippableItemData *UMTD_EquipmentInstance::GetItemData() const
{
    return ItemData;
}

inline void UMTD_EquipmentInstance::SetGrantedHandles(FMTD_AbilitySet_GrantedHandles &InGrantedHandles)
{
    GrantedHandles = InGrantedHandles;
}
