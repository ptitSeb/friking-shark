SET (SRCS_BONUSTYPES

	./BonusTypes/BombBonusType.h
	./BonusTypes/BonusTypes.h
	./BonusTypes/LiveBonusType.h
	./BonusTypes/PointBonusType.h
	./BonusTypes/resource.h
	./BonusTypes/stdafx.h
	./BonusTypes/WeaponUpgradeBonusType.h

	./BonusTypes/BombBonusType.cpp
	./BonusTypes/BonusTypes.cpp
	./BonusTypes/LiveBonusType.cpp
	./BonusTypes/PointBonusType.cpp
	./BonusTypes/stdafx.cpp
	./BonusTypes/WeaponUpgradeBonusType.cpp
)

IF(ANDROID)
	ADD_LIBRARY ( BonusTypes ${SRCS_BONUSTYPES} )
ELSE()
	ADD_LIBRARY ( BonusTypes SHARED ${SRCS_BONUSTYPES} )

	TARGET_LINK_LIBRARIES (
			BonusTypes
		GameGUILib
		GameRunTimeLib
		VectorLib
		GameEntityLib
		${ANDROID_LOG_LIBRARY}
	)
ENDIF()