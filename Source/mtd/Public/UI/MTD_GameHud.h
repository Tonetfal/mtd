#pragma once

#include "GameFramework/HUD.h"
#include "mtd.h"

#include "MTD_GameHud.generated.h"

UCLASS()
class MTD_API AMTD_GameHud : public AHUD
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
public:
	UUserWidget *GetPlayerWidget() const
		{ return PlayerWidget; }
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="MTD UI",
		meta=(AllowPrivateAccess="true"))
	TSubclassOf<UUserWidget> PlayerWidgetClass = nullptr;

	UPROPERTY()
	TObjectPtr<UUserWidget> PlayerWidget = nullptr;
};
