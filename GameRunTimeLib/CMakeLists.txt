SET (SRCS_GAMERUNTIMELIB

	./GameRunTimeLib/ConfigFile.h
	./GameRunTimeLib/EventBase.h
	./GameRunTimeLib/GameRunTimeLib.h
	./GameRunTimeLib/Persistency.h
	./GameRunTimeLib/PersistentTypes.h
	./GameRunTimeLib/PlatformDependent.h
	./GameRunTimeLib/TimeMeter.h
	./GameRunTimeLib/stdafx.h
	./GameRunTimeLib/SystemLoaderHelper.h
	./GameRunTimeLib/SystemModuleHelpers.h
	./GameRunTimeLib/SystemObjectBase.h
	./GameRunTimeLib/SystemObjectWrapper.h
	./GameRunTimeLib/SystemUnknownBase.h

	./GameRunTimeLib/ConfigFile.cpp
	./GameRunTimeLib/EventBase.cpp
	./GameRunTimeLib/Persistency.cpp
	./GameRunTimeLib/PersistentTypes.cpp
	./GameRunTimeLib/PlatformDependent.cpp
	./GameRunTimeLib/TimeMeter.cpp
	./GameRunTimeLib/stdafx.cpp
	./GameRunTimeLib/SystemLoaderHelper.cpp
	./GameRunTimeLib/SystemModuleHelpers.cpp
	./GameRunTimeLib/SystemObjectBase.cpp
	./GameRunTimeLib/SystemObjectWrapper.cpp
	./GameRunTimeLib/SystemUnknownBase.cpp
)

ADD_LIBRARY (GameRunTimeLib STATIC ${SRCS_GAMERUNTIMELIB})

