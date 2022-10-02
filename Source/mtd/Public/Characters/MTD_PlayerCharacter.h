#pragma once

#include "mtd.h"
#include "GameFramework/Character.h"

#include "Components/MTD_WeaponComponent.h"

#include "MTD_PlayerCharacter.generated.h"

class UMTD_HealthComponent;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class MTD_API AMTD_PlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMTD_PlayerCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void InitDelegates();

public:	
	virtual void SetupPlayerInputComponent(
		UInputComponent *PlayerInputComponent) override;

public:
	UFUNCTION(BlueprintCallable, Category="MTD Movement")
	float ConsumeTurnRatio();
	
	UFUNCTION(BlueprintCallable, Category="MTD Movement")
	float GetMovementDirectionAngle() const;

private:
	void LookUp(float Ratio);
	void TurnRight(float Ratio);
	void MoveForward(float Ratio);
	void MoveRight(float Ratio);
	void StartAttacking();
	void StopAttacking();

protected:
	UFUNCTION(BlueprintNativeEvent, Category="MTD Death")
	void OnDeath();
	virtual void OnDeath_Implementation();

	virtual void PlayDeathAnimation() const;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD Components",
		meta=(AllowPrivateAccess="true"))
	TObjectPtr<USpringArmComponent> CameraBoom = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD Components",
		meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCameraComponent> FollowCamera = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD Components",
		meta=(AllowPrivateAccess="true"))
	TObjectPtr<UMTD_HealthComponent> Health = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD Components",
		meta=(AllowPrivateAccess="true"))
	TObjectPtr<UMTD_WeaponComponent> Weapon = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Animations",
		meta=(AllowPrivateAccess="true"))
	TObjectPtr<UAnimMontage> DeathAnimMontage = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD Movement",
		meta=(AllowPrivateAccess="true"))
	bool bIsTurning = false;
	
	UPROPERTY(VisibleAnywhere, Category="MTD Movement",
		meta=(AllowPrivateAccess="true"))
	float TurnRatio = 0.f;
};
