// GameGraphics.cpp : Defines the entry point for the DLL application.
//

#include "./stdafx.h"
#include <GL/gl.h>
#include "GameGraphics.h"
#include "OpenGLGraphics.h"
#include "OpenGLCamera.h"
#include "OpenGLLight.h"
#include "OpenGLRender.h"
#include "OpenGLTexture.h"
#include "OpenGLModel.h"
#include "OpenGLViewport.h"
#include "OpenGLFont.h"
#include "OpenGLShader.h"

BEGIN_SYSTEM_MODULE()
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(COpenGLRender,"Render");
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(COpenGLTexture,"Texture");
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(COpenGLModel,"Model");
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(COpenGLFont,"Font");
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(COpenGLViewport,"Viewport");
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(COpenGLCamera,"Camera");
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(COpenGLLight,"Light");
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(COpenGLShader,"Shader");
END_SYSTEM_MODULE()

