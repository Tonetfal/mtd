#pragma once

#include "GameplayEffect.h"
#include "GameFramework/Actor.h"
#include "mtd.h"

#include "MTD_Projectile.generated.h"

class UCapsuleComponent;
class UMTD_GameplayEffect;
class UMTD_ProjectileMovementComponent;
class UMTD_TeamComponent;
struct FGameplayEventData;

/**
 * Projectile actor that is fired by a ranged weapon or a tower.
 */
UCLASS()
class MTD_API AMTD_Projectile
    : public AActor
{
    GENERATED_BODY()

public:
    AMTD_Projectile();

    /**
     * Initialize with ability system component.
     * @param   InAbilitySystemComponent: instigator's ability system component to initialize with.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Projectile")
    void InitializeAbilitySystem(UAbilitySystemComponent *InAbilitySystemComponent);

    /**
     * Add gameplay effect class to grant when hitting an actor with ability system.
     * @param   GeClass: gameplay effect class to add.
     */
    void AddGameplayEffectClassToGrantOnHit(const TSubclassOf<UMTD_GameplayEffect> &GeClass);

    /*
     * Set damage gameplay effect to use to deal damage to actors.
     * @param   GeClass: gameplay effect class to use as damage class.
     */
    void SetGameplayEffectDamageClass(const TSubclassOf<UMTD_GameplayEffect> &GeClass);

    /**
     * Get collision component.
     * @return  Collision component.
     */
    UCapsuleComponent *GetCollisionComponent() const;

    /**
     * Get movement component.
     * @return Movement component.
     */
    UMTD_ProjectileMovementComponent *GetMovementComponent() const;

    //~AActor Interface
    virtual void PostInitializeComponents() override;

protected:
    virtual void BeginPlay() override;
    //~End of AActor Interface

    /**
     * Event to fire when projectile hits something.
     */
    UFUNCTION()
    void OnCollisionHit(
        UPrimitiveComponent *HitComponent,
        AActor *OtherActor,
        UPrimitiveComponent *OtherComp,
        FVector NormalImpulse,
        const FHitResult &Hit);

    /**
     * Custom logic to call prior to main hit logic.
     * @param   EventData: data describing the hit event.
     */
    UFUNCTION(BlueprintNativeEvent)
    void OnProjectilePreHit(const FGameplayEventData &EventData);
    virtual void OnProjectilePreHit_Implementation(const FGameplayEventData &EventData);
    
    /**
     * Hit logic.
     * @param   EventData: data describing the hit event.
     */
    UFUNCTION(BlueprintNativeEvent)
    void OnProjectileHit(const FGameplayEventData &EventData);
    virtual void OnProjectileHit_Implementation(const FGameplayEventData &EventData);
    
    /**
     * Custom logic to call after main hit logic.
     * @param   EventData: data describing the hit event.
     */
    UFUNCTION(BlueprintNativeEvent)
    void OnProjectilePostHit(const FGameplayEventData &EventData);
    virtual void OnProjectilePostHit_Implementation(const FGameplayEventData &EventData);

    /**
     * Event to call when projectile has to get destroyed for some reason.
     */
    UFUNCTION(BlueprintNativeEvent, Category="MTD|Projectile")
    void OnSelfDestroy();
    virtual void OnSelfDestroy_Implementation();

    /**
     * Apply all gameplay effects to a target.
     * @param   Target: target actor that will be granted with gameplay effects.
     */
    virtual void ApplyGameplayEffectsToTarget(AActor *Target);

private:
    /**
     * Sweep around projectile to find all hitable actors.
     * @param   OutActors: output parameters. Actors to hit.
     */
    void SweepActorsAround(TArray<AActor *> &OutActors) const;

    /**
     * Instantiate all gameplay effect spec handles to grant on a hit target.
     * @param   EventData: data describing the hit event.
     */
    void PrepareGameplayEffectSpecHandles(const FGameplayEventData &EventData);

    /**
     * Create gameplay event data describing hit.
     * @param   HitResult: hit data.
     * @return  Gameplay event data.
     */
    FGameplayEventData PrepareGameplayEventData(const FHitResult &HitResult) const;

    /**
     * Instantiate balance damage spec handle.
     * @param   KnockBackDirection: direction hit actor will need to knock back towards.
     * @param   EffectContext: gameplay effect context handle to use to create the effect.
     * @return  Balance damage spec handle.
     */
    FGameplayEffectSpecHandle GetBalanceDamageSpecHandle(const FVector KnockBackDirection,
        const FGameplayEffectContextHandle &EffectContext) const;
    
    /**
     * Instantiate damage spec handle.
     * @param   EffectContext: gameplay effect context handle to use to create the effect.
     * @return  Damage spec handle.
     */
    FGameplayEffectSpecHandle GetDamageSpecHandle(const FGameplayEffectContextHandle &EffectContext) const;

    /**
     * Compute direction a hit actor needs to knock back towards.
     */
    FVector ComputeKnockBackDirection() const;

public:
    /** Amount of base damage to deal. */
    float Damage = 0.f;

    /** Amount of flat damage to add to final damage. Is not scaled. */
    float DamageAdditive = 0.f;

    /** Scale damage value to multiply the damage by. */
    float DamageMultiplier = 1.f;

    /** Amount of balance damage to deal to an actor. Is not scaled. */
    float BalanceDamage = 7.5f;

    /**
     * If true, projectile will deal damage around destroy point, if false, it will not.
     *
     * @see RadialRange
     */
    bool bIsRadial = false;

    /** Range from projectile that's used to search for targets in. */
    float RadialRange = 50.f;

protected:
    /** Movement component to use to move projectiles. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components")
    TObjectPtr<UMTD_ProjectileMovementComponent> MovementComponent = nullptr;
    
    /** Gameplay effect to use to apply damage. */
    UPROPERTY(EditDefaultsOnly, Category="MTD|Projectile")
    TSubclassOf<UGameplayEffect> GameplayEffectDamageInstantClass = nullptr;
    
    /** Gameplay effect to use to apply balance damage. */
    UPROPERTY(EditDefaultsOnly, Category="MTD|Projectile")
    TSubclassOf<UGameplayEffect> GameplayEffectBalanceDamageInstantClass = nullptr;

private:
    /** Collision component to use to decide whether we hit something or not. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UCapsuleComponent> CollisionComponent = nullptr;

    /** Visual representation of projectile. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UStaticMeshComponent> MeshComponent = nullptr;

    /** Maximum time in seconds a projectile will live for, automatically destroyed when expires. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Projectile", meta=(AllowPrivateAccess="true",
        ClampMin="0.1"))
    float TimeToSelfDestroy = 15.f;

    /** Gameplay effects classes to grant on hit to hit actors. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="MTD|Projectile", meta=(AllowPrivateAccess="true"))
    TArray<TSubclassOf<UMTD_GameplayEffect>> GameplayEffectClassesToGrantOnHit;

    /** Instigator's ability system component. */
    UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;

    /** Gameplay effect class to deal damage. */
    UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
    TSubclassOf<UMTD_GameplayEffect> GameplayEffectDamageClass = nullptr;
    
    /** Gameplay effect spec handles to deal grant on hit to hit actors. */
    UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
    TArray<FGameplayEffectSpecHandle> GameplayEffectsToGrantOnHit;

    /** Draw sweep for radial attack? */
    UPROPERTY(EditAnywhere, Category="MTD|Projectile|Debug")
    bool bDebugSweep = false;

    /** Time in seconds the radial attack sweep is drawn for. */
    UPROPERTY(EditAnywhere, Category="MTD|Projectile|Debug", meta=(EditCondition="bDebugSweep"))
    float SweepDrawTime = 5.f;
};

inline void AMTD_Projectile::AddGameplayEffectClassToGrantOnHit(const TSubclassOf<UMTD_GameplayEffect> &GeClass)
{
    GameplayEffectClassesToGrantOnHit.Add(GeClass);
}

inline void AMTD_Projectile::SetGameplayEffectDamageClass(const TSubclassOf<UMTD_GameplayEffect> &GeClass)
{
    GameplayEffectDamageClass = GeClass;
}

inline UCapsuleComponent *AMTD_Projectile::GetCollisionComponent() const
{
    return CollisionComponent;
}

inline UMTD_ProjectileMovementComponent *AMTD_Projectile::GetMovementComponent() const
{
    return MovementComponent;
}
