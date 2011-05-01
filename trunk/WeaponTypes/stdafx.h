#ifndef __STDAFX__
#define __STDAFX__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "GameRunTimeLib.h"
#include "VectorLib.h"
#include "GameEntityLib.h"

using namespace std;

#include "SingletonWrapper.h"

DECLARE_CUSTOM_WRAPPER1(CEntityTypeWrapper,IEntityType,m_piEntityType)

extern CSingletonWrapper<IPlayAreaManager> g_PlayAreaManagerWrapper;
#endif