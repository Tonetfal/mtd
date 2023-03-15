#pragma once

#include "mtd.h"

/**
 * Simple struct holding all MTD tags.
 *
 * Note these are not gameplay tags.
 */
struct FMTD_Tags
{
public:
    inline static const FName Character = "Character";
    inline static const FName Player = "Player";
    inline static const FName Foe = "Foe";
    inline static const FName Tower = "Tower";
    inline static const FName Core = "Core";
    inline static const FName Interactable = "Interactable";
};
