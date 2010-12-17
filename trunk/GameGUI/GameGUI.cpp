// GameGUI.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "GameGUI.h"
#include "GameGUILib.h"
#include "GameGUIManager.h"
#include "GameGUILabel.h"
#include "GameGUIButton.h"
#include "GameGUIGroup.h"
#include "GameGUIEdit.h"

BEGIN_SYSTEM_MODULE()
SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CGameGUIManager,"CGameGUIManager");
SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CGameGUIButton,"CGameGUIButton");
SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CGameGUILabel,"CGameGUILabel");
SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CGameGUILabel,"CGameGUIGroup");
SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CGameGUIEdit,"CGameGUIEdit");
END_SYSTEM_MODULE();

