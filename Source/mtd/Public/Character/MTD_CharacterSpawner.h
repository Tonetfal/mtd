#pragma once

#include "mtd.h"
#include "GameFramework/Actor.h"

#include "MTD_CharacterSpawner.generated.h"

class AMTD_BaseCharacter;

UCLASS()
class MTD_API AMTD_CharacterSpawner : public AActor
{
	GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnSpawnSignature, AActor*, Actor);
	
public:	
	AMTD_CharacterSpawner();

    void StartSpawning();
    void StopSpawning();

protected:
    //~AActor Interface
	virtual void BeginPlay() override;
    //~End of AActor Interface

private:
    void PrepareNextSpawnCall();
    float GetSpawnDelay() const;
    FTransform GetSpawnTransform() const;
    FVector GetRandomPointOnSpawnArea() const;
    
    UFUNCTION()
    void OnSpawn();

public:
    UPROPERTY(BlueprintAssignable)
    FOnSpawnSignature OnSpawnDelegate;

private:
    UPROPERTY()
    TObjectPtr<UWorld> World = nullptr;
    
    bool bCanSpawn = false;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Character Spawner",
        meta=(AllowPrivateAccess="true"))
    TSubclassOf<AMTD_BaseCharacter> CharacterClass = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="MTD|Character Spawner",
        meta=(AllowPrivateAccess="true", ClampMin="0.1"))
    float MaxSecondsToSpawn = 3.f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="MTD|Character Spawner",
        meta=(AllowPrivateAccess="true", ClampMin="0.1"))
    float MinSecondsToSpawn = 1.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="MTD|Character Spawner",
        meta=(AllowPrivateAccess="true", ClampMin="0.1"))
    float SpawnRange = 1000.f;
    
    FTimerHandle SpawnTimerHandle;
};

inline float AMTD_CharacterSpawner::GetSpawnDelay() const
{
    const float Delay = FMath::FRandRange(MinSecondsToSpawn, MaxSecondsToSpawn);
    return Delay;
}
