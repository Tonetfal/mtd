#pragma once

#include "AbilitySystem/MTD_AbilityAnimationSet.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Pawn.h"
#include "mtd.h"
#include "MTD_GameResultInterface.h"

#include "MTD_TowerAsd.generated.h"

struct FGameplayEffectSpecHandle;
class AMTD_PlayerState;
class AMTD_Projectile;
class UBoxComponent;
class UMTD_AbilityAnimationSet;
class UMTD_AbilitySystemComponent;
class UMTD_HealthComponent;
class UMTD_HeroComponent;
class UMTD_PawnExtensionComponent;
class UMTD_TowerData;
class USphereComponent;

UCLASS()
class MTD_API AMTD_TowerAsd : public APawn, public IAbilitySystemInterface, public IMTD_GameResultInterface
{
    GENERATED_BODY()

public:
    AMTD_TowerAsd();
    virtual void Tick(float DeltaTime) override;

    //~AActor interface
    virtual void PreInitializeComponents() override;
    virtual void PostInitProperties() override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void Reset() override;
    //~End of AActor interface

    //~APawn interface
    virtual void NotifyControllerChanged() override;
    //~End of APawn interface

    FMTD_AbilityAnimations GetAbilityAnimMontages(FGameplayTag AbilityTag) const;

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

    UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="MTD|Tower Stats")
    float GetScaledDamage();
    float GetScaledDamage_Implementation();

    UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="MTD|Tower Stats")
    float GetScaledFirerate();
    float GetScaledFirerate_Implementation();

    UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="MTD|Tower Stats")
    float GetScaledVisionRange();
    float GetScaledVisionRange_Implementation();

    UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="MTD|Tower Stats")
    float GetScaledVisionHalfDegrees();
    float GetScaledVisionHalfDegrees_Implementation();

    UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="MTD|Tower Stats")
    float GetScaledProjectileSpeed();
    float GetScaledProjectileSpeed_Implementation();

    UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category="MTD|Tower Stats")
    float GetReloadTime();
    float GetReloadTime_Implementation();

private:
    void OnFire(AActor *FireTarget);
    AMTD_Projectile *SpawnProjectile();

    void SetupProjectile(AMTD_Projectile *Projectile, AActor *FireTarget);
    void SetupProjectileCollision(AMTD_Projectile *Projectile) const;
    void SetupProjectileMovement(AMTD_Projectile *Projectile, AActor *FireTarget);
    void SetupProjectileEffectHandles(TArray<FGameplayEffectSpecHandle> &EffectHandles);

    void StartReloading();
    void OnReloadFinished();

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

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<UBoxComponent> CollisionComponent = nullptr;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<UBoxComponent> NavVolumeComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<USkeletalMeshComponent> MeshComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<USphereComponent> ProjectileSpawnPosition = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_PawnExtensionComponent> PawnExtentionComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_HeroComponent> HeroComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_HealthComponent> HealthComponent = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Ability System",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<const UMTD_AbilityAnimationSet> AnimationSet = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Ability System",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_TowerData> TowerData = nullptr;

    UPROPERTY(BlueprintReadWrite, Category="MTD|Tower",
        meta=(AllowPrivateAccess="true"))
    float Level = 0.f;

    UPROPERTY(BlueprintReadWrite, Category="MTD|Tower",
        meta=(AllowPrivateAccess="true"))
    float BaseDamage = -1.f;

    UPROPERTY(BlueprintReadWrite, Category="MTD|Tower",
        meta=(AllowPrivateAccess="true"))
    float BaseFirerate = -1.f;

    UPROPERTY(BlueprintReadWrite, Category="MTD|Tower",
        meta=(AllowPrivateAccess="true"))
    float BaseVisionRange = -1.f;

    UPROPERTY(BlueprintReadWrite, Category="MTD|Tower",
        meta=(AllowPrivateAccess="true"))
    float BaseVisionHalfDegrees = -1.f;

    UPROPERTY(BlueprintReadWrite, Category="MTD|Tower",
        meta=(AllowPrivateAccess="true"))
    float BaseProjectileSpeed = -1.f;

    bool bIsReloading = false;

    FTimerHandle ReloadTimerHandle;
};

inline UMTD_HealthComponent *AMTD_TowerAsd::GetHealthComponent() const
{
    return HealthComponent;
}
