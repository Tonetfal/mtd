#pragma once

#include "AbilitySystemInterface.h"
#include "GameFramework/Pawn.h"
#include "mtd.h"
#include "Character/MTD_GameResultInterface.h"

#include "MTD_Core.generated.h"

class UMTD_AbilitySystemComponent;
class UMTD_HealthComponent;
class USphereComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnCoreHealthChangedSignature,
    float, OldHealth,
    float, NewHealth);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCoreDestroyedSignature);

UCLASS()
class MTD_API AMTD_Core : public APawn, public IAbilitySystemInterface, public IMTD_GameResultInterface
{
    GENERATED_BODY()

public:
    AMTD_Core();

protected:
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category="MTD|Core")
    UMTD_AbilitySystemComponent *GetMtdAbilitySystemComponent() const;

    //~IAbilitySystemInterface interface
    virtual UAbilitySystemComponent *GetAbilitySystemComponent() const override;
    //~End IAbilitySystemInterface interface

    UFUNCTION(BlueprintImplementableEvent, Category="MTD|Core")
    void OnCoreDestroyed(AActor *OwningActor);

    UFUNCTION(BlueprintImplementableEvent, Category="MTD|Core")
    void OnCoreHealthChanged(
        UMTD_HealthComponent *InHealthComponent, float OldValue, float NewValue, AActor *InInstigator);

public:
    UPROPERTY(BlueprintCallable, BlueprintAssignable)
    FOnCoreHealthChangedSignature OnCoreHealthChangedDelegate;

    UPROPERTY(BlueprintCallable, BlueprintAssignable)
    FOnCoreDestroyedSignature OnCoreDestroyedDelegate;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Core",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<USphereComponent> CollisionSphere = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Core",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="MTD|Core",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_HealthComponent> HealthComponent = nullptr;
};
