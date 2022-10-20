#pragma once

#include "Equipment/MTD_EquipmentInstance.h"
#include "mtd.h"

#include "MTD_WeaponInstance.generated.h"

UCLASS()
class MTD_API UMTD_WeaponInstance : public UMTD_EquipmentInstance
{
	GENERATED_BODY()

protected:
	virtual void ModStats(float Multiplier) override;

private:
	UPROPERTY(EditAnywhere, Category="MTD|Stats",
		meta=(AllowPrivateAccess="true"))
	FMTD_EquipmentWeaponStats WeaponStats;
};
