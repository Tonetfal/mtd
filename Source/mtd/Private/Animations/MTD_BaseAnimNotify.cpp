#include "Animations/MTD_BaseAnimNotify.h"

void UMTD_BaseAnimNotify::Notify(
	USkeletalMeshComponent *MeshComp,
	UAnimSequenceBase *Animation,
	const FAnimNotifyEventReference &EventReference)
{
	OnNotifiedDelegate.Broadcast(MeshComp);
	Super::Notify(MeshComp, Animation, EventReference);
}
