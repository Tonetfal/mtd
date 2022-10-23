#pragma once

#include "MTD_EquipmentCoreTypes.generated.h"

USTRUCT(BlueprintType)
struct FMTD_EquipmentPlayerStats
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, DisplayName="Health")
	float HealthStat = 0.f;
	
	UPROPERTY(EditDefaultsOnly, DisplayName="Damage")
	float DamageStat = 0.f;
	
	UPROPERTY(EditDefaultsOnly, DisplayName="Movement speed")
	float SpeedStat = 0.f;
};

USTRUCT(BlueprintType)
struct FMTD_EquipmentWeaponStats
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	float BaseDamage = 0.f;
};

USTRUCT(BlueprintType)
struct FMTD_EquipmentRangedWeaponStats
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	float BaseDamage = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ProjectileSpeed = 1000.f;
};
