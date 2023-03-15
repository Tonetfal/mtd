#include "System/MTD_GameInstance.h"

#include "MTD_CoreTypes.h"
#include "Subsystems/MTD_GameInstanceSubsystem.h"

void UMTD_GameInstance::Init()
{
    Super::Init();
    PostInitializeSubsystems();
    
    FGenericTeamId::SetAttitudeSolver(&SolveTeamAttitude);

    // @todo make unit tests instead
    // VerifyLevelCurveTable();
    // VerifyAttributePointsCurveTable();
}

void UMTD_GameInstance::PostInitializeSubsystems()
{
    // Get all MTD game instance subsystems
    const TArray<UMTD_GameInstanceSubsystem *> &MtdGameInstanceSubsystems =
        GetSubsystemArray<UMTD_GameInstanceSubsystem>();

    // Create subsystems multimap
    TMultiMap<int32, UMTD_GameInstanceSubsystem *> SortedSubsystems;
	for (UMTD_GameInstanceSubsystem* Subsystem : MtdGameInstanceSubsystems)
	{
	    SortedSubsystems.Add(Subsystem->GetPriority(), Subsystem);
	}

    // Sort in ascending order
    SortedSubsystems.KeySort(TGreater<int32>());

    // Post initialize subsystems
    for (auto &[_, Subsystem] : SortedSubsystems)
    {
        Subsystem->PostPrioritizedInit();
    }
}

// @todo keep until unit tests are implemented
// static bool CheckTableValues(const UCurveTable *CurveTable, const FName &RowName)
// {
//     check(IsValid(CurveTable));
//     
//     const FRealCurve *FoundRow = CurveTable->FindCurve(RowName, {});
//     if (!FoundRow)
//     {
//         MTD_WARN("Row could not be found.");
//         return false;
//     }
//     
//     auto RowIt = FoundRow->GetKeyHandleIterator();
//     auto EndIt = RowIt;
//     EndIt.SetToEnd();
//     
//     float Tmp = FoundRow->GetKeyValue(*RowIt);
//     int32 i = 0;
//     for (; (RowIt != EndIt); ++RowIt)
//     {
//         const float Value = FoundRow->GetKeyValue(*RowIt);
//
//         // All values must be greater than the previous one
//         if (Tmp > Value)
//         {
//             MTD_WARN("Value [%f] index [%d] is greater than value [%f] index [%d].", Value, i - 1, Tmp, i);
//             return false;
//         }
//     
//         Tmp = Value;
//         i++;
//     }
//
//     return true;
// }
//
// void UMTD_GameInstance::VerifyLevelCurveTable() const
// {
//     if (!IsValid(LevelExperienceCurveTable))
//     {
//         MTDS_WARN("Level xxperience curve table is invalid.");
//         return;
//     }
//
//     CheckTableValues(LevelExperienceCurveTable, UMTD_LevelComponent::ExpRowName);
//     CheckTableValues(LevelExperienceCurveTable, UMTD_LevelComponent::TotalExpRowName);
// }
//
// void UMTD_GameInstance::VerifyAttributePointsCurveTable() const
// {
//     if (!IsValid(AttributePointsCurveTable))
//     {
//         MTDS_WARN("Attribute points curve table is invalid.");
//         return;
//     }
//
//     CheckTableValues(AttributePointsCurveTable, UMTD_LevelComponent::AttributePointsRowName);
// }
