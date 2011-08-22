SET (SRCS_SOUNDSYSTEMS

	./WeaponTypes/BombProjectileType.h
	./WeaponTypes/BulletProjectileType.h
	./WeaponTypes/HomingMissileProjectileType.h
	./WeaponTypes/ProjectileLauncherType.h
	./WeaponTypes/resource.h
	./WeaponTypes/stdafx.h
	./WeaponTypes/WeaponTypes.h

	./WeaponTypes/BombProjectileType.cpp
	./WeaponTypes/BulletProjectileType.cpp
	./WeaponTypes/HomingMissileProjectileType.cpp
	./WeaponTypes/ProjectileLauncherType.cpp
	./WeaponTypes/stdafx.cpp
	./WeaponTypes/WeaponTypes.cpp
)

ADD_LIBRARY ( WeaponTypes SHARED ${SRCS_SOUNDSYSTEMS} )

TARGET_LINK_LIBRARIES (
         WeaponTypes
	GameGUILib
	GameRunTimeLib
	VectorLib
	GameEntityLib
	${PNG_LIBRARY}
	${OPENAL_LIBRARY}
	${OPENGL_LIBRARY}
)

