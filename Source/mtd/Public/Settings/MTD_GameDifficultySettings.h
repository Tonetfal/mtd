#pragma once

#include "Engine/DeveloperSettings.h"
#include "mtd.h"

#include "MTD_GameDifficultySettings.generated.h"

class UMTD_FoeQuantityDefinition;
class UMTD_LevelDefinition;

/**
 * Settings containing all game difficulty data.
 */
UCLASS(Config="Game", DefaultConfig, DisplayName="Difficulty System")
class MTD_API UMTD_GameDifficultySettings
    : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    /**
     * Get singleton instance of inventory system settings.
     * @return  Singleton instance of inventory system settings.
     */
	static UMTD_GameDifficultySettings *Get();

    //~UDeveloperSettings Interface
    virtual FName GetCategoryName() const override;
    //~End of UDeveloperSettings Interface

private:
    /** Singleton instance of the inventory system settings. */
    inline static TObjectPtr<UMTD_GameDifficultySettings> DefaultSettings = nullptr;

public:
    /** All available game levels. */
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="General")
    TArray<TSoftObjectPtr<UMTD_LevelDefinition>> GameLevels;

    /** Asset defining how much Quantity is required to spawn an enemy by default. */
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="General")
    TSoftObjectPtr<UMTD_FoeQuantityDefinition> FoeQuantityAsset = nullptr;
};

inline FName UMTD_GameDifficultySettings::GetCategoryName() const
{
    return TEXT("MTD");
}
