#pragma once

#include "Character/MTD_PawnComponent.h"
#include "CombatSystem/MTD_MeleeEventsInterface.h"
#include "CombatSystem/MTD_MeleeHitboxData.h"
#include "mtd.h"

#include "MTD_CombatComponent.generated.h"

class AMTD_BaseCharacter;

UCLASS(Blueprintable, ClassGroup="Character", meta=(BlueprintSpawnableComponent))
class MTD_API UMTD_CombatComponent
    : public UActorComponent
    , public IMTD_MeleeCombatInterface
{
    GENERATED_BODY()

public:
    UMTD_CombatComponent();

    UFUNCTION(BlueprintCallable, Category="MTD|Combat Component")
    static UMTD_CombatComponent *FindCombatComponent(const AActor *Actor);

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
    virtual void AddMeleeHitboxes(const TArray<FName> &HitboxNicknames) override;
    virtual void RemoveMeleeHitboxes(const TArray<FName> &HitboxNicknames) override;

    virtual void DisableMeleeHitboxes() override;
    virtual void ResetMeleeHitTargets() override;
    //~End of IMTD_MeleeCharacterInterface

private:
    void ConstructHitboxMap();
    void PerformHitboxCheck();
    void PerformHit(const FHitResult &Hit);

private:
    /** Cached owner as MTD Base Character. */
    UPROPERTY()
    TObjectPtr<AMTD_BaseCharacter> PrivateCharacterOwner = nullptr;
    
    /** Data Asset defining all hitbox data. */
    UPROPERTY(EditAnywhere, Category="MTD|Combat Component")
    TArray<TObjectPtr<UMTD_MeleeHitboxData>> HitboxData;

    /** Array that stores all object types that will be searched for when performing sweeps for hits. */
    UPROPERTY(EditAnywhere, Category="MTD|Combat Component")
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesToHit;

    /** Draw hitboxes? */
    UPROPERTY(EditAnywhere, Category="MTD|Combat Component|Debug")
    bool bDebugMelee = false;

    /** Time in seconds the hitboxes will be drawn for. */
    UPROPERTY(EditAnywhere, Category="MTD|Combat Component|Debug", meta=(EditCondition="bDebugMelee"))
    float SweepDrawTime = 0.1f;

    /** Targets that has been hit with active attack, and that will be ignored until the attack ends. */
    UPROPERTY()
    TArray<TObjectPtr<AActor>> MeleeHitTargets;

    /** Dispatched Hitbox Data. */
    TMap<FName, FMTD_ActiveHitboxEntry> HitboxMap;

    /** Array of active hitbox definitions that are used to perform trace on tick. */
    TArray<FMTD_MeleeHitSphereDefinition> ActiveHitboxes;

    /** Should try to perform any combat system trace? */
    bool bIsMeleeInProgress = false;
};

inline UMTD_CombatComponent *UMTD_CombatComponent::FindCombatComponent(const AActor *Actor)
{
    return ((IsValid(Actor)) ? (Actor->FindComponentByClass<UMTD_CombatComponent>()) : (nullptr));
}
