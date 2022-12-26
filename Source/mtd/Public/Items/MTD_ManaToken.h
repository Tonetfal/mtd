#pragma once

#include "mtd.h"
#include "Items/MTD_FloatingToken.h"

#include "MTD_ManaToken.generated.h"

class UMTD_ManaComponent;
UCLASS()
class MTD_API AMTD_ManaToken : public AMTD_FloatingToken
{
    GENERATED_BODY()

protected:
    virtual bool CanBeActivatedOn(APawn *Pawn) const override;
    virtual void OnActivate_Implementation(APawn *Pawn) override;
    virtual APawn *FindNewTarget() const override;

    virtual void OnPawnAdded(APawn *Pawn) override;
    virtual void OnPawnRemoved(APawn *Pawn) override;
    virtual void SetNewTarget(APawn *Pawn) override;

    UFUNCTION(BlueprintCallable)
    static TArray<AMTD_ManaToken *> SpawnMana(
        AActor *Owner,
        const FTransform &Transform,
        int32 ManaAmount,
        const TMap<int32, TSubclassOf<AMTD_ManaToken>> &ManaTokensTable);

    UFUNCTION(BlueprintCallable)
    static void GiveStartVelocity(AMTD_ManaToken *ManaToken, const float BaseSpeed, const float MaxSpeedBonus);
    
    UFUNCTION(BlueprintCallable)
    static void GiveStartVelocityToTokens(const TArray<AMTD_ManaToken *> &ManaTokens, const float BaseSpeed,
        const float MaxSpeedBonus);

private:
    UFUNCTION()
    void OnTargetManaAttributeChanged(UMTD_ManaComponent *ManaComponent, float OldValue, float NewValue,
        AActor* InInstigator);

    void EnableScan();
    void DisableScan();

    void AddToListening(AActor *Actor);
    void RemoveFromListening(AActor *Actor);

private:
    UPROPERTY(EditDefaultsOnly, Category="MTD|Mana Token", meta=(ClampMin="0.0",
        ShortTooltip="Amount of mana the pawn will be granted."))
    int32 ManaAmount = 1;

    /// When enabled it means that all the detected (hence there must be at least one trigger) triggeres have full mana
    /// pool, hence listened for mana attribute changes in order to target the one who has lost some mana. After that
    /// the scan will be disabled.
    bool bScanMode = false;
};
