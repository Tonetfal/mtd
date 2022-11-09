#pragma once

#include "mtd.h"
#include "MTD_PawnComponent.h"

#include "MTD_PawnExtensionComponent.generated.h"

class UMTD_PawnData;
class UMTD_AbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynamicMulticastSignature);

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class MTD_API UMTD_PawnExtensionComponent : public UMTD_PawnComponent
{
	GENERATED_BODY()

public:
	UMTD_PawnExtensionComponent();
	
	UFUNCTION(BlueprintPure, Category = "MTD|Pawn")
	static UMTD_PawnExtensionComponent *FindPawnExtensionComponent(
		const AActor *Actor)
	{
		return IsValid(Actor) ? Actor->FindComponentByClass<
			UMTD_PawnExtensionComponent>() : nullptr;
	}
	
	template <class T>
	const T *GetPawnData() const
		{ return Cast<T>(PawnData); }

	void SetPawnData(const UMTD_PawnData *InPawnData);

	UFUNCTION(BlueprintPure, Category = "MTD|Pawn")
	UMTD_AbilitySystemComponent *GetMtdAbilitySystemComponent() const
		{ return AbilitySystemComponent; }
	
	void InitializeAbilitySystem(
		UMTD_AbilitySystemComponent *InAsc, AActor *InOwnerActor);
	void UninitializeAbilitySystem();
	
	void HandleControllerChanged();
	void SetupPlayerInputComponent();

	bool CheckPawnReadyToInitialize();
	
	UFUNCTION(BlueprintCallable, Category="MTD|Pawn", meta=(ExpandBoolAsExecs="ReturnValue"))
	bool IsPawnReadyToInitialize() const
		{ return bPawnReadyToInitialize; }

	void OnPawnReadyToInitialize_RegisterAndCall(
		FSimpleMulticastDelegate::FDelegate Delegate);
	void OnAbilitySystemInitialized_RegisterAndCall(
		FSimpleMulticastDelegate::FDelegate Delegate);
	void OnAbilitySystemUninitialized_Register(
		FSimpleMulticastDelegate::FDelegate Delegate);

protected:
	virtual void OnRegister() override;

protected:
	FSimpleMulticastDelegate OnPawnReadyToInitialize;
	
	UPROPERTY(BlueprintAssignable, meta=(DisplayName="On Pawn Ready To Initialize"))
	FDynamicMulticastSignature BP_OnPawnReadyToInitialize;
	
	FSimpleMulticastDelegate OnAbilitySystemInitialized;
	FSimpleMulticastDelegate OnAbilitySystemUninitialized;

private:
	UPROPERTY()
	TObjectPtr<UMTD_AbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MTD|Pawn",
		meta=(AllowPrivateAccess="true"))
	TObjectPtr<const UMTD_PawnData> PawnData = nullptr;

	bool bPawnReadyToInitialize = false;
};
