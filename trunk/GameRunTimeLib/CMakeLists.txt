SET (SRCS_GAMERUNTIMELIB

./ConfigFile.h
./EventBase.h
./GameRunTimeLib.h
./Persistency.h
./PersistentTypes.h
./PlatformDependent.h
./stdafx.h
./SystemLoaderHelper.h
./SystemModuleHelpers.h
./SystemObjectBase.h
./SystemObjectWrapper.h
./SystemUnknownBase.h

./ConfigFile.cpp
./EventBase.cpp
./Persistency.cpp
./PersistentTypes.cpp
./PlatformDependent.cpp
./stdafx.cpp
./SystemLoaderHelper.cpp
./SystemModuleHelpers.cpp
./SystemObjectBase.cpp
./SystemObjectWrapper.cpp
./SystemUnknownBase.cpp
)

ADD_LIBRARY ( GameRunTimeLib ${SRCS_GAMERUNTIMELIB} )

