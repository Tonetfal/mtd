#pragma once

#include "mtd.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "MTD_GameInstanceSubsystem.generated.h"

/**
 * Custom game instance subsystem suppoting priority base initialization.
 */
UCLASS(Abstract)
class MTD_API UMTD_GameInstanceSubsystem
    : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    /**
     * Event to call when all UGameInstanceSubsystems have been initialized. The order is defined by their priorities.
     */
    virtual void PostPrioritizedInit();

    /**
     * Get initialization priority. Higher the number, higher the priority.
     * @return  Initialization priority.
     */
    virtual int32 GetPriority() const;
};

/**
 * Custom game instance subsystem suppoting priority base initialization and ticking.
 */
UCLASS(Abstract)
class MTD_API UMTD_TickableGameInstanceSubsystem
    : public UMTD_GameInstanceSubsystem
    , public FTickableGameObject
{
    GENERATED_BODY()

public:
    //~FTickableGameObject Interface
    virtual ETickableTickType GetTickableTickType() const override;
    virtual bool IsAllowedToTick() const override final;
    virtual void Tick(float DeltaSeconds) override;
    virtual TStatId GetStatId() const override PURE_VIRTUAL(UTickableWorldSubsystem::GetStatId, return TStatId(););
    //~End of FTickableGameObject Interface

    //~UMTD_GameInstanceSubsystem Interface
    virtual void Initialize(FSubsystemCollectionBase &Collection) override;
    virtual void Deinitialize() override;
    //~End of UMTD_GameInstanceSubsystem Interface

    /**
     * Check whether the subsystem is initialized.
     * @return  If true, subsystem is initialized, false otherwise.
     */
    bool IsInitialized() const;

private:
    /** If true, subsystem is initialized, false otherwise. */
    bool bInitialized = false;
};

inline bool UMTD_TickableGameInstanceSubsystem::IsInitialized() const
{
    return bInitialized;
}
