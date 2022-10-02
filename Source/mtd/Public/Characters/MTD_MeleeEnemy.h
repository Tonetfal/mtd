#pragma once

#include "mtd.h"
#include "Characters/MTD_BaseEnemy.h"

#include "MTD_MeleeEnemy.generated.h"

UCLASS(Blueprintable)
class MTD_API AMTD_MeleeEnemy : public AMTD_BaseEnemy
{
	GENERATED_BODY()

public:
	virtual void Attack() override;
};
