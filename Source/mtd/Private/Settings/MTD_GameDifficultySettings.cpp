#include "Settings/MTD_GameDifficultySettings.h"

UMTD_GameDifficultySettings *UMTD_GameDifficultySettings::Get()
{
	// Use the override if we have it
	if (IsValid(DefaultSettings))
	{
		return DefaultSettings;
	}

	// Use CDO otherwise
	DefaultSettings = CastChecked<UMTD_GameDifficultySettings>(
	    UMTD_GameDifficultySettings::StaticClass()->GetDefaultObject());

    return DefaultSettings;
}
