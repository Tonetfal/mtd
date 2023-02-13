#pragma once

#include "mtd.h"

#include "MTD_EquipmentDefinition.generated.h"

class UMTD_AbilitySet;
class UMTD_EquipmentInstance;

/**
 * Definition of an equipment instance actor to spawn in the world.
 */
USTRUCT(BlueprintType)
struct FMTD_EquipmentActorDefinition
{
    GENERATED_BODY()

public:
    /** Actor class to spawn. */
    UPROPERTY(EditAnywhere)
    TSubclassOf<AActor> ActorClass = nullptr;

    /** Socket to attach the spawned actor to. */
    UPROPERTY(EditAnywhere)
    FName AttachToSocket = "Weapon";

    /** Attach transforms to use. */
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
    /** Equipment instance class to use. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UMTD_EquipmentInstance> EquipmentInstanceClass = nullptr;

    /** Ability sets to grant to equipment owner. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<const UMTD_AbilitySet> AbilitySetToGrant = nullptr;

    /** Equipment actor spawn definition. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FMTD_EquipmentActorDefinition ActorToSpawnDefinition;
};
