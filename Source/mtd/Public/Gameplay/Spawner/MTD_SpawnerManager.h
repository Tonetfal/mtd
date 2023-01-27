#pragma once

#include "mtd.h"
#include "UObject/Object.h"

#include "MTD_SpawnerManager.generated.h"

class AMTD_CharacterSpawner;

UCLASS()
class MTD_API UMTD_SpawnerManager
    : public UObject
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnCharacterSpawnedSignature,
        const TArray<AActor*>&, SpawnedCharacters);
    
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnCharacterKilledSignature,
        const TArray<AActor*>&, KilledCharacters);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnAllCharactersKilledSignature,
        int32, KilledCharactersCount);

public:
    void Initialize();

    /**
     * Should be called every tick by external means.
     */
    // @todo make the object be tick-able
    void DispatchCachedData();

public:
    UPROPERTY(BlueprintAssignable)
    FOnCharacterSpawnedSignature OnCharactersSpawnedDelegate;
    
    UPROPERTY(BlueprintAssignable)
    FOnCharacterKilledSignature OnCharactersKilledDelegate;
    
    UPROPERTY(BlueprintAssignable)
    FOnAllCharactersKilledSignature OnAllCharactersKilledDelegate;

private:
    void CacheSpawners();
    void InitializeSpawners();

    UFUNCTION()
    void OnWaveStarted(int32 WaveNumber, float RemainingSeconds);
    
    UFUNCTION()
    void OnWaveEnded(float WaveDuration);

    UFUNCTION()
    void OnCharacterSpawn(AActor *Actor);
    
    UFUNCTION()
    void OnCharacterKilled(AActor *Actor);

    UFUNCTION()
    void OnSpawnedPrepared(int32 TotalCharactersToSpawn);

private:
    /** All the spawners placed on map. */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Spawner Manager|Runtime",
        meta=(AllowPrivateAccess="true"))
    TArray<TObjectPtr<AMTD_CharacterSpawner>> Spawners;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Spawner Manager|Runtime",
        meta=(AllowPrivateAccess="true"))
    int32 SpawnedCharactersOnCurrentWave = 0;
    
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Spawner Manager|Runtime",
        meta=(AllowPrivateAccess="true"))
    int32 KilledCharactersOnCurrentWave = 0;
    
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Spawner Manager|Runtime",
        meta=(AllowPrivateAccess="true"))
    int32 TotalCharactersToSpawnOnCurrentWave = 0;

    /** Cached spawned characters to prevent sending so many events each spawned one. */
    UPROPERTY()
    TArray<AActor*> SpawnedCharacters;
    
    /** Cached killed mobs to prevent sending so many events each spawned enemy. */
    UPROPERTY()
    TArray<AActor*> KilledCharacters;
};

