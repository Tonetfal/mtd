#pragma once

#include "AbilitySystemInterface.h"
#include "Character/MTD_GameResultInterface.h"
#include "GameFramework/Pawn.h"
#include "mtd.h"

#include "MTD_Tower.generated.h"

class AMTD_PlayerState;
class AMTD_Projectile;
class AMTD_TowerController;
class UBoxComponent;
class UMTD_AbilitySystemComponent;
class UMTD_BuilderSet;
class UMTD_GameplayEffect;
class UMTD_HealthComponent;
class UMTD_HeroComponent;
class UMTD_PawnExtensionComponent;
class UMTD_TowerData;
class USphereComponent;
struct FOnAttributeChangeData;

/**
 * Tower pawn that fires projectiles at its enemies.
 *
 * A tower can be a part of a team, and depending on it, it'll fire projectiles on different targets. However, team
 * related code is handled inside its specific AI controller.
 *
 * It makes use of gameplay ability system to handle different gameplay actions.
 *
 * Must be used along with AMTD_TowerController or its derivates in order to be able to see enemies.
 *
 * @see AMTD_TowerController
 */
UCLASS()
class MTD_API AMTD_Tower
    : public APawn
    , public IAbilitySystemInterface
    , public IMTD_GameResultInterface
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynamicMulticastSignature);
    
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
        FOnLevelUpSignature,
        int32, NewLevel,
        int32, OldLevel);

public:
    AMTD_Tower();

private:
    /**
     * Initialize all attributes, and apply a dynamic health scaling.
     */
    void InitializeAttributes();
    
    /**
     * Read attribute table values given a new level.
     * @param   InLevel: level to read the attribute table with.
     */
    void ReadTableBaseValues(int32 InLevel);

    /**
     * Apply dynamic health scaling.
     */
    void ApplyTowerHealthScaling();

    /**
     * Restore health value at maximum.
     */
    void MaxHealthRestore();

protected:
    //~AActor interface
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaSeconds) override;
    //~End of AActor interface

    //~APawn interface
    virtual void NotifyControllerChanged() override;
    //~End of APawn interface

    /**
     * Get current level value.
     * @return  Current level value.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Tower")
    int32 GetCurrentLevel() const;
    
    /**
     * Add a delta to current level.
     * @param   InDeltaLevel: delta that has to be added to current level. Must be a positive number. May be clamped.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Tower")
    void AddLevel(int32 InDeltaLevel);

    //~IMTD_GameResultInterface Interface
    virtual void OnGameTerminated_Implementation(EMTD_GameResult GameResult) override;
    //~End of IMTD_GameResultInterface Interface

    /**
     * Get MTD player state.
     * @return  MTD player state.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Tower")
    AMTD_PlayerState *GetMtdPlayerState() const;

    /**
     * Get MTD ability system component.
     * @return  MTD ability system component.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Tower")
    UMTD_AbilitySystemComponent *GetMtdAbilitySystemComponent() const;

    //~IAbilitySystemInterface interface
    virtual UAbilitySystemComponent *GetAbilitySystemComponent() const override;
    //~End IAbilitySystemInterface interface

public:
    /**
     * Get scaled damage by builder attributes.
     * @return  Scaled damage.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="MTD|Tower Stats")
    float GetScaledDamage() const;
    float GetScaledFirerate_Implementation() const;

    /**
     * Get scaled firerate by builder attributes.
     * @return  Scaled firerate.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="MTD|Tower Stats")
    float GetScaledFirerate() const;
    float GetScaledDamage_Implementation() const;

    /**
     * Get scaled vision range by builder attributes.
     * @return  Scaled vision range.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="MTD|Tower Stats")
    float GetScaledVisionRange() const;
    float GetScaledVisionRange_Implementation() const;

    /**
     * Get scaled half degrees by builder attributes.
     * @return  Scaled half degrees.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="MTD|Tower Stats")
    float GetScaledVisionHalfDegrees() const;
    float GetScaledVisionHalfDegrees_Implementation() const;
    
    /**
     * Get scaled projectile speed by builder attributes.
     * @return  Scaled projectile speed.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="MTD|Tower Stats")
    float GetScaledProjectileSpeed() const;
    float GetScaledProjectileSpeed_Implementation() const;

    /**
     * Get scaled reload time by builder attributes.
     * @return  Scaled reload time.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="MTD|Tower Stats")
    float GetReloadTime() const;
    float GetReloadTime_Implementation() const;

protected:
    /**
     * Event to fire when there is a level up.
     * @param   NewLevel: new level the tower is on.
     * @param   OldLevel: old level the tower was on.
     */
    virtual void OnLevelUp(int32 NewLevel, int32 OldLevel);

    /**
     * Send gameplay event containing main data about leveling up.
     */
    void SendLevelUpEvent();

private:
    /**
     * Create and setup a projectile that will fly towards a given target.
     * @param   FireTarget: actor to launch projectile at.
     */
    void FireProjectile(AActor *FireTarget);

    /**
     * Spawn a deferred projectile actor.
     * @param   Transform: transform to spawn a projectile with.
     * @return  Spawned projectile. Might be nullptr.
     */
    AMTD_Projectile *SpawnProjectile(const FTransform &Transform);

    /**
     * Setup a newly created deferred projectile.
     * @param   Projectile: deferred projectile actor to setup.
     * @param   FireTarget: actor the projectile has to fly towards.
     */
    void SetupProjectile(AMTD_Projectile &Projectile, AActor *FireTarget);

    /**
     * Setup projectile collision channel.
     * @param   Projectile: deferred projectile actor to setup collision on.
     */
    void SetupProjectileCollision(AMTD_Projectile &Projectile) const;

    /**
     * Setup projectile movement component.
     * @param   Projectile: deferred projectile actor to setup movement component on.
     * @param   FireTarget: actor the projectile has to fly towards.
     */
    void SetupProjectileMovement(AMTD_Projectile &Projectile, AActor *FireTarget) const;
    
    /**
     * Setup projectile gameplay effects classes to grant on hit.
     * @param   Projectile: deferred projectile actor to setup gameplay effects classes on.
     */
    void SetupProjectileGameplayEffectClasses(AMTD_Projectile &Projectile) const;

private:
    /**
     * Start reloading. When reloading, no projectiles will be able to be fired.
     */
    void StartReloading();

    /**
     * Event to fire when reload finishes, the tower be able to fire projectiles once again.
     */
    void OnReloadFinished();

    /**
     * Start listening for game terminated event.
     */
    void ListenForGameTerminated();

    /**
     * Start listening for range related attributes changes on our instigator.
     */
    void ListenForRangeAttributeChanges();

    /**
     * Event to fire when range attribute has changed.
     */
    void OnRangeAttributeChanged(const FOnAttributeChangeData &Attribute);

    /**
     * Cache and validate instigator's ability system component.
     */
    void CacheInstigatorAbilitySystemComponent();

    /**
     * Check whether tower data is valid or not, before allowing the whole tower logic to run.
     * @return  If true, tower data is valid, false otherwise.
     */
    bool CheckTowerDataValidness() const;
    
private:
    
    /**
     * Event to fire when ability system component is initialized.
     */
    void OnAbilitySystemInitialized();
    
    /**
     * Event to fire when ability system component is initialized.
     */
    void OnAbilitySystemUninitialized();

protected:
    /**
     * Event to fire when our death has started.
     * @param   OwningActor: actor that has started dying.
     */
    UFUNCTION(BlueprintNativeEvent, Category="MTD|Tower")
    void OnDeathStarted(AActor *OwningActor);
    virtual void OnDeathStarted_Implementation(AActor *OwningActor);

    /**
     * Event to fire when our death has started.
     * @param   OwningActor: actor that has started dying.
     */
    UFUNCTION(BlueprintNativeEvent, Category="MTD|Tower")
    void OnDeathFinished(AActor *OwningActor);
    virtual void OnDeathFinished_Implementation(AActor *OwningActor);

private:
    /**
     * Disable all collisions.
     */
    virtual void DisableCollision();

    /**
     * Cause self destroy due out of bounds.
     * @param   DamageType: unused.
     */
    virtual void FellOutOfWorld(const UDamageType &DamageType) override;

    /**
     * Finish death chain logic.
     */
    void DestroyDueToDeath();

    /**
     * Uninitialize main data.
     */
    void Uninit();

public:
    /**
     * Delegate to fire when range attributes has changed.
     * Mainly required for vision sense to know when to update its parameters.
     */
    UPROPERTY(BlueprintAssignable)
    FDynamicMulticastSignature OnRangeAttributeChangedDelegate;

    /** Delegate to fire there was level a level up. */
    UPROPERTY(BlueprintAssignable)
    FOnLevelUpSignature OnLevelUpDelegate;

private:
    /** Controller as MTD tower controller. */
    UPROPERTY()
    TObjectPtr<AMTD_TowerController> TowerController = nullptr;

    /** Collision component to block other actors. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UBoxComponent> CollisionComponent = nullptr;

    /**  */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UBoxComponent> NavVolumeComponent = nullptr;

    /** Visual representation of this actor in world. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<USkeletalMeshComponent> MeshComponent = nullptr;

    /** Development purpose visual projectile spawn point representation. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<USphereComponent> ProjectileSpawnPosition = nullptr;

    /** Component to initialize this actor, and to store some additional data. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_PawnExtensionComponent> PawnExtentionComponent = nullptr;

    /** Component to initialize gameplay ability system of this actor. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_HeroComponent> HeroComponent = nullptr;

    /** Component to keep track of health and death. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_HealthComponent> HealthComponent = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Tower", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_TowerData> TowerData = nullptr;

    /** Current tower level value. */
    UPROPERTY(BlueprintReadWrite, Category="MTD|Tower", meta=(AllowPrivateAccess="true"))
    float CurrentLevel = 1.f;
    
    /** Max tower level value. */
    UPROPERTY(BlueprintReadWrite, Category="MTD|Tower", meta=(AllowPrivateAccess="true"))
    float MaxLevel = 5.f;
    
    /** Cached health retrieved from a curve table. */
    UPROPERTY(BlueprintReadWrite, Category="MTD|Tower", meta=(AllowPrivateAccess="true"))
    float BaseHealth = -1.f;

    /** Cached damage retrieved from a curve table. */
    UPROPERTY(BlueprintReadWrite, Category="MTD|Tower", meta=(AllowPrivateAccess="true"))
    float BaseDamage = -1.f;

    /** Cached firerate retrieved from a curve table. */
    UPROPERTY(BlueprintReadWrite, Category="MTD|Tower", meta=(AllowPrivateAccess="true"))
    float BaseFirerate = -1.f;

    /** Cached vision retrieved from a curve table. */
    UPROPERTY(BlueprintReadWrite, Category="MTD|Tower", meta=(AllowPrivateAccess="true"))
    float BaseVisionRange = -1.f;

    /** Cached vision half degrees retrieved from a curve table. */
    UPROPERTY(BlueprintReadWrite, Category="MTD|Tower", meta=(AllowPrivateAccess="true"))
    float BaseVisionHalfDegrees = -1.f;

    /** Cached projectile speed retrieved from a curve table. */
    UPROPERTY(BlueprintReadWrite, Category="MTD|Tower", meta=(AllowPrivateAccess="true"))
    float BaseProjectileSpeed = -1.f;
    
    /** Cached balance damage retrieved from a curve table. */
    UPROPERTY(BlueprintReadWrite, Category="MTD|Tower", meta=(AllowPrivateAccess="true"))
    float BalanceDamage = -1.f;

    /** If true, tower is realoading, and hence unable to fire, false otherwise. */
    bool bIsReloading = false;

    /** Timer handle reload event is associated with. */
    FTimerHandle ReloadTimerHandle;

    /** Gameplay effect to dynamically scale tower health. */
    UPROPERTY(EditDefaultsOnly, Category="MTD|Tower")
    TSubclassOf<UMTD_GameplayEffect> TowerHealthAttributeScalingGeClass = nullptr;

    /** Cached instigator's abiltiy system component. */
    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> InstigatorAbilitySystemComponent = nullptr;

    /** Instigator's builder set to keep track of some stat attributes. */
    UPROPERTY()
    TObjectPtr<const UMTD_BuilderSet> InstigatorBuilderSet = nullptr;
};
