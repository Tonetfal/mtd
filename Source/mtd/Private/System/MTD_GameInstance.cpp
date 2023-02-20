#include "System/MTD_GameInstance.h"

#include "Character/MTD_LevelComponent.h"
#include "Engine/CurveTable.h"
#include "InventorySystem/Items/MTD_ItemDataManager.h"
#include "InventorySystem/Items/MTD_ItemDropManager.h"
#include "MTD_CoreTypes.h"

void UMTD_GameInstance::Init()
{
    Super::Init();
    FGenericTeamId::SetAttitudeSolver(&SolveTeamAttitude);

    VerifyLevelCurveTable();
    VerifyAttributePointsCurveTable();
}

void UMTD_GameInstance::OnStart()
{
    Super::OnStart();

    UMTD_ItemDropManager::Construct();
    
    const UMTD_ItemDataManager *ItemDataManager = UMTD_ItemDataManager::Get();
    if (ItemDataManager)
    {
        ItemDataManager->CheckDataValidness();
    }
}

void UMTD_GameInstance::Shutdown()
{
    Super::Shutdown();
    
    UMTD_ItemDropManager::Destroy();
}

static bool CheckTableValues(const UCurveTable *CurveTable, const FName &RowName)
{
    check(IsValid(CurveTable));
    
    const FRealCurve *FoundRow = CurveTable->FindCurve(RowName, {});
    if (!FoundRow)
    {
        MTD_WARN("Row could not be found.");
        return false;
    }
    
    auto RowIt = FoundRow->GetKeyHandleIterator();
    auto EndIt = RowIt;
    EndIt.SetToEnd();
    
    float Tmp = FoundRow->GetKeyValue(*RowIt);
    int32 i = 0;
    for (; (RowIt != EndIt); ++RowIt)
    {
        const float Value = FoundRow->GetKeyValue(*RowIt);

        // All values must be greater than the previous one
        if (Tmp > Value)
        {
            MTD_WARN("Value [%f] index [%d] is greater than value [%f] index [%d].", Value, i - 1, Tmp, i);
            return false;
        }
    
        Tmp = Value;
        i++;
    }

    return true;
}

void UMTD_GameInstance::VerifyLevelCurveTable() const
{
    if (!IsValid(LevelExperienceCurveTable))
    {
        MTDS_WARN("Level xxperience curve table is invalid.");
        return;
    }

    CheckTableValues(LevelExperienceCurveTable, UMTD_LevelComponent::ExpRowName);
    CheckTableValues(LevelExperienceCurveTable, UMTD_LevelComponent::TotalExpRowName);
}

void UMTD_GameInstance::VerifyAttributePointsCurveTable() const
{
    if (!IsValid(AttributePointsCurveTable))
    {
        MTDS_WARN("Attribute points curve table is invalid.");
        return;
    }

    CheckTableValues(AttributePointsCurveTable, UMTD_LevelComponent::AttributePointsRowName);
}
