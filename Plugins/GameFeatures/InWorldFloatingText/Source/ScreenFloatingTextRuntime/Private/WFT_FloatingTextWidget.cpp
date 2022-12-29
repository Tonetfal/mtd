#include "SFT_FloatingTextWidget.h"

#include "Animation/WidgetAnimation.h"
#include "CommonTextBlock.h"

USFT_FloatingTextWidget::USFT_FloatingTextWidget()
{
    TextBlock = CreateDefaultSubobject<UCommonTextBlock>(TEXT("TextBlock"));
}

void USFT_FloatingTextWidget::PostInitialize()
{
    if (!IsValid(DisappearAnimation))
    {
        UE_LOG(LogScreenFloatingText, Warning, TEXT("Disappear Animation is invalid."));
    }
    else
    {
        PlayAnimation(DisappearAnimation);

        const float EndTime = DisappearAnimation->GetEndTime();

        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::OnFloatingTextDisappeared, EndTime);
    }
}

void USFT_FloatingTextWidget::OnFloatingTextDisappeared()
{
    OnDisappearedDelegate.ExecuteIfBound(this);
}
