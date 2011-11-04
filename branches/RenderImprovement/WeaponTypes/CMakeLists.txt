SET (SRCS_WEAPONTYPES

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

IF(ANDROID)
	ADD_LIBRARY ( WeaponTypes ${SRCS_WEAPONTYPES} )
ELSE()
	ADD_LIBRARY ( WeaponTypes SHARED ${SRCS_WEAPONTYPES} )

	TARGET_LINK_LIBRARIES (
			WeaponTypes
		GameGUILib
		GameRunTimeLib
		VectorLib
		GameEntityLib
		${ANDROID_LOG_LIBRARY}
	)
ENDIF()
