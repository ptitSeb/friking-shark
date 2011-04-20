#ifndef __STDAFX__
#define __STDAFX__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

using namespace std;

#include "GameRunTimeLib.h"
#include "VectorLib.h"
#include "SingletonWrapper.h"
#include "GameEntityLib.h"


DECLARE_CUSTOM_WRAPPER1(CEntityTypeWrapper,IEntityType,m_piEntityType)

extern CSingletonWrapper<IPlayAreaManager> g_PlayAreaManagerWrapper;
#endif