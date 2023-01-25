﻿#include "Interaction/MTD_InteractActor.h"

AMTD_InteractActor::AMTD_InteractActor()
{
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;
}

bool AMTD_InteractActor::TryInteract_Implementation(AMTD_BaseCharacter *Character, const FName InteractionID)
{
    if (!Execute_CanInteract(this, Character, InteractionID))
    {
        return false;
    }

    Execute_PreInteract(this, Character, InteractionID);
    const bool bResult = Execute_Interact(this, Character, InteractionID);
    Execute_PostInteract(this, Character, InteractionID);

    return bResult;
}
