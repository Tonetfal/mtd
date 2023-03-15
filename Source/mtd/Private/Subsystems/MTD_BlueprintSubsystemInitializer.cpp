#include "Subsystems/MTD_BlueprintSubsystemInitializer.h"

#include "Settings/MTD_BlueprintSubsystemSettings.h"

void UMTD_BlueprintWorldSubsystemInitializer::Initialize(FSubsystemCollectionBase &Collection)
{
    Super::Initialize(Collection);

    const auto Settings = GetDefault<UMTD_BlueprintSubsystemSettings>();
    for (const TSoftClassPtr<UWorldSubsystem> &Subsystem : Settings->BlueprintWorldSubsystems)
    {
        if (!Subsystem)
        {
            MTDS_WARN("A subsystem is invalid.");
            continue;
        }
        
        Collection.InitializeDependency(Subsystem.LoadSynchronous());
    }
}

void UMTD_BlueprintGameInstanceSubsystemInitializer::Initialize(FSubsystemCollectionBase &Collection)
{
    Super::Initialize(Collection);
    
    const auto Settings = GetDefault<UMTD_BlueprintSubsystemSettings>();
    for (const TSoftClassPtr<UGameInstanceSubsystem> &Subsystem : Settings->BlueprintGameInstanceSubsystems)
    {
        if (!Subsystem)
        {
            MTDS_WARN("A subsystem is invalid.");
            continue;
        }
        
        Collection.InitializeDependency(Subsystem.LoadSynchronous());
    }
}
