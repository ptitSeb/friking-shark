// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <list>
#include <stack>
#include <crtdbg.h>
#include "VectorLib.h"
#include "GameLib.h"
#include "GameGraphics.h"
#include "OpenGLGraphics.h"

// TODO: reference additional headers your program requires here
using namespace std;

DECLARE_CUSTOM_WRAPPER1(COpenGLViewportWrapper,IOpenGLViewport,m_piViewport);
DECLARE_CUSTOM_WRAPPER1(CGenericTextureWrapper,IGenericTexture,m_piTexture);
DECLARE_CUSTOM_WRAPPER1(CGenericLightWrapper,IGenericLight,m_piLight);
DECLARE_CUSTOM_WRAPPER1(CGenericShaderWrapper,IGenericShader,m_piShader);