#pragma once

#include "mtd.h"

#include "MTD_BlueprintSubsystemSettings.generated.h"

/**
 * Settings for blueprinted subsystems.
 */
UCLASS(Config="Game", DefaultConfig, DisplayName="Blueprint Subsystems")
class MTD_API UMTD_BlueprintSubsystemSettings
    : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    //~UDeveloperSettings Interface
    virtual FName GetCategoryName() const override;
    //~End of UDeveloperSettings Interface

public:
    /** Blueprinted world subsystems to load. */
    UPROPERTY(Config, EditAnywhere, Category="General")
    TArray<TSoftClassPtr<UWorldSubsystem>> BlueprintWorldSubsystems;
    
    /** Blueprinted game instance subsystems to load. */
    UPROPERTY(Config, EditAnywhere, Category="General")
    TArray<TSoftClassPtr<UGameInstanceSubsystem>> BlueprintGameInstanceSubsystems;
};

inline FName UMTD_BlueprintSubsystemSettings::GetCategoryName() const
{
    return TEXT("MTD");
}
