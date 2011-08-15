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


// BonusTypes.cpp : Defines the entry point for the DLL application.
//
#include "./stdafx.h"
#include "BonusTypes.h"
#include "WeaponUpgradeBonusType.h"
#include "BombBonusType.h"
#include "PointBonusType.h"
#include "LiveBonusType.h"

BEGIN_SYSTEM_MODULE()
  SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CWeaponUpgradeBonusType,"WeaponUpgradeBonus")
  SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CBombBonusType,"BombBonus")
  SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CPointBonusType,"PointBonus")
  SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CLiveBonusType,"LiveBonus")
END_SYSTEM_MODULE()
