SET (SRCS_GAMERUNTIME
	./GameRunTime/GameRunTime.h
	./GameRunTime/InterfaceLeakAPI.h
	./GameRunTime/resource.h
	./GameRunTime/stdafx.h
	./GameRunTime/System.h
	./GameRunTime/SystemManager.h
	./GameRunTime/SystemModule.h

	./GameRunTime/GameRunTime.cpp
	./GameRunTime/InterfaceLeakAPI.cpp
	./GameRunTime/stdafx.cpp
	./GameRunTime/System.cpp
	./GameRunTime/SystemManager.cpp
	./GameRunTime/SystemModule.cpp
)

ADD_LIBRARY ( ./GameRunTime SHARED ${SRCS_GAMERUNTIME} )

TARGET_LINK_LIBRARIES (
         ./GameRunTime
	GameGUILib
	GameRunTimeLib
	VectorLib
)

