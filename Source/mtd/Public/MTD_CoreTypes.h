#pragma once

#include "mtd.h"
#include "GenericTeamAgentInterface.h"

#include "MTD_CoreTypes.generated.h"

UENUM(BlueprintType)
enum class EMTD_TeamId : uint8
{
	Player = 0		UMETA(DisplayName="Player"),
	Tower			UMETA(DisplayName="Tower"),
	Core			UMETA(DisplayName="Core"),
	Enemy			UMETA(DisplayName="Enemy"),
	Neutral			UMETA(DisplayName="Neutral"),
	Count			UMETA(Hidden),
	Unknown = 255	UMETA(DisplayName="Unknown")
};

ETeamAttitude::Type SolveTeamAttitude(FGenericTeamId Lhs, FGenericTeamId Rhs);
