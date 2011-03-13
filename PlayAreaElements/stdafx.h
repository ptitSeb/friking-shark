#ifndef __STDAFX__
#define __STDAFX__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "GameRunTimeLib.h"
#include "VectorLib.h"
#include "GameEntityLib.h"
#include "SingletonWrapper.h"
#include "PlayAreaElements.h"

DECLARE_CUSTOM_WRAPPER1(CEntityTypeWrapper,IEntityType,m_piEntityType)
DECLARE_CUSTOM_WRAPPER1(CFormationTypeWrapper,IFormationType,m_piFormationType)

extern CSingletonWrapper<IPlayAreaManager> g_PlayAreaManagerWrapper;
extern CSingletonWrapper<IFrameManager> g_FrameManagerSingleton;

#endif