#pragma once

#include "mtd.h"
#include "Blueprint/UserWidget.h"
#include "Core/MTD_CoreTypes.h"

#include "MTD_PlayerHudWidget.generated.h"

USTRUCT(BlueprintType)
struct FMTD_TraceData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TEnumAsByte<ETeamAttitude::Type> AttitudeTowardsHittingObject;
	
	UPROPERTY(BlueprintReadWrite)
	float HitDistanceRatio;
};

UCLASS()
class MTD_API UMTD_PlayerHudWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual bool Initialize() override;

	UFUNCTION(BlueprintCallable, Category="MTD UI")
	float GetHealthRatio() const;
	
	UFUNCTION(BlueprintCallable, Category="MTD UI")
	bool IsPlayerAlive() const;
	
	UFUNCTION(BlueprintCallable, Category="MTD UI")
	bool IsPlayerSpectating() const;
	
	UFUNCTION(BlueprintCallable, Category="MTD UI")
	FMTD_TraceData GetTraceData() const;
	
	UFUNCTION(BlueprintImplementableEvent, Category="MTD UI")
	void OnTakeDamage();

private:
	void OnHealthChanged(int32 NewHealth, int32 HealthDelta);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Player HUD Widget",
		meta=(AllowPrivateAccess="true", ClampMin = "0.1"))
	float MaxScaleDownLength = 500.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Player HUD Widget",
		meta=(AllowPrivateAccess="true", ClampMin = "0.1"))
	float TraceLineLength = 1000.f;
};
