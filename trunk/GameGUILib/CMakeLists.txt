SET (SRCS_GAMEGUILIB

	./GameDialogBase.h
	./GameGUIBBoxGizmo.h
	./GameGUILib.h
	./GameGUIRotationGizmo.h
	./GameGUITranslationGizmo.h
	./GameGUIUtil.h
	./GameWindowBase.h
	./stdafx.h

	./GameDialogBase.cpp
	./GameGUIBBoxGizmo.cpp
	./GameGUILib.cpp
	./GameGUIRotationGizmo.cpp
	./GameGUITranslationGizmo.cpp
	./GameGUIUtil.cpp
	./GameWindowBase.cpp
	./stdafx.cpp
)

ADD_LIBRARY ( GameGUILib ${SRCS_GAMEGUILIB} )
