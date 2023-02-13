#pragma once

#include "mtd.h"
#include "UObject/Interface.h"

#include "MTD_GameResultInterface.generated.h"

UENUM(BlueprintType)
enum class EMTD_GameResult : uint8
{
    Lose,
    Win
};

UINTERFACE(MinimalAPI)
class UMTD_GameResultInterface
    : public UInterface
{
	GENERATED_BODY()
};

class MTD_API IMTD_GameResultInterface
{
	GENERATED_BODY()

public:
    /**
     * Event to fire when game is terminated for some reason.
     * @param   GameResult: game termination reason.
     */
    UFUNCTION(BlueprintImplementableEvent, Category="MTD|Game Result Interface")
	void OnGameTerminated(EMTD_GameResult GameResult);
	virtual void OnGameTerminated_Implementation(EMTD_GameResult GameResult)
	{
	    // Empty
	};
};
