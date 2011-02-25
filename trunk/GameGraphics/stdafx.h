#ifndef _STDAFX_
#define _STDAFX_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <list>
#include <stack>
#include "GameRunTimeLib.h"
#include "VectorLib.h"
#include "GameLib.h"
#include "GameGraphics.h"
#include "OpenGLGraphics.h"

#define GL_GLEXT_PROTOTYPES
#include "GLee.h"
#ifdef WIN32
#include <GL/glu.h>
#else 
#include <GL/glut.h>
#endif

DECLARE_CUSTOM_WRAPPER1(COpenGLViewportWrapper,IOpenGLViewport,m_piViewport)
DECLARE_CUSTOM_WRAPPER1(CGenericTextureWrapper,IGenericTexture,m_piTexture)
DECLARE_CUSTOM_WRAPPER1(CGenericLightWrapper,IGenericLight,m_piLight)
DECLARE_CUSTOM_WRAPPER1(CGenericShaderWrapper,IGenericShader,m_piShader)

#endif