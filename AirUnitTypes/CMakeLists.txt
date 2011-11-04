SET (SRCS_AIRUNITTYPES

	./AirUnitTypes/AirUnitTypes.h
	./AirUnitTypes/BomberType.h
	./AirUnitTypes/FighterType.h
	./AirUnitTypes/PlayerType.h
	./AirUnitTypes/resource.h
	./AirUnitTypes/stdafx.h

	./AirUnitTypes/AirUnitTypes.cpp
	./AirUnitTypes/BomberType.cpp
	./AirUnitTypes/FighterType.cpp
	./AirUnitTypes/PlayerType.cpp
	./AirUnitTypes/stdafx.cpp
)

IF(ANDROID)
	ADD_LIBRARY ( AirUnitTypes ${SRCS_AIRUNITTYPES} )
ELSE()

	ADD_LIBRARY ( AirUnitTypes SHARED ${SRCS_AIRUNITTYPES} )

	TARGET_LINK_LIBRARIES (
			AirUnitTypes
		GameGUILib
		GameRunTimeLib
		VectorLib
		GameEntityLib
		${ANDROID_LOG_LIBRARY}
	)
ENDIF()
