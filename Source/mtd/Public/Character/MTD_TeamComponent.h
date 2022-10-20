#pragma once

#include "Components/ActorComponent.h"
#include "mtd.h"
#include "MTD_CoreTypes.h"

#include "MTD_TeamComponent.generated.h"

UCLASS(meta=(BlueprintSpawnableComponent))
class MTD_API UMTD_TeamComponent : public UActorComponent,
	public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	UMTD_TeamComponent();

	void SetMtdTeamId(EMTD_TeamId Id);
	EMTD_TeamId GetMtdTeamId() const;
	
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual void SetGenericTeamId(const FGenericTeamId &Id) override;

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(
		const AActor &Other) const override;
	
protected:
	//~UActorComponent interface
	virtual void BeginPlay() override;
	//~End of UActorComponent interface

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD Team Component",
		meta=(AllowPrivateAccess="true"))
	EMTD_TeamId TeamId = EMTD_TeamId::Unknown;
};
