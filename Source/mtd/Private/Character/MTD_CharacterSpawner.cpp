#include "Character/MTD_CharacterSpawner.h"

#include "Character/MTD_BaseCharacter.h"
#include "Kismet/GameplayStatics.h"

AMTD_CharacterSpawner::AMTD_CharacterSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;
}

void AMTD_CharacterSpawner::BeginPlay()
{
    Super::BeginPlay();

    if (!ensureAlways(MaxSecondsToSpawn >= MinSecondsToSpawn))
    {
        return;
    }

    if (!ensureAlways(CharacterClass))
    {
        return;
    }

    World = GetWorld();
    bCanSpawn = true;
}

void AMTD_CharacterSpawner::StartSpawning()
{
    if (!bCanSpawn)
    {
        return;
    }
    
    PrepareNextSpawnCall();
}

void AMTD_CharacterSpawner::StopSpawning()
{
    World->GetTimerManager().ClearTimer(SpawnTimerHandle);
}

void AMTD_CharacterSpawner::PrepareNextSpawnCall()
{
    const float Delay = GetSpawnDelay();
    World->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ThisClass::OnSpawn, Delay, false);
}

FTransform AMTD_CharacterSpawner::GetSpawnTransform() const
{
    FTransform Transform = GetActorTransform();
    const FVector Offset = GetRandomPointOnSpawnArea();
    Transform.AddToTranslation(Offset);

    return Transform;
}

FVector AMTD_CharacterSpawner::GetRandomPointOnSpawnArea() const
{
    const float X = FMath::FRandRange(-1.f, +1.f);
    const float Y = ((FMath::RandBool()) ? (+1.f) : (-1.f)) * (1.f - FMath::Abs(X));
    const FVector Direction = FVector(X, Y, 0.f);
    const FVector Position = Direction * SpawnRange;

    return Position;
}

void AMTD_CharacterSpawner::OnSpawn()
{
    PrepareNextSpawnCall();

    const FTransform Transform = GetSpawnTransform();
    
    const auto HdlMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    
    auto Character = World->SpawnActorDeferred<AMTD_BaseCharacter>(
        CharacterClass, Transform, nullptr, nullptr, HdlMethod);
    Character->SpawnDefaultController();
    UGameplayStatics::FinishSpawningActor(Character, Transform);
    
    OnSpawnDelegate.Broadcast(Character);
}
