SET (SRCS_GAMEGUILIB

	./GameGUILib/GameDialogBase.h
	./GameGUILib/GameGUIBBoxGizmo.h
	./GameGUILib/GameGUILib.h
	./GameGUILib/GameGUIRotationGizmo.h
	./GameGUILib/GameGUITranslationGizmo.h
	./GameGUILib/GameGUIUtil.h
	./GameGUILib/GameWindowBase.h
	./GameGUILib/stdafx.h

	./GameGUILib/GameDialogBase.cpp
	./GameGUILib/GameGUIBBoxGizmo.cpp
	./GameGUILib/GameGUILib.cpp
	./GameGUILib/GameGUIRotationGizmo.cpp
	./GameGUILib/GameGUITranslationGizmo.cpp
	./GameGUILib/GameGUIUtil.cpp
	./GameGUILib/GameWindowBase.cpp
	./GameGUILib/stdafx.cpp
)

ADD_LIBRARY ( GameGUILib ${SRCS_GAMEGUILIB} )
