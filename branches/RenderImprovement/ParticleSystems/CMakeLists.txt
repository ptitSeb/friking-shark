SET (SRCS_PARTICLESYSTEMS


	./ParticleSystems/LightParticleType.h
	./ParticleSystems/ParticleColorModifierType.h
	./ParticleSystems/ParticleEmitterModifierType.h
	./ParticleSystems/ParticleEmitterType.h
	./ParticleSystems/ParticleModifierType.h
	./ParticleSystems/ParticleSizeModifierType.h
	./ParticleSystems/ParticleSphereEmitterType.h
	./ParticleSystems/ParticleSystems.h
	./ParticleSystems/ParticleSystemType.h
	./ParticleSystems/ParticleWhirlpoolModifierType.h
	./ParticleSystems/PointParticleType.h
	./ParticleSystems/resource.h
	./ParticleSystems/stdafx.h
	./ParticleSystems/TextureParticleType.h

	./ParticleSystems/LightParticleType.cpp
	./ParticleSystems/ParticleColorModifierType.cpp
	./ParticleSystems/ParticleEmitterModifierType.cpp
	./ParticleSystems/ParticleEmitterType.cpp
	./ParticleSystems/ParticleModifierType.cpp
	./ParticleSystems/ParticleSizeModifierType.cpp
	./ParticleSystems/ParticleSphereEmitterType.cpp
	./ParticleSystems/ParticleSystems.cpp
	./ParticleSystems/ParticleSystemType.cpp
	./ParticleSystems/ParticleWhirlpoolModifierType.cpp
	./ParticleSystems/PointParticleType.cpp
	./ParticleSystems/stdafx.cpp
	./ParticleSystems/TextureParticleType.cpp
)

IF(ANDROID)
	ADD_LIBRARY ( ParticleSystems ${SRCS_PARTICLESYSTEMS} )
ELSE()
	ADD_LIBRARY ( ParticleSystems SHARED ${SRCS_PARTICLESYSTEMS} )

	TARGET_LINK_LIBRARIES (
			ParticleSystems
		GameGUILib
		GameRunTimeLib
		VectorLib
		GameEntityLib
		${ANDROID_LOG_LIBRARY}
	)
ENDIF()
