#pragma once

#include "mtd.h"
#include "UObject/Interface.h"

#include "MTD_MeleeEventsInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UMTD_MeleeCharacterInterface : public UInterface
{
	GENERATED_BODY()
};

class MTD_API IMTD_MeleeCharacterInterface
{
	GENERATED_BODY()

public:
	virtual void AddMeleeHitboxes(const TArray<FName> &HitboxNicknames) = 0;
	virtual void RemoveMeleeHitboxes(const TArray<FName> &HitboxNicknames) = 0;

    virtual void DisableMeleeHitboxes() = 0;
    virtual void ResetMeleeHitTargets() = 0;
};
