#include "Utility/MTD_MeleeHitboxData.h"

TArray<FMeleeHitSphereDefinition> UMeleeHitboxData::GetMeleeHitSpheres(
	TArray<int32> Indices)
{
	TArray<FMeleeHitSphereDefinition> HitSphereSubset;

	const int32 Spheres = MeleeHitSpheres.Num();
	for (const int32 Index : Indices)
	{
		if (Spheres > Index && Index >= 0)
		{
			HitSphereSubset.Add(MeleeHitSpheres[Index]);
		}
	}

	return HitSphereSubset;
}
