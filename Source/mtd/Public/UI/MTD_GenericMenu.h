#pragma once

#include "CommonButtonBase.h"
#include "CommonUserWidget.h"
#include "mtd.h"

#include "MTD_GenericMenu.generated.h"

/**
 * Generic widget to handle a number of selectable buttons which spawn another widgets, and only one of them must be
 * present at a time. Makes sure that only one button can selected at a time, however none can be selected as well.
 */
UCLASS()
class MTD_API UMTD_GenericMenu
    : public UCommonUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

protected:
    UFUNCTION(BlueprintCallable, Category="MTD|Generic Menu")
    void CreateWidget(TSubclassOf<UUserWidget> UserWidgetClass);
    
    UFUNCTION(BlueprintCallable, Category="MTD|Generic Menu")
    void DeleteWidget();

    UFUNCTION(BlueprintCallable, Category="MTD|Generic Menu")
    void MenuButtonPress(UCommonButtonBase *Button, bool bSelected);
    
    UFUNCTION(BlueprintImplementableEvent, DisplayName="MapButtons", Category="MTD|Generic Menu")
    void K2_MapButtons();

    /**
     * Compute OurPadding + SizeWidget's Padding.
     * @return  Padding the spawned widget should have.
     */
    FMargin ComputePadding() const;

private:
    /** A widget spawned by CreateWidget. */
    UPROPERTY(BlueprintReadWrite, Category="MTD|Generic Menu", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UUserWidget> SpawnedMenuWidget = nullptr;

    /** Should be filled in K2_MapButtons before user interaction with the widget. */
    UPROPERTY(BlueprintReadWrite, Category="MTD|Generic Menu", meta=(AllowPrivateAccess="true"))
    TMap<TObjectPtr<UCommonButtonBase>, TSubclassOf<UUserWidget>> ButtonsMapping;

    UPROPERTY(BlueprintReadOnly, Category="MTD|Generic Menu", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UCommonButtonBase> SelectedButton = nullptr;

    /** Widget used by ComputePadding. Should be a single widget determening total size of this menu widget. */
    UPROPERTY(BlueprintReadWrite, Category="MTD|Generic Menu", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UWidget> SizeWidget = nullptr;
};
