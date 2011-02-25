#ifndef __STDAFX__
#define __STDAFX__

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <list>
#include "VectorLib.h"
#include "GameLib.h"
#include "GameRunTimeLib.h"
#include "GameEntityLib.h"
#include "SingletonWrapper.h"
#include "PlayAreaElements.h"

DECLARE_CUSTOM_WRAPPER1(CEntityTypeWrapper,IEntityType,m_piEntityType)
DECLARE_CUSTOM_WRAPPER1(CFormationTypeWrapper,IFormationType,m_piFormationType)

extern CSingletonWrapper<IEntityManager> g_EntityManagerWrapper;
extern CSingletonWrapper<IPlayAreaManager> g_PlayAreaManagerWrapper;
extern CSingletonWrapper<IFrameManager> g_FrameManagerSingleton;

using namespace std;
#endif