#pragma once

#include "AbilitySystemInterface.h"
#include "Character/MTD_GameResultInterface.h"
#include "CombatSystem/MTD_MeleeEventsInterface.h"
#include "CombatSystem/MTD_MeleeHitboxData.h"
#include "GameFramework/Character.h"
#include "mtd.h"

#include "MTD_BaseCharacter.generated.h"

class AMTD_PlayerState;
class UMTD_AbilitySystemComponent;
class UMTD_BalanceComponent;
class UMTD_HealthComponent;
class UMTD_HeroComponent;
class UMTD_ManaComponent;
class UMTD_PawnExtensionComponent;

UCLASS()
class MTD_API AMTD_BaseCharacter :
    public ACharacter, 
    public IAbilitySystemInterface,
    public IMTD_GameResultInterface,
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

    //~IMTD_MeleeCharacterInterface
    virtual void AddMeleeHitboxes(const TArray<FName> &HitboxNicknames) override;
    virtual void RemoveMeleeHitboxes(const TArray<FName> &HitboxNicknames) override;

    virtual void DisableMeleeHitboxes() override;
    virtual void ResetMeleeHitTargets() override;
    //~End of IMTD_MeleeCharacterInterface

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Character")
    float GetMovementDirectionAngle() const;

protected:
    virtual void OnAbilitySystemInitialized();
    virtual void OnAbilitySystemUninitialized();
    virtual void DestroyDueToDeath();
    virtual void Uninit();

    UFUNCTION(BlueprintNativeEvent, Category="MTD|Character")
    void OnDeathStarted(AActor *OwningActor);
    virtual void OnDeathStarted_Implementation(AActor *OwningActor);

    UFUNCTION(BlueprintNativeEvent, Category="MTD|Character")
    void OnDeathFinished(AActor *OwningActor);
    virtual void OnDeathFinished_Implementation(AActor *OwningActor);

    //~ACharacter interface
    virtual void SetupPlayerInputComponent(UInputComponent *PlayerInputComponent) override;
    //~End of ACharacter interface

    void ConstructHitboxMap();
    void PerformHitboxTrace();
    void PerformHit(const FHitResult &Hit);

    virtual void InitializeAttributes();

    virtual void FellOutOfWorld(const UDamageType &DamageType) override;

public:
    UMTD_HealthComponent *GetHealthComponent() const;
    UMTD_ManaComponent *GetManaComponent() const;
    UMTD_BalanceComponent *GetBalanceComponent() const;

    UFUNCTION(BlueprintCallable, Category="MTD|Character")
    AMTD_PlayerState *GetMtdPlayerState() const;

    UFUNCTION(BlueprintCallable, Category="MTD|Character")
    UMTD_AbilitySystemComponent *GetMtdAbilitySystemComponent() const;

    //~IAbilitySystemInterface interface
    virtual UAbilitySystemComponent *GetAbilitySystemComponent() const override;
    //~End IAbilitySystemInterface interface

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_PawnExtensionComponent> PawnExtentionComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_HeroComponent> HeroComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_HealthComponent> HealthComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_ManaComponent> ManaComponent = nullptr;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_BalanceComponent> BalanceComponent = nullptr;

    /** Data Asset defining all hitbox data. */
    UPROPERTY(EditAnywhere, Category="MTD|Combat System")
    TArray<TObjectPtr<UMTD_MeleeHitboxData>> HitboxData;

    /** Array that stores all object types that will be used as trace channels in Combat System. */
    UPROPERTY(EditAnywhere, Category="MTD|Combat System")
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesToHit;

    /** Draw hitboxes? */
    UPROPERTY(EditAnywhere, Category="MTD|Combat System|Debug")
    bool bDebugMelee = false;

    /** Seconds the hitboxes will be drawn for. */
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

inline UMTD_BalanceComponent *AMTD_BaseCharacter::GetBalanceComponent() const
{
    return BalanceComponent;
}
