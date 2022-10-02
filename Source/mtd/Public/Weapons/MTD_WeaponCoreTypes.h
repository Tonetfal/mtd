#pragma once

#include "mtd.h"
#include "MTD_WeaponCoreTypes.generated.h"

class AMTD_BaseWeapon;

USTRUCT(BlueprintType)
struct FMTD_WeaponData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="MTD Weapon")
	TSubclassOf<AMTD_BaseWeapon> WeaponClass = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category="MTD Weapon")
	TObjectPtr<UAnimMontage> AttackAnimMontage = nullptr;
};