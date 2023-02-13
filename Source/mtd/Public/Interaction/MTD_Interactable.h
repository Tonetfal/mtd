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

/**
 * Interactable object interface.
 *
 * It's intended to have different interaction identifiers to differently interact with an object.
 *
 * For instance, a dropped item can be picked up, as well as equipped. There are two different interaction IDs for that.
 *
 * Main public methods are:
 * - CanInteract
 * - TryInteract
 *
 * Main protected methods are:
 * - PreInteract
 * - Interact
 * - PostInteract
 */
class MTD_API IMTD_Interactable
{
    GENERATED_BODY()

public:
    inline static FName DefaultInteractionID = "DefaultInteraction";

public:
    /**
     * Check whether the given character can interact with object.
     * @param   Character: character that is trying to interact with object.
     * @param   InteractionID: interaction identifier.
     * @return  If true, the character can interact with us, false otherwise.
     */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool CanInteract(AMTD_BaseCharacter *Character, const FName InteractionID = "DefaultInteraction");
    virtual bool CanInteract_Implementation(AMTD_BaseCharacter *Character,
        const FName InteractionID = "DefaultInteraction")
    {
        return true;
    }

    /**
     * Try to interact with object.
     * @param   Character: character that is trying to interact.
     * @param   InteractionID: interaction identifier.
     * @return  If true, the object has been successfully interacted with, false otherwise.
     */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool TryInteract(AMTD_BaseCharacter *Character, const FName InteractionID = "DefaultInteraction");
    virtual bool TryInteract_Implementation(AMTD_BaseCharacter *Character,
        const FName InteractionID = "DefaultInteraction")
    {
        return true;
    }

protected:
    /**
     * Pre interaction custom code.
     * @param   Character: character that is about to interact with us.
     * @param   InteractionID: interaction identifier.
     */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void PreInteract(AMTD_BaseCharacter *Character, const FName InteractionID);
    virtual void PreInteract_Implementation(AMTD_BaseCharacter *Character, const FName InteractionID)
    {
        // Empty
    }
    
    /**
     * Interaction process.
     * @param   Character: character that is interacting with us.
     * @param   InteractionID: interaction identifier.
     * @return  If true, the character has successfully interacted with us, false otherwise.
     */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool Interact(AMTD_BaseCharacter *Character, const FName InteractionID);
    virtual bool Interact_Implementation(AMTD_BaseCharacter *Character, const FName InteractionID)
    {
        return true;
    }
    
    /**
     * Post interaction custom code.
     * @param   Character: character that has interacted with us.
     * @param   InteractionID: interaction identifier.
     */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void PostInteract(AMTD_BaseCharacter *Character, const FName InteractionID);
    virtual void PostInteract_Implementation(AMTD_BaseCharacter *Character, const FName InteractionID)
    {
        // Empty
    }
};
