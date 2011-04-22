// EntityEditor.cpp : Defines the exported functions for the DLL application.
//

#include "./stdafx.h"
#include "EntityEditor.h"
#include "EntityEditorMainWindow.h"
#include "EntityEditorObjectSelector.h"
#include "EntityEditorObjectLabel.h"
#include "EntityEditorClassSelector.h"
#include "EntityEditorNamedObjectSelector.h"
#include "EntityEditorGeneralPropertyPanel.h"
#include "EntityEditorAnimationPropertyPanel.h"
#include "EntityEditorModelPropertyPanel.h"
#include "EntityEditorEventPropertyPanel.h"
#include "EntityEditorSoundPropertyPanel.h"
#include "EntityEditorParticleSystemPropertyPanel.h"
#include "EntityEditorEntityPropertyPanel.h"

BEGIN_SYSTEM_MODULE()
SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEntityEditorMainWindow,"CEntityEditorMainWindow")
SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEntityEditorObjectSelector,"CEntityEditorObjectSelector")
SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEntityEditorObjectLabel,"CEntityEditorObjectLabel")
SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEntityEditorClassSelector,"CEntityEditorClassSelector")
SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEntityEditorNamedObjectSelector,"CEntityEditorNamedObjectSelector")
SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEntityEditorGeneralPropertyPanel,"CEntityEditorGeneralPropertyPanel")
SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEntityEditorAnimationPropertyPanel,"CEntityEditorAnimationPropertyPanel")
SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEntityEditorModelPropertyPanel,"CEntityEditorModelPropertyPanel")
SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEntityEditorEventPropertyPanel,"CEntityEditorEventPropertyPanel")
SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEntityEditorSoundPropertyPanel,"CEntityEditorSoundPropertyPanel")
SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEntityEditorParticleSystemPropertyPanel,"CEntityEditorParticleSystemPropertyPanel")
SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEntityEditorEntityPropertyPanel,"CEntityEditorEntityPropertyPanel")
END_SYSTEM_MODULE()
