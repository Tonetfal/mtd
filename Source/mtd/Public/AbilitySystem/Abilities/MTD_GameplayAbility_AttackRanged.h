#pragma once

#include "AbilitySystem/Abilities/MTD_GameplayAbility.h"
#include "mtd.h"

#include "MTD_GameplayAbility_AttackRanged.generated.h"

class AMTD_BaseCharacter;
class AMTD_PlayerState;
class AMTD_Projectile;
class UCameraComponent;
class UMTD_AbilitySystemComponent;
class UMTD_EquipmentManagerComponent;
class UMTD_ProjectileData;
class UMTD_ProjectileMovementComponent;
class UMTD_TeamComponent;
class UMTD_WeaponInstance;
class UMTD_WeaponItemData;

/**
 * Gameplay Ability to setup a projectile that will perform the final logic.
 *
 * Note: This is an abstract class because it lacks gameplay event listening. It should be done inside BPs. Without it
 * we don't know when to fire a projectile.
 */
UCLASS(Abstract)
class MTD_API UMTD_GameplayAbility_AttackRanged
    : public UMTD_GameplayAbility
{
    GENERATED_BODY()

private:
    struct FMTD_Parameters
    {
        UWorld *World = nullptr;
        UMTD_AbilitySystemComponent *MtdAbilitySystemComponent = nullptr;
        AActor *AvatarActor = nullptr;
        AMTD_BaseCharacter *BaseCharacter = nullptr;
        const AMTD_PlayerState *MtdPlayerState = nullptr;
        const UMTD_EquipmentManagerComponent *EquipmentManager = nullptr;
        const UMTD_WeaponInstance *WeaponInstance = nullptr;
        const UMTD_ProjectileData *ProjectileData = nullptr;
        TSubclassOf<AMTD_Projectile> ProjectileClass = nullptr;
        const UMTD_WeaponItemData *WeaponItemData = nullptr;
        const AController *CharacterController = nullptr;
        const UMTD_TeamComponent *TeamComponent = nullptr;

        AMTD_Projectile *SpawnedProjectile = nullptr;
        UMTD_ProjectileMovementComponent *ProjectileMovementComponent = nullptr;
        FVector ProjectileDirection = FVector::ZeroVector;

        FVector FirePointWorldPosition = FVector::ZeroVector;
        FRotator CharacterRotation = FRotator::ZeroRotator;

        const UCameraComponent *CameraComponent = nullptr;
        FVector CameraWorldPosition = FVector::ZeroVector;
        FVector CameraForward = FVector::ZeroVector;

        bool bValid = false;
        bool bIsPlayer = false;
    };

protected:
    UFUNCTION(BlueprintCallable, Category="MTD|Attack Ranged Gameplay Ability")
    void FireProjectile();

private:
    bool FormParameters(FMTD_Parameters &OutParameters) const;
    bool SpawnProjectile(FMTD_Parameters &Parameters) const;
    bool ShouldTraceIgnore(const AActor *HitActor) const;
    FVector GetProjectileDirection(const FMTD_Parameters &Parameters) const;
    void ComputeFirePoint(FMTD_Parameters &Parameters) const;
    void SetupProjectileMovementParameters(const FMTD_Parameters &Parameters) const;
    void SetupProjectileDamage(const FMTD_Parameters &Parameters) const;
    void SetupProjectileEffectsToGrantOnHit(const FMTD_Parameters &Parameters) const;
    void SetupProjectileAsc(const FMTD_Parameters &Parameters) const;
    void SetupProjectileCollisionProfile(const FMTD_Parameters &Parameters) const;
    void SetupProjectile(const FMTD_Parameters &Parameters) const;

protected:
    /** Aim assist trace length. */
    UPROPERTY(EditDefaultsOnly, Category="MTD|Attack Ranged Gameplay Ability")
    float AimTraceLength = 10000.f;

    /** Draw trace for aim assist? */
    UPROPERTY(EditDefaultsOnly, Category="MTD|Attack Ranged Gameplay Ability|Debug")
    bool bDebugTrace = false;

    /** Time in seconds aim assist trace line will be drawn for. */
    UPROPERTY(EditDefaultsOnly, Category="MTD|Attack Ranged Gameplay Ability|Debug", meta=(EditCondition="bDebugTrace"))
    float TraceDrawTime = 5.f;
};

