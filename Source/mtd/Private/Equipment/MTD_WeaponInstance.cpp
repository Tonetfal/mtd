#include "Equipment/MTD_WeaponInstance.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/MTD_BalanceSet.h"
#include "AbilitySystem/Attributes/MTD_CombatSet.h"
#include "Engine/StaticMeshSocket.h"
#include "Inventory/Items/MTD_WeaponItemData.h"
#include "Utility/MTD_Utility.h"

void UMTD_WeaponInstance::ModStats_Internal(float Multiplier, UAbilitySystemComponent *Asc)
{
    Super::ModStats_Internal(Multiplier, Asc);

    check(Asc->GetAttributeSet(UMTD_CombatSet::StaticClass()));
    check(Asc->GetAttributeSet(UMTD_BalanceSet::StaticClass()));

    const auto WeaponData = Cast<UMTD_WeaponItemData>(ItemData);
    check(WeaponData);

    APPLY_MOD_TO_ATTRIBUTE(Combat, DamageBase, WeaponData->MeleeDamage);
    APPLY_MOD_TO_ATTRIBUTE(Combat, DamageRangedBase, WeaponData->RangedDamage);
}

FVector UMTD_WeaponInstance::GetFirePointWorldPosition() const
{
    if ((!IsValid(FirePointSocket)) && (!InitializeFirePoint()))
    {
        return FVector::ZeroVector;
    }

    FTransform Transform;
    FirePointSocket->GetSocketTransform(Transform, WeaponMesh);

    const FVector SocketLocation = Transform.GetLocation();
    return SocketLocation;
}

bool UMTD_WeaponInstance::InitializeFirePoint() const
{
    const AActor *Actor = GetSpawnedActor();
    if (!IsValid(Actor))
    {
        return false;
    }

    WeaponMesh = FMTD_Utility::GetActorComponent<UStaticMeshComponent>(Actor);
    if (!IsValid(WeaponMesh))
    {
        return false;
    }

    FirePointSocket = WeaponMesh->GetSocketByName(FirePointSocketName);
    if (!IsValid(FirePointSocket))
    {
        return false;
    }

    return true;
}
