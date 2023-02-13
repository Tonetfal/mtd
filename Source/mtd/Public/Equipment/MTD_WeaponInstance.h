#pragma once

#include "Equipment/MTD_EquipmentInstance.h"
#include "GameplayAbilitySpec.h"
#include "mtd.h"

#include "MTD_WeaponInstance.generated.h"

class UGameplayEffect;
class UMTD_ProjectileData;
class UMTD_WeaponItemData;

/**
 * Equipment instance that manages main logical and visual data.
 *
 * Logic wise, it manages owner's stats by granting them on equip, and by retrieving them on unequip.
 *
 * Visual wise, it manages the equipment actor spawn in the world.
 */
UCLASS()
class MTD_API UMTD_WeaponInstance
    : public UMTD_EquipmentInstance
{
    GENERATED_BODY()

public:
    /**
     * Get projectile data.
     * @return  Projectile data.
     */
    const UMTD_ProjectileData *GetProjectileData() const;

    /**
     * Get weapon fire point in world coordinates.
     * @return  Weapon fire point in world coordinates.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Weapon")
    FVector GetFirePointWorldPosition() const;

protected:
    //~UMTD_EquipmentInstance Interface
    virtual void ModStats_Internal(float Multiplier, UAbilitySystemComponent *AbilitySystemComponent) override;
    //~End of UMTD_EquipmentInstance Interface

private:
    /**
     * Initialize weapon fire point.
     * @return  If true, fire point has been successfully initialized or has already been, false otherwise.
     */
    bool InitializeFirePoint() const;

private:
    /** Gameplay effects to grant on melee hit. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Weapon", meta=(AllowPrivateAccess="true"))
    TArray<TSubclassOf<UGameplayEffect>> GameplayEffectsToGrantOnHit;

    /** Projectile data to use when firing projectiles. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Equipment|Stats|Ranged Weapon",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_ProjectileData> ProjectileData = nullptr;

    /** Socket name of the point projectiles have to spawn from. */
    UPROPERTY(EditDefaultsOnly, Category="MTD|Equipment|Ranged Weapon")
    FName FirePointSocketName = "FirePoint";

    /** Fire point socket on weapon actor. */
    UPROPERTY()
    mutable TObjectPtr<const UStaticMeshSocket> FirePointSocket = nullptr;

    /** Physical weapon actor mesh. */
    UPROPERTY()
    mutable TObjectPtr<const UStaticMeshComponent> WeaponMesh = nullptr;
};

inline const UMTD_ProjectileData *UMTD_WeaponInstance::GetProjectileData() const
{
    return ProjectileData;
}
