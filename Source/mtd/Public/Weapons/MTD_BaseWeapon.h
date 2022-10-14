#pragma once

#include "GameFramework/Actor.h"
#include "mtd.h"
#include "MTD_BaseWeapon.generated.h"

UCLASS()
class MTD_API AMTD_BaseWeapon : public AActor
{
	GENERATED_BODY()

public:

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

	// Data table
};