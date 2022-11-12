#pragma once

#include "GameplayAbilitySpec.h"
#include "Equipment/MTD_EquipmentInstance.h"
#include "mtd.h"

#include "MTD_WeaponInstance.generated.h"

class UGameplayEffect;

UCLASS()
class MTD_API UMTD_WeaponInstance : public UMTD_EquipmentInstance
{
    GENERATED_BODY()

protected:
    virtual void ModStats(float Multiplier) override;

    UFUNCTION(BlueprintCallable, Category="MTD|Weapon")
    TArray<FGameplayEffectSpecHandle> GetGameplayEffectSpecHandlesToGrantOnHit() const;

private:
    UPROPERTY(EditDefaultsOnly, Category="MTD|Equipment|Stats|Weapon",
        meta=(AllowPrivateAccess="true"))
    FMTD_EquipmentWeaponStats WeaponStats;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Weapon",
        meta=(AllowPrivateAccess="true", ShortTooltip="Gameplay effects to grant on melee attack hit"))
    TArray<TSubclassOf<UGameplayEffect>> GameplayEffectsToGrantOnHit;
};
