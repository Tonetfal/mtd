#pragma once

#include "GameFramework/MovementComponent.h"
#include "mtd.h"

#include "MTD_ProjectileMovementComponent.generated.h"

UCLASS(meta=(ToolTip="Default projectile movement component class used in this project."))
class MTD_API UMTD_ProjectileMovementComponent : public UMovementComponent
{
    GENERATED_BODY()

public:
    UMTD_ProjectileMovementComponent();

    //~UActorComponent Interface
    virtual void TickComponent(float DeltaSeconds, ELevelTick TickType,
        FActorComponentTickFunction *ThisTickFunction) override;
    //~End of UActorComponent Interface
    
    //~UMovementComponent Interface
    virtual void InitializeComponent() override;
    virtual float GetMaxSpeed() const override;
    //~End of UMovementComponent Interface

    /** Add acceleration towards the direction the projectile is following the next tick. */
    void AddAcceleration(float InAcceleration);

    /** Clear the acceleration added by AddAcceleration. */
    void ClearAcceleration();

private:
    FVector ComputeMoveDelta(float DeltaSeconds);
    void Accelerate(float DeltaSeconds);
    void RotateTowardsHoming(float DeltaSeconds);
    void ComputeVelocity();
    
    void SetSpeed(float Speed);
    
    FVector GetHomingDistanceVector() const;
    FVector GetHomingDirection() const;
    
    bool CheckStillInWorld() const;

public:
    /** The speed the projectile will start at. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD|Projectile Movement Component", meta=(ClampMin="0.0"))
    float InitialSpeed = 0.f;
    
    /** Maximum speed the projectile will be travelling at. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD|Projectile Movement Component", meta=(ClampMin="0.0"))
    float MaxSpeed = 0.f;

    /** Speed the projectile may accelerate itself each second by. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD|Projectile Movement Component", meta=(ClampMin="0.0"))
    float Acceleration = 0.f;

    /**
     * If set, HomingTarget must be specified. Rotation rate should be specified as well. Direction field will not be
     * taken into account.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD|Projectile Movement Component")
    bool bIsHoming = false;

    /** Degrees the homing projectile will be able to rotate by per second. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0.0", Category="MTD|Projectile Movement Component"))
    float RotationRate = 0.f;

    /**
     * If bIsHoming is set, the projectile will move towards the target. If the target dies - the projectile will
     * follow the direction it was following just before the death.
     */
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="MTD|Projectile Movement Component")
    TWeakObjectPtr<AActor> HomingTarget = nullptr;

    /** If bIsHoming is not set, this will be the direction the projectile will follow until destructuion. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="MTD|Projectile Movement Component")
    FVector Direction = FVector::ZeroVector;

private:
    /** Speed the projectile is travelling at. */
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="MTD|Projectile Movement Component",
        meta=(AllowPrivateAccess="true"))
    float CurrentSpeed = 0.f;

    /** Acceleration that will be added to the speed the next tick towards the direction. */
    float PendingAcceleration = 0.f;
    
    /** Acceleration that will be added to the speed this tick towards the direction. */
    float PendingAccelerationThisUpdate = 0.f;
};

inline float UMTD_ProjectileMovementComponent::GetMaxSpeed() const
{
    return MaxSpeed;
}

inline void UMTD_ProjectileMovementComponent::AddAcceleration(float InAcceleration)
{
    PendingAcceleration += InAcceleration;
}

inline void UMTD_ProjectileMovementComponent::ClearAcceleration()
{
    PendingAcceleration = 0.f;
}

inline void UMTD_ProjectileMovementComponent::ComputeVelocity()
{
    Velocity = Direction * CurrentSpeed;
}

inline void UMTD_ProjectileMovementComponent::SetSpeed(float Speed)
{
    CurrentSpeed = (CurrentSpeed < MaxSpeed) ? (Speed) : (MaxSpeed);
}
