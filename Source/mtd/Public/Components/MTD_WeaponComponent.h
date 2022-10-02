#pragma once

#include "mtd.h"

#include "Weapons/MTD_BaseWeapon.h"
#include "Weapons/MTD_WeaponCoreTypes.h"

#include "MTD_WeaponComponent.generated.h"

UCLASS()
class MTD_API UMTD_WeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMTD_WeaponComponent();
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
public:
	void StartAttacking();
	void StopAttacking();
	void ForceStopAttacking();

protected:
	virtual void SpawnWeapon();
	virtual void SetupNotifyDelegates();
	virtual void AttachWeaponToSocket(AMTD_BaseWeapon *WeaponToAttach,
		USceneComponent *AttachTo, FName SocketName);
	virtual void SetWeaponAttackCollisionProfileName();

private:
	void PlayAnimMontage(UAnimMontage *AnimMontage) const;

protected:
	virtual void Attack();
	virtual void OnAttackActivation(USkeletalMeshComponent *TargetMesh);
	virtual void OnAttackDeactivation(USkeletalMeshComponent *TargetMesh);
	virtual void OnAttackAnimationTerminated(
		USkeletalMeshComponent *TargetMesh);
	
	virtual void AllowAttacking();

	bool IsNotifyForUs(const USkeletalMeshComponent *TargetMesh) const;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Weapon Component",
		meta=(AllowPrivateAccess="true"))
	FMTD_WeaponData WeaponData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Weapon Component",
		meta=(AllowPrivateAccess="true"))
	FName WeaponEquipSocketName = "WeaponSocket";
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Weapon Component",
		meta=(AllowPrivateAccess="true"))
	float TimeBetweenAttacks = 0.5f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD Weapon Component|Runtime",
		meta=(AllowPrivateAccess="true"))
	TObjectPtr<AMTD_BaseWeapon> Weapon = nullptr;

	bool bWantsAttacking = false;
	bool bIsAllowedAttacking = true;

	FTimerHandle AttackPermissionTimerHandle;
	FTimerHandle NextAttackTimerHandle;
};