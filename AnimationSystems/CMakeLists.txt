SET (SRCS_ANIMATIONSYSTEMS


	./AnimationSystems/AnimationObjectBase.h
	./AnimationSystems/AnimationSystems.h
	./AnimationSystems/AnimationType.h
	./AnimationSystems/EntityAnimationObjectType.h
	./AnimationSystems/EventAnimationObjectType.h
	./AnimationSystems/ModelAnimationObjectType.h
	./AnimationSystems/ParticleSystemAnimationObjectType.h
	./AnimationSystems/resource.h
	./AnimationSystems/SoundAnimationObjectType.h
	./AnimationSystems/stdafx.h
	./AnimationSystems/TextAnimationObjectType.h

	./AnimationSystems/AnimationObjectBase.cpp
	./AnimationSystems/AnimationSystems.cpp
	./AnimationSystems/AnimationType.cpp
	./AnimationSystems/EntityAnimationObjectType.cpp
	./AnimationSystems/EventAnimationObjectType.cpp
	./AnimationSystems/ModelAnimationObjectType.cpp
	./AnimationSystems/ParticleSystemAnimationObjectType.cpp
	./AnimationSystems/SoundAnimationObjectType.cpp
	./AnimationSystems/stdafx.cpp
	./AnimationSystems/TextAnimationObjectType.cpp
)

if(STATIC_BUILD)
	ADD_LIBRARY ( AnimationSystems STATIC ${SRCS_ANIMATIONSYSTEMS} )
else()
	ADD_LIBRARY ( AnimationSystems SHARED ${SRCS_ANIMATIONSYSTEMS} )

TARGET_LINK_LIBRARIES (
         AnimationSystems
	GameGUILib
	GameRunTimeLib
	VectorLib
	GameEntityLib
	${PNG_LIBRARY}
	${OPENAL_LIBRARY}
	${OPENGL_LIBRARY}
)

endif()
