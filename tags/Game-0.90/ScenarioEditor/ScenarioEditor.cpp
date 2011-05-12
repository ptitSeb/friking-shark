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


// ScenarioEditor.cpp : Defines the exported functions for the DLL application.
//

#include "./stdafx.h"
#include "ScenarioEditor.h"
#include "ScenarioEditorMainWindow.h"
#include "ScenarioEditorColorSelector.h"
#include "ScenarioEditorObjectSelector.h"
#include "ScenarioEditorObjectLabel.h"


BEGIN_SYSTEM_MODULE()
SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CScenarioEditorMainWindow,"CScenarioEditorMainWindow")
SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CScenarioEditorColorSelector,"CScenarioEditorColorSelector")
SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CScenarioEditorObjectSelector,"CScenarioEditorObjectSelector")
SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CScenarioEditorObjectLabel,"CScenarioEditorObjectLabel")
END_SYSTEM_MODULE()
