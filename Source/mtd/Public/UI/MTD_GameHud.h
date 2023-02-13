#pragma once

#include "GameFramework/HUD.h"
#include "mtd.h"

#include "MTD_GameHud.generated.h"

/**
 * 
 */
UCLASS()
class MTD_API AMTD_GameHud
    : public AHUD
{
    GENERATED_BODY()

protected:
    //~AHUD Interface
    virtual void BeginPlay() override;
    //~End of AHUD Interface

public:
    UUserWidget *GetPlayerWidget() const;

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="MTD UI", meta=(AllowPrivateAccess="true"))
    TSubclassOf<UUserWidget> PlayerWidgetClass = nullptr;

    UPROPERTY()
    TObjectPtr<UUserWidget> PlayerWidget = nullptr;
};

inline UUserWidget *AMTD_GameHud::GetPlayerWidget() const
{
    return PlayerWidget;
}
