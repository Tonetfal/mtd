#pragma once

#include "mtd.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "MTD_CharacterMovementComponent.generated.h"

UCLASS()
class MTD_API UMTD_CharacterMovementComponent :
	public UCharacterMovementComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
public:
	virtual void TickComponent(
		float DeltaSeconds,
		ELevelTick TickType,
		FActorComponentTickFunction *ThisTickFunction) override;
	
public:
	UFUNCTION(BlueprintCallable)
	float GetMovementDirectionAngle() const;

private:
	float GetMovementDirectionAngleImpl();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Movement: Walking",
		meta=(AllowPrivateAccess="true"))
	float MaxWalkSpeedBase = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Movement: Walking",
		meta=(AllowPrivateAccess="true"))
	float MaxWalkSpeedBackwards = 450.f;
	
	float DirectionRadians = 0.f;
};
