#include "Gameplay/Difficulty/MTD_GameDifficultySubsystem.h"

#include "Gameplay/Levels/MTD_LevelDefinition.h"
#include "Gameplay/Levels/MTD_LevelDifficultyDefinition.h"
#include "Settings/MTD_GameDifficultySettings.h"

UMTD_GameDifficultySubsystem *UMTD_GameDifficultySubsystem::Get(const UObject *WorldContextObject)
{
    if (!IsValid(WorldContextObject))
    {
        MTD_WARN("World context object is invalid.");
        return nullptr;
    }
    
    const UWorld *World = WorldContextObject->GetWorld();
    if (!IsValid(World))
    {
        MTD_WARN("World is invalid.");
        return nullptr;
    }
    
    const auto Subsystem = World->GetSubsystem<UMTD_GameDifficultySubsystem>();
    if (!IsValid(Subsystem))
    {
        MTD_WARN("Game difficulty subsystem is invalid.");
        return nullptr;
    }
    
    return Subsystem;
}

void UMTD_GameDifficultySubsystem::PostInitialize()
{
    Super::PostInitialize();
    
    // @todo pick a level through UI instead of hardcoding the first level be always be the case
    
    // This is test purpose code...
    const auto GameDifficultySettings = UMTD_GameDifficultySettings::Get();
    if (!ensure(!GameDifficultySettings->GameLevels.IsEmpty()))
    {
        return;
    }

    if (!ensure(!GameDifficultySettings->GameLevels[0].IsNull()))
    {
        return;
    }
    
    const UMTD_LevelDefinition *FirstGameLevel = GameDifficultySettings->GameLevels[0].LoadSynchronous();
    check(IsValid(FirstGameLevel))
    
    const UMTD_LevelDifficultyDefinition *EasyDifficulty = FirstGameLevel->Difficulties[EMTD_LevelDifficulty::Easy];
    check(IsValid(EasyDifficulty));

    SelectedLevelDefinition = FirstGameLevel;
    SelectedLevelDifficultyDefinition = EasyDifficulty;

    // Notify about initialization
    bInitialized = true;
    OnSubsystemInitializedSelectedDelegate.Broadcast(SelectedLevelDefinition, SelectedLevelDifficultyDefinition);
}

float UMTD_GameDifficultySubsystem::GetScaledDifficulty() const
{
    if (!IsValid(SelectedLevelDifficultyDefinition))
    {
        MTDS_WARN("Selected level difficulty definition is invalid.");
        return 0.f;
    }

    // @todo actually compute the difficulty by applying a formula and using current wave as the scalar
    return SelectedLevelDifficultyDefinition->BaseDifficulty;
}

float UMTD_GameDifficultySubsystem::GetScaledQuantity() const
{
    if (!IsValid(SelectedLevelDifficultyDefinition))
    {
        MTDS_WARN("Selected level difficulty definition is invalid.");
        return 0.f;
    }

    // @todo actually compute the quantity by applying a formula and using current wave as the scalar
    return SelectedLevelDifficultyDefinition->BaseQuantity;
}

float UMTD_GameDifficultySubsystem::GetTotalCurrentWaveTime() const
{
    if (!IsValid(SelectedLevelDifficultyDefinition))
    {
        MTDS_WARN("Selected level difficulty definition is invalid.");
        return 0.f;
    }

    // @todo actually compute the time by applying a formula and using current wave as the scalar
    return SelectedLevelDifficultyDefinition->BaseTime;
}

void UMTD_GameDifficultySubsystem::OnSubsystemInitialized_RegisterAndCall(
    FOnDifficultySubsystemInitializedSignature::FDelegate Delegate)
{
    // Avoid registering if already did
    if (!OnSubsystemInitializedSelectedDelegate.IsBoundToObject(Delegate.GetUObject()))
    {
        // Register the delegate
        OnSubsystemInitializedSelectedDelegate.Add(Delegate);
    }

    if (bInitialized)
    {
        // Call the delegate if subsystem already initialized
        Delegate.Execute(SelectedLevelDefinition, SelectedLevelDifficultyDefinition);
    }
}
