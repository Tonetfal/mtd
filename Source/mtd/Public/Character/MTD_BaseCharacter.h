#pragma once

#include "AbilitySystem/MTD_AbilityAnimationSet.h"
#include "AbilitySystemInterface.h"
#include "Character/MTD_GameResultInterface.h"
#include "CombatSystem/MTD_MeleeEventsInterface.h"
#include "CombatSystem/MTD_MeleeHitboxData.h"
#include "GameFramework/Character.h"
#include "mtd.h"

#include "MTD_BaseCharacter.generated.h"

class AMTD_PlayerState;
class UMTD_AbilitySystemComponent;
class UMTD_HealthComponent;
class UMTD_HeroComponent;
class UMTD_ManaComponent;
class UMTD_PawnExtensionComponent;

UCLASS()
class MTD_API AMTD_BaseCharacter : public ACharacter, public IAbilitySystemInterface, public IMTD_GameResultInterface,
    public IMTD_MeleeCharacterInterface
{
    GENERATED_BODY()

public:
    AMTD_BaseCharacter();

    //~AActor interface
    virtual void PreInitializeComponents() override;
    virtual void PostInitProperties() override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void Reset() override;
    
    virtual void Tick(float DeltaSeconds) override;
    //~End of AActor interface

    //~APawn interface
    virtual void NotifyControllerChanged() override;
    //~End of APawn interface

    FMTD_AbilityAnimations GetAbilityAnimMontages(FGameplayTag AbilityTag) const;

    //~IMTD_MeleeCharacterInterface
    virtual void AddMeleeHitboxes(const TArray<FName> &HitboxNicknames) override;
    virtual void RemoveMeleeHitboxes(const TArray<FName> &HitboxNicknames) override;

    virtual void DisableMeleeHitboxes() override;
    virtual void ResetMeleeHitTargets() override;
    //~End of IMTD_MeleeCharacterInterface

protected:
    virtual void OnAbilitySystemInitialized();
    virtual void OnAbilitySystemUninitialized();
    virtual void DestroyDueToDeath();
    virtual void Uninit();

    UFUNCTION(BlueprintNativeEvent, Category="Character")
    void OnDeathStarted(AActor *OwningActor);
    virtual void OnDeathStarted_Implementation(AActor *OwningActor);

    UFUNCTION(BlueprintNativeEvent, Category="Character")
    void OnDeathFinished(AActor *OwningActor);
    virtual void OnDeathFinished_Implementation(AActor *OwningActor);

    virtual void DisableControllerInput();
    virtual void DisableMovement();
    virtual void DisableCollision();

    //~APlayer interface
    virtual void SetupPlayerInputComponent(UInputComponent *PlayerInputComponent) override;
    //~End of APlayer interface

    void ConstructHitboxMap();
    void PerformHitboxTrace();

    virtual void FellOutOfWorld(const UDamageType &DamageType) override;

public:
    UMTD_HealthComponent *GetHealthComponent() const;
    UMTD_ManaComponent *GetManaComponent() const;

    UFUNCTION(BlueprintCallable, Category="MTD|Character")
    AMTD_PlayerState *GetMtdPlayerState() const;

    UFUNCTION(BlueprintCallable, Category="MTD|Character")
    UMTD_AbilitySystemComponent *GetMtdAbilitySystemComponent() const;

    //~IAbilitySystemInterface interface
    virtual UAbilitySystemComponent *GetAbilitySystemComponent() const override;
    //~End IAbilitySystemInterface interface

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_PawnExtensionComponent> PawnExtentionComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_HeroComponent> HeroComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_HealthComponent> HealthComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_ManaComponent> ManaComponent = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Ability System",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<const UMTD_AbilityAnimationSet> AnimationSet = nullptr;

    /** Data Asset defining all hitbox data. */
    UPROPERTY(EditAnywhere, Category="MTD|Combat System")
    TArray<TObjectPtr<UMTD_MeleeHitboxData>> HitboxData;

    /** Array that stores all object types that will be used as trace channels in Combat System. */
    UPROPERTY(EditAnywhere, Category="MTD|Combat System")
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesToHit;

    /** Gameplay tag that will be used to identify the hit event. */
    UPROPERTY(EditAnywhere, Category="MTD|Combat System")
    FGameplayTag TagToFireOnHit = FGameplayTag::EmptyTag;

    /** Draw hitboxes? */
    UPROPERTY(EditAnywhere, Category="MTD|Combat System|Debug")
    bool bDebugMelee = false;
    
    UPROPERTY(EditAnywhere, Category="MTD|Combat System|Debug")
    float DrawTime = 0.1f;

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

inline UMTD_HealthComponent *AMTD_BaseCharacter::GetHealthComponent() const
{
    return HealthComponent;
}

inline UMTD_ManaComponent *AMTD_BaseCharacter::GetManaComponent() const
{
    return ManaComponent;
}
