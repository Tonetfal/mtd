#include "Subsystems/MTD_GameInstanceSubsystem.h"

void UMTD_GameInstanceSubsystem::PostPrioritizedInit()
{
    // Empty
}

int32 UMTD_GameInstanceSubsystem::GetPriority() const
{
    return 10;
}

ETickableTickType UMTD_TickableGameInstanceSubsystem::GetTickableTickType() const 
{ 
	// Don't let CDOs ever tick by default, unless child class doesn't override this function
	const ETickableTickType TickType =
	    ((IsTemplate()) ? (ETickableTickType::Never) : (FTickableGameObject::GetTickableTickType()));
    return TickType;
}

bool UMTD_TickableGameInstanceSubsystem::IsAllowedToTick() const
{
	// No matter what IsTickable says, don't let CDOs or uninitialized world subsystems tick
	return ((!IsTemplate()) && (bInitialized));
}

void UMTD_TickableGameInstanceSubsystem::Tick(float DeltaSeconds)
{
	checkf(IsInitialized(), TEXT("Ticking should have been disabled for an uninitialized subsystem : remember to call "
	    "IsInitialized in the subsystem's IsTickable, IsTickableInEditor and/or IsTickableWhenPaused implementation"));
}

void UMTD_TickableGameInstanceSubsystem::Initialize(FSubsystemCollectionBase &Collection)
{
	check(!bInitialized);
	bInitialized = true;
}

void UMTD_TickableGameInstanceSubsystem::Deinitialize()
{
	check(bInitialized);
	bInitialized = false;
}
