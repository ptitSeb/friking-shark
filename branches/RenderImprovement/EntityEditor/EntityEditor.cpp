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


// EntityEditor.cpp : Defines the exported functions for the DLL application.
//

#include "./stdafx.h"
#include "EntityEditor.h"
#include "EntityEditorMainWindow.h"
#include "EntityEditorObjectListSelector.h"
#include "EntityEditorClassSelector.h"
#include "EntityEditorNamedObjectSelector.h"
#include "EntityEditorGeneralPropertyPanel.h"
#include "EntityEditorAnimationPropertyPanel.h"
#include "EntityEditorModelPropertyPanel.h"
#include "EntityEditorEventPropertyPanel.h"
#include "EntityEditorSoundPropertyPanel.h"
#include "EntityEditorParticleSystemPropertyPanel.h"
#include "EntityEditorEntityPropertyPanel.h"

BEGIN_SYSTEM_MODULE(EntityEditor)
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEntityEditorMainWindow,"CEntityEditorMainWindow")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEntityEditorObjectListSelector,"CEntityEditorObjectListSelector")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEntityEditorClassSelector,"CEntityEditorClassSelector")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEntityEditorNamedObjectSelector,"CEntityEditorNamedObjectSelector")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEntityEditorGeneralPropertyPanel,"CEntityEditorGeneralPropertyPanel")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEntityEditorAnimationPropertyPanel,"CEntityEditorAnimationPropertyPanel")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEntityEditorModelPropertyPanel,"CEntityEditorModelPropertyPanel")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEntityEditorEventPropertyPanel,"CEntityEditorEventPropertyPanel")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEntityEditorSoundPropertyPanel,"CEntityEditorSoundPropertyPanel")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEntityEditorParticleSystemPropertyPanel,"CEntityEditorParticleSystemPropertyPanel")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEntityEditorEntityPropertyPanel,"CEntityEditorEntityPropertyPanel")
END_SYSTEM_MODULE(EntityEditor)
