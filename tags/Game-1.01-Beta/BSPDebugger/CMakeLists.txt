SET (SRCS_BSPDEBUGGER

	./BSPDebugger/BSPDebugger.h
	./BSPDebugger/BSPDebuggerMainWindow.h
	./BSPDebugger/stdafx.h

	./BSPDebugger/BSPDebugger.cpp
	./BSPDebugger/BSPDebuggerMainWindow.cpp
	./BSPDebugger/stdafx.cpp
)


ADD_LIBRARY ( BSPDebugger SHARED ${SRCS_BSPDEBUGGER} )

TARGET_LINK_LIBRARIES (
         BSPDebugger
	GameGUILib
	GameRunTimeLib
	VectorLib
)

