#pragma once

#include "mtd.h"
#include "MTD_BaseCharacter.h"

#include "MTD_BasePlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UMTD_PlayerExtensionComponent;

UCLASS()
class MTD_API AMTD_BasePlayerCharacter : public AMTD_BaseCharacter
{
    GENERATED_BODY()

public:
    AMTD_BasePlayerCharacter();

protected:
    //~AActor Interface
    virtual void BeginPlay() override;
    //~End of AActor Interface
    
    //~AMTD_BaseCharacter Interface
    virtual void InitializeAttributes() override;
    virtual void OnDeathStarted_Implementation(AActor *OwningActor) override;
    //~End of AMTD_BaseCharacter Interface
    
private:
    void InitializeInput();

    void DisableControllerInput();
    void DisableMovement();
    void DisableCollision();

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<USpringArmComponent> SpringArmComponent = nullptr;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UCameraComponent> CameraComponent = nullptr;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_PlayerExtensionComponent> PlayerExtensionComponent = nullptr;

    UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
    float Level = 0.f;
};
