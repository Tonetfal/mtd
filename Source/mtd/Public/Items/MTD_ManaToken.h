#pragma once

#include "Items/MTD_FloatingToken.h"
#include "mtd.h"

#include "MTD_ManaToken.generated.h"

class UMTD_GameplayEffect;
class UMTD_ManaComponent;

UCLASS()
class MTD_API AMTD_ManaToken
    : public AMTD_FloatingToken
{
    GENERATED_BODY()

protected:
    virtual bool CanBeActivatedOn(APawn *Pawn) const override;
    virtual void OnActivate_Implementation(APawn *Pawn) override;
    virtual APawn *FindNewTarget() const override;

    virtual void OnPawnAdded(APawn *Pawn) override;
    virtual void SetNewTarget(APawn *Pawn) override;

    UFUNCTION(BlueprintCallable)
    static TArray<AMTD_ManaToken *> SpawnMana(
        AActor *Owner,
        const FTransform &Transform,
        int32 ManaAmount,
        const TMap<int32, TSubclassOf<AMTD_ManaToken>> &ManaTokensTable,
        float TriggersIgnoreTime = 0.f);

    UFUNCTION(BlueprintCallable)
    static void GiveStartVelocity(AMTD_ManaToken *ManaToken, const float BaseSpeed, const float MaxSpeedBonus);
    
    UFUNCTION(BlueprintCallable)
    static void GiveStartVelocityToTokens(const TArray<AMTD_ManaToken *> &ManaTokens, const float BaseSpeed,
        const float MaxSpeedBonus);
    
    virtual void EnableScan() override;
    virtual void DisableScan() override;

    virtual void AddToListening(AActor *Actor) override;
    virtual void RemoveFromListening(AActor *Actor) override;

private:
    void OnManaChangeHandleScanCase(UMTD_ManaComponent *ManaComponent);
    
    UFUNCTION()
    void OnTargetManaAttributeChanged(UMTD_ManaComponent *ManaComponent, float OldValue, float NewValue,
        AActor* InInstigator);
    
    UFUNCTION()
    void OnTargetMaxManaAttributeChanged(UMTD_ManaComponent *ManaComponent, float OldValue, float NewValue,
        AActor* InInstigator);

private:
    /** Amount of mana the pawn will be granted with. */
    UPROPERTY(EditDefaultsOnly, Category="MTD|Mana Token", meta=(ClampMin="0.0"))
    int32 ManaAmount = 1;
};
