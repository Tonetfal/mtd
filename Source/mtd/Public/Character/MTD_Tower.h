#pragma once

#include "AbilitySystemInterface.h"
#include "GameFramework/Pawn.h"
#include "mtd.h"
#include "MTD_GameResultInterface.h"
#include "MTD_TowerExtensionComponent.h"

#include "MTD_Tower.generated.h"

class AMTD_PlayerState;
class AMTD_Projectile;
class UBoxComponent;
class UMTD_AbilityAnimationSet;
class UMTD_AbilitySystemComponent;
class UMTD_BuilderSet;
class UMTD_GameplayEffect;
class UMTD_HealthComponent;
class UMTD_HeroComponent;
class UMTD_PawnExtensionComponent;
class UMTD_TowerData;
class USphereComponent;
struct FGameplayEffectSpecHandle;
struct FGameplayEventData;
struct FOnAttributeChangeData;

UCLASS()
class MTD_API AMTD_Tower : public APawn, public IAbilitySystemInterface, public IMTD_GameResultInterface
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(
        FDynamicMulticastSignature);
    
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
        FOnLevelUpSignature,
        int32, NewLevel,
        int32, OldLevel);

public:
    AMTD_Tower();

    //~AActor interface
    virtual void Tick(float DeltaTime) override;
    virtual void PreInitializeComponents() override;
    virtual void PostInitProperties() override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void Reset() override;
    //~End of AActor interface

    //~APawn interface
    virtual void NotifyControllerChanged() override;
    //~End of APawn interface

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Tower")
    int32 GetCurrentLevel() const;
    
    UFUNCTION(BlueprintCallable, Category="MTD|Tower")
    void AddLevel(int32 InDeltaLevel);

protected:
    virtual void OnAbilitySystemInitialized();
    virtual void OnAbilitySystemUninitialized();
    virtual void DestroyDueToDeath();
    virtual void Uninit();

    UFUNCTION(BlueprintNativeEvent, Category="MTD|Tower")
    void OnDeathStarted(AActor *OwningActor);
    virtual void OnDeathStarted_Implementation(AActor *OwningActor);

    UFUNCTION(BlueprintNativeEvent, Category="MTD|Tower")
    void OnDeathFinished(AActor *OwningActor);
    virtual void OnDeathFinished_Implementation(AActor *OwningActor);

    virtual void DisableCollision();

    virtual void FellOutOfWorld(const UDamageType &DamageType) override;

public:
    UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="MTD|Tower Stats")
    float GetScaledDamage() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="MTD|Tower Stats")
    float GetScaledFirerate() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="MTD|Tower Stats")
    float GetScaledVisionRange() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="MTD|Tower Stats")
    float GetScaledVisionHalfDegrees() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="MTD|Tower Stats")
    float GetScaledProjectileSpeed() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="MTD|Tower Stats")
    float GetReloadTime() const;

protected:
    float GetScaledFirerate_Implementation() const;
    float GetScaledDamage_Implementation() const;
    float GetScaledVisionRange_Implementation() const;
    float GetScaledVisionHalfDegrees_Implementation() const;
    float GetScaledProjectileSpeed_Implementation() const;
    float GetReloadTime_Implementation() const;

    void InitializeAttributes();

    UFUNCTION()
    virtual void OnLevelUp(int32 NewLevel, int32 OldLevel);
    
    void SendLevelUpEvent();

    //~IMTD_GameResultInterface Interface
    virtual void OnGameTerminated_Implementation(EMTD_GameResult GameResult) override;
    //~End of IMTD_GameResultInterface Interface

private:
    void OnFire(AActor *FireTarget);
    AMTD_Projectile *SpawnProjectile(const FTransform &Transform);

    void SetupProjectile(AMTD_Projectile &Projectile, AActor *FireTarget);
    void SetupProjectileCollision(AMTD_Projectile &Projectile) const;
    void SetupProjectileMovement(AMTD_Projectile &Projectile, AActor *FireTarget) const;
    void SetupProjectileGameplayEffectClasses(AMTD_Projectile &Projectile) const;
    void SetupProjectileHitCallback();

    virtual void OnProjectileHit(const FGameplayEventData *EventData);

protected:
    UFUNCTION(BlueprintImplementableEvent, DisplayName="OnProjectileHit")
    void K2_OnProjectileHit(const FGameplayEventData &EventData);

private:
    void StartReloading();
    void OnReloadFinished();

    void StartListeningForGameTerminated();
    void StartListeningForRangeAttributeChanges();
    void OnRangeAttributeChanged(const FOnAttributeChangeData &Attribute);
    void CachePlayerAsc();
    bool CheckTowerDataValidness() const;

    FVector GetTargetDistanceVector(const USceneComponent *Projectile, const USceneComponent *Target) const;
    FVector GetTargetDirection(const USceneComponent *Projectile, const USceneComponent *Target) const;

public:
    UMTD_HealthComponent *GetHealthComponent() const;

    UFUNCTION(BlueprintCallable, Category="MTD|Tower")
    AMTD_PlayerState *GetMtdPlayerState() const;

    UFUNCTION(BlueprintCallable, Category="MTD|Tower")
    UMTD_AbilitySystemComponent *GetMtdAbilitySystemComponent() const;

    //~IAbilitySystemInterface interface
    virtual UAbilitySystemComponent *GetAbilitySystemComponent() const override;
    //~End IAbilitySystemInterface interface

public:
    UPROPERTY(BlueprintAssignable)
    FDynamicMulticastSignature OnRangeAttributeChangedDelegate;

    UPROPERTY(BlueprintAssignable)
    FOnLevelUpSignature OnLevelUpDelegate;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UBoxComponent> CollisionComponent = nullptr;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UBoxComponent> NavVolumeComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<USkeletalMeshComponent> MeshComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<USphereComponent> ProjectileSpawnPosition = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_PawnExtensionComponent> PawnExtentionComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_HeroComponent> HeroComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_HealthComponent> HealthComponent = nullptr;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_TowerExtensionComponent> TowerExtensionComponent = nullptr;

    UPROPERTY(BlueprintReadWrite, Category="MTD|Tower", meta=(AllowPrivateAccess="true"))
    float CurrentLevel = 1.f;
    
    UPROPERTY(BlueprintReadWrite, Category="MTD|Tower", meta=(AllowPrivateAccess="true"))
    float MaxLevel = 5.f;

    /** Cached value retrieved from a curve table. */
    UPROPERTY(BlueprintReadWrite, Category="MTD|Tower", meta=(AllowPrivateAccess="true"))
    float BaseDamage = -1.f;

    /** Cached value retrieved from a curve table. */
    UPROPERTY(BlueprintReadWrite, Category="MTD|Tower", meta=(AllowPrivateAccess="true"))
    float BaseFirerate = -1.f;

    /** Cached value retrieved from a curve table. */
    UPROPERTY(BlueprintReadWrite, Category="MTD|Tower", meta=(AllowPrivateAccess="true"))
    float BaseVisionRange = -1.f;

    /** Cached value retrieved from a curve table. */
    UPROPERTY(BlueprintReadWrite, Category="MTD|Tower", meta=(AllowPrivateAccess="true"))
    float BaseVisionHalfDegrees = -1.f;

    /** Cached value retrieved from a curve table. */
    UPROPERTY(BlueprintReadWrite, Category="MTD|Tower", meta=(AllowPrivateAccess="true"))
    float BaseProjectileSpeed = -1.f;
    
    /** Cached value retrieved from a curve table. */
    UPROPERTY(BlueprintReadWrite, Category="MTD|Tower", meta=(AllowPrivateAccess="true"))
    float BalanceDamage = -1.f;

    bool bIsReloading = false;

    FTimerHandle ReloadTimerHandle;

    UPROPERTY(EditDefaultsOnly, Category="MTD|Tower")
    TSubclassOf<UMTD_GameplayEffect> TowerHealthAttributeScalingGeClass = nullptr;

    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> InstigatorAsc = nullptr;
    
    UPROPERTY()
    TObjectPtr<const UMTD_BuilderSet> InstigatorBuilderSet = nullptr;
};

inline UMTD_HealthComponent *AMTD_Tower::GetHealthComponent() const
{
    return HealthComponent;
}
