#pragma once

#include "mtd.h"
#include "Animation/AnimNotifies/AnimNotify.h"

#include "MTD_BaseAnimNotify.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnNotifiedSignature,
	USkeletalMeshComponent*)

UCLASS()
class MTD_API UMTD_BaseAnimNotify : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(
		USkeletalMeshComponent *MeshComp,
		UAnimSequenceBase *Animation,
		const FAnimNotifyEventReference &EventReference) override;

public:
	FOnNotifiedSignature OnNotifiedDelegate;
};
