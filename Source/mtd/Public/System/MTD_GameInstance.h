#pragma once

#include "Engine/GameInstance.h"
#include "mtd.h"

#include "MTD_GameInstance.generated.h"

UCLASS()
class MTD_API UMTD_GameInstance
    : public UGameInstance
{
    GENERATED_BODY()

public:
    //~UGameInstance Interface
    virtual void Init() override;
    //~End of UGameInstance Interface

    /**
     * Run post initialization for MTD subsystems.
     */
    void PostInitializeSubsystems();
};
