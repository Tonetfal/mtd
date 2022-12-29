#pragma once

#include "GameFramework/Actor.h"
#include "ScreenFloatingTextRuntime/sft.h"

#include "SFT_FloatingTextActor.generated.h"

class UCommonTextBlock;
class USFT_FloatingTextWidget;
class UWidgetComponent;

UCLASS()
class SCREENFLOATINGTEXTRUNTIME_API ASFT_FloatingTextActor : public AActor
{
    GENERATED_BODY()

public:
    ASFT_FloatingTextActor();
    
    UFUNCTION(BlueprintCallable, Category="SFT|Floating Text Actor")
    void PostInitialize();

    UCommonTextBlock *GetTextBlock() const;

protected:
    virtual void BeginPlay() override;
    
private:
    void OnFloatingTextWidgetDisappeared(USFT_FloatingTextWidget *Widget);

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SFT|Floating Text Actor", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UWidgetComponent> WidgetComponent = nullptr;
};
