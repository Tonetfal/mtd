#include "Equipment/MTD_RangedWeaponInstance.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/MTD_CombatSet.h"
#include "Engine/StaticMeshSocket.h"
#include "Utility/MTD_Utility.h"

void UMTD_RangedWeaponInstance::ModStats(float Multiplier)
{
    Super::ModStats(Multiplier);

    UAbilitySystemComponent *Asc = GetAbilitySystemComponent();
    if (!IsValid(Asc))
    {
        return;
    }

    check(Asc->GetAttributeSet(UMTD_CombatSet::StaticClass()));

    Asc->ApplyModToAttribute(UMTD_CombatSet::GetDamageRangedBaseAttribute(),
        EGameplayModOp::Additive, RangedWeaponStats.BaseDamage * Multiplier);
}

FVector UMTD_RangedWeaponInstance::GetFirePointWorldPosition() const
{
    if (!IsValid(FirePointSocket) && !InitializeFirePoint())
    {
        return FVector::ZeroVector;
    }

    FTransform Transform;
    FirePointSocket->GetSocketTransform(Transform, WeaponMesh);

    const FVector SocketLocation = Transform.GetLocation();
    return SocketLocation;
}

bool UMTD_RangedWeaponInstance::InitializeFirePoint() const
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
