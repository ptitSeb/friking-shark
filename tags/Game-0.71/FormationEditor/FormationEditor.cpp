// FormationEditor.cpp : Defines the exported functions for the DLL application.
//

#include "./stdafx.h"
#include "FormationEditor.h"
#include "FormationEditorMainWindow.h"
#include "FormationEditorObjectSelector.h"
#include "FormationEditorObjectLabel.h"


BEGIN_SYSTEM_MODULE()
SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CFormationEditorMainWindow,"CFormationEditorMainWindow")
SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CFormationEditorObjectSelector,"CFormationEditorObjectSelector")
SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CFormationEditorObjectLabel,"CFormationEditorObjectLabel")
END_SYSTEM_MODULE()
