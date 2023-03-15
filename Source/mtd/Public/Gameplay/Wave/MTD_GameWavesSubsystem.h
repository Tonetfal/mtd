#pragma once

#include "mtd.h"
#include "Subsystems/WorldSubsystem.h"

#include "MTD_GameWavesSubsystem.generated.h"

class UMTD_AbilitySet;
class UMTD_LevelDefinition;
class UMTD_LevelDifficultyDefinition;

/**
 * Subsystem that manages waves on a game level.
 *
 * A wave can be started by a player himself, or by an internal timer. The class keeps track of the current wave, as
 * well as the time since the way has started, and when a way has to be ended.
 */
UCLASS()
class MTD_API UMTD_GameWavesSubsystem
    : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
        FOnWaveStartSignature,
        int32, WaveNumber,
        float, RemainingForceStartTime);
    
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnWaveEndSignature,
        float, WaveDuration);
    
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(
        FOnWaveForceStartSignature);
    
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnWaveForceStartTimerTickSignature,
        float, RemainingTime);

public:
    /**
     * Get the subsystem from a world context.
     * @param   WorldContextObject: context used to retrieve the subsystem.
     * @return  This subsystem. May be nullptr.
     */
    static UMTD_GameWavesSubsystem *Get(const UObject *WorldContextObject);
    
    //~WorldSubsystem Interface
    virtual bool ShouldCreateSubsystem(UObject *Outer) const override;
    virtual void PostInitialize() override;
    //~End of WorldSubsystem Interface

    /**
     * Start a wave if not already in progress.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Wave Manager")
    void StartWave();
    
    /**
     * Get wave the game is currently on.
     * @return  Wave the game is currently on.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Wave Manager")
    int32 GetCurrentWave() const;
    
    /**
     * Get time in seconds the current wave has been running for.
     * @return  Time in seconds the current wave has been running for.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Wave Manager")
    float GetWaveDuration() const;
    
    /**
     * Get time in seconds after the world was brought to play at time of current wave start.
     * @return  Time in seconds after the world was brought to play at time of current wave start.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Wave Manager")
    float GetWaveStartTimeSeconds() const;

private:
    /**
     * Force start a wave if not already in progress.
     */
    UFUNCTION()
    void OnWaveForceStart();
    
    /**
     * End a wave if it's in progress.
     */
    void EndWave();

    /**
     * Event to fire when all spawned foes were killed.
     * @param   DeathCount: amount of slayed foes during the last wave.
     */
    UFUNCTION()
    void OnAllFoesKilled(int32 DeathCount);
    
    /**
     * Setup a timer to launch force start upon expiration.
     * @param   Seconds: seconds to execute the force start after.
     */
    void SetupForceWaveStartTimer(float Seconds);
    
    /**
     * Event to fire each second to progress the force start timer.
     */
    void WaveForceStartTimerTick();

    /** Event to fire when game difficulty subsystem finishes initialization. */
    void OnGameDifficultySubsystemInit(const UMTD_LevelDefinition *SelectedLevelDefinition,
        const UMTD_LevelDifficultyDefinition *SelectedLevelDifficultyDefinition);

public:
    /** Delegate to fire when a wave starts. */
    UPROPERTY(BlueprintAssignable)
    FOnWaveStartSignature OnWaveStartDelegate;

    /** Delegate to fire when a wave ends. */
    UPROPERTY(BlueprintAssignable)
    FOnWaveEndSignature OnWaveEndDelegate;

    /** Delegate to fire when a ware is force started. */
    UPROPERTY(BlueprintAssignable)
    FOnWaveForceStartSignature OnWaveForceStartDelegate;

    /** Delegate to fire each force start timer tick. */
    UPROPERTY(BlueprintAssignable)
    FOnWaveForceStartTimerTickSignature OnWaveForceStartTimerTickDelegate;

private:
    /** Tells whether force wave start countdown can be setup or not. */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Wave Manager|Runtime|Wave Force Start",
        meta=(AllowPrivateAccess="true"))
    bool bIsAllowedForceStart = false;

    /** Time in seconds the force start on first wave will take place after. */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Wave Manager|Runtime|Wave Force Start",
        meta=(AllowPrivateAccess="true"))
    float TimeToForceStart_FirstWave = 60.f;

    /** Time in seconds the force start on a wave that is not the first will take place after. */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Wave Manager|Runtime|Wave Force Start",
        meta=(AllowPrivateAccess="true"))
    float TimeToForceStart_Regular = 30.f;

    /** Remaining time in seconds the force start will take place. */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Wave Manager|Runtime|Wave Force Start",
        meta=(AllowPrivateAccess="true"))
    float RemainingTimeToForceStart = -1.f;

    /** Counter of the waves. */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Wave Manager|Runtime",
        meta=(AllowPrivateAccess="true"))
    int32 CurrentWave = 0;

    /** Tells whether a wave is currently running or not. */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Wave Manager|Runtime",
        meta=(AllowPrivateAccess="true"))
    bool bIsWaveRunning = false;

    /** Time in seconds after the world was brought to play at time of current wave start. */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Wave Manager|Runtime",
        meta=(AllowPrivateAccess="true"))
    float WaveStartTime = 0.f;
    
    FTimerHandle ForceWaveStartTimerTickTimerHandle;
};

inline int32 UMTD_GameWavesSubsystem::GetCurrentWave() const
{
    return CurrentWave;
}

inline float UMTD_GameWavesSubsystem::GetWaveStartTimeSeconds() const
{
    return WaveStartTime;
}
