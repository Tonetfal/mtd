#pragma once

#include "mtd.h"

#include "MTD_GameInstance.generated.h"

class UDataTable;
class UMTD_CharacterQuantityDefinition;
class UMTD_LevelDefinition;

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
    const UCurveTable *GetAttributePointsCurveTable() const;
    const UMTD_CharacterQuantityDefinition *GetEnemyQuantityAsset() const;
    const TArray<TObjectPtr<UMTD_LevelDefinition>> &GetGameLevels() const;

protected:
    virtual void OnStart() override;

public:
    virtual void Shutdown() override;

private:
    void VerifyBaseItemDataTable() const;
    void VerifyArmorDataTable() const;
    void VerifyWeaponDataTable() const;
    void VerifyMaterialDataTable() const;
    void VerifyLevelCurveTable() const;
    void VerifyAttributePointsCurveTable() const;
    
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
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Players", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UCurveTable> AttributePointsCurveTable = nullptr;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spawners", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_CharacterQuantityDefinition> EnemyQuantityAsset = nullptr;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Levels", meta=(AllowPrivateAccess="true"))
    TArray<TObjectPtr<UMTD_LevelDefinition>> GameLevels;
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

inline const UCurveTable *UMTD_GameInstance::GetAttributePointsCurveTable() const
{
    return AttributePointsCurveTable;
}

inline const UMTD_CharacterQuantityDefinition *UMTD_GameInstance::GetEnemyQuantityAsset() const
{
    return EnemyQuantityAsset;
}

inline const TArray<TObjectPtr<UMTD_LevelDefinition>> &UMTD_GameInstance::GetGameLevels() const
{
    return GameLevels;
}
