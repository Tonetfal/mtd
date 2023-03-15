#pragma once

#include "GameFramework/Actor.h"
#include "mtd.h"

#include "MTD_FoeSpawner.generated.h"

class AMTD_BaseFoeCharacter;
class AMTD_TowerDefenseMode;
class UMTD_FoeQuantityDefinition;
class UMTD_LevelDefinition;
class UMTD_LevelDifficultyDefinition;

/**
 * @todo Cache the closest core, and assign it to the spawned foe instead of computing it every time they're spawned.
 */


/**
 * Foe spawner responsible for deciding when to start and stop spawning, how many and which foes to spawn.
 */
UCLASS()
class MTD_API AMTD_FoeSpawner
    : public AActor
{
	GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnSpawnSignature,
        AMTD_BaseFoeCharacter *, FoeCharacter);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnPreparedForNewWaveSignature,
        int32, TotalFoesToSpawn);
	
public:	
	AMTD_FoeSpawner();
    
    /**
     * Initialize a spawner.
     * @param   SelectedLevelDefinition: level definition to initialize the spawner with.
     * @param   SelectedLevelDifficultyDefinition: level difficulty definition to initialize the spawner with.
     */
    void Initialize(const UMTD_LevelDefinition *SelectedLevelDefinition,
		const UMTD_LevelDifficultyDefinition *SelectedLevelDifficultyDefinition);

    /**
     * Start spawning foes.
     */
    void StartSpawning();

    /**
     * Stop spawning foes.
     */
    void StopSpawning();

    /**
     * Get total amount of a certain foe to spawn on current wave.
     * @param   FoeClass: foe class to search for.
     * @return  Total amount of a certain foe to spawn on current wave.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Foe Spawner")
    int32 GetTotalFoesOfClassToSpawn(TSubclassOf<AMTD_BaseFoeCharacter> FoeClass) const;

    /**
     * Get total amount of each foe to spawn on current wave.
     * @return  Total amount of each foe to spawn on current wave.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Foe Spawner")
    TMap<TSubclassOf<AMTD_BaseFoeCharacter>, int32> GetTotalFoesOfClassesToSpawn() const;

    /**
     * Get total amount of foes to spawn on current wave.
     * @return  Total amount of foes to spawn on current wave.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Foe Spawner")
    int32 GetTotalFoesToSpawn() const;
    
    //~AActor Interface
    virtual void Tick(float DeltaSeconds) override;
    //~End of AActor Interface

private:
    /**
     * Get transforms a foe has to spawn at.
     * @return  Transforms a foe has to spawn at.
     */
    FTransform GetSpawnTransform() const;

    /**
     * Get a random point in spawn area.
     * @return  Random point in spawn area.
     */
    FVector GetRandomPointInSpawnArea() const;

    /**
     * Spawn a given foe.
     * @param   FoeClass: foe class to use to spawning.
     */
    void SpawnFoe(TSubclassOf<AMTD_BaseFoeCharacter> FoeClass);

    /**
     * Start listening for wave end.
     */
    void ListenForWaveEnd();

    /**
     * Cache foe quantity and level difficulty definition assets.
     */
    void CacheLevelData();

    /**
     * Build foe to spawn cache map.
     */
    void BuildCacheMap();

    /**
     * Prepare data for next wave.
     *
     * Cache total wave time, scaled quantity, cache foe spawns etc.
     */
    void PrepareForNextWave();

    /**
     * Cache all foe spawns for a given wave.
     *
     * Using a map of foe classes and arrays of ints which elements represent a Nth tick, fill the information about how
     * many foes have to spawn at a particular tick. If none has to be spawned, an element will be 0, positive 
     * otherwise.
     * @param   WaveTotalTime: time in seconds spawning logic will be running for. Upon expiration spawning will finish.
     * @param   Wave: wave the computition has to run for.
     */
    void CacheFoeSpawns(float WaveTotalTime, int32 Wave);

    /**
     * Cache IsSpawning state.
     * @param   bFlag: if true, spawner will start spawning logic, false otherwise.
     */
    void SetIsSpawning(bool bFlag);

    /**
     * Event called when wave ends.
     * @param   WaveDuration: duration in seconds a wave lasted for.
     */
    UFUNCTION()
    void OnWaveEnd(float WaveDuration);

public:
    /** Delegate to fire when a foe is spawned. */
    UPROPERTY(BlueprintAssignable)
    FOnSpawnSignature OnSpawnDelegate;

    /** Delegate to fire when spawner has prepared data for a new wave. */
    UPROPERTY(BlueprintAssignable)
    FOnPreparedForNewWaveSignature OnPreparedForNewWaveDelegate;

private:
    /** Times spawner will update spawning logic per second. */
    UPROPERTY(EditDefaultsOnly, Category="MTD|Foe Spawner", meta=(ClampMin="1.0"))
    int32 TicksPerSecond = 50;

    /** Maximum range a foe can spawn from the central point at. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="MTD|Foe Spawner",
        meta=(AllowPrivateAccess="true", ClampMin="0.1"))
    float MaxSpawnRange = 1000.f;

    /** Ticks counter for current wave. */
    UPROPERTY(VisibleInstanceOnly, Category="MTD|Foe Spawner|Runtime")
    int32 TotalTicksCurrentWave = 0;

    /** Time in seconds interval between two successive ticks. */
    UPROPERTY(VisibleInstanceOnly, Category="MTD|Foe Spawner|Runtime")
    float TickInterval = 0.f;

    /** Tick since a wave has started. */
    UPROPERTY(VisibleInstanceOnly, Category="MTD|Foe Spawner|Runtime")
    int32 TicksSinceWaveStart = 0;

    /** Is spawner running spawning logic? */
    UPROPERTY(VisibleInstanceOnly, Category="MTD|Foe Spawner|Runtime")
    bool bIsSpawning = false;

    /** Cached foe quantity rate from difficulty settings. */
    UPROPERTY(VisibleInstanceOnly, Category="MTD|Foe Spawner|Runtime")
    float GlobalQuantityRate = 0.f;

    /** Cached level difficulty definition from difficulty settings. */
    UPROPERTY(VisibleInstanceOnly, Category="MTD|Foe Spawner|Runtime")
    TObjectPtr<const UMTD_LevelDifficultyDefinition> LevelDiffDefinition = nullptr;

    /** Cached foe quantity definition from difficulty settings. */
    UPROPERTY(VisibleInstanceOnly, Category="MTD|Foe Spawner|Runtime")
    TObjectPtr<const UMTD_FoeQuantityDefinition> FoeQuantityAsset = nullptr;

    /**  */
    TMap<TSubclassOf<AMTD_BaseFoeCharacter>, TArray<int32 /* amount to spawn on Nth tick */>> CachedFoesToSpawn;
};

