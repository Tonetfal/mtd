#pragma once

#include "mtd.h"
#include "Engine/DataAsset.h"

#include "MTD_PawnData.generated.h"

class APawn;
class UMTD_AbilitySet;
class UMTD_InputConfig;

UCLASS(BlueprintType, Const,
	meta=(ShortTooltip = "Data asset used to define a Pawn."))
class UMTD_PawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// Ability sets to grant to this pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MTD|Abilities")
	TArray<UMTD_AbilitySet*> AbilitySets;

//	// What mapping of ability tags to use for actions taking by this pawn
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MTD|Abilities")
//	UMTD_AbilityTagRelationshipMapping* TagRelationshipMapping;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MTD|Input")
	TObjectPtr<UMTD_InputConfig> InputConfig = nullptr;
};
