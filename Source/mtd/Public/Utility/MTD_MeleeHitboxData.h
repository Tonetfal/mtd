#pragma once

#include "Engine/DataAsset.h"
#include "mtd.h"

#include "MTD_MeleeHitboxData.generated.h"

USTRUCT(BlueprintType)
struct FMeleeHitSphereDefinition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName Nickname;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector Offset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Radius;
};

UCLASS(BlueprintType)
class MTD_API UMeleeHitboxData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FMeleeHitSphereDefinition> MeleeHitSpheres;

	UFUNCTION(BlueprintPure)
	TArray<FMeleeHitSphereDefinition> GetMeleeHitSpheres(TArray<int32> Indices);
};
