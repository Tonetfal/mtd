#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "mtd.h"

#include "MTD_AttackNotifyState.generated.h"

UCLASS()
class MTD_API UMTD_AttackNotifyState
    : public UAnimNotifyState
{
	GENERATED_BODY()

public:
    //~UAnimNotifyState Interface
    virtual void NotifyBegin(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation,
        float TotalDuration) override;
    virtual void NotifyEnd(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation) override;
    //~End of UAnimNotifyState Interface

private:
    /** Unique melee hitbox nicknames. */
    UPROPERTY(EditAnywhere, Category="MTD|Combat System")
    TArray<FName> MeleeHitboxNicknames;
};
