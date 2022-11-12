#pragma once

#include "AIController.h"
#include "Character/MTD_TeamComponent.h"
#include "mtd.h"

#include "MTD_EnemyController.generated.h"

UCLASS()
class MTD_API AMTD_EnemyController : public AAIController
{
    GENERATED_BODY()

public:
    AMTD_EnemyController();

    virtual FPathFollowingRequestResult MoveTo(const FAIMoveRequest &MoveRequest, FNavPathSharedPtr *OutPath) override;

    virtual FGenericTeamId GetGenericTeamId() const override
    {
        return Team->GetGenericTeamId();
    }

protected:
    virtual void BeginPlay() override;

private:
    void OnMoveFinished(FAIRequestID RequestID, const FPathFollowingResult &Result);

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Components",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_TeamComponent> Team = nullptr;

    UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
    bool bIsMoving = false;
};
