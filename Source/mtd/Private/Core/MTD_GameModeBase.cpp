#include "Core/MTD_GameModeBase.h"

#include "Core/MTD_CoreTypes.h"
#include "Core/MTD_LevelPathManager.h"

#include "Kismet/GameplayStatics.h"

AMTD_GameModeBase::AMTD_GameModeBase()
{
}

void AMTD_GameModeBase::BeginPlay()
{
	Super::BeginPlay();

	FGenericTeamId::SetAttitudeSolver(&SolveTeamAttitude);
	
	TArray<AActor*> LevelPathManagers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), 
		AMTD_LevelPathManager::StaticClass(), LevelPathManagers);
	
	if (LevelPathManagers.IsEmpty())
	{
		MTD_ERROR("Level path manager is missing!");
		return;
	}
	if (LevelPathManagers.Num() > 1)
	{
		MTD_WARN("There are more than one level path manager! "
				"Only the first one will be used!");
	}
	
	LevelPathManager = Cast<AMTD_LevelPathManager>(LevelPathManagers[0]);
}

void AMTD_GameModeBase::StartPlay()
{
	Super::StartPlay();
}

AMTD_LevelPathManager *AMTD_GameModeBase::GetLevelPathManager() const
{
	return LevelPathManager;
}