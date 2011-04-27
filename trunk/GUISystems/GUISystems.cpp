// GameGUI.cpp : Defines the entry point for the DLL application.
//

#include "./stdafx.h"
#include "GUISystems.h"
#include "GameGUI.h"
#include "GameGUILib.h"
#include "MainWindow.h"
#include "MainMenu.h"
#include "GameMenu.h"
#include "GameInterface.h"
#include "ConfirmationDialog.h"
#include "GameSimpleDialog.h"

BEGIN_SYSTEM_MODULE()
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CMainWindow,"CMainWindow")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CMainMenu,"CMainMenu")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CGameMenu,"CGameMenu")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CGameInterface,"CGameInterface")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CConfirmationDialog,"CConfirmationDialog")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CGameSimpleDialog,"CGameSimpleDialog")
END_SYSTEM_MODULE()

