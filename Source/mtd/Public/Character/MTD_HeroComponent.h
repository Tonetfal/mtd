#pragma once

#include "Character/MTD_PawnComponent.h"
#include "mtd.h"

#include "MTD_HeroComponent.generated.h"

class UMTD_InputConfig;
class UMTD_AbilitySystemComponent;
class UPlayerMappableInputConfig;
class UMTD_PawnData;
struct FInputActionValue;
struct FGameplayTag;

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class MTD_API UMTD_HeroComponent : public UMTD_PawnComponent
{
    GENERATED_BODY()

public:
    UMTD_HeroComponent();

    UFUNCTION(BlueprintCallable, Category="MTD|Input")
    void AddAdditionalInputConfig(const UMTD_InputConfig *InputConfig);

protected:
    virtual void OnRegister() override;

    //~UActorComponent interface
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    //~End of UActorComponent interface

    virtual bool IsPawnComponentReadyToInitialize() const override;
    void OnPawnReadyToInitialize();

    virtual void InitializePlayerInput(UInputComponent *InputComponent);

    void Input_AbilityInputTagPressed(FGameplayTag InputTag);
    void Input_AbilityInputTagReleased(FGameplayTag InputTag);

    UFUNCTION(BlueprintImplementableEvent, Category="MTD|Input",
        DisplayName="Can Move")
    bool K2_CanMove();

    UFUNCTION(BlueprintImplementableEvent, Category="MTD|Input",
        DisplayName="Can Look")
    bool K2_CanLook();

    void Input_Move(const FInputActionValue &InputActionValue);
    void Input_LookMouse(const FInputActionValue &InputActionValue);

    UFUNCTION(BlueprintCallable, Category="MTD|Hero Component")
    UMTD_AbilitySystemComponent *GetMtdAbilitySystemComponent() const;

private:
    UPROPERTY()
    TObjectPtr<const UMTD_PawnData> PawnData = nullptr;

    bool bPawnHasInitialized = false;
};
