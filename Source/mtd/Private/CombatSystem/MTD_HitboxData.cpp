#include "CombatSystem/MTD_HitboxData.h"

TArray<FMTD_HitboxDefinition> UMTD_HitboxData::GetMeleeHitSpheres(const TArray<int32> &Indices)
{
    TArray<FMTD_HitboxDefinition> HitboxesSubset;

    const int32 Spheres = Hitboxes.Num();
    for (const int32 Index : Indices)
    {
        if (((Spheres > Index) && (Index >= 0)))
        {
            HitboxesSubset.Add(Hitboxes[Index]);
        }
    }

    return HitboxesSubset;
}
