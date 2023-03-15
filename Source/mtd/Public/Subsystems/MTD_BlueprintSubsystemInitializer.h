#pragma once

#include "mtd.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Subsystems/WorldSubsystem.h"

#include "MTD_BlueprintSubsystemInitializer.generated.h"

/**
 * Blueprint subsystems are not discovered by the world on load.
 * This subsystem only exists to load the other subsystems in the world.
 */
UCLASS()
class UMTD_BlueprintWorldSubsystemInitializer
    : public UWorldSubsystem 
{
    GENERATED_BODY()

public:
    //~UWorldSubsystem Interface
    virtual void Initialize(FSubsystemCollectionBase &Collection) override;
    //~End of UWorldSubsystem Interface
};

/**
 * Blueprint subsystems are not discovered by the game instance on load.
 * This subsystem only exists to load the other subsystems in the game instance.
 */
UCLASS()
class UMTD_BlueprintGameInstanceSubsystemInitializer
    : public UGameInstanceSubsystem 
{
    GENERATED_BODY()

public:
    //~UGameInstanceSubsystem Interface
    virtual void Initialize(FSubsystemCollectionBase &Collection) override;
    //~End of UGameInstanceSubsystem Interface
};
