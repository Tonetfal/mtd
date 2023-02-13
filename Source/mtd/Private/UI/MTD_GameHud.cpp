#include "UI/MTD_GameHud.h"

#include "Blueprint/UserWidget.h"

void AMTD_GameHud::BeginPlay()
{
    Super::BeginPlay();

    APlayerController *PlayerController = GetOwningPlayerController();
    if (!IsValid(PlayerController))
    {
        return;
    }

    if (!PlayerWidgetClass)
    {
        MTDS_WARN("Player widget class is NULL.");
        return;
    }

    // Create and initialize widget
    PlayerWidget = CreateWidget(GetWorld(), PlayerWidgetClass);
    PlayerWidget->SetOwningPlayer(PlayerController);

    if (IsValid(PlayerWidget))
    {
        // Show on screen
        PlayerWidget->AddToViewport();
    }
}
