#pragma once

#include "GameFramework/Actor.h"
#include "mtd.h"

#include "MTD_CharacterSpawner.generated.h"

class AMTD_BaseCharacter;
class AMTD_TowerDefenseMode;
class UMTD_CharacterQuantityDefinition;
class UMTD_LevelDifficultyDefinition;

/**
 * @todo Cache the closest core, and assign it to the spawned character instead of computing it every time they're 
 * spawned.
 */

UCLASS()
class MTD_API AMTD_CharacterSpawner
    : public AActor
{
	GENERATED_BODY()

public:
    friend class UMTD_SpawnerManager;

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnSpawnSignature,
        AActor*, Actor);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnPreparedForNewWaveSignature,
        int32, TotalCharactersToSpawn);
	
public:	
	AMTD_CharacterSpawner();

    void StartSpawning();
    void StopSpawning();

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Character Spawner")
    int32 GetTotalCharactersOfClassToSpawn(TSubclassOf<AMTD_BaseCharacter> CharacterClass) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Character Spawner")
    TMap<TSubclassOf<AMTD_BaseCharacter>, int32> GetTotalCharactersOfClassesToSpawn() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Character Spawner")
    int32 GetTotalCharactersToSpawn() const;
    
    //~AActor Interface
    virtual void Tick(float DeltaSeconds) override;

protected:
    virtual void BeginPlay() override;
    //~End of AActor Interface
    
    void Initialize();

private:
    FTransform GetSpawnTransform() const;
    FVector GetRandomPointOnSpawnArea() const;
    
    UFUNCTION()
    void SpawnCharacter(TSubclassOf<AMTD_BaseCharacter> CharacterClass);

    void ListenForWaveEnd();
    void CacheLevelData();
    void BuildCacheMap();
    void PrepareForNewWave();
    void ComputeEnemySpawns(float WaveTotalTime, int32 CurrentWave);

    void SetIsSpawning(bool bFlag);

    UFUNCTION()
    void OnWaveEnd(float WaveDuration);

public:
    UPROPERTY(BlueprintAssignable)
    FOnSpawnSignature OnSpawnDelegate;
    
    UPROPERTY(BlueprintAssignable)
    FOnPreparedForNewWaveSignature OnPreparedForNewWaveDelegate;

private:
    UPROPERTY(EditDefaultsOnly, Category="MTD|Character Spawner", meta=(ClampMin="1.0"))
    int32 TicksPerSecond = 50;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="MTD|Character Spawner",
        meta=(AllowPrivateAccess="true", ClampMin="0.1"))
    float SpawnRange = 1000.f;

    UPROPERTY(VisibleInstanceOnly, Category="MTD|Character Spawner|Runtime")
    int32 TotalTicksCurrentWave = 0;
    
    UPROPERTY(VisibleInstanceOnly, Category="MTD|Character Spawner|Runtime")
    float TickInterval = 0.f;
    
    UPROPERTY(VisibleInstanceOnly, Category="MTD|Character Spawner|Runtime")
    int32 TicksSinceWaveStart = 0;

    UPROPERTY(VisibleInstanceOnly, Category="MTD|Character Spawner|Runtime")
    bool bIsSpawning = false;
    
    UPROPERTY(VisibleInstanceOnly, Category="MTD|Character Spawner|Runtime")
    float GlobalQuantityRate = 0.f;

    UPROPERTY(VisibleInstanceOnly, Category="MTD|Character Spawner|Runtime")
    TObjectPtr<const UMTD_LevelDifficultyDefinition> LevelDiffDefinition = nullptr;

    UPROPERTY()
    TObjectPtr<AMTD_TowerDefenseMode> TowerDefenseMode = nullptr;

    UPROPERTY()
    TObjectPtr<const UMTD_CharacterQuantityDefinition> EnemyQuantityAsset = nullptr;

    TMap<TSubclassOf<AMTD_BaseCharacter>, TArray<int32 /* amount to spawn */>> CachedCharactersToSpawn;
};

