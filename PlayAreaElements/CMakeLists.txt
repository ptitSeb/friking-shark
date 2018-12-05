SET (SRCS_PLAYAREAELEMENTS

	./PlayAreaElements/FormationType.h
	./PlayAreaElements/PlayAreaCheckPoint.h
	./PlayAreaElements/PlayAreaElementBase.h
	./PlayAreaElements/PlayAreaElements.h
	./PlayAreaElements/PlayAreaEntity.h
	./PlayAreaElements/PlayAreaFormation.h
	./PlayAreaElements/resource.h
	./PlayAreaElements/stdafx.h

	./PlayAreaElements/FormationType.cpp
	./PlayAreaElements/PlayAreaCheckPoint.cpp
	./PlayAreaElements/PlayAreaElementBase.cpp
	./PlayAreaElements/PlayAreaElements.cpp
	./PlayAreaElements/PlayAreaEntity.cpp
	./PlayAreaElements/PlayAreaFormation.cpp
	./PlayAreaElements/stdafx.cpp
)

if(STATIC_BUILD)
	ADD_LIBRARY ( PlayAreaElements STATIC ${SRCS_PLAYAREAELEMENTS} )
else()
	ADD_LIBRARY ( PlayAreaElements SHARED ${SRCS_PLAYAREAELEMENTS} )

TARGET_LINK_LIBRARIES (
         PlayAreaElements
	GameGUILib
	GameRunTimeLib
	VectorLib
	GameEntityLib
	${PNG_LIBRARY}
	${OPENAL_LIBRARY}
	${OPENGL_LIBRARY}
)

endif()
