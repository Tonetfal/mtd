#pragma once

#include "Engine/AssetManager.h"
#include "mtd.h"

#include "MTD_AssetManager.generated.h"

UCLASS()
class MTD_API UMTD_AssetManager : public UAssetManager
{
    GENERATED_BODY()

public:
    UMTD_AssetManager();

    static UMTD_AssetManager &Get();

protected:
    //~UAssetManager interface
    virtual void StartInitialLoading() override;
    //~End of UAssetManager interface

private:
    void InitializeAbilitySystem();
};
