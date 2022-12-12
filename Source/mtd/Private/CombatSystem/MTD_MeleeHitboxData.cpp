#include "CombatSystem/MTD_MeleeHitboxData.h"

TArray<FMTD_MeleeHitSphereDefinition> UMTD_MeleeHitboxData::GetMeleeHitSpheres(TArray<int32> Indices)
{
    TArray<FMTD_MeleeHitSphereDefinition> HitSphereSubset;

    const int32 Spheres = MeleeHitSpheres.Num();
    for (const int32 Index : Indices)
    {
        if ((Spheres > Index) && (Index >= 0))
        {
            HitSphereSubset.Add(MeleeHitSpheres[Index]);
        }
    }

    return HitSphereSubset;
}
