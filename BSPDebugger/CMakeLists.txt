SET (SRCS_BSPDEBUGGER

	./BSPDebugger/BSPDebugger.h
	./BSPDebugger/BSPDebuggerMainWindow.h
	./BSPDebugger/stdafx.h

	./BSPDebugger/BSPDebugger.cpp
	./BSPDebugger/BSPDebuggerMainWindow.cpp
	./BSPDebugger/stdafx.cpp
)


if(STATIC_BUILD)
	ADD_LIBRARY ( BSPDebugger STATIC ${SRCS_BSPDEBUGGER} )
else()
	ADD_LIBRARY ( BSPDebugger SHARED ${SRCS_BSPDEBUGGER} )

TARGET_LINK_LIBRARIES (
         BSPDebugger
	GameGUILib
	GameRunTimeLib
	VectorLib
)

endif()
