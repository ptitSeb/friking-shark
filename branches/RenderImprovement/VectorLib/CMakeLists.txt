SET (SRCS_VECTORLIB

	./VectorLib/3DSFiles.h
	./VectorLib/3DSTypes.h
	./VectorLib/ASEFiles.h
	./VectorLib/GBSFiles.h
	./VectorLib/GCMFiles.h
	./VectorLib/OBJFiles.h
	./VectorLib/StdAfx.h
	./VectorLib/VectorLib.h

	./VectorLib/3DSFiles.cpp
	./VectorLib/3DSTypes.cpp
	./VectorLib/ASEFiles.cpp
	./VectorLib/GBSFiles.cpp
	./VectorLib/GCMFiles.cpp
	./VectorLib/OBJFiles.cpp
	./VectorLib/StdAfx.cpp
	./VectorLib/VectorLib.cpp
)

ADD_LIBRARY ( VectorLib ${SRCS_VECTORLIB} )

