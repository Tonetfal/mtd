#pragma once

#include "Character/MTD_PawnComponent.h"
#include "mtd.h"

#include "MTD_HeroComponent.generated.h"

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class MTD_API UMTD_HeroComponent : public UMTD_PawnComponent
{
	GENERATED_BODY()

public:
	UMTD_HeroComponent();

	//~UActorComponent interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent interface

	void OnPawnReadyToInitialize();

protected:
	virtual bool IsPawnComponentReadyToInitialize() const override;
	virtual void OnRegister() override;

private:
	bool bPawnHasInitialized = false;
};
