#pragma once

#include "Components/PawnComponent.h"
#include "mtd.h"

#include "MTD_PawnComponent.generated.h"

UINTERFACE(BlueprintType)
class MTD_API UMTD_ReadyInterface
    : public UInterface
{
    GENERATED_BODY()
};

class IMTD_ReadyInterface
{
    GENERATED_BODY()

public:
    /**
     * Check whether this component is ready to initialize.
     */
    virtual bool IsPawnComponentReadyToInitialize() const = 0;
};


UCLASS(Blueprintable, ClassGroup="Pawn", meta=(BlueprintSpawnableComponent))
class MTD_API UMTD_PawnComponent
    : public UPawnComponent
    , public IMTD_ReadyInterface
{
    GENERATED_BODY()

public:
    //~IMTD_ReadyInterface Interface
    virtual bool IsPawnComponentReadyToInitialize() const override
    {
        return true;
    }
    //~End of IMTD_ReadyInterface Interface
};
