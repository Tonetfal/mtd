#include "Gameplay/MTD_WaveManager.h"

#include "AbilitySystem/MTD_AbilitySet.h"
#include "GameModes/MTD_TowerDefenseMode.h"
#include "Gameplay/Levels/MTD_LevelDefinition.h"
#include "Gameplay/Spawner/MTD_FoeSpawnerManager.h"
#include "Kismet/GameplayStatics.h"

void UMTD_WaveManager::Initialize()
{
    auto Tdm = Cast<AMTD_TowerDefenseMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (!IsValid(Tdm))
    {
        MTDS_WARN("Tower Defense Mode is invalid.");
        return;
    }

    const UMTD_LevelDefinition *LevelDefinition = Tdm->GetSelectedLevelDefinition();
    if (!IsValid(LevelDefinition))
    {
        MTDS_WARN("Selected Level Definition is invalid.");
        return;
    }

    TimeToForceStart_FirstWave = LevelDefinition->FirstWaveForceStartTime;
    TimeToForceStart_Regular = LevelDefinition->RegularWaveForceStartTime;

    UMTD_FoeSpawnerManager *SpawnerManager = Tdm->GetSpawnerManager();
    if (!IsValid(SpawnerManager))
    {
        MTDS_WARN("Spawner Manager is invalid.");
        return;
    }

    // Try to setup a timer that will force start a wave
    SetupForceWaveStartTimer(TimeToForceStart_Regular);

    // End a wave when all the spawned mobs have died
    SpawnerManager->OnAllFoesKilledDelegate.AddDynamic(this, &ThisClass::OnAllSpawnedCharactersKilled);
}

void UMTD_WaveManager::StartWave()
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

void UMTD_WaveManager::OnWaveForceStart()
{
    OnWaveForceStartDelegate.Broadcast();
    StartWave();
}

void UMTD_WaveManager::EndWave()
{
    MTD_LOG("Wave [%d] has ended.", CurrentWave);

    // Progress
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

float UMTD_WaveManager::GetWaveDuration() const
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

void UMTD_WaveManager::OnAllSpawnedCharactersKilled(int32 KilledCharactersCount)
{
    EndWave();
}

void UMTD_WaveManager::SetupForceWaveStartTimer(float Seconds)
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

void UMTD_WaveManager::WaveForceStartTimerTick()
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
