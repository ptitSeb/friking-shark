SET (SRCS_GAMEENTITYLIB

	EntityBase.h
	EntityTypeBase.h
	EntityUtilities.h
	GameEntityLib.h
	Route.h
	SingletonWrapper.h
	stdafx.h
	SteeringBehaviours.h
	
	EntityBase.cpp
	EntityTypeBase.cpp
	EntityUtilities.cpp
	GameEntityLib.cpp
	Route.cpp
	stdafx.cpp
	SteeringBehaviours.cpp
)

ADD_LIBRARY (GameEntityLib ${SRCS_GAMEENTITYLIB})
