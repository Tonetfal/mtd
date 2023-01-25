#pragma once

#include "mtd.h"

#include "MTD_GameInstance.generated.h"

class UDataTable;

UCLASS()
class MTD_API UMTD_GameInstance
    : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;
    
    const UDataTable *GetBaseItemDataTable() const;
    const UDataTable *GetArmorDataTable() const;
    const UDataTable *GetWeaponDataTable() const;
    const UDataTable *GetMaterialDataTable() const;
    const UCurveTable *GetLevelExpCurveTable() const;

private:
    void VerifyBaseItemDataTable() const;
    void VerifyArmorDataTable() const;
    void VerifyWeaponDataTable() const;
    void VerifyMaterialDataTable() const;
    void VerifyLevelCurveTable() const;
    
private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Items", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UDataTable> BaseItemDataTable = nullptr;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Items", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UDataTable> ArmorDataTable = nullptr;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Items", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UDataTable> WeaponDataTable = nullptr;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Items", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UDataTable> MaterialDataTable = nullptr;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Players", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UCurveTable> LevelExperienceCurveTable = nullptr;
};

inline const UDataTable *UMTD_GameInstance::GetBaseItemDataTable() const
{
    return BaseItemDataTable;
}

inline const UDataTable *UMTD_GameInstance::GetArmorDataTable() const
{
    return ArmorDataTable;
}

inline const UDataTable *UMTD_GameInstance::GetWeaponDataTable() const
{
    return WeaponDataTable;
}

inline const UDataTable *UMTD_GameInstance::GetMaterialDataTable() const
{
    return MaterialDataTable;
}

inline const UCurveTable *UMTD_GameInstance::GetLevelExpCurveTable() const
{
    return LevelExperienceCurveTable;
}
