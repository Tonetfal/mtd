#pragma once

#include "mtd.h"

#include "GameplayEffect.h"

#include "MTD_ProjectileCoreTypes.generated.h"

class AMTD_Projectile;

USTRUCT(BlueprintType)
struct FMTD_ProjectileMovementParameters
{
	GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite)
    float InitialSpeed = 0.f;
    
    UPROPERTY(BlueprintReadWrite)
    float MaxSpeed = 0.f;
    
    UPROPERTY(BlueprintReadWrite)
    float Acceleration = 0.f;
	
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsHoming = false;
	
    UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AActor> HomingTarget = nullptr;

    UPROPERTY(BlueprintReadWrite)
	FVector Direction = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct FMTD_ProjectileParameters
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	FCollisionProfileName CollisionProfileName { FName(TEXT("NoCollision")) };

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bIsRadial = false;
	
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,
    	meta=(EditCondition="bIsRadial"))
	float RadialDamageRadius = 100.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FGameplayEffectSpecHandle> GameplayEffectsToGrantOnHit;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FMTD_ProjectileMovementParameters MovementParameters;
};

UCLASS(BlueprintType)
class MTD_API UMTD_FireData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AMTD_Projectile> ProjectileClass = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FMTD_ProjectileParameters ProjectileParameters;
};
