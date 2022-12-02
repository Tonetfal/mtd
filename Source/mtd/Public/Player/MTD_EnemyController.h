#pragma once

#include "AIController.h"
#include "Character/MTD_TeamComponent.h"
#include "mtd.h"

#include "MTD_EnemyController.generated.h"

class UMovementComponent;

UCLASS()
class MTD_API AMTD_EnemyController : public AAIController
{
    GENERATED_BODY()

public:
    AMTD_EnemyController();
    virtual void Tick(float DeltaSeconds) override;

    virtual FGenericTeamId GetGenericTeamId() const override
    {
        return Team->GetGenericTeamId();
    }

protected:
    virtual void OnPossess(APawn *InPawn) override;

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Components",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_TeamComponent> Team = nullptr;

    UPROPERTY()
    TObjectPtr<UMovementComponent> OwnerMovementComponent = nullptr;
};
