#pragma once

#include "Blueprint/UserWidget.h"
#include "CommonUserWidget.h"
#include "ScreenFloatingTextRuntime/sft.h"

#include "SFT_FloatingTextWidget.generated.h"

class UCommonTextBlock;

UCLASS()
class SCREENFLOATINGTEXTRUNTIME_API USFT_FloatingTextWidget : public UCommonUserWidget
{
    GENERATED_BODY()

public:
    DECLARE_DELEGATE_OneParam(FOnDisappearedSignature, USFT_FloatingTextWidget*);

public:
    USFT_FloatingTextWidget();
    
    void PostInitialize();

    UCommonTextBlock *GetTextBlock() const;

private:
    UFUNCTION()
    void OnFloatingTextDisappeared();

public:
    FOnDisappearedSignature OnDisappearedDelegate;

private:
    UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true", BindWidget))
    TObjectPtr<UCommonTextBlock> TextBlock = nullptr;

    UPROPERTY(Transient, meta=(BindWidgetAnim))
    TObjectPtr<UWidgetAnimation> DisappearAnimation = nullptr;
};

inline UCommonTextBlock *USFT_FloatingTextWidget::GetTextBlock() const
{
    return TextBlock;
}
