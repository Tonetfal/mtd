#pragma once

#include "mtd.h"
#include "MTD_BaseCharacter.h"

#include "MTD_BasePlayerCharacter.generated.h"

class UCameraComponent;
class UMTD_AbilitiesUiData;
class UMTD_PlayerExtensionComponent;
class USpringArmComponent;

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

    //~IMTD_GameResultInterface Interface
    virtual void OnGameTerminated_Implementation(EMTD_GameResult GameResult) override;
    //~End of IMTD_GameResultInterface Interface
    
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
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|UI", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_AbilitiesUiData> UiData = nullptr;

    UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
    float Level = 0.f;
};
