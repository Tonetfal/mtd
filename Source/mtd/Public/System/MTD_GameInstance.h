#pragma once

#include "Engine/GameInstance.h"
#include "mtd.h"

#include "MTD_GameInstance.generated.h"

class UDataTable;
class UMTD_FoeQuantityDefinition;
class UMTD_LevelDefinition;

UCLASS()
class MTD_API UMTD_GameInstance
    : public UGameInstance
{
    GENERATED_BODY()

public:
    //~UGameInstance Interface
    virtual void Init() override;
    
protected:
    virtual void OnStart() override;

public:
    virtual void Shutdown() override;
    //~End of UGameInstance Interface
    
    const UCurveTable *GetLevelExpCurveTable() const;
    const UCurveTable *GetAttributePointsCurveTable() const;
    const UMTD_FoeQuantityDefinition *GetFoeQuantityAsset() const;
    const TArray<TObjectPtr<UMTD_LevelDefinition>> &GetGameLevels() const;

private:
    void VerifyLevelCurveTable() const;
    void VerifyAttributePointsCurveTable() const;
    
private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Players", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UCurveTable> LevelExperienceCurveTable = nullptr;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Players", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UCurveTable> AttributePointsCurveTable = nullptr;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spawners", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_FoeQuantityDefinition> FoeQuantityAsset = nullptr;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Levels", meta=(AllowPrivateAccess="true"))
    TArray<TObjectPtr<UMTD_LevelDefinition>> GameLevels;
};

inline const UCurveTable *UMTD_GameInstance::GetLevelExpCurveTable() const
{
    return LevelExperienceCurveTable;
}

inline const UCurveTable *UMTD_GameInstance::GetAttributePointsCurveTable() const
{
    return AttributePointsCurveTable;
}

inline const UMTD_FoeQuantityDefinition *UMTD_GameInstance::GetFoeQuantityAsset() const
{
    return FoeQuantityAsset;
}

inline const TArray<TObjectPtr<UMTD_LevelDefinition>> &UMTD_GameInstance::GetGameLevels() const
{
    return GameLevels;
}
