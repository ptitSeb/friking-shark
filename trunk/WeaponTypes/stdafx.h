#ifndef __STDAFX__
#define __STDAFX__

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <crtdbg.h>
#include "VectorLib.h"
#include "GameLib.h"
#include "GameEntityLib.h"

DECLARE_CUSTOM_WRAPPER1(CEntityTypeWrapper,IEntityType,m_piEntityType)

#endif