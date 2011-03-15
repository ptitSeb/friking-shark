// EntityEditor.cpp : Defines the exported functions for the DLL application.
//

#include "./stdafx.h"
#include "EntityEditor.h"
#include "EntityEditorMainWindow.h"
#include "EntityEditorObjectSelector.h"
#include "EntityEditorObjectLabel.h"
#include "EntityEditorClassSelector.h"
#include "EntityEditorNamedObjectSelector.h"


BEGIN_SYSTEM_MODULE()
SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEntityEditorMainWindow,"CEntityEditorMainWindow")
SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEntityEditorObjectSelector,"CEntityEditorObjectSelector")
SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEntityEditorObjectLabel,"CEntityEditorObjectLabel")
SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEntityEditorClassSelector,"CEntityEditorClassSelector")
SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEntityEditorNamedObjectSelector,"CEntityEditorNamedObjectSelector")
END_SYSTEM_MODULE()
