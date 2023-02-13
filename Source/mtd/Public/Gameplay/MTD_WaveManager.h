#pragma once

#include "mtd.h"
#include "UObject/Object.h"

#include "MTD_WaveManager.generated.h"

class UMTD_AbilitySet;

/**
 * Class that manages waves on a game level.
 *
 * A wave can be started by a player himself, or by an internal timer. The class keeps track of the current wave, as
 * well as the time since the way has started, and when a way has to be ended.
 */
UCLASS()
class MTD_API UMTD_WaveManager
    : public UObject
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
    void Initialize();
    
    UFUNCTION(BlueprintCallable, Category="MTD|Wave Manager")
    void StartWave();
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Wave Manager")
    int32 GetCurrentWave() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Wave Manager")
    float GetWaveDuration() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Wave Manager")
    float GetWaveStartTimeSeconds() const;

private:
    UFUNCTION()
    void OnWaveForceStart();
    void EndWave();

    UFUNCTION()
    void OnAllSpawnedCharactersKilled(int32 KilledCharactersCount);
    
    void SetupForceWaveStartTimer(float Seconds);
    
    /** Is called each second. */
    void WaveForceStartTimerTick();

public:
    UPROPERTY(BlueprintAssignable)
    FOnWaveStartSignature OnWaveStartDelegate;
    
    UPROPERTY(BlueprintAssignable)
    FOnWaveEndSignature OnWaveEndDelegate;
    
    UPROPERTY(BlueprintAssignable)
    FOnWaveForceStartSignature OnWaveForceStartDelegate;
    
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

inline int32 UMTD_WaveManager::GetCurrentWave() const
{
    return CurrentWave;
}

inline float UMTD_WaveManager::GetWaveStartTimeSeconds() const
{
    return WaveStartTime;
}
