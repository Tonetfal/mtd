#include "Equipment/MTD_WeaponInstance.h"

#include "AbilitySystem/Attributes/MTD_CombatSet.h"
#include "AbilitySystemComponent.h"
#include "Engine/StaticMeshSocket.h"
#include "Inventory/Items/MTD_WeaponItemData.h"
#include "Utility/MTD_Utility.h"

FVector UMTD_WeaponInstance::GetFirePointWorldPosition() const
{
    // Avoid running the function if initialization failed
    if (!InitializeFirePoint())
    {
        return FVector::ZeroVector;
    }

    // Get socket transforms
    FTransform Transform;
    FirePointSocket->GetSocketTransform(Transform, WeaponMesh);

    // Get socket location
    const FVector &SocketLocation = Transform.GetLocation();
    return SocketLocation;
}

void UMTD_WeaponInstance::ModStats_Internal(float Multiplier, UAbilitySystemComponent *AbilitySystemComponent)
{
    Super::ModStats_Internal(Multiplier, AbilitySystemComponent);

    check(AbilitySystemComponent->GetAttributeSet(UMTD_CombatSet::StaticClass()));

    // Modify owner's melee and ranged damage
    const auto WeaponData = CastChecked<UMTD_WeaponItemData>(ItemData);
    MOD_ATTRIBUTE_BASE(Combat, DamageBase, WeaponData->WeaponItemParameters.MeleeDamage);
    MOD_ATTRIBUTE_BASE(Combat, DamageRangedBase, WeaponData->WeaponItemParameters.RangedDamage);
}

bool UMTD_WeaponInstance::InitializeFirePoint() const
{
    // Avoid initializing twice
    if (((IsValid(WeaponMesh)) && (IsValid(FirePointSocket))))
    {
        return true;
    }
    
    const AActor *Actor = GetSpawnedActor();
    if (!IsValid(Actor))
    {
        return false;
    }

    // Save weapon mesh object
    WeaponMesh = FMTD_Utility::GetActorComponent<UStaticMeshComponent>(Actor);
    if (!IsValid(WeaponMesh))
    {
        return false;
    }

    // Save fire point socket on weapon
    FirePointSocket = WeaponMesh->GetSocketByName(FirePointSocketName);
    if (!IsValid(FirePointSocket))
    {
        return false;
    }

    return true;
}
