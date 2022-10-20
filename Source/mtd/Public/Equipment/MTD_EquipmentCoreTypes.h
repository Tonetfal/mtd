#pragma once

#include "MTD_EquipmentCoreTypes.generated.h"

USTRUCT(BlueprintType)
struct FMTD_EquipmentPlayerStats
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float HealthStat = 0.f;
	
	UPROPERTY(EditAnywhere)
	float DamageStat = 0.f;
	
	UPROPERTY(EditAnywhere)
	float SpeedStat = 0.f;
};

USTRUCT(BlueprintType)
struct FMTD_EquipmentWeaponStats
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float BaseDamage = 0.f;
};
