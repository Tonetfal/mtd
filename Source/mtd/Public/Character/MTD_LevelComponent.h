#pragma once

#include "AttributeSet.h"
#include "Character/MTD_AscComponent.h"
#include "mtd.h"

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
    : public UMTD_AscComponent
{
    GENERATED_BODY()

public:
    inline static const FName ExpRowName = "Experience";
    inline static const FName TotalExpRowName = "TotalExperience";
    inline static const FName AttributePointsRowName = "AttributePoints";

public:
    DECLARE_DELEGATE_FourParams(
        FMainLevelAttributeChangedSignature,
        UMTD_LevelComponent *, /* LevelComponent */
        float, /* OldValue */
        float, /* NewValue */
        AActor * /* Instigator */);
    
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
        FLevelAttributeChangedSignature,
        UMTD_LevelComponent *, LevelComponent,
        float, OldValue,
        float, NewValue,
        AActor *, Instigator);

public:
    UMTD_LevelComponent();

    /**
     * Find level component on a given actor.
     * @param   Actor: actor to search for level component in.
     * @result  Level component residing on the given actor. Can be nullptr.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Level Component")
    static UMTD_LevelComponent *FindLevelComponent(const AActor *Actor);

    //~UMTD_AscComponent Interface
    virtual void InitializeWithAbilitySystem(UMTD_AbilitySystemComponent *InAbilitySystemComponent) override;
    virtual void UninitializeFromAbilitySystem() override;
    //~End of UMTD_AscComponent Interface

    /**
     * Add given amount of experience.
     * @param   InExp: amount of experience to add.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Level Component")
    void AddExp(int32 InExp);
    
    /**
     * Get current level.
     * @return  Current level.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Level Component")
    int32 GetLevel() const;

    /**
     * Get current experience.
     * @return  Current experience.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Level Component")
    int32 GetExp() const;

    /**
     * Get current experience in normalized form [0.0, 1.0].
     * @return  Current experience in normalized form [0.0, 1.0].
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Level Component")
    int32 GetExpForNextLevel() const;

    /**
     * Get amount of experience remaining to get to the next level.
     * @return  Experience remaining to get to the next level.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Level Component")
    int32 GetExpForLevel(int32 Level) const;
    
    /**
     * Get amount of experience remaining to get to the next level in normalized form [0.0, 1.0].
     * @return  Experience remaining to get to the next level in normalized form [0.0, 1.0].
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Level Component")
    int32 GetExpNormilized() const;

    /**
     * Get total experience.
     * @return  Total experience.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Level Component")
    int32 GetTotalExp() const;
    
    /**
     * Get total amount of experience to get to the next level.
     * @return  Total amount of experience to get to the next level.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Level Component")
    int32 GetTotalExpForNextLevel() const;
    
    /**
     * Get total amount of experience to get to the given level.
     * @param   Level: level to search experience total amount for.
     * @return  Total amount of experience to get to the given level.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Level Component")
    int32 GetTotalExpForLevel(int32 Level) const;

    /**
     * Get amount of remaining attribute points that can be used to increase attributes.
     * @return  Amount of remaining attribute points that can be used to increase attributes.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Level Component")
    int32 GetRemainingAttributePoints() const;

    /**
     * Use a given amount of attribute points to upgrade a given attribute.
     * @param   Amount: amount of attribute points to use.
     * @param   Attribute: attribute to upgrade.
     * @return  If true, [1 - 'Amount'] attribute points have been used to upgrade a given attribute, false otherwise. 
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Level Component")
    bool UseAttributePoints(int32 Amount, EMTD_UpgradeAttribute Attribute);
    
protected:
    /**
     * Event to fire when level attribute is changed.
     * @param   ChangeData: data about attribute change.
     */
    virtual void OnLevelChanged(const FOnAttributeChangeData &ChangeData);
    
    /**
     * Event to fire when experience attribute is changed.
     * @param   ChangeData: data about attribute change.
     */
    virtual void OnExperienceChanged(const FOnAttributeChangeData &ChangeData);

private:
    /**
     * Send a gameplay event about level up.
     */
    void SendLevelUpEvent();

    /**
     * Bind player and builder attributes to EMTD_UpgradeAttribute enum.
     */
    void MapUpgradeAttributes();

    /**
     * Cache all level and experience related data rows.
     * @return  If false, all data has been successfully cached, false otherwise.
     */
    bool CacheExpRows();

public:
    /** The very first delegate to fire when level attribute is changed. Only one listener is allowed. */
    FMainLevelAttributeChangedSignature OnPrimaryLevelAttributeChangedDelegate;
    
    /** Delegate to fire when level attribute is changed. */
    UPROPERTY(BlueprintAssignable)
    FLevelAttributeChangedSignature OnLevelChangedDelegate;
    
    /** Delegate to fire when experience attribute is changed. */
    UPROPERTY(BlueprintAssignable)
    FLevelAttributeChangedSignature OnExperienceChangedDelegate;

private:
    /** Player set defining all level and experience related data to use. */
    UPROPERTY()
    TObjectPtr<const UMTD_PlayerSet> PlayerSet = nullptr;

    /** Spare attribute points. */
    int32 RemainingAttributePoints = 0;

    /** EMTD_UpgradeAttributes binding to their corresponding player and tower gameplay attributes. */
    TMap<EMTD_UpgradeAttribute, FGameplayAttribute> UpgradeAttributesMapping;

    /**
     * Cached delta experience data row. Defines how much experience is needed to get to a level from the previous one.
     */
    const FRealCurve *ExpLevelRow = nullptr;
    
    /**
     * Cached total experience data row. Defines how much experience is needed to get to a certain level from level 0.
     */
    const FRealCurve *TotalExpLevelRow = nullptr;
    
    /** Cached attribute points data row. Defines how many points are given for leveling up at a certain level. */
    const FRealCurve *AttributePointsRow = nullptr;
};

inline UMTD_LevelComponent *UMTD_LevelComponent::FindLevelComponent(const AActor *Actor)
{
    return ((IsValid(Actor)) ? (Actor->FindComponentByClass<UMTD_LevelComponent>()) : (nullptr));
}
