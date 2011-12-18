//  Friking Shark, a Flying Shark arcade remake.
//  Copyright (C) 2011  Javier Martin Garcia (javiermartingarcia@gmail.com)
//	
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//  


// GameGUI.cpp : Defines the entry point for the DLL application.
//

#include "./stdafx.h"
#include "GUISystems.h"
#include "GameGUI.h"
#include "GameGUILib.h"
#include "MainWindow.h"
#include "MainMenu.h"
#include "GameMenu.h"
#include "OptionsMenu.h"
#include "GameInterface.h"
#include "GameOverDialog.h"
#include "ControlsDialog.h"
#include "KeyCaptureDialog.h"
#include "HighScoresDialog.h"
#include "HighScoresTable.h"
#include "LevelOptions.h"
#include "AudioOptions.h"
#include "VideoOptions.h"
#include "CreditsDialog.h"
#include "LoadDialog.h"
#include "SaveDialog.h"
#include "SavedGameRow.h"

BEGIN_SYSTEM_MODULE(GUISystems)
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CMainWindow,"CMainWindow")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CMainMenu,"CMainMenu")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CGameMenu,"CGameMenu")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(COptionsMenu,"COptionsMenu")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CGameInterface,"CGameInterface")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CGameOverDialog,"CGameOverDialog")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CControlsDialog,"CControlsDialog")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CKeyCaptureDialog,"CKeyCaptureDialog")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CHighScoresDialog,"CHighScoresDialog")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CHighScoresTable,"CHighScoresTable")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CLevelOptions,"CLevelOptions")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CAudioOptions,"CAudioOptions")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CVideoOptions,"CVideoOptions")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CCreditsDialog,"CCreditsDialog")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CLoadDialog,"CLoadDialog")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CSaveDialog,"CSaveDialog")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CSavedGameRow,"CSavedGameRow")
END_SYSTEM_MODULE(GUISystems)

