#pragma once

#include "GenericTeamAgentInterface.h"
#include "mtd.h"

#include "MTD_CoreTypes.generated.h"

/**
 * Enum containing all specific MTD teams.
 */
UENUM(BlueprintType)
enum class EMTD_TeamId : uint8
{
	Player = 0		UMETA(DisplayName="Player"),
	Tower			UMETA(DisplayName="Tower"),
	Core			UMETA(DisplayName="Core"),
	Foe			    UMETA(DisplayName="Foe"),
	Neutral			UMETA(DisplayName="Neutral"),
	Count			UMETA(Hidden),
	Unknown = 255	UMETA(DisplayName="Unknown")
};

/**
 * Default attitude solver used in this project.
 */
ETeamAttitude::Type SolveTeamAttitude(FGenericTeamId Lhs, FGenericTeamId Rhs);
