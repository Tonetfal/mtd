#include "Character/MTD_LevelComponent.h"

#include "AbilitySystem/Attributes/MTD_BuilderSet.h"
#include "AbilitySystem/Attributes/MTD_PlayerSet.h"
#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "System/MTD_GameInstance.h"

UMTD_LevelComponent::UMTD_LevelComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    PrimaryComponentTick.bStartWithTickEnabled = false;

    MapUpgradeAttributes();
}

void UMTD_LevelComponent::InitializeWithAbilitySystem(UMTD_AbilitySystemComponent *Asc)
{
    const AActor *Owner = GetOwner();
    check(IsValid(Owner));

    if (IsValid(AbilitySystemComponent))
    {
        MTDS_ERROR("Level Component for Owner [%s] has already been initilized with an ability system.",
            *Owner->GetName());
        return;
    }

    AbilitySystemComponent = Asc;
    if (!IsValid(AbilitySystemComponent))
    {
        MTDS_ERROR("Cannot initilize Level Component for Owner [%s] with a NULL ability system.", *Owner->GetName());
        return;
    }

    PlayerSet = AbilitySystemComponent->GetSet<UMTD_PlayerSet>();
    if (!IsValid(PlayerSet))
    {
        MTDS_ERROR("Cannot initialize Level Component with NULL Player Set on the ability system.");
        return;
    }
    
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
        UMTD_PlayerSet::GetLevelStatAttribute()).AddUObject(this, &ThisClass::OnLevelChanged);
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
        UMTD_PlayerSet::GetExperienceStatAttribute()).AddUObject(this, &ThisClass::OnExperienceChanged);

    CacheExpRows();
}

void UMTD_LevelComponent::UninitializeFromAbilitySystem()
{
    PlayerSet = nullptr;
    AbilitySystemComponent = nullptr;
}

void UMTD_LevelComponent::AddExp(int32 InExp)
{
    check(IsValid(AbilitySystemComponent));
    if (InExp < 1)
    {
        MTDS_WARN("EXP [%d] can only be positive.", InExp);
        return;
    }

    AbilitySystemComponent->ApplyModToAttribute(
        UMTD_PlayerSet::GetExperienceStatAttribute(), EGameplayModOp::Additive, InExp);
}

int32 UMTD_LevelComponent::GetLevel() const
{
    return ((IsValid(PlayerSet)) ? (PlayerSet->GetLevelStat()) : (0));
}

int32 UMTD_LevelComponent::GetExp() const
{
    return ((IsValid(PlayerSet)) ? (PlayerSet->GetExperienceStat()) : (0));
}

int32 UMTD_LevelComponent::GetExpForNextLevel() const
{
    const int32 Level = GetLevel();
    return GetExpForLevel(Level);
}

int32 UMTD_LevelComponent::GetExpForLevel(int32 Level) const
{
    const float Exp = ExpLevelRow->Eval(Level);
    return Exp;
}

int32 UMTD_LevelComponent::GetExpNormilized() const
{
    const int32 Level = GetLevel();
    const int32 A = GetExpForLevel(Level);
    const int32 B = GetExpForLevel(Level + 1);
    const float Ratio = (B / A);
    return Ratio;
}

int32 UMTD_LevelComponent::GetTotalExp() const
{
    const int32 Level = GetLevel();
    return GetTotalExpForLevel(Level);
}

int32 UMTD_LevelComponent::GetTotalExpForNextLevel() const
{
    const int32 Level = (GetLevel() + 1);
    return GetTotalExpForLevel(Level);
}

int32 UMTD_LevelComponent::GetTotalExpForLevel(int32 Level) const
{
    const int32 TotalExp = TotalExpLevelRow->Eval(Level);
    return TotalExp;
}

int32 UMTD_LevelComponent::GetRemainingAttributePoints() const
{
    return RemainingAttributePoints;
}

bool UMTD_LevelComponent::UseAttributePoints(int32 Amount, EMTD_UpgradeAttribute Attribute)
{
    if (!IsValid(AbilitySystemComponent))
    {
        MTDS_WARN("Ability System Component is invalid.");
        return false;
    }
    
    if (Amount < 1)
    {
        MTDS_WARN("Amount [%d] must be a positive number.", Amount);
        return false;
    }

    if (RemainingAttributePoints < 1)
    {
        MTDS_LOG("There is no Remaining Attribute Points");
        return false;
    }

    const FGameplayAttribute *GameplayAttribute = UpgradeAttributesMapping.Find(Attribute);
    check(GameplayAttribute);
    check(AbilitySystemComponent->HasAttributeSetForAttribute(*GameplayAttribute));

    if (RemainingAttributePoints < Amount)
    {
        MTDS_LOG("Remaining Attributes Points [%d] are less then requested amount [%d]. Only [%d] will be used.",
            RemainingAttributePoints, Amount, RemainingAttributePoints);
        Amount = RemainingAttributePoints;
    }

    AbilitySystemComponent->ApplyModToAttribute(*GameplayAttribute, EGameplayModOp::Additive, Amount);
    RemainingAttributePoints -= Amount;
    return true;
}

void UMTD_LevelComponent::OnUnregister()
{
    Super::OnUnregister();
}

void UMTD_LevelComponent::OnLevelChanged(const FOnAttributeChangeData &ChangeData)
{
    MTDS_LOG("Level up [%f] -> [%f].", ChangeData.OldValue, ChangeData.NewValue);

    if (AttributePointsRow)
    {
        for (int32 i = ChangeData.OldValue; (i < ChangeData.NewValue); i++)
        {
            ensure(AttributePointsRow->KeyExistsAtTime(i));
            const int32 Points = AttributePointsRow->Eval(i);
            RemainingAttributePoints += Points;
            
            MTDS_LOG("Player got [%d] Attribute Points for leveling up at [%d].", Points, i);
        }
    }
    else
    {
        MTDS_WARN("Attribute Points Row is null.");
    }
    
    OnLevelChangedDelegate.Broadcast(
        this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttrChangeData(ChangeData));
}

void UMTD_LevelComponent::OnExperienceChanged(const FOnAttributeChangeData &ChangeData)
{
    MTDS_VERBOSE("EXP [%f] -> [%f].", ChangeData.OldValue, ChangeData.NewValue);
    
    OnExperienceChangedDelegate.Broadcast(
        this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttrChangeData(ChangeData));
}

#define MAP(x, y, z) UpgradeAttributesMapping.Add( \
    EMTD_UpgradeAttribute:: ## x ## y, UMTD_ ## z ## Set::Get ## y ## Stat_BonusAttribute())
void UMTD_LevelComponent::MapUpgradeAttributes()
{
    MAP(Player, Damage, Player);
    MAP(Player, Health, Player);
    MAP(Player, Speed, Player);
    
    MAP(Tower, Damage, Builder);
    MAP(Tower, Health, Builder);
    MAP(Tower, Range, Builder);
    MAP(Tower, Speed, Builder);
}
#undef MAP

bool UMTD_LevelComponent::CacheExpRows()
{
    if (((ExpLevelRow) && (TotalExpLevelRow) && (AttributePointsRow)))
    {
        // Already cached
        return true;
    }
    
    const AActor *Owner = GetOwner();
    check(IsValid(Owner));

    const auto GameInstance = Cast<UMTD_GameInstance>(Owner->GetGameInstance());
    check(IsValid(GameInstance));

    const UCurveTable *ExperienceLevelTable = GameInstance->GetLevelExpCurveTable();
    if (!IsValid(ExperienceLevelTable))
    {
        MTDS_WARN("Experience Level Curve Table is invalid.");
        return false;
    }
    
    const UCurveTable *AttributePointsLevelTable = GameInstance->GetAttributePointsCurveTable();
    if (!IsValid(AttributePointsLevelTable))
    {
        MTDS_WARN("Attribute Points Curve Table is invalid.");
        return false;
    }
    
    const FRealCurve *ExpFoundRow = ExperienceLevelTable->FindCurve(ExpRowName, {});
    if (!ExpFoundRow)
    {
        MTDS_WARN("Could not find Row [%s] in Experience Level Curve Table.", *ExpRowName.ToString());
        return false;
    }
    
    const FRealCurve *TotalExpFoundRow = ExperienceLevelTable->FindCurve(TotalExpRowName, {});
    if (!TotalExpFoundRow)
    {
        MTDS_WARN("Could not find Row [%s] in Experience Level Curve Table.", *TotalExpRowName.ToString());
        return false;
    }
    
    const FRealCurve *AttributePointsFoundRow = AttributePointsLevelTable->FindCurve(AttributePointsRowName, {});
    if (!AttributePointsFoundRow)
    {
        MTDS_WARN("Could not find Row [%s] in Attribute Points Curve Table.", *AttributePointsRowName.ToString());
        return false;
    }

    ExpLevelRow = ExpFoundRow;
    TotalExpLevelRow = TotalExpFoundRow;
    AttributePointsRow = AttributePointsFoundRow;
    return true;
}
