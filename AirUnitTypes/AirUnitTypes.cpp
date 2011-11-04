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
#include "PlayerType.h"
#include "FighterType.h"
#include "BomberType.h"

BEGIN_SYSTEM_MODULE(AirUnitTypes)
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CPlayerType,"CPlayer")
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CFighterType,"CFighter")
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CBomberType,"CBomber")
END_SYSTEM_MODULE(AirUnitTypes)

