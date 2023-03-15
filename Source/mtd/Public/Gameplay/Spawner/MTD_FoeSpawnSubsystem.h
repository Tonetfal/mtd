#pragma once

#include "mtd.h"
#include "Subsystems/WorldSubsystem.h"

#include "MTD_FoeSpawnSubsystem.generated.h"

class AMTD_BaseFoeCharacter;
class AMTD_FoeSpawner;
class UMTD_LevelDefinition;
class UMTD_LevelDifficultyDefinition;

/**
 * Subsystem responsible for controlling foe spawners, as well as notifying some events about them.
 */
UCLASS()
class MTD_API UMTD_FoeSpawnSubsystem
    : public UTickableWorldSubsystem
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
     * Get the subsystem from a world context.
     * @param   WorldContextObject: context used to retrieve the subsystem.
     * @return  This subsystem. May be nullptr.
     */
    static UMTD_FoeSpawnSubsystem *Get(const UObject *WorldContextObject);
    
    //~UTickableWorldSubsystem Interface
    virtual TStatId GetStatId() const override;
    
    /**
     * Initialize spawner manager.
     *
     * Cache spawners, start listening for different events.
     */
    virtual void PostInitialize() override;
    
    virtual void Tick(float DeltaSeconds) override;
    //~End of UTickableWorldSubsystem Interface

    /**
     * Dispatch cached data about spawned and killed foes.
     */
    void DispatchCachedData();

private:
    /**
     * Event to fire when difficulty subsystem has been initialized.
     * @param   SelectedLevelDefinition: level definition subsystem has chosen.
     * @param   SelectedLevelDifficultyDefinition: level difficulty definition subsystem has chosen.
     */
    void OnDifficultySubsystemInitialized(const UMTD_LevelDefinition *SelectedLevelDefinition,
        const UMTD_LevelDifficultyDefinition *SelectedLevelDifficultyDefinition);
        
    /**
     * Initialize all foe spawners.
     * @param   SelectedLevelDefinition: level definition to initialize spawners with.
     * @param   SelectedLevelDifficultyDefinition: level difficulty definition to initialize spawners with.
     */
    void InitializeSpawners(const UMTD_LevelDefinition *SelectedLevelDefinition,
        const UMTD_LevelDifficultyDefinition *SelectedLevelDifficultyDefinition);

    /**
     * Cache all foe spawners placed on map.
     */
    void CacheSpawners();

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
    void OnFoeKilled(AActor *FoeActor);

    /**
     * Event to fire when a spawner has prepared to next wave.
     * @param   TotalFoesToSpawn: total foes that spawner will spawn the next wave.
     */
    UFUNCTION()
    void OnSpawnerPrepared(int32 TotalFoesToSpawn);

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
