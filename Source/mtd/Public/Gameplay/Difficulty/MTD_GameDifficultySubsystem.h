#pragma once

#include "mtd.h"
#include "Subsystems/WorldSubsystem.h"

#include "MTD_GameDifficultySubsystem.generated.h"

class UMTD_LevelDefinition;
class UMTD_LevelDifficultyDefinition;

/**
 * Subsystem responsible to define how difficult the game is at a certain point.
 */
UCLASS()
class UMTD_GameDifficultySubsystem
	: public UWorldSubsystem
{
    GENERATED_BODY()

public:
    DECLARE_MULTICAST_DELEGATE_TwoParams(
        FOnDifficultySubsystemInitializedSignature,
        const UMTD_LevelDefinition * /* SelectedLevelDefinition */,
        const UMTD_LevelDifficultyDefinition * /* SelectedLevelDifficultyDefinition */);

public:
    /**
     * Get the subsystem from a world context.
     * @param   WorldContextObject: context used to retrieve the subsystem.
     * @return  This subsystem. May be nullptr.
     */
    static UMTD_GameDifficultySubsystem *Get(const UObject *WorldContextObject);
    
    //~UWorldSubsystem Interface
    virtual void PostInitialize() override;
    //~End of UWorldSubsystem Interface

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

    // @todo move it in game wave subsystem instead
    /**
     * Get current wave time in seconds.
     * @return  Current wave time in seconds.
     */
    float GetTotalCurrentWaveTime() const;

    /**
     * Register a delegate to call when subsystem is initialized. Will be immediately fired if it already is.
     * @param   Delegate: delegate to register for the event.
     */
    void OnSubsystemInitialized_RegisterAndCall(FOnDifficultySubsystemInitializedSignature::FDelegate Delegate);

private:
    /** Delegate to fire when this subsystem is initialized. */
    FOnDifficultySubsystemInitializedSignature OnSubsystemInitializedSelectedDelegate;

private:
    /** Current level the game is on. */
    UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
    TObjectPtr<const UMTD_LevelDefinition> SelectedLevelDefinition = nullptr;

    /** Current level difficulty the game is on. */
    UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
    TObjectPtr<const UMTD_LevelDifficultyDefinition> SelectedLevelDifficultyDefinition = nullptr;

    /** If true, subsystem is initialized, false otherwise. */
    bool bInitialized = false;
};

inline const UMTD_LevelDifficultyDefinition *UMTD_GameDifficultySubsystem::GetSelectedLevelDifficultyDefinition() const
{
    return SelectedLevelDifficultyDefinition;
}

inline const UMTD_LevelDefinition *UMTD_GameDifficultySubsystem::GetSelectedLevelDefinition() const
{
    return SelectedLevelDefinition;
}
