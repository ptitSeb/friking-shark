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
#include "GameGUI.h"
#include "GameGUILib.h"
#include "GameGUILabel.h"
#include "GameGUIButton.h"
#include "GameGUISlider.h"
#include "GameGUIGroup.h"
#include "GameGUIEdit.h"
#include "GameGUIList.h"
#include "GameGUIManager.h"
#include "GameGUIMessageDialog.h"
#include "GameGUIConfirmDialog.h"
#include "GameGUIColorDialog.h"
#include "GameGUIFileDialog.h"
#include "GameGUIInputDialog.h"

BEGIN_SYSTEM_MODULE(GameGUI)
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CGameGUIManager,"CGameGUIManager")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CGameGUIButton,"CGameGUIButton")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CGameGUISlider,"CGameGUISlider")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CGameGUILabel,"CGameGUILabel")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CGameGUIGroup,"CGameGUIGroup")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CGameGUIEdit,"CGameGUIEdit")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CGameGUIList,"CGameGUIList")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CGameGUIMessageDialog,"CGameGUIMessageDialog")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CGameGUIConfirmDialog,"CGameGUIConfirmDialog")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CGameGUIColorDialog,"CGameGUIColorDialog")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CGameGUIFileDialog,"CGameGUIFileDialog")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CGameGUIInputDialog,"CGameGUIInputDialog")
END_SYSTEM_MODULE(GameGUI)

