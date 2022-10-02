#pragma once

#include "mtd.h"
#include "GameFramework/Pawn.h"

#include "MTD_BaseTower.generated.h"

class UMTD_HealthComponent;
class UCapsuleComponent;
class AMTD_Projectile;
class UMTD_ProjectileMovementComponent;
class USphereComponent;

UCLASS()
class MTD_API AMTD_BaseTower : public APawn
{
	GENERATED_BODY()
	
public:
	AMTD_BaseTower();

protected:
	virtual void BeginPlay() override;
	virtual void SetupVisionParameters();

public:
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void OnFire(AActor *FireTarget);
	virtual void OnReload();
	
	UFUNCTION(BlueprintNativeEvent, Category="MTD Functions")
	void OnDestroy();
	virtual void OnDestroy_Implementation();

protected:
	virtual void ProjectileSetup(
		AMTD_Projectile *Projectile, AActor *FireTarget);
	virtual AMTD_Projectile *SpawnProjectile();

public:
	virtual float GetScaledInitialSpeed() const;
	virtual float GetScaledFirerate() const;
	virtual float GetScaledAcceleration() const;
	virtual float GetScaledVisionRange() const;
	virtual float GetScaledVisionHalfDegrees() const;
	virtual int32 GetScaledDamage() const;
	virtual float GetScaledProjectileMaxSpeed() const;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD Components",
		meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCapsuleComponent> CollisionCapsule = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD Components",
		meta=(AllowPrivateAccess="true"))
	TObjectPtr<USkeletalMeshComponent> Mesh = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD Components",
		meta=(AllowPrivateAccess="true"))
	TObjectPtr<UMTD_HealthComponent> Health = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD Components",
		meta=(AllowPrivateAccess="true"))
	TObjectPtr<USphereComponent> SpawnPosition = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Tower",
		meta=(AllowPrivateAccess="true"))
	TSubclassOf<AMTD_Projectile> ProjectileClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Tower",
		meta=(AllowPrivateAccess="true"))
	float BaseProjectileInitialSpeed = 0.f;
	
	/** cm a projectile passes through in a second */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Tower",
		meta=(AllowPrivateAccess="true"))
	float BaseProjectileMaxSpeed = 1000.f;
	
	/** cm a projectile accelerates per second */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Tower",
		meta=(AllowPrivateAccess="true"))
	float BaseProjectileAcceleration = 100.f;
	
	/** Projectiles per minute */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Tower",
		meta=(AllowPrivateAccess="true"))
	float BaseFirerate = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Tower",
		meta=(AllowPrivateAccess="true"))
	float BaseVisionRange = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Tower",
		meta=(AllowPrivateAccess="true"))
	float BaseVisionHalfDegrees = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Tower",
		meta=(AllowPrivateAccess="true"))
	int32 BaseDamage = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Tower",
		meta=(AllowPrivateAccess="true"))
	bool bAreProjectilesHoming = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Tower",
		meta=(AllowPrivateAccess="true"))
	bool bAreProjectilesRadial = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD Tower|Runtime",
		meta=(AllowPrivateAccess="true"))
	bool bIsOnReload = false;
};
