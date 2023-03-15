#pragma once

#include "Engine/DeveloperSettings.h"
#include "mtd.h"

#include "MTD_PlayerLevelSettings.generated.h"

/**
 * Seetings containing all player's levelling up data.
 */
UCLASS(Config="Game", DefaultConfig, DisplayName="Player Level")
class MTD_API UMTD_PlayerLevelSettings
    : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    /**
     * Get singleton instance of player level settings.
     * @return  Singleton instance of player level settings.
     */
	static UMTD_PlayerLevelSettings *Get();

    //~UDeveloperSettings Interface
    virtual FName GetCategoryName() const override;
    //~End of UDeveloperSettings Interface

private:
    /** Singleton instance of the player level settings. */
    inline static TObjectPtr<UMTD_PlayerLevelSettings> DefaultSettings = nullptr;

public:
    /** Curve tables defining how much experience is needed to get to a certain level. */
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="Default")
    TSoftObjectPtr<UCurveTable> LevelExperienceCurveTable = nullptr;

    /** Curve table defining how many attribute points are given at a certain level. */
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="Default")
    TSoftObjectPtr<UCurveTable> AttributePointsCurveTable = nullptr;
};

inline FName UMTD_PlayerLevelSettings::GetCategoryName() const
{
    return TEXT("MTD");
}
