#include "Settings/MTD_PlayerLevelSettings.h"

UMTD_PlayerLevelSettings *UMTD_PlayerLevelSettings::Get()
{
	// Use the override if we have it
	if (IsValid(DefaultSettings))
	{
		return DefaultSettings;
	}

	// Use CDO otherwise
	DefaultSettings = CastChecked<UMTD_PlayerLevelSettings>(
	    UMTD_PlayerLevelSettings::StaticClass()->GetDefaultObject());

    return DefaultSettings;
}
