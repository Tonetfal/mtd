#pragma once

#include "mtd.h"
#include "Weapons/MTD_BaseWeapon.h"

#include "MTD_SwordWeapon.generated.h"

// Forward declaration
class UCapsuleComponent;

UCLASS()
class MTD_API AMTD_SwordWeapon : public AMTD_BaseWeapon
{
	GENERATED_BODY()

public:
	AMTD_SwordWeapon();
	
protected:
	virtual void BeginPlay() override;
	
public:
	virtual void Attack() override;
	virtual void ForceStopAttack() override;

	virtual void OnAttackActivation() override;
	virtual void OnAttackDeactivation() override;
	virtual void OnAttackAnimationTerminated() override;

protected:
	UFUNCTION()
	virtual void OnAttackAreaBeginOverlap(
		UPrimitiveComponent *OverlappedComponent,
		AActor *OtherActor,
		UPrimitiveComponent *OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult &SweepResult);
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD Components",
		meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCapsuleComponent> AttackArea = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Sword Weapon",
		meta=(AllowPrivateAccess="true"))
	int32 Damage = 1;
};
