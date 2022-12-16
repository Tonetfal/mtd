#pragma once

#include "AbilitySystem/MTD_AbilitySet.h"
#include "GameFramework/Actor.h"
#include "mtd.h"

#include "MTD_EquipmentDefinition.generated.h"

class UMTD_EquipmentInstance;

USTRUCT(BlueprintType)
struct FMTD_EquipmentActorToSpawn
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere)
    TSubclassOf<AActor> ActorToSpawn = nullptr;

    UPROPERTY(EditAnywhere)
    FName AttachToSocket{TEXT("None")};

    UPROPERTY(EditAnywhere)
    FTransform AttachTransform = FTransform::Identity;
};

UCLASS(Blueprintable, Const, Abstract, BlueprintType)
class MTD_API UMTD_EquipmentDefinition : public UObject
{
    GENERATED_BODY()

public:
    UMTD_EquipmentDefinition();

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MTD|Equipment")
    TSubclassOf<UMTD_EquipmentInstance> InstanceType = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MTD|Equipment")
    TObjectPtr<const UMTD_AbilitySet> AbilitySetToGrant = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MTD|Equipment")
    FMTD_EquipmentActorToSpawn ActorToSpawn;
};
