#pragma once

#include "AbilitySystem/Abilities/MTD_GameplayAbility.h"
#include "mtd.h"
#include "MTD_CoreTypes.h"

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
 * Gameplay ability to setup a projectile that will perform the final logic.
 *
 * Note: This is an abstract class because it lacks gameplay event listening, it should be done inside blueprints.
 * Without it we don't know when to fire a projectile.
 */
UCLASS(Abstract)
class MTD_API UMTD_GameplayAbility_AttackRanged
    : public UMTD_GameplayAbility
{
    GENERATED_BODY()

private:
    /**
     * Simple structure containing all cached and validated data used to create and setup a projectile.
     */
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
        FName CollisionProfileName = NAME_None;

        AMTD_Projectile *SpawnedProjectile = nullptr;
        UMTD_ProjectileMovementComponent *ProjectileMovementComponent = nullptr;
        FVector ProjectileDirection = FVector::ZeroVector;

        FVector FirePointWorldPosition = FVector::ZeroVector;
        FRotator CharacterRotation = FRotator::ZeroRotator;
        FTransform ProjectileTransforms = FTransform::Identity;

        const UCameraComponent *CameraComponent = nullptr;
        FVector CameraWorldPosition = FVector::ZeroVector;
        FVector CameraForward = FVector::ZeroVector;

        bool bValid = false;
        bool bIsPlayer = false;
    };

protected:
    /**
     * Create and setup a projectile.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Attack Ranged Gameplay Ability")
    void FireProjectile();

private:
    /**
     * Cache and validate parameters struct.
     * @param   OutParameters: output parameters struct.
     * @return  If true, everything is cached and validated successfully, false otherwise.
     */
    bool FormParameters(FMTD_Parameters &OutParameters) const;

    /**
     * Select a collision profile name to use for a given team ID.
     * @param   InstigatorTeamId: character's who has fired a projectile team ID.
     * @return  Collision profile name to use for a given team ID.
     */
    virtual FName SelectCollisionProfileName(const EMTD_TeamId InstigatorTeamId) const;

    /**
     * Tells whether given actor should be used to aim assist or not.
     * @param   HitActor: actor to check.
     * @return  If true, the actor should be aim assisted at, false otherwise.
     */
    virtual bool ShouldTraceIgnore(const AActor *HitActor) const;

    /**
     * Spawn a projectile.
     * @param   Parameters: struct holding all cached and validated data to create and setup a projectile.
     * @return  If true, the projectile has been successfully created, and its data has been cached and validated,
     * false otherwise.
     */
    bool SpawnProjectile(FMTD_Parameters &Parameters) const;
    
    /**
     * Get direction the projectile should move towards.
     * @param   Parameters: struct holding all cached and validated data to create and setup a projectile.
     * @return  Direction the projectile should move towards.
     */
    FVector GetProjectileDirection(const FMTD_Parameters &Parameters) const;
    
    /**
     * Compute point the projectile has to be fired from.
     * @param   Parameters: struct holding all cached and validated data to create and setup a projectile.
     */
    void ComputeFirePoint(FMTD_Parameters &Parameters) const;
    
    /**
     * Setup projectile movement component's parameters.
     * @param   Parameters: struct holding all cached and validated data to create and setup a projectile.
     */
    void SetupProjectileMovementParameters(const FMTD_Parameters &Parameters) const;
    
    /**
     * Setup projectile's damage related parameters.
     * @param   Parameters: struct holding all cached and validated data to create and setup a projectile.
     */
    void SetupProjectileDamage(const FMTD_Parameters &Parameters) const;
    
    /**
     * Setup projectile's gameplay effects to grant on hit.
     * @param   Parameters: struct holding all cached and validated data to create and setup a projectile.
     */
    void SetupProjectileEffectsToGrantOnHit(const FMTD_Parameters &Parameters) const;
    
    /**
     * Setup projectile's gameplay ability system to use.
     * @param   Parameters: struct holding all cached and validated data to create and setup a projectile.
     */
    void SetupProjectileAsc(const FMTD_Parameters &Parameters) const;
    
    /**
     * Setup projectile's collision.
     * @param   Parameters: struct holding all cached and validated data to create and setup a projectile.
     */
    void SetupProjectileCollision(const FMTD_Parameters &Parameters) const;
    
    /**
     * Perform the whole projectile setup process.
     * @param   Parameters: struct holding all cached and validated data to create and setup a projectile.
     */
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

