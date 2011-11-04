SET (SRCS_GAMEENTITYLIB

	./GameEntityLib/EntityBase.h
	./GameEntityLib/EntityTypeBase.h
	./GameEntityLib/EntityUtilities.h
	./GameEntityLib/GameEntityLib.h
	./GameEntityLib/Route.h
	./GameEntityLib/SingletonWrapper.h
	./GameEntityLib/stdafx.h
	./GameEntityLib/SteeringBehaviours.h
	
	./GameEntityLib/EntityBase.cpp
	./GameEntityLib/EntityTypeBase.cpp
	./GameEntityLib/EntityUtilities.cpp
	./GameEntityLib/GameEntityLib.cpp
	./GameEntityLib/Route.cpp
	./GameEntityLib/stdafx.cpp
	./GameEntityLib/SteeringBehaviours.cpp
)

ADD_LIBRARY (GameEntityLib ${SRCS_GAMEENTITYLIB})
