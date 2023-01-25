#pragma once

#include "mtd.h"
#include "UObject/Interface.h"

#include "MTD_Interactable.generated.h"

class AMTD_BaseCharacter;

UINTERFACE(BlueprintType)
class UMTD_Interactable
    : public UInterface
{
    GENERATED_BODY()
};

class MTD_API IMTD_Interactable
{
    GENERATED_BODY()

public:
    inline static FName DefaultInteractionID = "DefaultInteraction";

public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool CanInteract(AMTD_BaseCharacter *Character, const FName InteractionID = "DefaultInteraction");
    virtual bool CanInteract_Implementation(AMTD_BaseCharacter *Character,
        const FName InteractionID = "DefaultInteraction")
    {
        return true;
    }

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool TryInteract(AMTD_BaseCharacter *Character, const FName InteractionID = "DefaultInteraction");
    virtual bool TryInteract_Implementation(AMTD_BaseCharacter *Character,
        const FName InteractionID = "DefaultInteraction")
    {
        return true;
    }

protected:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool Interact(AMTD_BaseCharacter *Character, const FName InteractionID);
    virtual bool Interact_Implementation(AMTD_BaseCharacter *Character, const FName InteractionID)
    {
        return true;
    }

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void PostInteract(AMTD_BaseCharacter *Character, const FName InteractionID);
    virtual void PostInteract_Implementation(AMTD_BaseCharacter *Character, const FName InteractionID)
    {
        // Empty
    }
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void PreInteract(AMTD_BaseCharacter *Character, const FName InteractionID);
    virtual void PreInteract_Implementation(AMTD_BaseCharacter *Character, const FName InteractionID)
    {
        // Empty
    }
};
