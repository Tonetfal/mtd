#pragma once

#include "GameplayEffect.h"
#include "GameFramework/Actor.h"
#include "mtd.h"

#include "MTD_Projectile.generated.h"

class UMTD_ProjectileMovementComponent;
class UMTD_TeamComponent;
class UCapsuleComponent;

UCLASS()
class MTD_API AMTD_Projectile : public AActor
{
    GENERATED_BODY()

public:
    AMTD_Projectile();

    const UCapsuleComponent *GetCollisionComponent() const;
    UCapsuleComponent *GetCollisionComponent();

    const UMTD_ProjectileMovementComponent *GetMovementComponent() const;
    UMTD_ProjectileMovementComponent *GetMovementComponent();

protected:
    virtual void BeginPlay() override;

protected:
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

    virtual void ApplyGameplayEffectToTarget(AActor *Target);

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TArray<FGameplayEffectSpecHandle> GameplayEffectsToGrantOnHit;

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
        meta=(AllowPrivateAccess="true"))
    float SecondsToSelfDestroy = 120.f;

    FTimerHandle SelfDestroyTimerHandle;
};

inline const UCapsuleComponent *AMTD_Projectile::GetCollisionComponent() const
{
    return CollisionComponent;
}

inline UCapsuleComponent *AMTD_Projectile::GetCollisionComponent()
{
    return CollisionComponent;
}

inline const UMTD_ProjectileMovementComponent *AMTD_Projectile::GetMovementComponent() const
{
    return MovementComponent;
}

inline UMTD_ProjectileMovementComponent *AMTD_Projectile::GetMovementComponent()
{
    return MovementComponent;
}
