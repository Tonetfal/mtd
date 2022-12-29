#include "SFT_FloatingTextComponent.h"

#include "SFT_FloatingTextActor.h"

USFT_FloatingTextComponent::USFT_FloatingTextComponent()
{
    FloatingTextActorClass = ASFT_FloatingTextActor::StaticClass();
}

void USFT_FloatingTextComponent::SpawnFloatingText(const FVector &SpawnLocation,
    ASFT_FloatingTextActor *&OutFloatingTextActor, UCommonTextBlock *&OutTextBlock)
{
    if (!FloatingTextActorClass)
    {
        UE_LOG(LogScreenFloatingText, Warning, TEXT("Floating Text Actor Class is NULL."));
        return;
    }
    
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    UWorld *World = GetWorld();
    AActor *Actor = World->SpawnActor(FloatingTextActorClass, &SpawnLocation, nullptr, SpawnParams);
    if (!IsValid(Actor))
    {
        UE_LOG(LogScreenFloatingText, Warning, TEXT("Failed to spawn a Floating Text Actor."));
        return;
    }

    auto FloatingTextActor = Cast<ASFT_FloatingTextActor>(Actor);
    UCommonTextBlock *TextBlock = FloatingTextActor->GetTextBlock();

    OutFloatingTextActor = FloatingTextActor;
    OutTextBlock = TextBlock;

    FloatingTextActors.Add(FloatingTextActor);
}
