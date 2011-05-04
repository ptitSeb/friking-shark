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

class CWorldManager;

class CWorldEntity:public CEntityBase
{
    CWorldManager *m_pManager;

public:
	CTraceInfo GetTrace(const CVector &p1,const CVector &p2);

	void OnDamage(double dDamage,IEntity *pAggresor);
    void Render(IGenericRender *piRender,IGenericCamera *piCamera);

    CWorldEntity(CWorldManager *pManager);
    ~CWorldEntity(void);
};
