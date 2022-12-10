#pragma once

#include "mtd.h"
#include "GameModes/MTD_GameModeBase.h"

#include "MTD_TowerDefenseMode.generated.h"

class AMTD_CharacterSpawner;
class AMTD_BaseCharacter;
class UMTD_AbilitySet;
class AMTD_Core;

UENUM(BlueprintType)
enum class EMTD_GamePhase : uint8
{
    Build,
    Combat
};

UCLASS()
class MTD_API AMTD_TowerDefenseMode : public AMTD_GameModeBase
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWaveStartSignature, int32, WaveNumber, float, RemainingSeconds);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveEndSignature, float, WaveDuration);
    
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWaveForceStartSignature);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveForceStartTimerTickSignature, float, RemainingSeconds);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChangedSignature, EMTD_GamePhase, Phase);
    
public:
    AMTD_TowerDefenseMode();

    UFUNCTION(BlueprintCallable, Category="MTD|Tower Defense Mode")
    void StartWave();

    //~AMTD_GameModeBase Interface
    virtual AActor *GetGameTarget(APawn *Client) const override;
    //~End of AMTD_GameModeBase Interface
    
    int32 GetWave() const;
    EMTD_GamePhase GetGamePhase() const;

protected:
    //~AActor Interface
    virtual void BeginPlay() override;
    //~End of AActor Interface

private:
    UFUNCTION()
    void OnWaveForceStart();
    void EndWave();
    
    void StartSpawning();
    void StopSpawning();
    
    void CacheCores();
    void CacheSpawners();
    void DispatchAbilities();
    void SetupForceWaveStartTimer(float Seconds);

    /** Is called each second. */
    void WaveForceStartTimerTick();
    
    UFUNCTION()
    void OnCoreDestroyed();

    UFUNCTION()
    void OnMobSpawn(AActor *Actor);
    
    UFUNCTION()
    void OnMobDied(AActor *Actor);

public:
    UPROPERTY(BlueprintAssignable)
    FOnWaveStartSignature OnWaveStartDelegate;
    
    UPROPERTY(BlueprintAssignable)
    FOnWaveEndSignature OnWaveEndDelegate;
    
    UPROPERTY(BlueprintAssignable)
    FOnWaveForceStartSignature OnWaveForceStartDelegate;
    
    UPROPERTY(BlueprintAssignable)
    FOnWaveForceStartTimerTickSignature OnWaveForceStartTimerTickDelegate;

    UPROPERTY(BlueprintAssignable)
    FOnPhaseChangedSignature OnPhaseChangedDelegate;

private:
    /** Ability sets that the players will be granted with. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="MTD|Tower Defense Mode",
        meta=(AllowPrivateAccess="true"))
    TArray<TObjectPtr<UMTD_AbilitySet>> AbilitySets;

    /** Base class of the character that will receive the abilities. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="MTD|Tower Defense Mode",
        meta=(AllowPrivateAccess="true"))
    TSubclassOf<AMTD_BaseCharacter> PlayerClass = nullptr;
    
    /** All the cores placed on map. */
    UPROPERTY()
    TArray<TObjectPtr<AMTD_Core>> Cores;
    
    /** All the spawners placed on map. */
    UPROPERTY()
    TArray<TObjectPtr<AMTD_CharacterSpawner>> Spawners;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Tower Defense Mode|Wave Force Start",
        meta=(AllowPrivateAccess="true"))
    bool bShouldSetupForceWaveStartCountdown = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="MTD|Tower Defense Mode|Wave Force Start",
        meta=(AllowPrivateAccess="true"))
    float SecondsToWaveForceStart = 60.f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Tower Defense Mode|Wave Force Start",
        meta=(AllowPrivateAccess="true"))
    float RemainingSecondsToWaveForceStart = -1.f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Tower Defense Mode|Runtime",
        meta=(AllowPrivateAccess="true"))
    int32 CurrentWave = 0;
    
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Tower Defense Mode|Runtime",
        meta=(AllowPrivateAccess="true"))
    EMTD_GamePhase CurrentGamePhase = EMTD_GamePhase::Build;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Tower Defense Mode|Runtime",
        meta=(AllowPrivateAccess="true"))
    bool bWaveRunning = false;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Tower Defense Mode|Runtime",
        meta=(AllowPrivateAccess="true"))
    int32 SpawnedMobsOnCurrentWave = 0;
    
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Tower Defense Mode|Runtime",
        meta=(AllowPrivateAccess="true"))
    int32 KilledMobsOnCurrentWave = 0;
    
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Tower Defense Mode|Runtime",
        meta=(AllowPrivateAccess="true"))
    float WaveDuration = -1.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="MTD|Tower Defense Mode",
        meta=(AllowPrivateAccess="true", ClampMin="1.0"))
    int32 MobsToSpawn = 0;
    
    FTimerHandle ForceWaveStartTimerTickTimerHandle;
};

inline int32 AMTD_TowerDefenseMode::GetWave() const
{
    return CurrentWave;
}

inline EMTD_GamePhase AMTD_TowerDefenseMode::GetGamePhase() const
{
    return CurrentGamePhase;
}

