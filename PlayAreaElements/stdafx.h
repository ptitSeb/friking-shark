//  Friking Shark, a Flying Shark arcade remake.
//  Copyright (C) 2011  Javier Martin Garcia (javiermartingarcia@gmail.com)
//	
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//  


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
#ifdef STATIC_BUILD
DECLARE_CUSTOM_WRAPPER2(CFormationTypeWrapper,IFormationType,m_piFormationType,IFormationTypeDesign,m_piFormationTypeDesign)
#else
DECLARE_CUSTOM_WRAPPER1(CFormationTypeWrapper,IFormationType,m_piFormationType)
#endif

extern CSingletonWrapper<IEntityManager> g_EntityManagerWrapper;
extern CSingletonWrapper<IPlayAreaManager> g_PlayAreaManagerWrapper;
extern CSingletonWrapper<IFrameManager> g_FrameManagerSingleton;
extern CSingletonWrapper<ITerrainDesign> g_WorldManagerWrapper;

#endif