SET (SRCS_FORMATIONEDITOR

	./FormationEditor/FormationEditor.h
	./FormationEditor/FormationEditorMainWindow.h
	./FormationEditor/FormationEditorObjectLabel.h
	./FormationEditor/FormationEditorObjectSelector.h
	./FormationEditor/stdafx.h

	./FormationEditor/FormationEditor.cpp
	./FormationEditor/FormationEditorMainWindow.cpp
	./FormationEditor/FormationEditorObjectLabel.cpp
	./FormationEditor/FormationEditorObjectSelector.cpp
	./FormationEditor/stdafx.cpp
)


IF(ANDROID)
	ADD_LIBRARY ( FormationEditor ${SRCS_FORMATIONEDITOR} )
ELSE()
	ADD_LIBRARY ( FormationEditor SHARED ${SRCS_FORMATIONEDITOR} )

	TARGET_LINK_LIBRARIES (
			FormationEditor
		GameGUILib
		GameRunTimeLib
		VectorLib
		GameEntityLib
		${ANDROID_LOG_LIBRARY}
	)
ENDIF()