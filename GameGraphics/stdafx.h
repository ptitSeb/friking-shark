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
#include "GameGraphics.h"
#include "OpenGLGraphics.h"

#define GL_GLEXT_PROTOTYPES
#include "GLee.h"
#include <GL/glu.h>

DECLARE_CUSTOM_WRAPPER1(CGenericTextureWrapper,IGenericTexture,m_piTexture)
DECLARE_CUSTOM_WRAPPER1(CGenericLightWrapper,IGenericLight,m_piLight)
DECLARE_CUSTOM_WRAPPER1(CGenericShaderWrapper,IGenericShader,m_piShader)

#endif
