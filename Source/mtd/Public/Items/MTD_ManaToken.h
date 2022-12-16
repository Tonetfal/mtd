#pragma once

#include "mtd.h"
#include "Items/MTD_FloatingToken.h"

#include "MTD_ManaToken.generated.h"

UCLASS()
class MTD_API AMTD_ManaToken : public AMTD_FloatingToken
{
    GENERATED_BODY()

public:
    virtual void Tick(float DeltaSeconds) override;

protected:
    virtual bool CanBeActivatedOn(APawn *Pawn) const override;
    virtual void OnActivate_Implementation(APawn *Pawn) override;
    virtual APawn *FindNewTarget() const override;

    virtual void OnPawnAdded(APawn *Pawn) override;

private:
    UPROPERTY(EditDefaultsOnly, Category="MTD|Mana Token", meta=(ClampMin="0.0",
        ShortTooltip="Amount of mana the pawn will be granted."))
    int32 ManaAmount = 1;
};
