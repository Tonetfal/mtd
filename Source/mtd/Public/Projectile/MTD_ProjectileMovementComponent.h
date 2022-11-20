#pragma once

#include "GameFramework/MovementComponent.h"
#include "mtd.h"
#include "Projectile/MTD_ProjectileCoreTypes.h"

#include "MTD_ProjectileMovementComponent.generated.h"

UCLASS()
class MTD_API UMTD_ProjectileMovementComponent : public UMovementComponent
{
    GENERATED_BODY()

public:
    virtual void TickComponent(
        float DeltaSeconds, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

protected:
    virtual bool CheckStillInWorld();

private:
    FVector ComputeHomingDirection() const;
    FVector ComputeVelocity(FVector MovementDirection) const;
    FRotator ComputeRotator(FVector DirectionToTarget) const;
    FVector GetRotatedDirection(FVector DirectionToTarget, float DeltaSeconds) const;

public:
    void SetMovementParameters(FMTD_ProjectileMovementParameters Parms);

private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD|Projectile Movement Component",
        meta=(AllowPrivateAccess="true"))
    FMTD_ProjectileMovementParameters MovementParameters;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD|Projectile Movement Component",
        meta=(AllowPrivateAccess="true"))
    float CurrentSpeed = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD|Projectile Movement Component",
        meta=(AllowPrivateAccess="true"))
    FVector Direction;
};
