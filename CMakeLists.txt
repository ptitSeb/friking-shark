# $Revision: 1019 $ $Date: 2009-09-29 18:22:45 +1000 (Tue, 29 Sep 2009) $ $Author: dgorst $

PROJECT (FrikingShark)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.1)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

FIND_PACKAGE ( X11 REQUIRED )
FIND_PACKAGE ( OpenGL REQUIRED )
FIND_PACKAGE ( OpenAL REQUIRED )
FIND_PACKAGE ( PNG REQUIRED )
FIND_PACKAGE ( Vorbis REQUIRED )

# Libraries

INCLUDE_DIRECTORIES (
	./VectorLib 
	./GameManagers
	./GameEntityLib
	./GameGUI
	./GameGUILib
	./GameGraphics
	./GameRunTime
	./GameRunTimeLib
	./PlayAreaElements
	./AnimationSystems
	./ParticleSystems
	./SoundSystems
)

# Sources

INCLUDE ( ./AirUnitTypes/CMakeLists.txt )
INCLUDE ( ./AnimationSystems/CMakeLists.txt )
INCLUDE ( ./BSPDebugger/CMakeLists.txt )
INCLUDE ( ./BonusTypes/CMakeLists.txt )
INCLUDE ( ./EntityEditor/CMakeLists.txt )
INCLUDE ( ./FormationEditor/CMakeLists.txt )
INCLUDE ( ./GameEngine/CMakeLists.txt )
INCLUDE ( ./GameGraphics/CMakeLists.txt )
INCLUDE ( ./GameGUI/CMakeLists.txt )
INCLUDE ( ./GameManagers/CMakeLists.txt )
INCLUDE ( ./GameRunTime/CMakeLists.txt )
INCLUDE ( ./GroundUnitTypes/CMakeLists.txt )
INCLUDE ( ./GUISystems/CMakeLists.txt )
INCLUDE ( ./IATestSystems/CMakeLists.txt )
INCLUDE ( ./ParticleSystems/CMakeLists.txt )
INCLUDE ( ./PlayAreaElements/CMakeLists.txt )
INCLUDE ( ./ScenarioEditor/CMakeLists.txt )
INCLUDE ( ./SoundSystems/CMakeLists.txt )
INCLUDE ( ./WeaponTypes/CMakeLists.txt )

# Set the executable name.

#IF (NOT WINDOWS)
#	ADD_DEFINITIONS(-D_GNU_SOURCE=1 -D_REENTRANT -DGCC_SYMBOLS_PRIVATE_EXTERN=0)
#ELSE (NOT WINDOWS)
#	ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS )
#ENDIF (NOT WINDOWS)

MESSAGE (STATUS "Linking with ${INCLUDED_LIBRARIES}.")
MESSAGE (STATUS "Include directory paths are ${INCLUDE_DIRECTORIES}.")
MESSAGE (STATUS "Using source files ${SRCS}.")

link_libraries (
	${INCLUDED_LIBRARIES}
)

# Main build details.

ADD_SUBDIRECTORY (
./GameGUILib
)
ADD_SUBDIRECTORY (
./GameRunTimeLib
)
ADD_SUBDIRECTORY (
./VectorLib
)
ADD_SUBDIRECTORY (
./GameEntityLib
)

ADD_EXECUTABLE ( ./Demo/Bin/GameEngine MACOSX_BUNDLE ${SRCS} )

TARGET_LINK_LIBRARIES (
         ./Demo/Bin/GameEngine
	GameGUILib
	GameRunTimeLib
	VectorLib
	GameEntityLib
	${PNG_LIBRARY}
	${OPENAL_LIBRARY}
	${OPENGL_LIBRARY}
)
