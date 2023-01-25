#include "UI/MTD_GenericMenu.h"

void UMTD_GenericMenu::NativeConstruct()
{
    Super::NativeConstruct();
    K2_MapButtons();
}

void UMTD_GenericMenu::NativeDestruct()
{
    Super::NativeDestruct();
    DeleteWidget();
}

void UMTD_GenericMenu::CreateWidget(TSubclassOf<UUserWidget> UserWidgetClass)
{
    if (!UserWidgetClass)
    {
        MTDS_WARN("User Widget Class is NULL.");
        return;
    }

    // Try to erase previous widget
    DeleteWidget();

    SpawnedMenuWidget = CreateWidgetInstance(*this, UserWidgetClass, FName(""));
    SpawnedMenuWidget->SetPadding(ComputePadding());
    SpawnedMenuWidget->AddToViewport();
}

void UMTD_GenericMenu::DeleteWidget()
{
    if (IsValid(SpawnedMenuWidget))
    {
        SpawnedMenuWidget->RemoveFromParent();
        SpawnedMenuWidget->MarkAsGarbage();
        SpawnedMenuWidget = nullptr;
    }
}

void UMTD_GenericMenu::MenuButtonPress(UCommonButtonBase *Button, bool bSelected)
{
    if (!IsValid(Button))
    {
        MTDS_WARN("Button is invalid.");
        return;
    }

    if (IsValid(SelectedButton))
    {
        // After SetIsSelected will change the button state, it will call this function and *may* get to this point as
        // well, performing some logic which it should not, thus the SelectedButton is saved in a local variable and
        // nulled afterwards
        UCommonButtonBase *LocalSelectedButton = SelectedButton;
        SelectedButton = nullptr;
        
        LocalSelectedButton->SetIsSelected(false);
    }

    if (bSelected)
    {
        // Save it before checks
        SelectedButton = Button;
        
        const auto FoundUserWidgetClass = ButtonsMapping.Find(Button);
        if (!FoundUserWidgetClass)
        {
            MTDS_WARN("Could not found User Widget Class for button [%s].", *Button->GetName());
            return;
        }

        const auto UserWidgetClass = *FoundUserWidgetClass;
        if (!UserWidgetClass)
        {
            MTDS_WARN("User Widget Class for button [%s] is NULL.", *Button->GetName());
            return;
        }
        
        CreateWidget(UserWidgetClass);
    }
    else
    {
        DeleteWidget();
    }
}

FMargin UMTD_GenericMenu::ComputePadding() const
{
    FMargin Margin;
    if (!IsValid(SizeWidget))
    {
        MTDS_WARN("Size Widget is invalid.");
        return Margin;
    }

    const FVector2D Size = SizeWidget->GetDesiredSize();
    Margin = Padding;
    Margin.Left += Size.X;

    return Margin;
}
