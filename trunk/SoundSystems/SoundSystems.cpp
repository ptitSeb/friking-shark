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
#include "SoundSystems.h"
#include "SoundSystemManager.h"
#include "SoundType.h"

BEGIN_SYSTEM_MODULE()
  SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CSoundSystemManager,"CSoundSystemManager")
  SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CSoundType,"SoundType")
END_SYSTEM_MODULE()

double DeciblesToPercentage(double dDb)
{
  double dTotalLog=log10(1000.0);
  double dLog=log10(dDb);
  double dValue=100.0*dLog/dTotalLog;
  return dValue;
}

double PercentageToDecibels(double dPercentage)
{
  double dTotalLog=log10(10000.0);
  double dValue=dTotalLog*dPercentage/100.0;
  return dValue==0?0:pow(10.0,dValue);
}