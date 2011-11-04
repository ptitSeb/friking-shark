SET (SRCS_BSPDEBUGGER

	./BSPDebugger/BSPDebugger.h
	./BSPDebugger/BSPDebuggerMainWindow.h
	./BSPDebugger/stdafx.h

	./BSPDebugger/BSPDebugger.cpp
	./BSPDebugger/BSPDebuggerMainWindow.cpp
	./BSPDebugger/stdafx.cpp
)

IF(ANDROID)
	ADD_LIBRARY ( BSPDebugger ${SRCS_BSPDEBUGGER} )
ELSE()
	ADD_LIBRARY ( BSPDebugger SHARED ${SRCS_BSPDEBUGGER} )

	TARGET_LINK_LIBRARIES (
			BSPDebugger
		GameGUILib
		GameRunTimeLib
		VectorLib
		${ANDROID_LOG_LIBRARY}
	)
ENDIF()

