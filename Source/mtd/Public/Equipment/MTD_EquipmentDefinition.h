#pragma once

#include "mtd.h"

#include "MTD_EquipmentDefinition.generated.h"

class UMTD_AbilitySet;
class UMTD_EquipmentInstance;

USTRUCT(BlueprintType)
struct FMTD_EquipmentActorDefinition
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere)
    TSubclassOf<AActor> ActorClass = nullptr;

    UPROPERTY(EditAnywhere)
    FName AttachToSocket = "Weapon";

    UPROPERTY(EditAnywhere)
    FTransform AttachTransform = FTransform::Identity;
};

/**
 * Data asset defining equipment held by a pawn.
 */
UCLASS(Blueprintable, BlueprintType, Const)
class UMTD_EquipmentDefinitionAsset
    : public UDataAsset
{
    GENERATED_BODY()

public:
    UMTD_EquipmentDefinitionAsset();

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UMTD_EquipmentInstance> EquipmentInstanceClass = nullptr;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<const UMTD_AbilitySet> AbilitySetToGrant = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FMTD_EquipmentActorDefinition ActorToSpawnDefinition;
};
