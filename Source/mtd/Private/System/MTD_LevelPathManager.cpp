#include "System/MTD_LevelPathManager.h"

AMTD_LevelPathManager::AMTD_LevelPathManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMTD_LevelPathManager::BeginPlay()
{
	Super::BeginPlay();

	TArray<int32> PathsToDelete;
	int32 i = 0;
	for (const FMTD_Path &Path: Paths)
	{
		if (Path.Points.IsEmpty())
		{
			MTD_WARN("Path '%s' is empty, so it will be removed from the list!",
				*Path.Name.ToString());
					
			PathsToDelete.Add(i);
		}
		i++;
	}

	for (const int32 Index : PathsToDelete)
	{
		Paths.RemoveAt(Index);
	}
}

int32 FindClosestPoint(const FMTD_Path &Path, FVector Location)
{
	const TArray<FMTD_PathPoint> Points = Path.Points;
	if (Points.IsEmpty())
	{
		MTD_WARN("Passed an empty path container");
		return -1;
	}

	int32 ClosestPoint = 0;
	float ClosestDistance = FVector(Location - Points[0].Point).Length();

	int32 i = 0;
	for (FMTD_PathPoint Point : Points)
	{
		const float Distance = FVector(Location - Point.Point).Length();
		if (ClosestDistance > Distance)
		{
			ClosestPoint = i;
			ClosestDistance = Distance;
		}
		i++;
	}
	return ClosestPoint;
}

TArray<FVector> AMTD_LevelPathManager::GetNavigationPath(
	FVector Location, FName PathChannel)
{
	const FMTD_Path *Found = Paths.FindByPredicate(
		[&] (const FMTD_Path &Path)
		{
			return Path == PathChannel;
		});
	
	if (!Found)
	{
		MTD_WARN("Path '%s' does not exist", *PathChannel.ToString());
		return {};
	}
	
	const int32 StartIndex = FindClosestPoint(*Found, Location);
	const int32 Num = Found->Points.Num();
	if (StartIndex == -1 || StartIndex >= Num)
		return {};

	TArray<FVector> NavPath;
	for (int32 i = StartIndex; i < Num; i++)
		NavPath.Add(Found->Points[i].Point);
	
	return NavPath;
}
