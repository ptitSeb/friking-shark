SET (SRCS_GAMEENGINE

	./GameEngine/GameEngine.h
	./GameEngine/resource.h
	./GameEngine/StdAfx.h

	./GameEngine/GameEngine.cpp
	./GameEngine/StdAfx.cpp
)

link_libraries (
	${INCLUDED_LIBRARIES} rt
)

if(MACOS)
ADD_EXECUTABLE ( GameEngine MACOSX_BUNDLE ${SRCS_GAMEENGINE} )
else()
ADD_EXECUTABLE ( GameEngine ${SRCS_GAMEENGINE} )
endif()

if(SDL)
	SET(LIBS_GAMEENGINE ${SDL2_LIBRARIES})
else()
	SET(LIBS_GAMEENGINE "")
endif()

TARGET_LINK_LIBRARIES (
         GameEngine
	GameGUILib
	GameRunTimeLib
	VectorLib
	GameEntityLib
	GameRunTime
	${PNG_LIBRARY}
	${OPENAL_LIBRARY}
	${OPENGL_LIBRARY}
	${LIBS_GAMEENGINE}
)
