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
#include "GameGUI.h"
#include "GUIDefs.h"
#include "GameGraphics.h"
#include "GameManagers.h"
#include "SoundSystems.h"

DECLARE_CUSTOM_WRAPPER1(CGameControllerWrapper,IGameController,m_piGameController)
DECLARE_CUSTOM_WRAPPER1(CGenericViewportWrapper,IGenericViewport,m_piViewport)
DECLARE_CUSTOM_WRAPPER1(CPlayerManagerWrapper,IPlayerManager,m_piPlayerManager)
DECLARE_CUSTOM_WRAPPER1(CPlayerProfileWrapper,IPlayerProfile,m_piPlayerProfile)
#ifdef STATIC_BUILD
DECLARE_CUSTOM_WRAPPER2(CPlayAreaManagerWrapper,IPlayAreaManager,m_piPlayAreaManager,IPlayAreaDesign,m_piPlayAreaDesign)
#else
DECLARE_CUSTOM_WRAPPER1(CPlayAreaManagerWrapper,IPlayAreaManager,m_piPlayAreaManager)
#endif
DECLARE_CUSTOM_WRAPPER1(CFrameManagerWrapper,IFrameManager,m_piFrameManager)
DECLARE_CUSTOM_WRAPPER1(CEntityManagerWrapper,IEntityManager,m_piEntityManager)
DECLARE_CUSTOM_WRAPPER2(CWorldManagerWrapper,ITerrainDesign,m_piTerrain,IWorldManager,m_piWorldManager)
DECLARE_CUSTOM_WRAPPER1(CSoundManagerWrapper,ISoundManager,m_piSoundManager)
DECLARE_CUSTOM_WRAPPER1(CSoundTypeWrapper,ISoundType,m_piSoundType)

#endif