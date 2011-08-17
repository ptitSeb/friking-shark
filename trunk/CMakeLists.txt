# $Revision$ $Date$ $Author$

PROJECT (frikingshark)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.4)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

FIND_PACKAGE ( X11 REQUIRED )
FIND_PACKAGE ( OpenGL REQUIRED )
FIND_PACKAGE ( OpenAL REQUIRED )
FIND_PACKAGE ( PNG REQUIRED )
FIND_LIBRARY (ALUT_LIBRARY alut /usr/local/lib /usr/lib /sw/lib)
FIND_LIBRARY (VORBISFILE_LIBRARY vorbisfile /usr/local/lib /usr/lib /sw/lib)
FIND_LIBRARY (VORBIS_LIBRARY vorbis /usr/local/lib /usr/lib /sw/lib)
FIND_LIBRARY (OGG_LIBRARY ogg /usr/local/lib /usr/lib /sw/lib)

# FIND_PACKAGE ( Vorbis REQUIRED )
set(CMAKE_SHARED_LIBRARY_PREFIX )
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/Demo/Bin/Release/)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/Demo/Bin/Release/)

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

#IF (NOT WINDOWS)
#	ADD_DEFINITIONS(-D_GNU_SOURCE=1 -D_REENTRANT -DGCC_SYMBOLS_PRIVATE_EXTERN=0)
#ELSE (NOT WINDOWS)
#	ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS )
#ENDIF (NOT WINDOWS)

# -fPIC
IF(UNIX AND NOT WIN32)
  FIND_PROGRAM(CMAKE_UNAME uname /bin /usr/bin /usr/local/bin )
  IF(CMAKE_UNAME)
    EXEC_PROGRAM(uname ARGS -m OUTPUT_VARIABLE CMAKE_SYSTEM_PROCESSOR)
    SET(CMAKE_SYSTEM_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR} CACHE INTERNAL
"processor type (i386 and x86_64)")
    IF(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
      ADD_DEFINITIONS(-fPIC)
    ENDIF(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
  ENDIF(CMAKE_UNAME)
ENDIF(UNIX AND NOT WIN32)


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

install(TARGETS GameEngine ./GameRunTime GameGraphics GameGUI AirUnitTypes AnimationSystems BonusTypes GameManagers GroundUnitTypes GUISystems ParticleSystems PlayAreaElements SoundSystems WeaponTypes EntityEditor FormationEditor ScenarioEditor DESTINATION share/frikingshark/Bin/Release)
install(DIRECTORY Demo/Resources/Models/ DESTINATION share/frikingshark/Resources/Models/ FILES_MATCHING PATTERN "*.gbs")
install(DIRECTORY Demo/Resources/Models/ DESTINATION share/frikingshark/Resources/Models/ FILES_MATCHING PATTERN "*.gcm")
install(DIRECTORY Demo/Resources/Sounds/ DESTINATION share/frikingshark/Resources/Sounds/ FILES_MATCHING PATTERN "*.ogg")
install(DIRECTORY Demo/Resources/Textures/ DESTINATION share/frikingshark/Resources/Textures/ FILES_MATCHING PATTERN "*.png")
install(DIRECTORY Demo/Resources/Scripts/ DESTINATION share/frikingshark/Resources/Scripts/ FILES_MATCHING PATTERN "*.cfg")
install(DIRECTORY Demo/Resources/Shaders/ DESTINATION share/frikingshark/Resources/Shaders/ FILES_MATCHING PATTERN "*.c")
install(DIRECTORY Demo/Resources/GUI/ DESTINATION share/frikingshark/Resources/GUI/ FILES_MATCHING PATTERN "*.cfg")
install(DIRECTORY Demo/Resources/GUI/ DESTINATION share/frikingshark/Resources/GUI/ FILES_MATCHING PATTERN "*.png")
install(DIRECTORY Demo/Resources/ScenarioEditor/ DESTINATION share/frikingshark/Resources/ScenarioEditor/ FILES_MATCHING PATTERN "*.cfg")
install(DIRECTORY Demo/Resources/ScenarioEditor/ DESTINATION share/frikingshark/Resources/ScenarioEditor/ FILES_MATCHING PATTERN "*.png")
install(DIRECTORY Demo/Resources/FormationEditor/ DESTINATION share/frikingshark/Resources/FormationEditor/ FILES_MATCHING PATTERN "*.cfg")
install(DIRECTORY Demo/Resources/FormationEditor/ DESTINATION share/frikingshark/Resources/FormationEditor/ FILES_MATCHING PATTERN "*.png")
install(DIRECTORY Demo/Resources/EntityEditor/ DESTINATION share/frikingshark/Resources/EntityEditor/ FILES_MATCHING PATTERN "*.cfg")
install(DIRECTORY Demo/Resources/EntityEditor/ DESTINATION share/frikingshark/Resources/EntityEditor/ FILES_MATCHING PATTERN "*.png")
install(DIRECTORY Demo/Resources/ DESTINATION share/frikingshark/Resources/ FILES_MATCHING PATTERN "Level*.ges")
install(DIRECTORY Demo/Resources/ DESTINATION share/frikingshark/Resources/ FILES_MATCHING PATTERN "Background.ges")
install(DIRECTORY Demo/Player/ DESTINATION share/frikingshark/Player/ FILES_MATCHING PATTERN "*.cfg")
install(FILES     License.txt DESTINATION share/doc/frikingshark/ RENAME copyright)
install(FILES     Demo/Linux/frikingshark DESTINATION games PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE)
install(FILES     Demo/Linux/frikingshark.xpm DESTINATION share/pixmaps/)
install(FILES     Demo/Linux/frikingshark.desktop DESTINATION share/applications/)
install(FILES     Demo/Linux/menu DESTINATION share/menu/ RENAME frikingshark )

include (InstallRequiredSystemLibraries)
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set (CPACK_PACKAGE_VERSION_MAJOR "1")
set (CPACK_PACKAGE_VERSION_MINOR "0")
set (CPACK_PACKAGE_CONTACT "javiermartingarcia@gmail.com")
set (CPACK_PACKAGE_INSTALL_DIRECTORY "frikingshark")
set (CPACK_GENERATOR "DEB")
include (CPack)
