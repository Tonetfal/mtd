#pragma once

#include "GameFramework/Actor.h"
#include "mtd.h"
#include "MTD_Interactable.h"

#include "MTD_InteractActor.generated.h"

UCLASS()
class MTD_API AMTD_InteractActor
    : public AActor
    , public IMTD_Interactable
{
    GENERATED_BODY()

public:
    AMTD_InteractActor();

    //~IMTD_Interactable Interface
    virtual bool TryInteract_Implementation(AMTD_BaseCharacter *Character, const FName InteractionID) override;
    //~End of IMTD_Interactable Interface
};
