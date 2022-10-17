#pragma once

#include "Character/MTD_PawnComponent.h"
#include "mtd.h"

#include "MTD_HeroComponent.generated.h"

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
	
	void Input_Move(const FInputActionValue &InputActionValue);
	void Input_LookMouse(const FInputActionValue &InputActionValue);
	
private:
	UPROPERTY()
	TObjectPtr<const UMTD_PawnData> PawnData = nullptr;
	
	bool bPawnHasInitialized = false;
};
