#ifndef __STDAFX__
#define __STDAFX__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "GameRunTimeLib.h"
#include "VectorLib.h"
#include "GameLib.h"
#include "GameGUI.h"
#include "GUIDefs.h"
#include "GameGraphics.h"

DECLARE_CUSTOM_WRAPPER1(CGameControllerWrapper,IGameController,m_piGameController)
DECLARE_CUSTOM_WRAPPER1(CGenericViewportWrapper,IGenericViewport,m_piViewport)
DECLARE_CUSTOM_WRAPPER1(CPlayAreaManagerWrapper,IPlayAreaManager,m_piPlayAreaManager)
DECLARE_CUSTOM_WRAPPER1(CFrameManagerWrapper,IFrameManager,m_piFrameManager)
DECLARE_CUSTOM_WRAPPER1(CEntityManagerWrapper,IEntityManager,m_piEntityManager)
DECLARE_CUSTOM_WRAPPER1(CGameRenderWrapper,IGameRender,m_piRender)
DECLARE_CUSTOM_WRAPPER2(CWorldManagerWrapper,ITerrainDesign,m_piTerrain,IWorldManager,m_piWorldManager)

#endif