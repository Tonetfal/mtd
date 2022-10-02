#pragma once

#include "mtd.h"
#include "GameFramework/Actor.h"

#include "MTD_BaseWeapon.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackEndSignature);

UCLASS(Abstract)
class MTD_API AMTD_BaseWeapon : public AActor
{
	GENERATED_BODY()

public:
	AMTD_BaseWeapon();

public:
	virtual void Attack();
	virtual void ForceStopAttack();
	
	virtual void OnAttackActivation();
	virtual void OnAttackDeactivation();
	virtual void OnAttackAnimationTerminated();

	virtual void SetAttackCollisionProfileName(FName ProfileName);

public:
	FOnAttackEndSignature OnAttackEndDelegate;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD Components")
	TObjectPtr<UStaticMeshComponent> Mesh = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD Sword Weapon|Runtime",
		meta=(AllowPrivateAccess="true"))
	FName ActivatedAttackCollisionProfileName = "NoCollision";
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD Sword Weapon|Runtime",
		meta=(AllowPrivateAccess="true"))
	FName DeactivatedAttackCollisionProfileName = "NoCollision";
};