SET (SRCS_GAMEGRAPHICS

	./GameGraphics/GameGraphics.h
	./GameGraphics/GameKeys.h
	./GameGraphics/GLee.h
	./GameGraphics/OpenGLCamera.h
	./GameGraphics/OpenGLFont.h
	./GameGraphics/OpenGLGraphics.h
	./GameGraphics/OpenGLLight.h
	./GameGraphics/OpenGLModel.h
	./GameGraphics/OpenGLRender.h
	./GameGraphics/OpenGLShader.h
	./GameGraphics/OpenGLTexture.h
	./GameGraphics/OpenGLViewport.h
	./GameGraphics/resource.h
	./GameGraphics/stdafx.h

	./GameGraphics/GLee.c
	./GameGraphics/GameGraphics.cpp
	./GameGraphics/OpenGLCamera.cpp
	./GameGraphics/OpenGLFont.cpp
	./GameGraphics/OpenGLLight.cpp
	./GameGraphics/OpenGLModel.cpp
	./GameGraphics/OpenGLRender.cpp
	./GameGraphics/OpenGLShader.cpp
	./GameGraphics/OpenGLTexture.cpp
	./GameGraphics/OpenGLViewport.cpp
	./GameGraphics/stdafx.cpp
)

ADD_LIBRARY ( GameGraphics SHARED ${SRCS_GAMEGRAPHICS} )

TARGET_LINK_LIBRARIES (
         GameGraphics
	GameGUILib
	GameRunTimeLib
	VectorLib
	GameEntityLib
	${PNG_LIBRARY}
	${OPENAL_LIBRARY}
	${OPENGL_LIBRARY}
)

