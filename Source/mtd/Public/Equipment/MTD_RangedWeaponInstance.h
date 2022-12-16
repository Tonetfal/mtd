#pragma once

#include "Equipment/MTD_WeaponInstance.h"
#include "mtd.h"
#include "Projectile/MTD_ProjectileCoreTypes.h"

#include "MTD_RangedWeaponInstance.generated.h"

class AMTD_Projectile;
class UMTD_ProjectileData;

UCLASS()
class MTD_API UMTD_RangedWeaponInstance : public UMTD_WeaponInstance
{
    GENERATED_BODY()

public:
    virtual void ModStats(float Multiplier) override;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Weapon")
    FVector GetFirePointWorldPosition() const;

private:
    bool InitializeFirePoint() const;

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Equipment|Stats|Weapon", meta=(AllowPrivateAccess="true"))
    FMTD_EquipmentRangedWeaponStats RangedWeaponStats;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Weapon", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_ProjectileData> ProjectileData = nullptr;

    const FName FirePointSocketName{TEXT("FirePoint")};

    UPROPERTY()
    mutable TObjectPtr<const UStaticMeshSocket> FirePointSocket = nullptr;

    UPROPERTY()
    mutable TObjectPtr<const UStaticMeshComponent> WeaponMesh = nullptr;
};
