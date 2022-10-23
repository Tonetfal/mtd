#pragma once

#include "GameplayEffect.h"
#include "GameFramework/Actor.h"
#include "mtd.h"
#include "Projectile/MTD_ProjectileCoreTypes.h"

#include "MTD_Projectile.generated.h"

// Forward declaration
class UMTD_ProjectileMovementComponent;
class UMTD_TeamComponent;
class UCapsuleComponent;

UCLASS()
class MTD_API AMTD_Projectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AMTD_Projectile();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category="MTD|Projectile")
	virtual void SetupProjectile(FMTD_ProjectileParameters Params);

protected:
	UFUNCTION()
	void OnBeginOverlap(
		UPrimitiveComponent *HitComponent,
		AActor *OtherActor,
		UPrimitiveComponent *OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult &SweepResult);

	UFUNCTION(BlueprintImplementableEvent, Category="MTD|Projectile")
	void OnSelfDestroy();
	virtual void OnSelfDestroy_Implementation();

	virtual void ApplyGameplayEffectToTarget(AActor *Target);
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components",
		meta=(AllowPrivateAccess="true"))
	TObjectPtr<UStaticMeshComponent> Mesh = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components",
		meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCapsuleComponent> CollisionCapsule = nullptr;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components")
	TObjectPtr<UMTD_ProjectileMovementComponent> ProjectileMovement = nullptr;

private:
	UPROPERTY()
	FMTD_ProjectileParameters ProjectileParameters;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Projectile",
		meta=(AllowPrivateAccess="true"))
	float SecondsToSelfDestroy = 120.f;
	
	FTimerHandle SelfDestroyTimerHandle;
};
