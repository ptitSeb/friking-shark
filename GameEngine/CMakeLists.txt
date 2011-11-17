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


IF(ANDROID)

	LIST(APPEND SRCS_GAMEENGINE ./GameEngine/android_native_app_glue.h)
	LIST(APPEND SRCS_GAMEENGINE ./GameEngine/android_native_app_glue.c)

	ADD_LIBRARY ( GameEngine SHARED ${SRCS_GAMEENGINE} )
	TARGET_LINK_LIBRARIES (
			GameEngine
		GameRunTime
		GameManagers
		GameGraphics
		GameGUI
		GUISystems
		AnimationSystems
		BonusTypes
		AirUnitTypes
		GroundUnitTypes
		ParticleSystems
		PlayAreaElements
		WeaponTypes
		SoundSystems
		GameGUILib
		GameRunTimeLib
		VectorLib
		GameEntityLib
		android
		${PNG_LIBRARY}
		${OPENAL_LIBRARY}
		${OPENGL_LIBRARY}
		
		${EGL_LIBRARY}
		${ZLIB_LIBRARY}
		${ANDROID_LIBRARY}
		${ANDROID_LOG_LIBRARY}
		${TREMOLO_LIBRARY}
	)
	IF(ANDROID)
		IF(ANDROID_GLES1)
			TARGET_LINK_LIBRARIES (
					GameEngine
				${GLES1_LIBRARY}
			)
		ELSE()
			TARGET_LINK_LIBRARIES (
					GameEngine
				${GLES2_LIBRARY}
			)
		ENDIF()
	ENDIF(ANDROID)

ELSE()

	ADD_EXECUTABLE ( GameEngine MACOSX_BUNDLE ${SRCS_GAMEENGINE} )
	TARGET_LINK_LIBRARIES (
			GameEngine
		GameGUILib
		./GameRunTime
		GameRunTimeLib
		VectorLib
		GameEntityLib
		pthread
	)

ENDIF()

