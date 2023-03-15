#include "Settings/MTD_ItemDropSettings.h"

UMTD_ItemDropSettings *UMTD_ItemDropSettings::Get()
{
	// Use the override if we have it
	if (IsValid(DefaultSettings))
	{
		return DefaultSettings;
	}

	// Use CDO otherwise
	DefaultSettings = CastChecked<UMTD_ItemDropSettings>(
	    UMTD_ItemDropSettings::StaticClass()->GetDefaultObject());

    return DefaultSettings;
}
