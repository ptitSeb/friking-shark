SET (SRCS_ENTITYEDITOR

	./EntityEditor/EntityEditorAnimationPropertyPanel.h
	./EntityEditor/EntityEditorClassSelector.h
	./EntityEditor/EntityEditorEntityPropertyPanel.h
	./EntityEditor/EntityEditorEventPropertyPanel.h
	./EntityEditor/EntityEditorGeneralPropertyPanel.h
	./EntityEditor/EntityEditor.h
	./EntityEditor/EntityEditorMainWindow.h
	./EntityEditor/EntityEditorModelPropertyPanel.h
	./EntityEditor/EntityEditorNamedObjectSelector.h
	./EntityEditor/EntityEditorObjectListSelector.h
	./EntityEditor/EntityEditorParticleSystemPropertyPanel.h
	./EntityEditor/EntityEditorSoundPropertyPanel.h
	./EntityEditor/stdafx.h

	./EntityEditor/EntityEditorAnimationPropertyPanel.cpp
	./EntityEditor/EntityEditorClassSelector.cpp
	./EntityEditor/EntityEditor.cpp
	./EntityEditor/EntityEditorEntityPropertyPanel.cpp
	./EntityEditor/EntityEditorEventPropertyPanel.cpp
	./EntityEditor/EntityEditorGeneralPropertyPanel.cpp
	./EntityEditor/EntityEditorMainWindow.cpp
	./EntityEditor/EntityEditorModelPropertyPanel.cpp
	./EntityEditor/EntityEditorNamedObjectSelector.cpp
	./EntityEditor/EntityEditorObjectListSelector.cpp
	./EntityEditor/EntityEditorParticleSystemPropertyPanel.cpp
	./EntityEditor/EntityEditorSoundPropertyPanel.cpp
	./EntityEditor/stdafx.cpp
)

link_libraries (
	${INCLUDED_LIBRARIES}
)

if(STATIC_BUILD)
	ADD_LIBRARY ( EntityEditor STATIC ${SRCS_ENTITYEDITOR} )
else()
	ADD_LIBRARY ( EntityEditor SHARED ${SRCS_ENTITYEDITOR} )

TARGET_LINK_LIBRARIES (
         EntityEditor
	GameGUILib
	GameRunTimeLib
	VectorLib
	GameEntityLib
	${PNG_LIBRARY}
	${OPENAL_LIBRARY}
	${OPENGL_LIBRARY}
)

endif()
