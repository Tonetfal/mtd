#pragma once

#include "GameModes/MTD_GameModeBase.h"
#include "mtd.h"

#include "MTD_TowerDefenseMode.generated.h"

class AMTD_BaseCharacter;
class AMTD_FoeSpawner;
class AMTD_Core;
class UMTD_AbilitySet;
class UMTD_LevelDifficultyDefinition;
class UMTD_FoeSpawnSubsystem;

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
     * Get current wave.
     * @return  Current wave.
     */
    int32 GetCurrentWave() const;

    /**
     * Get game phase.
     * @return  Game phase.
     */
    EMTD_GamePhase GetGamePhase() const;

    //~AActor Interface
    virtual void PostInitializeComponents() override;
    
protected:
    virtual void BeginPlay() override;
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
     * @param   Core: destroyed core.
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
    /** Time in seconds to delay before switching a phase. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="MTD|Tower Defense Mode",
        meta=(AllowPrivateAccess="true", ClampMin="0.1"))
    float TimePhaseSwitchDelay = 0.1f;

    /** Timer handle to delay phase switch. */
    FTimerHandle PhaseSwitchDelayTimerHandle;

    /** Current phase the game is on. */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Tower Defense Mode|Runtime",
        meta=(AllowPrivateAccess="true"))
    EMTD_GamePhase CurrentGamePhase = EMTD_GamePhase::Build;
    
    /** Cached cores placed on current level. */
    UPROPERTY()
    TArray<TObjectPtr<AMTD_Core>> Cores;
};

inline EMTD_GamePhase AMTD_TowerDefenseMode::GetGamePhase() const
{
    return CurrentGamePhase;
}
