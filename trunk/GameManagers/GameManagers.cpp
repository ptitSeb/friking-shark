// GameManagers.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "EntityManager.h"
#include "PhysicsManager.h"
#include "PlayAreaManager.h"
#include "WorldManager.h"
#include "GameController.h"
#include "FrameManager.h"

BEGIN_SYSTEM_MODULE()
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEntityManager,"CEntityManager");
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CPhysicManager,"CPhysicManager");
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CWorldManager,"CWorldManager");
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CPlayAreaManager,"CPlayAreaManager");
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CGameController,"CGameController");
		SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CFrameManager,"CFrameManager");
END_SYSTEM_MODULE();
