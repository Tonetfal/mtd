#include "Character/MTD_LevelComponent.h"

#include "AbilitySystem/Attributes/MTD_BuilderSet.h"
#include "AbilitySystem/Attributes/MTD_PlayerSet.h"
#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "AbilitySystem/MTD_GameplayTags.h"
#include "System/MTD_GameInstance.h"

UMTD_LevelComponent::UMTD_LevelComponent()
{
    // Nothing to tick for
    PrimaryComponentTick.bCanEverTick = false;
    PrimaryComponentTick.bStartWithTickEnabled = false;

    // Build the map as soon as possible
    MapUpgradeAttributes();

    TMap<int32, int32> Map;
    Map.CreateConstIterator();
}

void UMTD_LevelComponent::InitializeWithAbilitySystem(UMTD_AbilitySystemComponent *InAbilitySystemComponent)
{
    Super::InitializeWithAbilitySystem(InAbilitySystemComponent);
    if (!AbilitySystemComponent)
    {
        return;
    }

    // Cache the player set to avoid searching for it in ability system component every time it's needed
    PlayerSet = AbilitySystemComponent->GetSet<UMTD_PlayerSet>();
    if (!IsValid(PlayerSet))
    {
        MTDS_ERROR("Cannot initialize level component with NULL player set on the ability system.");
        return;
    }

    // Listen for level and experience attribute changes
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
        UMTD_PlayerSet::GetLevelAttribute()).AddUObject(this, &ThisClass::OnLevelChanged);
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
        UMTD_PlayerSet::GetLevelExperienceAttribute()).AddUObject(this, &ThisClass::OnExperienceChanged);

    // Cache for further needs
    CacheExpRows();
}

void UMTD_LevelComponent::UninitializeFromAbilitySystem()
{
    // Nullify ability system related data
    PlayerSet = nullptr;
    
    Super::UninitializeFromAbilitySystem();
}

void UMTD_LevelComponent::AddExp(int32 InExp)
{
    check(IsValid(AbilitySystemComponent));
    if (InExp < 1)
    {
        MTDS_WARN("Experience value [%d] can only be positive.", InExp);
        return;
    }

    // Compute final experience
    const int32 CurrentExp = AbilitySystemComponent->GetNumericAttribute(UMTD_PlayerSet::GetLevelExperienceAttribute());
    const int32 NewExp = (CurrentExp + InExp);

    // Apply modifier to the current experience value
    AbilitySystemComponent->SetNumericAttributeBase(UMTD_PlayerSet::GetLevelExperienceAttribute(), NewExp);
}

int32 UMTD_LevelComponent::GetLevel() const
{
    return ((IsValid(PlayerSet)) ? (PlayerSet->GetLevel()) : (0));
}

int32 UMTD_LevelComponent::GetExp() const
{
    return ((IsValid(PlayerSet)) ? (PlayerSet->GetLevelExperience()) : (0));
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
        MTDS_WARN("Ability system component is invalid.");
        return false;
    }
    
    if (Amount < 1)
    {
        MTDS_WARN("Amount [%d] must be a positive number.", Amount);
        return false;
    }

    if (RemainingAttributePoints < 1)
    {
        MTDS_LOG("There is no remaining attribute points");
        return false;
    }

    // Find the attribute, and check whether it's a valid one
    const FGameplayAttribute *GameplayAttribute = UpgradeAttributesMapping.Find(Attribute);
    check(GameplayAttribute);
    check(AbilitySystemComponent->HasAttributeSetForAttribute(*GameplayAttribute));

    // Clamp the amount
    if (RemainingAttributePoints < Amount)
    {
        MTDS_LOG("Remaining attributes points [%d] are less then requested amount [%d]. Only [%d] will be used.",
            RemainingAttributePoints, Amount, RemainingAttributePoints);
        Amount = RemainingAttributePoints;
    }

    // Modify the attribute by the clamped amount
    AbilitySystemComponent->ApplyModToAttribute(*GameplayAttribute, EGameplayModOp::Additive, Amount);

    // Decrease the remaining points
    RemainingAttributePoints -= Amount;
    
    return true;
}

void UMTD_LevelComponent::OnLevelChanged(const FOnAttributeChangeData &ChangeData)
{
    MTDS_LOG("Level up [%f] -> [%f].", ChangeData.OldValue, ChangeData.NewValue);

    ensure(ChangeData.OldValue < ChangeData.NewValue);

    if (AttributePointsRow)
    {
        // Iterate from old level value to the new one, and give more attribute points for each level
        for (int32 i = ChangeData.OldValue; (i < ChangeData.NewValue); i++)
        {
            ensure(AttributePointsRow->KeyExistsAtTime(i));
            const int32 Points = AttributePointsRow->Eval(i);
            RemainingAttributePoints += Points;
            
            MTDS_LOG("Player got [%d] attribute points for leveling up at [%d].", Points, i);
        }
    }
    else
    {
        MTDS_WARN("Attribute points row is NULL.");
    }

    AActor *Instigator = GetInstigatorFromAttrChangeData(ChangeData);

    // Notify about level up using this delegate first of all
    OnPrimaryLevelAttributeChangedDelegate.ExecuteIfBound(this, ChangeData.OldValue, ChangeData.NewValue, Instigator);

    // Send gameplay event about level up
    SendLevelUpEvent();

    // Notify about level up
    OnLevelChangedDelegate.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, Instigator);
}

void UMTD_LevelComponent::OnExperienceChanged(const FOnAttributeChangeData &ChangeData)
{
    MTDS_VERBOSE("EXP [%f] -> [%f].", ChangeData.OldValue, ChangeData.NewValue);

    // Notify about experience value change
    OnExperienceChangedDelegate.Broadcast(
        this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttrChangeData(ChangeData));
}

void UMTD_LevelComponent::SendLevelUpEvent()
{
    if (!IsValid(AbilitySystemComponent))
    {
        return;
    }

    const AActor *Avatar = AbilitySystemComponent->GetAvatarActor();
    if (!IsValid(Avatar))
    {
        return;
    }

    // Send the "Gameplay.Event.LevelUp" gameplay event through the owner's
    // ability system. This can be used to trigger a level up gameplay ability.
    FGameplayEventData Payload;
    Payload.EventTag = FMTD_GameplayTags::Get().Gameplay_Event_LevelUp;
    Payload.Target = AbilitySystemComponent->GetAvatarActor();
    Payload.ContextHandle = AbilitySystemComponent->MakeEffectContext();;

    // Send the gameplay event
    AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
}

void UMTD_LevelComponent::MapUpgradeAttributes()
{
    // Bind each enum to a player/tower gamepaly attribute
#define MAP(x, y, z) UpgradeAttributesMapping.Add( \
    EMTD_UpgradeAttribute:: ## x ## y, UMTD_ ## z ## Set::Get ## y ## Stat_BonusAttribute())
    
    MAP(Player, Damage, Player);
    MAP(Player, Health, Player);
    MAP(Player, Speed, Player);
    
    MAP(Tower, Damage, Builder);
    MAP(Tower, Health, Builder);
    MAP(Tower, Range, Builder);
    MAP(Tower, Speed, Builder);

#undef MAP
}

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
        MTDS_WARN("Experience level curve table is invalid.");
        return false;
    }
    
    const UCurveTable *AttributePointsLevelTable = GameInstance->GetAttributePointsCurveTable();
    if (!IsValid(AttributePointsLevelTable))
    {
        MTDS_WARN("Attribute points curve table is invalid.");
        return false;
    }
    
    const FRealCurve *ExpFoundRow = ExperienceLevelTable->FindCurve(ExpRowName, {});
    if (!ExpFoundRow)
    {
        MTDS_WARN("Could not find row [%s] in experience level curve table.", *ExpRowName.ToString());
        return false;
    }
    
    const FRealCurve *TotalExpFoundRow = ExperienceLevelTable->FindCurve(TotalExpRowName, {});
    if (!TotalExpFoundRow)
    {
        MTDS_WARN("Could not find row [%s] in experience level curve table.", *TotalExpRowName.ToString());
        return false;
    }
    
    const FRealCurve *AttributePointsFoundRow = AttributePointsLevelTable->FindCurve(AttributePointsRowName, {});
    if (!AttributePointsFoundRow)
    {
        MTDS_WARN("Could not find cow [%s] in attribute points curve table.", *AttributePointsRowName.ToString());
        return false;
    }

    // Cache these values to avoid accessing them every time they are needed
    ExpLevelRow = ExpFoundRow;
    TotalExpLevelRow = TotalExpFoundRow;
    AttributePointsRow = AttributePointsFoundRow;
    return true;
}
