// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef __STDAFX__
#define __STDAFX__
#pragma once

#ifdef WIN32
#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <list>
#include "VectorLib.h"
#include "GameLib.h"
#include "GameGUI.h"
#include "GameGUILib.h"
#include "GameGraphics.h"
#include "OpenGLGraphics.h"
#include "GameManagers.h"
#include "ScenarioEditor.h"

using namespace std;

DECLARE_CUSTOM_WRAPPER1(CGenericViewportWrapper,IOpenGLViewport,m_piViewport)
DECLARE_CUSTOM_WRAPPER1(CFrameManagerWrapper,IFrameManager,m_piFrameManager)
DECLARE_CUSTOM_WRAPPER1(CGameControllerWrapper,IGameController,m_piGameController)
DECLARE_CUSTOM_WRAPPER2(CPlayAreaManagerWrapper,IPlayAreaManager,m_piPlayAreaManager,IPlayAreaDesign,m_piPlayAreaDesign)
DECLARE_CUSTOM_WRAPPER1(CEntityManagerWrapper,IEntityManager,m_piEntityManager)
DECLARE_CUSTOM_WRAPPER1(CGameRenderWrapper,IGameRender,m_piRender)
DECLARE_CUSTOM_WRAPPER2(CWorldManagerWrapper,ITerrainDesign,m_piTerrain,IWorldManager,m_piWorldManager)

DECLARE_CUSTOM_WRAPPER1(CRenderWrapper,IGenericRender,m_piRender)
DECLARE_CUSTOM_WRAPPER1(CViewportWrapper,IGenericViewport,m_piViewport)
DECLARE_CUSTOM_WRAPPER1(CButtonWrapper,IGameGUIButton,m_piButton)
DECLARE_CUSTOM_WRAPPER1(CLabelWrapper,IGameGUILabel,m_piLabel)
DECLARE_CUSTOM_WRAPPER1(CObjectLabelWrapper,IScenarioEditorObjectLabel,m_piLabel)
DECLARE_CUSTOM_WRAPPER2(COpenGLModelWrapper,IGenericModel,m_piModel,IOpenGLModel,m_piGLModel)

#endif