#pragma once

#include "Abilities/GameplayAbility.h"
#include "GameplayEffect.h"
#include "GameFramework/Actor.h"
#include "mtd.h"

#include "MTD_Projectile.generated.h"

class UMTD_GameplayEffect;
class UMTD_ProjectileMovementComponent;
class UMTD_TeamComponent;
class UCapsuleComponent;

UCLASS()
class MTD_API AMTD_Projectile : public AActor
{
    GENERATED_BODY()

public:
    AMTD_Projectile();

    UFUNCTION(BlueprintCallable, Category="MTD|Projectile")
    void InitializeAbilitySystem(UAbilitySystemComponent *InAbilitySystemComponent);
    
    void AddGameplayEffectClassToGrantOnHit(const TSubclassOf<UMTD_GameplayEffect> &GeClass);
    void SetGameplayEffectDamageClass(const TSubclassOf<UMTD_GameplayEffect> &GeClass);

    UCapsuleComponent *GetCollisionComponent() const;
    UMTD_ProjectileMovementComponent *GetMovementComponent() const;

    //~AActor Interface
    virtual void PostInitializeComponents() override;

protected:
    virtual void BeginPlay() override;
    //~End of AActor Interface

    UFUNCTION()
    void OnCollisionHit(
        UPrimitiveComponent *HitComponent,
        AActor *OtherActor,
        UPrimitiveComponent *OtherComp,
        FVector NormalImpulse,
        const FHitResult &Hit);

    UFUNCTION(BlueprintNativeEvent)
    void OnProjectilePreHit(const FGameplayEventData &EventData);
    virtual void OnProjectilePreHit_Implementation(const FGameplayEventData &EventData);
    
    UFUNCTION(BlueprintNativeEvent)
    void OnProjectileHit(const FGameplayEventData &EventData);
    virtual void OnProjectileHit_Implementation(const FGameplayEventData &EventData);
    
    UFUNCTION(BlueprintNativeEvent)
    void OnProjectilePostHit(const FGameplayEventData &EventData);
    virtual void OnProjectilePostHit_Implementation(const FGameplayEventData &EventData);

    UFUNCTION(BlueprintNativeEvent, Category="MTD|Projectile")
    void OnSelfDestroy();
    virtual void OnSelfDestroy_Implementation();

    virtual void ApplyGameplayEffectsToTarget(AActor *Target);

private:
    void SweepActorsAround(TArray<AActor *> &Actors) const;
    void PrepareGameplayEffectSpecHandles(const FGameplayEventData &EventData);
    FGameplayEventData PrepareGameplayEventData(const AActor *HitActor) const;
    FGameplayEffectSpecHandle GetBalanceDamageSpecHandle(const FVector KnockbackDirection,
        const FGameplayEffectContextHandle &EffectContext) const;
    FGameplayEffectSpecHandle GetDamageSpecHandle(const FGameplayEffectContextHandle &EffectContext) const;
    FVector ComputeKnockbackDirection() const;

public:
    float Damage = 0.f;
    float DamageAdditive = 0.f;
    float DamageMultiplier = 1.f;
    float BalanceDamage = 7.5f;
    
    bool bIsRadial = false;
    float RadialRange = 50.f;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components")
    TObjectPtr<UMTD_ProjectileMovementComponent> MovementComponent = nullptr;
    
    /** Gameplay Effect to use to apply damage. */
    UPROPERTY(EditDefaultsOnly, Category="MTD|Projectile")
    TSubclassOf<UGameplayEffect> GameplayEffectDamageInstantClass = nullptr;
    
    /** Gameplay Effect to use to apply balance damage. */
    UPROPERTY(EditDefaultsOnly, Category="MTD|Projectile")
    TSubclassOf<UGameplayEffect> GameplayEffectBalanceDamageInstantClass = nullptr;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UCapsuleComponent> CollisionComponent = nullptr;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UStaticMeshComponent> MeshComponent = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Projectile", meta=(AllowPrivateAccess="true", ClampMin="0.1"))
    float SecondsToSelfDestroy = 15.f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="MTD|Projectile", meta=(AllowPrivateAccess="true"))
    TArray<TSubclassOf<UMTD_GameplayEffect>> GameplayEffectClassesToGrantOnHit;

    UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;
    
    UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
    TSubclassOf<UMTD_GameplayEffect> GameplayEffectDamageClass = nullptr;

    UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
    TArray<FGameplayEffectSpecHandle> GameplayEffectsToGrantOnHit;

    /** Draw sweep for radial attack? */
    UPROPERTY(EditAnywhere, Category="MTD|Projectile|Debug")
    bool bDebugSweep = false;

    /** Time in seconds the radial attack sweep will be drawn for. */
    UPROPERTY(EditAnywhere, Category="MTD|Projectile|Debug", meta=(EditCondition="bDebugSweep"))
    float SweepDrawTime = 5.f;
};

inline UCapsuleComponent *AMTD_Projectile::GetCollisionComponent() const
{
    return CollisionComponent;
}

inline UMTD_ProjectileMovementComponent *AMTD_Projectile::GetMovementComponent() const
{
    return MovementComponent;
}
