#pragma once

#include "mtd.h"
#include "MTD_EquipmentCoreTypes.h"
#include "GameFramework/Pawn.h"

#include "MTD_EquipmentInstance.generated.h"

class UMTD_EquipmentManagerComponent;
class UAbilitySystemComponent;
class UMTD_AbilitySystemComponent;
class UMTD_CombatSet;
struct FMTD_EquipmentActorToSpawn;

UCLASS(BlueprintType, Blueprintable)
class UMTD_EquipmentInstance : public UObject
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

    UFUNCTION(BlueprintPure, Category="MTD|Equipment",
        meta=(DeterminesOutputType=PawnType))
    APawn *GetTypedPawn(TSubclassOf<APawn> PawnType) const;

    UFUNCTION(BlueprintPure, Category="MTD|Equipment")
    AActor *GetSpawnedActor() const;

    UFUNCTION(BlueprintPure, Category="MTD|Equipment")
    UAbilitySystemComponent *GetAbilitySystemComponent() const;

    UFUNCTION(BlueprintPure, Category="MTD|Equipment")
    UMTD_AbilitySystemComponent *GetMtdAbilitySystemComponent() const;

    virtual void SpawnEquipmentActor(
        const FMTD_EquipmentActorToSpawn &ActorToSpawn);
    virtual void DestroyEquipmentActor();

    virtual void OnEquipped();
    virtual void OnUnequipped();

protected:
    UFUNCTION(BlueprintImplementableEvent, Category="MTD|Equipment",
        meta=(DisplayName="OnEquipped"))
    void K2_OnEquipped();

    UFUNCTION(BlueprintImplementableEvent, Category="MTD|Equipment",
        meta=(DisplayName="OnUnequipped"))
    void K2_OnUnequipped();

    virtual void ModStats(float Multiplier);
    bool IsPlayer() const;

private:
    void SetOwner(AActor *InOwner);

    void GrantStats();
    void TakeBackStats();

private:
    UPROPERTY()
    TObjectPtr<AActor> Owner = nullptr;

    UPROPERTY()
    TObjectPtr<AActor> SpawnedActor = nullptr;

    UPROPERTY(EditAnywhere, Category="MTD|Equipment|Stats", meta=(AllowPrivateAccess="true"), DisplayName="Player")
    FMTD_EquipmentPlayerStats PlayerStats;
};

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
