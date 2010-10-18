// AirUnitTypes.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "PlayerType.h"
#include "FighterType.h"
#include "BomberType.h"

BEGIN_SYSTEM_MODULE()
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CPlayerType,"CPlayer");
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CFighterType,"CFighter");
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CBomberType,"CBomber");
END_SYSTEM_MODULE();

