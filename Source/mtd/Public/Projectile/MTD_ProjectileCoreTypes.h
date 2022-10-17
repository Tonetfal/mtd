#pragma once

#include "mtd.h"

#include "MTD_ProjectileCoreTypes.generated.h"

USTRUCT(BlueprintType)
struct MTD_API FMTD_ProjectileParameters
{
	GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float InitialSpeed = 0.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxSpeed = 1000.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Acceleration = 0.f;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Damage = 0.f;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsRadial = false;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RadialDamageRadius = 100.f;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRotationFollowsVelocity = false;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<AActor> HomingTarget = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Direction = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FName CollisionProfileName = "NoCollision";

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float SecondsToSelfDestroy = 120.f;
};
