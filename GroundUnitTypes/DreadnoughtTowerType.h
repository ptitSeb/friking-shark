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


#pragma once

class CDreadnoughtTowerType: public CStaticStructureType
{
public:
	
	unsigned int m_nPreRouteHealthLimit;

	IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);
	
	BEGIN_PROP_MAP(CDreadnoughtTowerType)
		PROP_CLASS_CHAIN(CStaticStructureType)
		PROP_VALUE_FLAGS(m_nPreRouteHealthLimit,"PreRouteHealthPercentLimit",60,MRPF_NORMAL|MRPF_OPTIONAL);
	END_PROP_MAP();	
	CDreadnoughtTowerType();
	~CDreadnoughtTowerType();
};


class CDreadnoughtTower: public CStaticStructure
{
	CDreadnoughtTowerType  *m_pType;
	unsigned int m_nCurrentTime;

	void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);
	void OnDamage(double dDamage,IEntity *piAggresor);
	
public:

	CDreadnoughtTower(CDreadnoughtTowerType *pType,unsigned int dwCurrentTime);
};