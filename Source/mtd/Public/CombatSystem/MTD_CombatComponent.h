#pragma once

#include "Character/Components/MTD_PawnComponent.h"
#include "CombatSystem/MTD_MeleeEventsInterface.h"
#include "CombatSystem/MTD_HitboxData.h"
#include "mtd.h"

#include "MTD_CombatComponent.generated.h"

class AMTD_BaseCharacter;

/**
 * Combat component that handles collision related logic for melee combat.
 */
UCLASS(Blueprintable, ClassGroup="Character", meta=(BlueprintSpawnableComponent))
class MTD_API UMTD_CombatComponent
    : public UActorComponent
    , public IMTD_MeleeCombatInterface
{
    GENERATED_BODY()

public:
    UMTD_CombatComponent();

    /**
     * Find combat component on a given actor.
     * @param   Actor: actor to search for combat component in.
     * @result  Combat component residing on the given actor. Can be nullptr.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Combat Component")
    static UMTD_CombatComponent *FindCombatComponent(const AActor *Actor);

    /**
     * Add an object type to check collision against.
     * @param   InObjectType: object type to add.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Combat Component")
    void AddObjectTypeToHit(EObjectTypeQuery InObjectType);

    //~AActor Interface
    virtual void TickComponent(float DeltaSeconds, ELevelTick TickType,
        FActorComponentTickFunction *ThisTickFunction) override;
    virtual void InitializeComponent() override;

protected:
    virtual void BeginPlay() override;
    //~End of AActor Interface

public:
    //~IMTD_MeleeCharacterInterface
    virtual void ActivateHitboxes(const TArray<FName> &HitboxNicknames) override;
    virtual void DeactivateHitboxes(const TArray<FName> &HitboxNicknames) override;

    virtual void DeactivateAllHitboxes() override;
    virtual void ResetHitTargets() override;
    //~End of IMTD_MeleeCharacterInterface

private:
    /**
     * Dispatch hitbox data, and create a hitbox map containing all activatable hitboxes.
     */
    void ConstructHitboxMap();

    /**
     * Sweep for targets using the active hitboxes.
     */
    void PerformHitboxCheck();

    /**
     * Fire a gameplay event containing data about the hit.
     */
    void PerformHit(const FHitResult &Hit);

private:
    /** Cached owner as MTD Base Character. */
    UPROPERTY()
    TObjectPtr<AMTD_BaseCharacter> PrivateCharacterOwner = nullptr;
    
    /** Set of hitboxes to use. */
    UPROPERTY(EditAnywhere, Category="MTD|Combat Component")
    TArray<TObjectPtr<UMTD_HitboxData>> HitboxData;

    /** Array that stores all object types that will be searched for when performing sweeps for hits. */
    UPROPERTY(EditAnywhere, Category="MTD|Combat Component")
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesToHit;

    /** Draw hitboxes? */
    UPROPERTY(EditAnywhere, Category="MTD|Combat Component|Debug")
    bool bDebugHitboxes = false;

    /** Time in seconds the hitboxes will be drawn for. */
    UPROPERTY(EditAnywhere, Category="MTD|Combat Component|Debug", meta=(EditCondition="bDebugMelee"))
    float SweepDrawTime = 0.1f;

    /** Targets that has been hit with active attack, and that will be ignored until the attack ends. */
    UPROPERTY()
    TArray<TObjectPtr<AActor>> HitTargets;

    /** Dispatched hitbox data. */
    TMap<FName, FMTD_HitboxDefinition> HitboxMap;

    /** Array of active hitbox definitions that are used to perform trace on tick. */
    TArray<FMTD_HitboxDefinition> ActiveHitboxes;

    /** Should try to perform any combat system trace? */
    bool bIsAttackInProgress = false;
};

inline UMTD_CombatComponent *UMTD_CombatComponent::FindCombatComponent(const AActor *Actor)
{
    return ((IsValid(Actor)) ? (Actor->FindComponentByClass<UMTD_CombatComponent>()) : (nullptr));
}
