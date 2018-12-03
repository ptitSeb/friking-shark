SET (SRCS_IATESTSYSTEMS

	./IATestSystems/IAEntityBase.h
	./IATestSystems/IATestMainWindow.h
	./IATestSystems/IATestSystems.h
	./IATestSystems/IATestUtils.h
	./IATestSystems/resource.h
	./IATestSystems/stdafx.h
	./IATestSystems/SteeringBehaviours.h
	./IATestSystems/TestScenarios.h

	./IATestSystems/IAEntityBase.cpp
	./IATestSystems/IATestMainWindow.cpp
	./IATestSystems/IATestSystems.cpp
	./IATestSystems/IATestUtils.cpp
	./IATestSystems/stdafx.cpp
	./IATestSystems/SteeringBehaviours.cpp
	./IATestSystems/TestScenarios.cpp
)

if(STATIC_BUILD)
	ADD_LIBRARY ( IATestSystems STATIC ${SRCS_IATESTSYSTEMS} )
else()
	ADD_LIBRARY ( IATestSystems SHARED ${SRCS_IATESTSYSTEMS} )
endif()

TARGET_LINK_LIBRARIES (
         IATestSystems
	GameGUILib
	GameRunTimeLib
	VectorLib
)

