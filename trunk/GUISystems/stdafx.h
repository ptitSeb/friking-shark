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
#include "Gamegui.h"
#include "GUIDefs.h"
#include "GameGraphics.h"
#include "OpenGLGraphics.h"
// TODO: reference additional headers your program requires here
using namespace std;

struct IGameRender;

DECLARE_CUSTOM_WRAPPER1(CGameControllerWrapper,IGameController,m_piGameController);
DECLARE_CUSTOM_WRAPPER1(CGenericViewportWrapper,IOpenGLViewport,m_piViewport);
DECLARE_CUSTOM_WRAPPER1(CPlayAreaManagerWrapper,IPlayAreaManager,m_piPlayAreaManager);
DECLARE_CUSTOM_WRAPPER1(CFrameManagerWrapper,IFrameManager,m_piFrameManager);
DECLARE_CUSTOM_WRAPPER1(CEntityManagerWrapper,IEntityManager,m_piEntityManager);
DECLARE_CUSTOM_WRAPPER1(CGameRenderWrapper,IGameRender,m_piRender);
DECLARE_CUSTOM_WRAPPER2(CWorldManagerWrapper,ITerrainDesign,m_piTerrain,IWorldManager,m_piWorldManager);
