#include "UI/MTD_GameHud.h"

#include "Blueprint/UserWidget.h"

void AMTD_GameHud::BeginPlay()
{
    Super::BeginPlay();

    APlayerController *Pc = GetOwningPlayerController();
    if (!IsValid(Pc))
    {
        return;
    }

    if (!PlayerWidgetClass)
    {
        return;
    }

    PlayerWidget = CreateWidget(GetWorld(), PlayerWidgetClass);
    PlayerWidget->SetOwningPlayer(Pc);

    if (IsValid(PlayerWidget))
    {
        PlayerWidget->AddToViewport();
    }
}
