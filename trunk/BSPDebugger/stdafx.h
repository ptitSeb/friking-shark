#ifndef __STDAFX__
#define __STDAFX__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "GameRunTimeLib.h"
#include "VectorLib.h"
#include "GameGUI.h"
#include "GameGUILib.h"
#include "GameGraphics.h"
#include "GameManagers.h"
#include "BSPDebugger.h"

DECLARE_CUSTOM_WRAPPER1(CFrameManagerWrapper,IFrameManager,m_piFrameManager)
DECLARE_CUSTOM_WRAPPER1(CGenericViewportWrapper,IGenericViewport,m_piViewport)
DECLARE_CUSTOM_WRAPPER1(CRenderWrapper,IGenericRender,m_piRender)
DECLARE_CUSTOM_WRAPPER1(CViewportWrapper,IGenericViewport,m_piViewport)
DECLARE_CUSTOM_WRAPPER1(CButtonWrapper,IGameGUIButton,m_piButton)
DECLARE_CUSTOM_WRAPPER1(CLabelWrapper,IGameGUILabel,m_piLabel)
DECLARE_CUSTOM_WRAPPER1(CModelWrapper,IGenericModel,m_piModel)

#endif