SET (SRCS_GROUNDUNITTYPES

	./GroundUnitTypes/DreadnoughtTowerType.h
	./GroundUnitTypes/GroundBossHatchType.h
	./GroundUnitTypes/GroundBossType.h
	./GroundUnitTypes/GroundUnitTypes.h
	./GroundUnitTypes/resource.h
	./GroundUnitTypes/StaticEntityType.h
	./GroundUnitTypes/StaticStructureType.h
	./GroundUnitTypes/stdafx.h
	./GroundUnitTypes/TurretType.h
	./GroundUnitTypes/VehicleType.h

	./GroundUnitTypes/DreadnoughtTowerType.cpp
	./GroundUnitTypes/GroundBossHatchType.cpp
	./GroundUnitTypes/GroundBossType.cpp
	./GroundUnitTypes/GroundUnitTypes.cpp
	./GroundUnitTypes/StaticEntityType.cpp
	./GroundUnitTypes/StaticStructureType.cpp
	./GroundUnitTypes/stdafx.cpp
	./GroundUnitTypes/TurretType.cpp
	./GroundUnitTypes/VehicleType.cpp
)

IF(ANDROID)
	ADD_LIBRARY ( GroundUnitTypes ${SRCS_GROUNDUNITTYPES} )
ELSE()
	ADD_LIBRARY ( GroundUnitTypes SHARED ${SRCS_GROUNDUNITTYPES} )

	TARGET_LINK_LIBRARIES (
			GroundUnitTypes
		GameGUILib
		GameRunTimeLib
		VectorLib
		GameEntityLib
		${ANDROID_LOG_LIBRARY}
	)
ENDIF()
