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

BEGIN_STRUCT_PROPS(SRoutePoint)
    PROP(vPosition,"Posicion")
    PROP_VALUE_FLAGS(bAbsolutePoint,"Absoluto",true,MRPF_NORMAL|MRPF_OPTIONAL)
    PROP_VALUE_FLAGS(nPause,"Pause",0,MRPF_NORMAL|MRPF_OPTIONAL)
    PROP_VALUE_FLAGS(dSpeedFactor,"SpeedFactor",1,MRPF_NORMAL|MRPF_OPTIONAL)
END_STRUCT_PROPS()

class CRoute:public IRoute
{
    vector<SRoutePoint>   m_vPoints;
    bool                  m_bCyclic;

	CVector GetAbsoluteRoutePoint(SRoutePoint *p);

public:

    CRoute();

	CVector     GetRelativePoint(unsigned int nIndex);
	CVector     GetEstimatedAbsolutePoint(unsigned int nIndex,double dTime);
	CVector     GetAbsolutePoint(unsigned nIndex);
	unsigned    GetPointCount();
    CVector     GetDirection(unsigned nSection);
    unsigned    GetNextPointIndex(unsigned nIndex);
    void        ProcessPoint(IEntity *piEntity,unsigned int dwCurrentFrame,double dTimeFraction);

	bool 		 GetPoint(unsigned int nIndex,SRoutePoint *psPoint);
	bool 		 AddPoint(unsigned int nIndex,const SRoutePoint &sPoint);
	bool 		 SetPoint(unsigned int nIndex,const SRoutePoint &sPoint);
	void		 RemovePoint(unsigned int nIndex);
	void 		 Clear();

    BEGIN_PROP_MAP(CRoute)
        PROP(m_vPoints,"Puntos")
        PROP(m_bCyclic,"Ciclica")
    END_PROP_MAP()
	CVector GetPlayAreaElementRoutePoint(CVector vPlayAreaElementPos,SRoutePoint *pPoint);
	CVector GetPlayAreaElementPoint(CVector vPlayAreaElementPos,unsigned nIndex);
};

DECLARE_SERIALIZABLE(CRoute)
