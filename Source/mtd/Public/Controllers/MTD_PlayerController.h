#pragma once

#include "mtd.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/PlayerController.h"

#include "Components/MTD_TeamComponent.h"

#include "MTD_PlayerController.generated.h"

UCLASS()
class MTD_API AMTD_PlayerController : public APlayerController,
	public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AMTD_PlayerController();
	
protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn *InPawn) override;
	
public:
	virtual FGenericTeamId GetGenericTeamId() const override
		{ return Team->GetGenericTeamId(); }

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD Components",
		meta=(AllowPrivateAccess="true"))
	TObjectPtr<UMTD_TeamComponent> Team = nullptr;
};
