#include "Gameplay/Wave/MTD_GameWavesSubsystem.h"

#include "AbilitySystem/MTD_AbilitySet.h"
#include "Gameplay/Difficulty/MTD_GameDifficultySubsystem.h"
#include "Gameplay/Levels/MTD_LevelDefinition.h"
#include "Gameplay/Spawner/MTD_FoeSpawnSubsystem.h"
#include "Kismet/GameplayStatics.h"

UMTD_GameWavesSubsystem *UMTD_GameWavesSubsystem::Get(const UObject *WorldContextObject)
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
    
    const auto Subsystem = World->GetSubsystem<UMTD_GameWavesSubsystem>();
    if (!IsValid(Subsystem))
    {
        MTD_WARN("Game waves subsystem is invalid.");
        return nullptr;
    }
    
    return Subsystem;
}

bool UMTD_GameWavesSubsystem::ShouldCreateSubsystem(UObject *Outer) const
{
    // @todo create this subsystem only for TD experience
    return Super::ShouldCreateSubsystem(Outer);
}

void UMTD_GameWavesSubsystem::PostInitialize()
{
    Super::PostInitialize();
    
    const auto GameDifficultySubsystem = UMTD_GameDifficultySubsystem::Get(this);
    check(IsValid(GameDifficultySubsystem));

    // Listen for difficulty initialization
    UMTD_GameDifficultySubsystem::FOnDifficultySubsystemInitializedSignature::FDelegate Delegate;
    Delegate.BindUObject(this, &ThisClass::OnGameDifficultySubsystemInit);
    GameDifficultySubsystem->OnSubsystemInitialized_RegisterAndCall(Delegate);

    // Try to setup a timer that will force start a wave
    SetupForceWaveStartTimer(TimeToForceStart_Regular);
    
    const auto FoeSpawnSubsystem = UMTD_FoeSpawnSubsystem::Get(this);
    check(IsValid(FoeSpawnSubsystem));

    // Listen for event that will end a wave when all the spawned mobs have died
    FoeSpawnSubsystem->OnAllFoesKilledDelegate.AddDynamic(this, &ThisClass::OnAllFoesKilled);
}

void UMTD_GameWavesSubsystem::StartWave()
{
    if (bIsWaveRunning)
    {
        return;
    }

    MTD_LOG("Wave [%d] has started.", CurrentWave);

    bIsWaveRunning = true;
    WaveStartTime = GetWorld()->GetTimeSeconds();
    OnWaveStartDelegate.Broadcast(CurrentWave, RemainingTimeToForceStart);
}

void UMTD_GameWavesSubsystem::OnWaveForceStart()
{
    OnWaveForceStartDelegate.Broadcast();
    StartWave();
}

void UMTD_GameWavesSubsystem::EndWave()
{
    MTD_LOG("Wave [%d] has ended.", CurrentWave);

    // Keep track of the progress
    bIsWaveRunning = false;
    CurrentWave++;

    // Notify and try to setup a new start timer
    if (OnWaveEndDelegate.IsBound())
    {
        OnWaveEndDelegate.Broadcast(GetWaveDuration());
    }

    // Try to setup a new timer to force start
    SetupForceWaveStartTimer(TimeToForceStart_Regular);
}

float UMTD_GameWavesSubsystem::GetWaveDuration() const
{
    // If no wave has taken place yet, the value will be the default one - 0
    if (WaveStartTime == 0.f)
    {
        return 0.f;
    }

    // Compute difference between current time and wave start time
    const float CurrentTime = GetWorld()->GetTimeSeconds();
    const float WaveDuration = (CurrentTime - WaveStartTime);
    return WaveDuration;
}

void UMTD_GameWavesSubsystem::OnAllFoesKilled(int32 DeathCount)
{
    EndWave();
}

void UMTD_GameWavesSubsystem::SetupForceWaveStartTimer(float Seconds)
{
    if (!bIsAllowedForceStart)
    {
        return;
    }

    // Give a different amount of time, probably a higher one, if it's the first wave
    RemainingTimeToForceStart = ((CurrentWave == 0) ?
        (TimeToForceStart_FirstWave) : (TimeToForceStart_Regular));

    // Call each second
    GetWorld()->GetTimerManager().SetTimer(ForceWaveStartTimerTickTimerHandle, this,
        &ThisClass::WaveForceStartTimerTick, 1.f, true);
}

void UMTD_GameWavesSubsystem::WaveForceStartTimerTick()
{
    RemainingTimeToForceStart -= 1.f;
    
    if (RemainingTimeToForceStart > 0.f)
    {
        // Notify about new tick
        OnWaveForceStartTimerTickDelegate.Broadcast(RemainingTimeToForceStart);
    }
    else
    {
        // Reset states
        RemainingTimeToForceStart = 0.f;
        GetWorld()->GetTimerManager().ClearTimer(ForceWaveStartTimerTickTimerHandle);

        // Force start
        OnWaveForceStart();
    }
}

void UMTD_GameWavesSubsystem::OnGameDifficultySubsystemInit(const UMTD_LevelDefinition *SelectedLevelDefinition,
    const UMTD_LevelDifficultyDefinition *SelectedLevelDifficultyDefinition)
{
    check(IsValid(SelectedLevelDefinition));

    TimeToForceStart_FirstWave = SelectedLevelDefinition->FirstWaveForceStartTime;
    TimeToForceStart_Regular = SelectedLevelDefinition->RegularWaveForceStartTime;
}
