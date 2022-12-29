#include "SFT_FloatingTextActor.h"

#include "SFT_FloatingTextWidget.h"
#include "Components/WidgetComponent.h"

ASFT_FloatingTextActor::ASFT_FloatingTextActor()
{
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;

    WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget Component"));
    SetRootComponent(WidgetComponent);
    
    WidgetComponent->SetWidgetClass(USFT_FloatingTextWidget::StaticClass());
    WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
}

void ASFT_FloatingTextActor::PostInitialize()
{
    UWidget *Widget = WidgetComponent->GetWidget();
    if (!IsValid(Widget))
    {
        UE_LOG(LogScreenFloatingText, Warning, TEXT("Widget is invalid."));
        return;
    }

    auto FloatingTextWidget = CastChecked<USFT_FloatingTextWidget>(Widget);
    FloatingTextWidget->PostInitialize();
}

UCommonTextBlock *ASFT_FloatingTextActor::GetTextBlock() const
{
    UWidget *Widget = WidgetComponent->GetWidget();
    if (!IsValid(Widget))
    {
        UE_LOG(LogScreenFloatingText, Warning, TEXT("Widget is invalid."));
        return nullptr;
    }

    auto FloatingTextWidget = CastChecked<USFT_FloatingTextWidget>(Widget);
    UCommonTextBlock *TextBlock = FloatingTextWidget->GetTextBlock();
    
    return TextBlock;
}

void ASFT_FloatingTextActor::BeginPlay()
{
    Super::BeginPlay();

    UWidget *Widget = WidgetComponent->GetWidget();
    if (!IsValid(Widget))
    {
        UE_LOG(LogScreenFloatingText, Warning, TEXT("Widget is invalid."));
        return;
    }

    auto FloatingTextWidget = Cast<USFT_FloatingTextWidget>(Widget);
    FloatingTextWidget->OnDisappearedDelegate.BindUObject(this, &ThisClass::OnFloatingTextWidgetDisappeared);
}

void ASFT_FloatingTextActor::OnFloatingTextWidgetDisappeared(USFT_FloatingTextWidget *Widget)
{
    Destroy();
}
