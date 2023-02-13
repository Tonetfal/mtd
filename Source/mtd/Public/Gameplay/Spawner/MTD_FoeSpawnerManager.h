#pragma once

#include "mtd.h"
#include "UObject/Object.h"

#include "MTD_FoeSpawnerManager.generated.h"

class AMTD_BaseFoeCharacter;
class AMTD_FoeSpawner;

UCLASS()
class MTD_API UMTD_FoeSpawnerManager
    : public UObject
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnFoeSpawnedSignature,
        const TArray<AActor *> &, SpawnedFoes);
    
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnFoeKilledSignature,
        const TArray<AActor *> &, KilledFoes);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnAllFoesKilledSignature,
        int32, KilledFoesCount);

public:
    /**
     * Initialize spawner manager.
     *
     * Cache spawners, start listening for different events.
     */
    void Initialize();

    /**
     * Dispatch cached data about spawned and killed foes.
     * 
     * Should be called every tick by external means.
     */
    // @todo make the object be tick-able
    void DispatchCachedData();

public:
    /** Delegate to fire when a character was spawned. */
    UPROPERTY(BlueprintAssignable)
    FOnFoeSpawnedSignature OnFoeSpawnedDelegate;
    
    /** Delegate to fire when a character was killed. */
    UPROPERTY(BlueprintAssignable)
    FOnFoeKilledSignature OnFoeKilledDelegate;
    
    /** Delegate to fire when all foes were killed. */
    UPROPERTY(BlueprintAssignable)
    FOnAllFoesKilledSignature OnAllFoesKilledDelegate;

private:
    /**
     * Cache all foe spawners placed on map.
     */
    void CacheSpawners();

    /**
     * Initialize all cached foe spawners.
     */
    void InitializeSpawners();

    /**
     * Event to fire when a wave is started.
     * @param   WaveNumber: started wave number.
     * @param   RemainingForceStartTime: remaining time in seconds to force start.
     */
    UFUNCTION()
    void OnWaveStarted(int32 WaveNumber, float RemainingForceStartTime);

    /**
     * Event to fire when wave is finished.
     * @param   WaveDuration: duration is seconds a wave has lasted for.
     */
    UFUNCTION()
    void OnWaveEnded(float WaveDuration);

    /**
     * Event to fire when a foe is spawned.
     * @param   FoeCharacter: spawned foe.
     */
    UFUNCTION()
    void OnFoeSpawn(AMTD_BaseFoeCharacter *FoeCharacter);

    /**
     * Event to fire when a foe is killed.
     * @param   FoeActor: killed foe.
     */
    UFUNCTION()
    void OnCharacterKilled(AActor *FoeActor);

    /**
     * Event to fire when a spawner has prepared to next wave.
     * @param   TotalFoesToSpawn: total foes that spawner will spawn the next wave.
     */
    UFUNCTION()
    void OnSpawnerPrepared(int32 TotalFoesToSpawn);

private:
    /** Cached spawners placed on map. */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Spawner Manager|Runtime",
        meta=(AllowPrivateAccess="true"))
    TArray<TObjectPtr<AMTD_FoeSpawner>> Spawners;

    /** Amount of spawned foes on current wave. */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Spawner Manager|Runtime",
        meta=(AllowPrivateAccess="true"))
    int32 SpawnedFoesOnCurrentWave = 0;

    /** Amount of killed foes on current wave. */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Spawner Manager|Runtime",
        meta=(AllowPrivateAccess="true"))
    int32 KilledFoesOnCurrentWave = 0;
    
    /** Total foes to spawn on current wave. */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Spawner Manager|Runtime",
        meta=(AllowPrivateAccess="true"))
    int32 TotalFoesToSpawnOnCurrentWave = 0;

    /** Cached spawned foes. Is used to significantly decrease amount of events sent. */
    UPROPERTY()
    TArray<AActor *> SpawnedFoes;
    
    /** Cached killed foes. Is used to significantly decrease amount of events sent. */
    UPROPERTY()
    TArray<AActor *> KilledFoes;
};

