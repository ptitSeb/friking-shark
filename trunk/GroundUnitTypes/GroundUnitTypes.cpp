// GroundUnitTypes.cpp : Defines the entry point for the DLL application.
//

#include "./stdafx.h"
#include "GroundUnitTypes.h"
#include "VehicleType.h"
#include "StaticEntityType.h"
#include "StaticStructureType.h"
#include "TurretType.h"

BEGIN_SYSTEM_MODULE()
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CVehicleType,"CVehicle")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CTurretType,"CTurret")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CStaticEntityType,"CStaticEntity")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CStaticStructureType,"CStaticStructure")
END_SYSTEM_MODULE()

