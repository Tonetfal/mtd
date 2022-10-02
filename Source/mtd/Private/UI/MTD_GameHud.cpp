#include "UI/MTD_GameHud.h"

#include "Blueprint/UserWidget.h"

void AMTD_GameHud::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerWidgetClass)
	{
		PlayerWidget = CreateWidget(GetWorld(), PlayerWidgetClass);

		if (IsValid(PlayerWidget))
			PlayerWidget->AddToViewport();
	}
}
