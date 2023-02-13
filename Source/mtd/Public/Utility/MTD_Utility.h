#pragma once

#include "mtd.h"
#include "MTD_CoreTypes.h"

#include "MTD_Utility.generated.h"

class ANavigationData;
class UNavigationSystemV1;
class UNavigationQueryFilter;
class AAIController;
struct FMTD_PathFindingContext;

USTRUCT()
struct FMTD_Utility
{
    GENERATED_BODY()

public:
    template <class T>
    static T *GetActorComponent(AActor *InActor);

    template <class T>
    static const T *GetActorComponent(const AActor *InActor);

    template <class T>
    static T *GetPawnController(APawn *InPawn);

    template <class T>
    static const T *GetPawnController(const APawn *InPawn);

    template <class T>
    static T *FindFirstNotifyByClass(UAnimSequenceBase *Anim);

    static FGenericTeamId GetMtdGenericTeamId(const AActor *InActor);
    static ETeamAttitude::Type GetMtdTeamAttitudeBetween(const AActor *Lhs, const AActor *Rhs);
    static EMTD_TeamId GenericToMtdTeamId(FGenericTeamId GenericId);

    static ENavigationQueryResult::Type ComputePathTo(const AActor *Other, float &Cost,
        const FMTD_PathFindingContext &Context);
};

/**
 * Context used to use path finding algorythms.
 */
USTRUCT()
struct FMTD_PathFindingContext
{
    GENERATED_BODY()

public:
    static FMTD_PathFindingContext Create(const APawn *Pawn);
    bool IsValid() const;

public:
    UPROPERTY()
    TObjectPtr<UWorld> World = nullptr;
    
    UPROPERTY()
    TObjectPtr<AAIController> AiController = nullptr;

    UPROPERTY()
    TSubclassOf<UNavigationQueryFilter> NavQueryFilter = nullptr;

    FVector StartPosition = FVector::ZeroVector;

    UPROPERTY()
    TObjectPtr<UNavigationSystemV1> NavigationSystem = nullptr;

    UPROPERTY()
    TObjectPtr<ANavigationData> NavigationData = nullptr;

private:
    bool bIsValid = false;
};

template <class T>
T *FMTD_Utility::GetActorComponent(AActor *InActor)
{
    if (!IsValid(InActor))
    {
        return nullptr;
    }

    UActorComponent *Comp = InActor->GetComponentByClass(T::StaticClass());
    if (!IsValid(Comp))
    {
        return nullptr;
    }

    return Cast<T>(Comp);
}

template <class T>
const T *FMTD_Utility::GetActorComponent(const AActor *InActor)
{
    if (!IsValid(InActor))
    {
        return nullptr;
    }

    UActorComponent *Comp = InActor->GetComponentByClass(T::StaticClass());
    if (!IsValid(Comp))
    {
        return nullptr;
    }

    return Cast<T>(Comp);
}

template <class T>
T *FMTD_Utility::GetPawnController(APawn *InPawn)
{
    if (!IsValid(InPawn))
    {
        return nullptr;
    }

    AController *Controller = InPawn->GetController();
    if (!IsValid(Controller))
    {
        return nullptr;
    }

    return Cast<T>(Controller);
}

template <class T>
const T *FMTD_Utility::GetPawnController(const APawn *InPawn)
{
    if (!IsValid(InPawn))
    {
        return nullptr;
    }

    const AController *Controller = InPawn->GetController();
    if (!IsValid(Controller))
    {
        return nullptr;
    }

    return Cast<T>(Controller);
}

template <class T>
T *FMTD_Utility::FindFirstNotifyByClass(UAnimSequenceBase *Anim)
{
    if (!IsValid(Anim))
    {
        return nullptr;
    }

    const TArray<FAnimNotifyEvent> &NotifyEvents = Anim->Notifies;
    for (auto &Item : NotifyEvents)
    {
        T *CastedItem = Cast<T>(Item.Notify);
        if (IsValid(CastedItem))
        {
            return CastedItem;
        }
    }
    return nullptr;
}
