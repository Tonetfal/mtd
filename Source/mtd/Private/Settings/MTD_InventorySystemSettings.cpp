#include "Settings/MTD_InventorySystemSettings.h"

#include "Engine/DataTable.h"

UMTD_InventorySystemSettings *UMTD_InventorySystemSettings::Get()
{
	// Use the override if we have it
	if (IsValid(DefaultSettings))
	{
		return DefaultSettings;
	}

	// Use CDO otherwise
	DefaultSettings = CastChecked<UMTD_InventorySystemSettings>(
	    UMTD_InventorySystemSettings::StaticClass()->GetDefaultObject());

    return DefaultSettings;
}
