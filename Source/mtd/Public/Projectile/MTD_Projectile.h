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

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnBeginOverlap(
        UPrimitiveComponent *HitComponent,
        AActor *OtherActor,
        UPrimitiveComponent *OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult &SweepResult);

    UFUNCTION(BlueprintNativeEvent, Category="MTD|Projectile")
    void OnSelfDestroy();
    virtual void OnSelfDestroy_Implementation();

    virtual void ApplyGameplayEffectsToTarget(AActor *Target);
    
    UFUNCTION(BlueprintNativeEvent)
    void OnProjectilePostHit(const FGameplayEventData &EventData);
    virtual void OnProjectilePostHit_Implementation(const FGameplayEventData &EventData);

    UFUNCTION(BlueprintNativeEvent)
    void OnProjectilePreHit(const FGameplayEventData &EventData);
    virtual void OnProjectilePreHit_Implementation(const FGameplayEventData &EventData);

private:
    FGameplayEventData PrepareGameplayEventData(FHitResult HitResult) const;

public:
    UPROPERTY(BlueprintReadWrite)
    float Damage = 0.f;
    
    UPROPERTY(BlueprintReadWrite)
    float DamageMultiplier = 1.f;
    
    UPROPERTY(BlueprintReadWrite)
    float BalanceDamage = 7.5f;

    // TODO: Add radial damage

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components")
    TObjectPtr<UMTD_ProjectileMovementComponent> MovementComponent = nullptr;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<UCapsuleComponent> CollisionComponent = nullptr;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<UStaticMeshComponent> MeshComponent = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Projectile",
        meta=(AllowPrivateAccess="true", ClampMin="0.1"))
    float SecondsToSelfDestroy = 15.f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="MTD|Projectile",
        meta=(AllowPrivateAccess="true"))
    TArray<TSubclassOf<UMTD_GameplayEffect>> GameplayEffectClassesToGrantOnHit;

    UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;
    
    UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
    TSubclassOf<UMTD_GameplayEffect> GameplayEffectDamageClass = nullptr;

    UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
    TArray<FGameplayEffectSpecHandle> GameplayEffectsToGrantOnHit;
};

inline UCapsuleComponent *AMTD_Projectile::GetCollisionComponent() const
{
    return CollisionComponent;
}

inline UMTD_ProjectileMovementComponent *AMTD_Projectile::GetMovementComponent() const
{
    return MovementComponent;
}
