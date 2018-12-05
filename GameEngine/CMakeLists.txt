SET (SRCS_GAMEENGINE

	./GameEngine/GameEngine.h
	./GameEngine/resource.h
	./GameEngine/StdAfx.h

	./GameEngine/GameEngine.cpp
	./GameEngine/StdAfx.cpp
)

if(NOT AMIGAOS4)
	link_libraries (
		${INCLUDED_LIBRARIES} rt
	)
endif()

if(MACOS)
ADD_EXECUTABLE ( GameEngine MACOSX_BUNDLE ${SRCS_GAMEENGINE} )
else()
ADD_EXECUTABLE ( GameEngine ${SRCS_GAMEENGINE} )
endif()

if(STATIC_BUILD)
	if(SDL)
	SET(LIBS_GAMEGRAPHICS 
		${SDL2_LIBRARIES}
	)
	else()
	SET(LIBS_GAMEGRAPHICS 
		${XINERAMA_LIBRARY}
		${XRANDR_LIBARY}
	)
	endif()

	if(GSALT)
	SET (LIBS_GAMEGRAPHICS ${LIBS_GAMEGRAPHICS} gsalt)
	endif()

TARGET_LINK_LIBRARIES (
		GameEngine
	-Wl,--whole-archive
	GameGUILib
	GameEntityLib
	VectorLib
	GameRunTimeLib
	GameRunTime
	IATestSystems
	BSPDebugger
	AirUnitTypes
	AnimationSystems
	BonusTypes
	EntityEditor
	FormationEditor
	GroundUnitTypes
	GameGraphics
	GameGUI
	GameManagers
	GUISystems
	ParticleSystems
	PlayAreaElements
	ScenarioEditor
	SoundSystems
	WeaponTypes
	-Wl,--no-whole-archive
	${PNG_LIBRARY}
	${OPENAL_LIBRARY}
	${OPENGL_LIBRARY}
	${LIBS_GAMEGRAPHICS}
	${ALUT_LIBRARY}
	${VORBISFILE_LIBRARY}
	${VORBIS_LIBRARY}
	${OGG_LIBRARY}
)
else()
if(SDL)
	SET(LIBS_GAMEENGINE ${SDL2_LIBRARIES})
else()
	SET(LIBS_GAMEENGINE "")
endif()
TARGET_LINK_LIBRARIES (
    	GameEngine
	GameGUILib
	GameEntityLib
	VectorLib
	GameRunTime
	GameRunTimeLib
	${PNG_LIBRARY}
	${OPENAL_LIBRARY}
	${OPENGL_LIBRARY}
	${LIBS_GAMEENGINE}
)
endif()