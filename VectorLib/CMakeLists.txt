SET (SRCS_VECTORLIB

./3DSFiles.h
./3DSTypes.h
./ASEFiles.h
./GBSFiles.h
./GCMFiles.h
# ./QuakeFiles.h
./StdAfx.h
./VectorLib.h

./3DSFiles.cpp
./3DSTypes.cpp
./ASEFiles.cpp
./GBSFiles.cpp
./GCMFiles.cpp
# ./QuakeFiles.cpp
./StdAfx.cpp
./VectorLib.cpp
)

ADD_LIBRARY ( VectorLib ${SRCS_VECTORLIB} )

