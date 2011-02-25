// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef __STDAFX__
#define __STDAFX__

#ifdef WIN32
  #ifdef GAMEMANAGERS_EXPORTS
  #define GAMEMANAGERS_API __declspec(dllexport)
  #else
  #define GAMEMANAGERS_API __declspec(dllimport)
  #endif

  #define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <list>
#include "VectorLib.h"
#include "GameLib.h"
#include "GameRunTimeLib.h"
#include "GameEntityLib.h"
#include "GameGraphics.h"
#include "OpenGLGraphics.h"

struct IEntityManager;

DECLARE_CUSTOM_WRAPPER1(CGameControllerWrapper,IGameController,m_piGameController)
DECLARE_CUSTOM_WRAPPER1(CEntityManagerWrapper,IEntityManager,m_piEntityManager)
DECLARE_CUSTOM_WRAPPER1(CGameRenderWrapper,IGameRender,m_piRender)
DECLARE_CUSTOM_WRAPPER1(CPlayAreaManagerWrapper,IPlayAreaManager,m_piPlayAreaManager)
DECLARE_CUSTOM_WRAPPER1(CEntityTypeWrapper,IEntityType,m_piEntityType)
DECLARE_CUSTOM_WRAPPER1(CFrameManagerWrapper,IFrameManager,m_piFrameManager)
DECLARE_CUSTOM_WRAPPER1(CWorldManagerWrapper,ITerrainDesign,m_piTerrain)

DECLARE_CUSTOM_WRAPPER2(CGenericTextureWrapper,IGenericTexture,m_piTexture,IOpenGLTexture,m_piGLTexture)
DECLARE_CUSTOM_WRAPPER1(CGenericFontWrapper,IGenericFont,m_piFont)
DECLARE_CUSTOM_WRAPPER2(CGenericModelWrapper,IGenericModel,m_piModel,IOpenGLModel,m_piGLModel)
DECLARE_CUSTOM_WRAPPER1(CGenericCameraWrapper,IGenericCamera,m_piCamera)
DECLARE_CUSTOM_WRAPPER1(CGenericRenderWrapper,IGenericRender,m_piRender)
DECLARE_CUSTOM_WRAPPER1(CGenericLightWrapper,IGenericLight,m_piLight)

using namespace std;

#endif