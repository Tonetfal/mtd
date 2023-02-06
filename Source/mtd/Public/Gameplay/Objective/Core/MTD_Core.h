#pragma once

#include "AbilitySystemInterface.h"
#include "Character/MTD_GameResultInterface.h"
#include "GameFramework/Actor.h"
#include "mtd.h"

#include "MTD_Core.generated.h"

class UMTD_AbilitySystemComponent;
class UMTD_HealthComponent;
class USphereComponent;

UCLASS()
class MTD_API AMTD_Core 
    : public AActor
    , public IAbilitySystemInterface
    , public IMTD_GameResultInterface
{
    GENERATED_BODY()
    
public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
        FOnCoreHealthChangedSignature, 
        float, OldHealth, 
        float, NewHealth);
        
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(
        FOnCoreDestroyedSignature);

public:
    AMTD_Core();

protected:
    //~AActor Interface
    virtual void BeginPlay() override;
    //~End of AActor Interface

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
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Core", meta=(AllowPrivateAccess="true"))
    TObjectPtr<USphereComponent> CollisionComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Core", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_HealthComponent> HealthComponent = nullptr;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Core", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_AbilitySystemComponent> MtdAbilitySystemComponent = nullptr;
};
