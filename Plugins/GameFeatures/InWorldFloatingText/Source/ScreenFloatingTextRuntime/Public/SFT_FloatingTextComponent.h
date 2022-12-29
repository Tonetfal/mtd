#pragma once

#include "Components/ActorComponent.h"
#include "ScreenFloatingTextRuntime/sft.h"

#include "SFT_FloatingTextComponent.generated.h"

class ASFT_FloatingTextActor;
class UCommonTextBlock;
class UWidgetComponent;

UCLASS(BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class SCREENFLOATINGTEXTRUNTIME_API USFT_FloatingTextComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USFT_FloatingTextComponent();

    UFUNCTION(BlueprintCallable, Category="SFT|Floating Text Component")
    void SpawnFloatingText(const FVector &SpawnLocation, ASFT_FloatingTextActor *&OutFloatingTextActor,
        UCommonTextBlock *&OutTextBlock);

private:
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<ASFT_FloatingTextActor> FloatingTextActorClass = nullptr;
    
    UPROPERTY()
    TArray<TObjectPtr<ASFT_FloatingTextActor>> FloatingTextActors;
};
