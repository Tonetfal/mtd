#include "AbilitySystem/Abilities/MTD_GameplayAbility_AttackRanged.h"

#include "AbilitySystem/Attributes/MTD_BalanceSet.h"
#include "AbilitySystem/Attributes/MTD_CombatSet.h"
#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/MTD_BaseCharacter.h"
#include "Character/MTD_BasePlayerCharacter.h"
#include "Character/MTD_TeamComponent.h"
#include "Character/MTD_Tower.h"
#include "Components/CapsuleComponent.h"
#include "Equipment/MTD_EquipmentInstance.h"
#include "Equipment/MTD_EquipmentManagerComponent.h"
#include "Equipment/MTD_WeaponInstance.h"
#include "Inventory/Items/MTD_WeaponItemData.h"
#include "Inventory/MTD_InventoryItemInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/MTD_PlayerState.h"
#include "Projectile/MTD_Projectile.h"
#include "Projectile/MTD_ProjectileCoreTypes.h"
#include "Projectile/MTD_ProjectileMovementComponent.h"

void UMTD_GameplayAbility_AttackRanged::FireProjectile()
{
    FMTD_Parameters Parameters;
    if (!FormParameters(Parameters))
    {
        return;
    }

    if (!SpawnProjectile(Parameters))
    {
        return;
    }

    ComputeFirePoint(Parameters);
    SetupProjectile(Parameters);
    
    UGameplayStatics::FinishSpawningActor(Parameters.SpawnedProjectile, Parameters.ProjectileTransforms);
}

FName UMTD_GameplayAbility_AttackRanged::SelectCollisionProfileName(const EMTD_TeamId InstigatorTeamId) const
{
    switch (InstigatorTeamId)
    {
    case EMTD_TeamId::Player:
    case EMTD_TeamId::Tower:
    case EMTD_TeamId::Core:
        return AllyProjectileCollisionProfileName;

    case EMTD_TeamId::Enemy:
        return EnemyProjectileCollisionProfileName;

    default:
        return NAME_None;
    }
}

bool UMTD_GameplayAbility_AttackRanged::FormParameters(FMTD_Parameters &OutParameters) const
{
    OutParameters.World = GetWorld();
    if (!IsValid(OutParameters.World))
    {
        MTDS_WARN("World is invalid.");
        return false;
    }
    
    OutParameters.AvatarActor = GetAvatarActorFromActorInfo();
    if (!IsValid(OutParameters.AvatarActor))
    {
        MTDS_WARN("Avatar Actor is invalid.");
        return false;
    }
    
    OutParameters.BaseCharacter = Cast<AMTD_BaseCharacter>(OutParameters.AvatarActor);
    if (!IsValid(OutParameters.BaseCharacter))
    {
        MTDS_WARN("Failed to cast Avatar Actor [%s] to MTD Base Character.", *OutParameters.AvatarActor->GetName());
        return false;
    }

    OutParameters.MtdPlayerState = OutParameters.BaseCharacter->GetMtdPlayerState();
    if (!IsValid(OutParameters.MtdPlayerState))
    {
        MTDS_WARN("MTD Player State is invalid.");
        return false;
    }

    OutParameters.EquipmentManager = OutParameters.MtdPlayerState->GetEquipmentManagerComponent();
    if (!IsValid(OutParameters.EquipmentManager))
    {
        MTDS_WARN("Equipment Manager Component is invalid.");
        return false;
    }

    OutParameters.WeaponInstance = Cast<UMTD_WeaponInstance>(OutParameters.EquipmentManager->GetEquipmentInstance(
        EMTD_EquipmentType::Weapon));
    if (!IsValid(OutParameters.WeaponInstance))
    {
        MTDS_WARN("Weapon Instance is invalid.");
        return false;
    }

    OutParameters.ProjectileData = OutParameters.WeaponInstance->GetProjectileData();
    if (!IsValid(OutParameters.ProjectileData))
    {
        MTDS_WARN("Projectile Data on Weapon Instance [%s] is invalid.", *OutParameters.WeaponInstance->GetName());
        return false;
    }

    OutParameters.ProjectileClass = OutParameters.ProjectileData->ProjectileClass;
    if (!OutParameters.ProjectileClass)
    {
        MTDS_WARN("Projectile Class on Projectile Data [%s] is NULL.", *OutParameters.ProjectileData->GetName());
        return false;
    }

    OutParameters.FirePointWorldPosition = OutParameters.WeaponInstance->GetFirePointWorldPosition();
    OutParameters.CharacterRotation = OutParameters.BaseCharacter->GetActorRotation();
    OutParameters.ProjectileTransforms = FTransform(OutParameters.CharacterRotation, OutParameters.FirePointWorldPosition); 

    OutParameters.WeaponItemData = Cast<UMTD_WeaponItemData>(OutParameters.WeaponInstance->GetItemData());
    if (!IsValid(OutParameters.WeaponItemData))
    {
        MTDS_WARN("Weapon Item Data on Weapon Instance [%s] is invalid.", *OutParameters.WeaponInstance->GetName());
        return false;
    }

    OutParameters.MtdAbilitySystemComponent =
        Cast<UMTD_AbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
    if (!IsValid(OutParameters.MtdAbilitySystemComponent))
    {
        MTDS_WARN("Ability System Component is invalid.");
        return false;
    }

    OutParameters.CharacterController = OutParameters.BaseCharacter->GetController();
    if (!IsValid(OutParameters.CharacterController))
    {
        MTDS_WARN("Controller on Character [%s] is invalid.", *OutParameters.BaseCharacter->GetName());
        return false;
    }

    OutParameters.TeamComponent = UMTD_TeamComponent::FindTeamComponent(OutParameters.CharacterController);
    if (!IsValid(OutParameters.TeamComponent))
    {
        MTDS_WARN("Team Component on Controller [%s] is invalid.", *OutParameters.CharacterController->GetName());
        return false;
    }
    
    // Select collision profile name
    const EMTD_TeamId InstigatorTeamId = OutParameters.TeamComponent->GetMtdTeamId();
    OutParameters.CollisionProfileName = SelectCollisionProfileName(InstigatorTeamId);

    OutParameters.bIsPlayer = (!OutParameters.MtdPlayerState->IsABot());
    if (OutParameters.bIsPlayer)
    {
        OutParameters.CameraComponent = OutParameters.BaseCharacter->FindComponentByClass<UCameraComponent>();
        if (!IsValid(OutParameters.CameraComponent))
        {
            MTDS_WARN("Camera Component on Character [%s] is invalid.");
            return false;
        }

        OutParameters.CameraWorldPosition = OutParameters.CameraComponent->GetComponentLocation();
        OutParameters.CameraForward = OutParameters.CameraComponent->GetForwardVector();
    }

    OutParameters.bValid = true;
    return true;
}

bool UMTD_GameplayAbility_AttackRanged::SpawnProjectile(FMTD_Parameters &Parameters) const
{
    constexpr auto SpawnCollisionMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    auto Projectile = Parameters.World->SpawnActorDeferred<AMTD_Projectile>(Parameters.ProjectileClass,
        Parameters.ProjectileTransforms, Parameters.BaseCharacter, Parameters.BaseCharacter, SpawnCollisionMethod);

    Parameters.SpawnedProjectile = Projectile;
    
    const bool bIsValid = IsValid(Projectile);
    if (bIsValid)
    {
        Parameters.ProjectileMovementComponent = Projectile->GetMovementComponent();
        if (!IsValid(Parameters.ProjectileMovementComponent))
        {
            MTDS_WARN("Projectile Movement Component on Projectile [%s] is invalid.", *Projectile->GetName());
            return false;
        }
    }
    
    return bIsValid;
}

bool UMTD_GameplayAbility_AttackRanged::ShouldTraceIgnore(const AActor *HitActor) const
{
    check(IsValid(HitActor));

    const TSubclassOf<AActor> Class = HitActor->GetClass();

#define TRACE_CHECK(X) if (Class->IsChildOf(X ## ::StaticClass())) return true
    TRACE_CHECK(AMTD_BasePlayerCharacter);
    TRACE_CHECK(AMTD_Tower);
    TRACE_CHECK(AMTD_InventoryItemInstance);
#undef TRACE_CHECK
    
    return false;
}

FVector UMTD_GameplayAbility_AttackRanged::GetProjectileDirection(const FMTD_Parameters &Parameters) const
{
    FVector Direction = Parameters.CameraForward;
    
    const FVector TraceStart = Parameters.CameraWorldPosition;
    const FVector TraceEnd = (TraceStart + (Parameters.CameraForward * AimTraceLength));
    const EDrawDebugTrace::Type DrawDebugTrace =
        ((bDebugTrace) ? (EDrawDebugTrace::ForDuration) : (EDrawDebugTrace::None));
    const TArray<AActor*> ActorsToIgnore;
    
    TArray<FHitResult> HitResults;
    UKismetSystemLibrary::LineTraceMulti(
        Parameters.World, TraceStart, TraceEnd, ETraceTypeQuery::TraceTypeQuery3, true, ActorsToIgnore,
        DrawDebugTrace, HitResults, true);

    for (const FHitResult &HitResult : HitResults)
    {
        if (((HitResult.bBlockingHit)) && (!ShouldTraceIgnore(HitResult.GetActor())))
        {
            // Aim at the hitting point
            const FVector Displacement = (HitResult.ImpactPoint - Parameters.FirePointWorldPosition);
            Direction = Displacement.GetSafeNormal();
            break;
        }
    }
    
    return Direction;
}
void UMTD_GameplayAbility_AttackRanged::ComputeFirePoint(FMTD_Parameters &Parameters) const
{
    if (Parameters.bIsPlayer)
    {
        Parameters.ProjectileDirection = GetProjectileDirection(Parameters);
    }
}

void UMTD_GameplayAbility_AttackRanged::SetupProjectileMovementParameters(const FMTD_Parameters &Parameters) const
{
    Parameters.ProjectileMovementComponent->InitialSpeed = Parameters.WeaponItemData->ProjectileSpeed;
    Parameters.ProjectileMovementComponent->MaxSpeed = Parameters.WeaponItemData->ProjectileSpeed;
    Parameters.ProjectileMovementComponent->Direction = Parameters.ProjectileDirection;
}

void UMTD_GameplayAbility_AttackRanged::SetupProjectileDamage(const FMTD_Parameters &Parameters) const
{
    const float RangedDamage = Parameters.MtdAbilitySystemComponent->GetNumericAttributeChecked(
        UMTD_CombatSet::GetDamageRangedBaseAttribute());
    const float BalanceDamage = Parameters.MtdAbilitySystemComponent->GetNumericAttributeChecked(
        UMTD_BalanceSet::GetDamageAttribute());

    Parameters.SpawnedProjectile->Damage = RangedDamage;
    Parameters.SpawnedProjectile->DamageAdditive = DamageAdditive;
    Parameters.SpawnedProjectile->DamageMultiplier = DamageMultiplier;
    Parameters.SpawnedProjectile->bIsRadial = Parameters.WeaponItemData->bRadial;
    Parameters.SpawnedProjectile->RadialRange = Parameters.WeaponItemData->RadialRange;
    Parameters.SpawnedProjectile->BalanceDamage = BalanceDamage;
}

void UMTD_GameplayAbility_AttackRanged::SetupProjectileEffectsToGrantOnHit(const FMTD_Parameters &Parameters) const
{
    for (const TSubclassOf<UMTD_GameplayEffect> GeClass : Parameters.ProjectileData->GameplayEffectsToGrantClasses)
    {
        Parameters.SpawnedProjectile->AddGameplayEffectClassToGrantOnHit(GeClass);
    }
}

void UMTD_GameplayAbility_AttackRanged::SetupProjectileAsc(const FMTD_Parameters &Parameters) const
{
    Parameters.SpawnedProjectile->InitializeAbilitySystem(Parameters.MtdAbilitySystemComponent);
}

void UMTD_GameplayAbility_AttackRanged::SetupProjectileCollisionProfile(const FMTD_Parameters &Parameters) const
{
    // Note: the collision profile must be setup at the very end because afterwards the projectile can immediately hit
    // something if the projectile has spawned inside something that has to collide with it. If not everything
    // initialization wise has taken place yet, it will never be used because the projectile has already impacted
    // something, hence the collision profile must be setup as late as possible.

    // Get the collision component to apply the profile on
    auto CollisionComponent = Parameters.SpawnedProjectile->GetCollisionComponent();
    if (!IsValid(CollisionComponent))
    {
        MTDS_WARN("Collision component on Projectile [%s] is invalid.", *Parameters.SpawnedProjectile->GetName());
        return;
    }

    // Change collision profile
    CollisionComponent->SetCollisionProfileName(Parameters.CollisionProfileName);
}

void UMTD_GameplayAbility_AttackRanged::SetupProjectile(const FMTD_Parameters &Parameters) const
{
    SetupProjectileMovementParameters(Parameters);
    SetupProjectileDamage(Parameters);
    SetupProjectileEffectsToGrantOnHit(Parameters);
    SetupProjectileAsc(Parameters);
    SetupProjectileCollisionProfile(Parameters);
}
