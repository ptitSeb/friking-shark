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


if(STATIC_BUILD)
	ADD_LIBRARY ( AirUnitTypes STATIC ${SRCS_AIRUNITTYPES} )
else()
	ADD_LIBRARY ( AirUnitTypes SHARED ${SRCS_AIRUNITTYPES} )
	
	TARGET_LINK_LIBRARIES (
		AirUnitTypes
		GameGUILib
		GameRunTimeLib
		VectorLib
		GameEntityLib
		${PNG_LIBRARY}
		${OPENAL_LIBRARY}
		${OPENGL_LIBRARY}
		)
endif()
		
