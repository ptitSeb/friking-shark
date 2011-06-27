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
#include "PlayAreaElementBase.h"

CPlayAreaElementBase::CPlayAreaElementBase()
{
    m_bActive=false;
	m_nActivationTime=0;
}

CPlayAreaElementBase::~CPlayAreaElementBase()
{
}

bool CPlayAreaElementBase::PrepareResources(){return true;}

void CPlayAreaElementBase::Activate(unsigned int dwCurrentTime){m_bActive=true;m_nActivationTime=dwCurrentTime;}
void CPlayAreaElementBase::Deactivate(){m_bActive=false;m_nActivationTime=0;}
bool CPlayAreaElementBase::IsActive(){return m_bActive;}
void CPlayAreaElementBase::Reset(){Deactivate();}
bool CPlayAreaElementBase::ProcessFrame(CVector vPlayPosition,SPlayAreaInfo *pPlayAreaInfo,unsigned int dwCurrentTime,double dInterval){return true;}
bool Util_IsInPlayArea(CVector vPosition,SPlayAreaInfo *pInfo)
{
    for(int x=0;x<pInfo->nPlaneCount;x++)
    {
        if(pInfo->planes[x].GetSide(vPosition)>0){return false;}
    }
    return true;
}

bool Util_IsInPlayArea(CVector vPosition,double dRadius,SPlayAreaInfo *pInfo)
{
	for(int x=0;x<pInfo->nPlaneCount;x++)
	{
		double dMinSide,dMaxSide;
		dMinSide=pInfo->planes[x].GetSide(vPosition-CVector(dRadius,0,0));
		dMaxSide=pInfo->planes[x].GetSide(vPosition+CVector(dRadius,0,0));
		if(dMinSide>0 && dMaxSide>0){return false;}
	}
	return true;
}
