SET (SRCS_SCENARIOEDITOR

	./ScenarioEditor/ScenarioEditorColorSelector.h
	./ScenarioEditor/ScenarioEditor.h
	./ScenarioEditor/ScenarioEditorMainWindow.h
	./ScenarioEditor/ScenarioEditorObjectLabel.h
	./ScenarioEditor/ScenarioEditorObjectListSelector.h
	./ScenarioEditor/ScenarioEditorObjectSelector.h
	./ScenarioEditor/stdafx.h

	./ScenarioEditor/ScenarioEditorColorSelector.cpp
	./ScenarioEditor/ScenarioEditor.cpp
	./ScenarioEditor/ScenarioEditorMainWindow.cpp
	./ScenarioEditor/ScenarioEditorObjectLabel.cpp
	./ScenarioEditor/ScenarioEditorObjectListSelector.cpp
	./ScenarioEditor/ScenarioEditorObjectSelector.cpp
	./ScenarioEditor/stdafx.cpp
)


IF(ANDROID)
	ADD_LIBRARY ( ScenarioEditor ${SRCS_SCENARIOEDITOR} )
ELSE()
	ADD_LIBRARY ( ScenarioEditor SHARED ${SRCS_SCENARIOEDITOR} )

	TARGET_LINK_LIBRARIES (
			ScenarioEditor
		GameGUILib
		GameRunTimeLib
		VectorLib
		GameEntityLib
		${ANDROID_LOG_LIBRARY}
	)
ENDIF()