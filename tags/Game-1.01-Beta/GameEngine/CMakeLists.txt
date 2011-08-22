SET (SRCS_GAMEENGINE

	./GameEngine/GameEngine.h
	./GameEngine/resource.h
	./GameEngine/StdAfx.h

	./GameEngine/GameEngine.cpp
	./GameEngine/StdAfx.cpp
)

link_libraries (
	${INCLUDED_LIBRARIES}
)

ADD_EXECUTABLE ( GameEngine MACOSX_BUNDLE ${SRCS_GAMEENGINE} )

TARGET_LINK_LIBRARIES (
         GameEngine
	GameGUILib
	GameRunTimeLib
	VectorLib
	GameEntityLib
	./GameRunTime
	${PNG_LIBRARY}
	${OPENAL_LIBRARY}
	${OPENGL_LIBRARY}
)

