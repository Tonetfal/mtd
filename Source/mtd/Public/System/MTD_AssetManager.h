#pragma once

#include "Engine/AssetManager.h"
#include "mtd.h"

#include "MTD_AssetManager.generated.h"

/**
 * Default asset manager used in this project.
 *
 * It's needed to initialize gameplay ability system.
 */
UCLASS()
class MTD_API UMTD_AssetManager
    : public UAssetManager
{
    GENERATED_BODY()

public:
    UMTD_AssetManager();

    /**
     * Get singleton instance of MTD asset manager.
     * @return  Singleton instance of MTD asset manager.
     */
    static UMTD_AssetManager &Get();

protected:
    //~UAssetManager interface
    virtual void StartInitialLoading() override;
    //~End of UAssetManager interface

private:
    /**
     * Initialize native gameplay tags before ability system.
     */
    void InitializeAbilitySystem();
};
