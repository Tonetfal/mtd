#pragma once

#include "AttributeSet.h"
#include "mtd.h"
#include "MTD_PawnComponent.h"

#include "MTD_LevelComponent.generated.h"

struct FOnAttributeChangeData;
class UMTD_AbilitySystemComponent;
class UMTD_PlayerSet;

UENUM(BlueprintType)
enum class EMTD_UpgradeAttribute : uint8
{
    PlayerHealth,
    PlayerDamage,
    PlayerSpeed,
    
    TowerHealth,
    TowerDamage,
    TowerRange,
    TowerSpeed,
};

UCLASS(Blueprintable, ClassGroup=("Player"), meta=(BlueprintSpawnableComponent))
class MTD_API UMTD_LevelComponent
    : public UMTD_PawnComponent
{
    GENERATED_BODY()

public:
    inline static const FName ExpRowName = "Experience";
    inline static const FName TotalExpRowName = "TotalExperience";
    inline static const FName AttributePointsRowName = "AttributePoints";

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
        FLevelAttributeChangedSignature,
        UMTD_LevelComponent*, LevelComponent,
        float, OldValue,
        float, NewValue,
        AActor*, Instigator);

public:
    UMTD_LevelComponent();

    UFUNCTION(BlueprintCallable, Category="MTD|Level Component")
    static UMTD_LevelComponent *FindLevelComponent(const AActor *Actor);

    UFUNCTION(BlueprintCallable, Category="MTD|Level Component")
    void InitializeWithAbilitySystem(UMTD_AbilitySystemComponent *Asc);

    UFUNCTION(BlueprintCallable, Category="MTD|Level Component")
    void UninitializeFromAbilitySystem();
    
    UFUNCTION(BlueprintCallable, Category="MTD|Level Component")
    void AddExp(int32 InExp);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Level Component")
    int32 GetLevel() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Level Component")
    int32 GetExp() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Level Component")
    int32 GetExpForNextLevel() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Level Component")
    int32 GetExpForLevel(int32 Level) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Level Component")
    int32 GetExpNormilized() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Level Component")
    int32 GetTotalExp() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Level Component")
    int32 GetTotalExpForNextLevel() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Level Component")
    int32 GetTotalExpForLevel(int32 Level) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Level Component")
    int32 GetRemainingAttributePoints() const;
    
    UFUNCTION(BlueprintCallable, Category="MTD|Level Component")
    bool UseAttributePoints(int32 Amount, EMTD_UpgradeAttribute Attribute);
    
protected:
    virtual void OnUnregister() override;
    virtual void OnLevelChanged(const FOnAttributeChangeData &ChangeData);
    virtual void OnExperienceChanged(const FOnAttributeChangeData &ChangeData);

private:
    void MapUpgradeAttributes();
    bool CacheExpRows();

public:
    UPROPERTY(BlueprintAssignable)
    FLevelAttributeChangedSignature OnLevelChangedDelegate;
    
    UPROPERTY(BlueprintAssignable)
    FLevelAttributeChangedSignature OnExperienceChangedDelegate;

private:
    UPROPERTY()
    TObjectPtr<UMTD_AbilitySystemComponent> AbilitySystemComponent = nullptr;

    UPROPERTY()
    TObjectPtr<const UMTD_PlayerSet> PlayerSet = nullptr;

    int32 RemainingAttributePoints = 0;
    TMap<EMTD_UpgradeAttribute, FGameplayAttribute> UpgradeAttributesMapping;

    const FRealCurve *ExpLevelRow = nullptr;
    const FRealCurve *TotalExpLevelRow = nullptr;
    const FRealCurve *AttributePointsRow = nullptr;
};

inline UMTD_LevelComponent *UMTD_LevelComponent::FindLevelComponent(const AActor *Actor)
{
    return ((IsValid(Actor)) ? (Actor->FindComponentByClass<UMTD_LevelComponent>()) : (nullptr));
}
