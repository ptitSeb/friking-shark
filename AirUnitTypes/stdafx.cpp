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


#include "./stdafx.h"

#ifdef WIN32
#pragma comment (lib, "VectorLib.lib")
#pragma comment (lib, "GameRuntime.lib")
#pragma comment (lib, "GameRuntimeLib.lib")
#pragma comment (lib, "GameEntityLib.lib")
#endif

#ifdef STATIC_BUILD
extern CSingletonWrapper<IPlayAreaManager> g_PlayAreaManagerWrapper;
extern CSingletonWrapper<IPlayerManager> g_PlayerManagerWrapper;
#else
CSingletonWrapper<IPlayAreaManager> g_PlayAreaManagerWrapper("GameSystem","PlayAreaManager");
CSingletonWrapper<IPlayerManager> g_PlayerManagerWrapper("GameSystem","PlayerManager");
#endif