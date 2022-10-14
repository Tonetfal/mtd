#pragma once

#include "GameFramework/MovementComponent.h"
#include "mtd.h"
#include "Projectile/MTD_ProjectileCoreTypes.h"

#include "MTD_ProjectileMovementComponent.generated.h"

/*
 *
 * Probable performance improvement: Add parameters to modify update call ratio
 * 
 */

UCLASS()
class MTD_API UMTD_ProjectileMovementComponent : public UMovementComponent
{
	GENERATED_BODY()

public:
protected:
	virtual void TickComponent(
		float DeltaSeconds,
		ELevelTick TickType, 
		FActorComponentTickFunction *ThisTickFunction) override;

protected:
	virtual bool CheckStillInWorld();

private:
	FVector ComputeHomingDirection() const;
	FVector ComputeVelocity(FVector MovementDirection) const;
	
public:
    void SetInitialSpeed(float Value);
    void SetMaxSpeed(float Value);
    void SetAcceleration(float Value);
	void SetHomingTarget(TWeakObjectPtr<AActor> Value);
	void SetDirection(FVector Value);
	void SetRotationFollowsVelocity(bool Value);
	void SetMovementParameters(FMTD_ProjectileParameters Parms);
	
    float GetInitialSpeed() const;
	virtual float GetMaxSpeed() const override;
    float GetAcceleration() const;
	TWeakObjectPtr<AActor> GetHomingTarget() const;
	FVector GetDirection() const;
	bool GetRotationFollowsVelocity() const;
	FMTD_ProjectileParameters GetMovementParameters() const;

private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Projectile Movement Component",
    	meta=(AllowPrivateAccess="true"))
	FMTD_ProjectileParameters MovementParameters;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Projectile Movement Component",
    	meta=(AllowPrivateAccess="true"))
	float CurrentSpeed = 0.f;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Projectile Movement Component",
    	meta=(AllowPrivateAccess="true"))
	FVector Direction;
};
