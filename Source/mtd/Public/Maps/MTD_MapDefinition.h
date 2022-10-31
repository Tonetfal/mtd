#pragma once

#include "UObject/NoExportTypes.h"
#include "mtd.h"
#include "MTD_MapDefinition.generated.h"

UENUM(BlueprintType)
enum class ELevelDifficulty : uint8
{
	Invalid,
	Easy,
	Medium,
	Hard
};

UCLASS(Blueprintable, BlueprintType)
class MTD_API UMTD_LevelDefinition : public UDataAsset
{
	GENERATED_BODY()

public:

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UCurveTable> EntitiesAttributeTable = nullptr;

	// EntitiesToSpawn from various portals
	// Rewards with possibilities
	// Achievements
};

UCLASS(Blueprintable, BlueprintType)
class MTD_API UMTD_MapDefinition : public UObject
{
	GENERATED_BODY()

public:
	UMTD_MapDefinition();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<ELevelDifficulty, TObjectPtr<UMTD_LevelDefinition>> LevelDefinitions;
};
