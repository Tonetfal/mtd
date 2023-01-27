#pragma once

#include "GameModes/MTD_GameModeBase.h"
#include "mtd.h"

#include "MTD_TowerDefenseMode.generated.h"

class AMTD_BaseCharacter;
class AMTD_CharacterSpawner;
class AMTD_Core;
class UMTD_AbilitySet;
class UMTD_LevelDifficultyDefinition;
class UMTD_SpawnerManager;
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

    float GetScaledDifficulty() const;
    float GetScaledQuantity() const;
    float GetTotalCurrentWaveTime() const;
    int32 GetCurrentWave() const;
    UMTD_WaveManager *GetWaveManager() const;
    UMTD_SpawnerManager *GetSpawnerManager() const;
    EMTD_GamePhase GetGamePhase() const;
    const UMTD_LevelDifficultyDefinition *GetSelectedLevelDifficultyDefinition() const;
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
    UFUNCTION()
    void OnWaveStarted(int32 WaveNumber, float RemainingSeconds);
    
    UFUNCTION()
    void OnWaveEnded(float WaveDuration);
    
    void CacheCores();
    
    UFUNCTION()
    void OnCoreDestroyed();

public:
    UPROPERTY(BlueprintAssignable)
    FOnPhaseChangedSignature OnPhaseChangedDelegate;

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="MTD|Tower Defense Mode",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_WaveManager> WaveManager = nullptr;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="MTD|Tower Defense Mode",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_SpawnerManager> SpawnerManager = nullptr;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Tower Defense Mode|Runtime",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<const UMTD_LevelDefinition> SelectedLevelDefinition = nullptr;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Tower Defense Mode|Runtime",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<const UMTD_LevelDifficultyDefinition> SelectedLevelDifficultyDefinition = nullptr;
    
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Tower Defense Mode|Runtime",
        meta=(AllowPrivateAccess="true"))
    EMTD_GamePhase CurrentGamePhase = EMTD_GamePhase::Build;
    
    /** All the cores placed on current map. */
    UPROPERTY()
    TArray<TObjectPtr<AMTD_Core>> Cores;
};

inline UMTD_WaveManager *AMTD_TowerDefenseMode::GetWaveManager() const
{
    return WaveManager;
}

inline UMTD_SpawnerManager *AMTD_TowerDefenseMode::GetSpawnerManager() const
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
