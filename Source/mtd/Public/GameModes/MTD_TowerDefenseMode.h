#pragma once

#include "GameModes/MTD_GameModeBase.h"
#include "mtd.h"

#include "MTD_TowerDefenseMode.generated.h"

class AMTD_BaseCharacter;
class AMTD_FoeSpawner;
class AMTD_Core;
class UMTD_AbilitySet;
class UMTD_LevelDifficultyDefinition;
class UMTD_FoeSpawnerManager;
class UMTD_WaveManager;

UENUM(BlueprintType)
enum class EMTD_GamePhase : uint8
{
    Build,
    Combat
};

UCLASS()
class MTD_API AMTD_TowerDefenseMode
    : public AMTD_GameModeBase
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnPhaseChangedSignature,
        EMTD_GamePhase, Phase);
    
public:
    AMTD_TowerDefenseMode();

    //~AMTD_GameModeBase Interface
    virtual AActor *GetGameTarget(APawn *Client) const override;
    //~End of AMTD_GameModeBase Interface

    /**
     * Get current scaled difficulty.
     * @return  Current scaled difficulty.
     */
    float GetScaledDifficulty() const;
    
    /**
     * Get current scaled quantity.
     * @return  Current scaled quantity.
     */
    float GetScaledQuantity() const;
    
    /**
     * Get current wave time in seconds.
     * @return  Current wave time in seconds.
     */
    float GetTotalCurrentWaveTime() const;
    
    /**
     * Get current wave.
     * @return  Current wave.
     */
    int32 GetCurrentWave() const;

    /**
     * Get wave manager.
     * @return  Wave manager.
     */
    UMTD_WaveManager *GetWaveManager() const;
    
    /**
     * Get spawner manager.
     * @return  Spawner manager.
     */
    UMTD_FoeSpawnerManager *GetSpawnerManager() const;

    /**
     * Get game phase.
     * @return  Game phase.
     */
    EMTD_GamePhase GetGamePhase() const;

    /**
     * Get selected level difficulty definition.
     * @return  Selected level difficulty definition.
     */
    const UMTD_LevelDifficultyDefinition *GetSelectedLevelDifficultyDefinition() const;
    
    /**
     * Get selected level definition.
     * @return  Selected level definition.
     */
    const UMTD_LevelDefinition *GetSelectedLevelDefinition() const;

    //~AActor Interface
    virtual void Tick(float DeltaSeconds) override;
    virtual void PostInitializeComponents() override;
    
protected:
    virtual void BeginPlay() override;

public:
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    //~End of AActor Interface

private:
    /**
     * Event to fire on wave start.
     * @param   WaveNumber: current wave.
     * @param   RemainingTime: remaining time in seconds.
     */
    UFUNCTION()
    void OnWaveStarted(int32 WaveNumber, float RemainingTime);

    /**
     * Event to fire on wave end.
     * @param   WaveDuration: time in seconds the wave has been running for.
     */
    UFUNCTION()
    void OnWaveEnded(float WaveDuration);

    /**
     * Event to fire when a core is destroyed.
     * @param   CoreActor: destroyed core.
     */
    UFUNCTION()
    void OnCoreDestroyed(AMTD_Core *Core);

    /**
     * Set current game phase.
     * @param   InGamePhase: game phase to set. 
     */
    void SetPhase(EMTD_GamePhase InGamePhase);

    /**
     * Cache cores placed on level.
     */
    void CacheCores();

public:
    /** Delegate to fire when game phase is changed. */
    UPROPERTY(BlueprintAssignable)
    FOnPhaseChangedSignature OnPhaseChangedDelegate;

private:
    /** Wave logic manager. Decides when start and end waves. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="MTD|Tower Defense Mode", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_WaveManager> WaveManager = nullptr;

    /** Spawner logic manager. Spawns enemies, and decides how many of which type has to be spawned. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="MTD|Tower Defense Mode", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_FoeSpawnerManager> SpawnerManager = nullptr;

    /** Time in seconds to delay before switching a phase. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="MTD|Tower Defense Mode", meta=(AllowPrivateAccess="true",
        ClampMin="0.1"))
    float TimePhaseSwitchDelay = 0.1f;

    /** Timer handle to delay phase switch. */
    FTimerHandle PhaseSwitchDelayTimerHandle;

    /** Current level the game is on. */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Tower Defense Mode|Runtime",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<const UMTD_LevelDefinition> SelectedLevelDefinition = nullptr;

    /** Current level difficulty the game is on. */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Tower Defense Mode|Runtime",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<const UMTD_LevelDifficultyDefinition> SelectedLevelDifficultyDefinition = nullptr;

    /** Current phase the game is on. */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Tower Defense Mode|Runtime",
        meta=(AllowPrivateAccess="true"))
    EMTD_GamePhase CurrentGamePhase = EMTD_GamePhase::Build;
    
    /** Cached cores placed on current level. */
    UPROPERTY()
    TArray<TObjectPtr<AMTD_Core>> Cores;
};

inline UMTD_WaveManager *AMTD_TowerDefenseMode::GetWaveManager() const
{
    return WaveManager;
}

inline UMTD_FoeSpawnerManager *AMTD_TowerDefenseMode::GetSpawnerManager() const
{
    return SpawnerManager;
}

inline EMTD_GamePhase AMTD_TowerDefenseMode::GetGamePhase() const
{
    return CurrentGamePhase;
}

inline const UMTD_LevelDifficultyDefinition *AMTD_TowerDefenseMode::GetSelectedLevelDifficultyDefinition() const
{
    return SelectedLevelDifficultyDefinition;
}

inline const UMTD_LevelDefinition *AMTD_TowerDefenseMode::GetSelectedLevelDefinition() const
{
    return SelectedLevelDefinition;
}
