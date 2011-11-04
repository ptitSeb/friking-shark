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


// FormationEditor.cpp : Defines the exported functions for the DLL application.
//

#include "./stdafx.h"
#include "FormationEditor.h"
#include "FormationEditorMainWindow.h"
#include "FormationEditorObjectSelector.h"
#include "FormationEditorObjectLabel.h"


BEGIN_SYSTEM_MODULE(FormationEditor)
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CFormationEditorMainWindow,"CFormationEditorMainWindow")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CFormationEditorObjectSelector,"CFormationEditorObjectSelector")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CFormationEditorObjectLabel,"CFormationEditorObjectLabel")
END_SYSTEM_MODULE(FormationEditor)
