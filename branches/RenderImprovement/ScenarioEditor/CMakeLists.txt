SET (SRCS_FORMATIONEDITOR

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

link_libraries (
	${INCLUDED_LIBRARIES}
)

ADD_LIBRARY ( ScenarioEditor SHARED ${SRCS_FORMATIONEDITOR} )

TARGET_LINK_LIBRARIES (
         ScenarioEditor
	GameGUILib
	GameRunTimeLib
	VectorLib
	GameEntityLib
	${PNG_LIBRARY}
	${OPENAL_LIBRARY}
	${OPENGL_LIBRARY}
)
