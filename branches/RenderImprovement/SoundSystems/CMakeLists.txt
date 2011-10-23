SET (SRCS_SOUNDSYSTEMS

	./SoundSystems/resource.h
	./SoundSystems/SoundSystemManager.h
	./SoundSystems/SoundSystems.h
	./SoundSystems/SoundType.h
	./SoundSystems/stdafx.h

	./SoundSystems/SoundSystemManager.cpp
	./SoundSystems/SoundSystems.cpp
	./SoundSystems/SoundType.cpp
	./SoundSystems/stdafx.cpp
)

ADD_LIBRARY ( SoundSystems SHARED ${SRCS_SOUNDSYSTEMS} )

TARGET_LINK_LIBRARIES (
         SoundSystems
	GameGUILib
	GameRunTimeLib
	VectorLib
	GameEntityLib
	${OPENAL_LIBRARY}
	${ALUT_LIBRARY}
	${VORBISFILE_LIBRARY}
	${VORBIS_LIBRARY}
	${OGG_LIBRARY}
)

