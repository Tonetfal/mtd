#pragma once

#include "Equipment/MTD_EquipmentInstance.h"
#include "GameplayAbilitySpec.h"
#include "mtd.h"

#include "MTD_WeaponInstance.generated.h"

class UGameplayEffect;
class UMTD_ProjectileData;
class UMTD_WeaponItemData;

UCLASS()
class MTD_API UMTD_WeaponInstance
    : public UMTD_EquipmentInstance
{
    GENERATED_BODY()

public:
    const UMTD_ProjectileData *GetProjectileData() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Weapon")
    FVector GetFirePointWorldPosition() const;

protected:
    virtual void ModStats_Internal(float Multiplier, UAbilitySystemComponent *Asc) override;

private:
    bool InitializeFirePoint() const;

private:
    UPROPERTY(EditInstanceOnly, Category="MTD|Equipment|Stats|Weapon", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_WeaponItemData> WeaponItemData = nullptr;
    
    /** Gameplay effects to grant on melee hit. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Weapon", meta=(AllowPrivateAccess="true"))
    TArray<TSubclassOf<UGameplayEffect>> GameplayEffectsToGrantOnHit;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Equipment|Stats|Ranged Weapon",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_ProjectileData> ProjectileData = nullptr;

    UPROPERTY(EditDefaultsOnly, Category="MTD|Equipment|Ranged Weapon")
    FName FirePointSocketName = "FirePoint";

    UPROPERTY()
    mutable TObjectPtr<const UStaticMeshSocket> FirePointSocket = nullptr;

    UPROPERTY()
    mutable TObjectPtr<const UStaticMeshComponent> WeaponMesh = nullptr;
};

inline const UMTD_ProjectileData *UMTD_WeaponInstance::GetProjectileData() const
{
    return ProjectileData;
}
