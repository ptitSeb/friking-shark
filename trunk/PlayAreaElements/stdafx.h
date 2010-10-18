// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <list>
#include <crtdbg.h>
#include "VectorLib.h"
#include "GameLib.h"
#include "GameRunTimeLib.h"
#include "GameEntityLib.h"
#include "SingletonWrapper.h"
#include "PlayAreaElements.h"

DECLARE_CUSTOM_WRAPPER1(CEntityTypeWrapper,IEntityType,m_piEntityType);
DECLARE_CUSTOM_WRAPPER1(CFormationTypeWrapper,IFormationType,m_piFormationType);

extern CSingletonWrapper<IEntityManager> g_EntityManagerWrapper;
extern CSingletonWrapper<IPlayAreaManager> g_PlayAreaManagerWrapper;
extern CSingletonWrapper<IFrameManager> g_FrameManagerSingleton;

// TODO: reference additional headers your program requires here
using namespace std;