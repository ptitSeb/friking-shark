SET (SRCS_GAMEMANAGERS

	./GameManagers/EntityManager.h
	./GameManagers/FrameManager.h
	./GameManagers/GameController.h
	./GameManagers/GameManagers.h
	./GameManagers/MusicManager.h
	./GameManagers/PhysicsManager.h
	./GameManagers/PlayAreaManager.h
	./GameManagers/PlayerManager.h
	./GameManagers/PlayerProfile.h
	./GameManagers/resource.h
	./GameManagers/stdafx.h
	./GameManagers/WorldEntity.h
	./GameManagers/WorldManager.h

	./GameManagers/EntityManager.cpp
	./GameManagers/FrameManager.cpp
	./GameManagers/GameController.cpp
	./GameManagers/GameManagers.cpp
	./GameManagers/MusicManager.cpp
	./GameManagers/PhysicsManager.cpp
	./GameManagers/PlayAreaManager.cpp
	./GameManagers/PlayerManager.cpp
	./GameManagers/PlayerProfile.cpp
	./GameManagers/stdafx.cpp
	./GameManagers/WorldEntity.cpp
	./GameManagers/WorldManager.cpp
)

if(STATIC_BUILD)
	ADD_LIBRARY ( GameManagers STATIC ${SRCS_GAMEMANAGERS} )
else()
	ADD_LIBRARY ( GameManagers SHARED ${SRCS_GAMEMANAGERS} )

TARGET_LINK_LIBRARIES (
    GameManagers
	GameGUILib
	GameRunTimeLib
	VectorLib
	GameEntityLib
	${PNG_LIBRARY}
	${OPENAL_LIBRARY}
	${OPENGL_LIBRARY}
)

endif()
