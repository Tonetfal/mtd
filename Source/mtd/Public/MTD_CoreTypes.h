#pragma once

#include "mtd.h"
#include "GenericTeamAgentInterface.h"

#include "MTD_CoreTypes.generated.h"

UENUM(BlueprintType)
enum class EMTD_TeamId : uint8
{
    Player = 0 UMETA(DisplayName="Player"),
    Tower UMETA(DisplayName="Tower"),
    Core UMETA(DisplayName="Core"),
    Enemy UMETA(DisplayName="Enemy"),
    Neutral UMETA(DisplayName="Neutral"),
    Count UMETA(Hidden),
    Unknown = 255 UMETA(DisplayName="Unknown")
};

/*
	Player = 0		UMETA(DisplayName="Player"),
	Tower			UMETA(DisplayName="Tower"),
	Core			UMETA(DisplayName="Core"),
	Enemy			UMETA(DisplayName="Enemy"),
	Neutral			UMETA(DisplayName="Neutral"),
	Count			UMETA(Hidden),
	Unknown = 255	UMETA(DisplayName="Unknown")
 */


ETeamAttitude::Type SolveTeamAttitude(FGenericTeamId Lhs, FGenericTeamId Rhs);


// Visible part of a path
USTRUCT(BlueprintType)
struct FMTD_PathPoint
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, meta=(MakeEditWidget))
    FVector Point;
};

// Container of visible points (in editor) 
USTRUCT(BlueprintType)
struct FMTD_Path
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere)
    FName Name;

    UPROPERTY(EditAnywhere)
    TArray<FMTD_PathPoint> Points;

    bool operator<(const FMTD_Path &Rhs) const
    {
        return static_cast<bool>(Name.Compare(Rhs.Name));
    }

    bool operator==(const FMTD_Path &Rhs) const
    {
        return Name.Compare(Rhs.Name) == 0;
    }

    bool operator<(const FName &Rhs) const
    {
        return static_cast<bool>(Name.Compare(Rhs));
    }

    bool operator==(const FName &Rhs) const
    {
        return Name.Compare(Rhs) == 0;
    }
};
