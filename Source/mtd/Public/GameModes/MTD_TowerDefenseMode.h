#pragma once

#include "mtd.h"
#include "GameModes/MTD_GameModeBase.h"

#include "MTD_TowerDefenseMode.generated.h"

class AMTD_Core;

UCLASS()
class MTD_API AMTD_TowerDefenseMode : public AMTD_GameModeBase
{
    GENERATED_BODY()

public:
    AMTD_TowerDefenseMode();

protected:
    virtual void BeginPlay() override;

public:
    virtual AActor *GetGameTarget(APawn *Client) const override;

private:
    UFUNCTION()
    void OnCoreDestroyed();

private:
    TArray<TObjectPtr<AMTD_Core>> Cores;
};
