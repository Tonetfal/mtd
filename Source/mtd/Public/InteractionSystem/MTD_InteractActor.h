#pragma once

#include "GameFramework/Actor.h"
#include "InteractionSystem/MTD_Interactable.h"
#include "mtd.h"

#include "MTD_InteractActor.generated.h"

/**
 * Simple interaction actor that overrides TryInteract method.
 */
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
