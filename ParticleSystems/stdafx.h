#ifndef __STDAFX__
#define __STDAFX__

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <list>
#include <crtdbg.h>
#include "VectorLib.h"
#include "GameLib.h"
#include "GameGraphics.h"
#include "OpenGLGraphics.h"

using namespace std;

DECLARE_CUSTOM_WRAPPER1(CGenericRenderWrapper,IGenericRender,m_piRender)
DECLARE_CUSTOM_WRAPPER1(CGenericLightWrapper,IGenericLight,m_piLight)

#endif