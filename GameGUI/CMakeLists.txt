SET (SRCS_GAMEGUI

	./GameGUI/GameGUISlider.h
	./GameGUI/GameGUIButton.h
	./GameGUI/GameGUIColorDialog.h
	./GameGUI/GameGUIConfirmDialog.h
	./GameGUI/GameGUIEdit.h
	./GameGUI/GameGUIFileDialog.h
	./GameGUI/GameGUIGroup.h
	./GameGUI/GameGUI.h
	./GameGUI/GameGUIInputDialog.h
	./GameGUI/GameGUILabel.h
	./GameGUI/GameGUIList.h
	./GameGUI/GameGUIManager.h
	./GameGUI/GameGUIMessageDialog.h
	./GameGUI/GameMainWindow.h
	./GameGUI/resource.h
	./GameGUI/stdafx.h

	./GameGUI/GameGUISlider.cpp
	./GameGUI/GameGUIButton.cpp
	./GameGUI/GameGUIColorDialog.cpp
	./GameGUI/GameGUIConfirmDialog.cpp
	./GameGUI/GameGUI.cpp
	./GameGUI/GameGUIEdit.cpp
	./GameGUI/GameGUIFileDialog.cpp
	./GameGUI/GameGUIGroup.cpp
	./GameGUI/GameGUIInputDialog.cpp
	./GameGUI/GameGUILabel.cpp
	./GameGUI/GameGUIList.cpp
	./GameGUI/GameGUIManager.cpp
	./GameGUI/GameGUIMessageDialog.cpp
	./GameGUI/GameMainWindow.cpp
	./GameGUI/stdafx.cpp
)

if(STATIC_BUILD)
	ADD_LIBRARY ( GameGUI STATIC ${SRCS_GAMEGUI} )
else()
	ADD_LIBRARY ( GameGUI SHARED ${SRCS_GAMEGUI} )

TARGET_LINK_LIBRARIES (
	GameGUI
	GameGUILib
	GameRunTimeLib
	VectorLib
	GameEntityLib
	${PNG_LIBRARY}
	${OPENAL_LIBRARY}
	${OPENGL_LIBRARY}
)

endif()
