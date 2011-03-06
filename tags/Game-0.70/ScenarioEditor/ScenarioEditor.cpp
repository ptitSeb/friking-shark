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
