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
#include "Route.h"
#include "SingletonWrapper.h"

CSingletonWrapper<IPlayAreaManager> g_PlayAreaManagerWrapper("GameSystem","PlayAreaManager");
CSingletonWrapper<IPlayerManager> g_PlayerManagerWrapper("GameSystem","PlayerManager");

CRoute::CRoute()
{
    m_bCyclic=false;
}

unsigned CRoute::GetPointCount()
{
    return (unsigned)m_vPoints.size();
}

CVector CRoute::GetAbsoluteRoutePoint(SRoutePoint *pPoint)
{
    if(pPoint->bAbsolutePoint){return pPoint->vPosition;}
    CVector vPos;
    CVector vMins,vMaxs;
	g_PlayAreaManagerWrapper.AddRef();
    g_PlayAreaManagerWrapper.m_piInterface->GetVisibleAirPlayPlane(&vMins,&vMaxs);
    CVector vSize=vMaxs-vMins;
    CVector vTempPos=pPoint->vPosition;
    vTempPos=g_PlayAreaManagerWrapper.m_piInterface->GetPlayMovementRight()*pPoint->vPosition.c[0];
    vTempPos+=g_PlayAreaManagerWrapper.m_piInterface->GetPlayMovementForward()*pPoint->vPosition.c[1];
    for(int x=0;x<3;x++)
    {   
        vPos.c[x]=vMins[x]+vTempPos.c[x]*vSize.c[x]*0.5+vSize.c[x]/2;
    }
	g_PlayAreaManagerWrapper.Release();
    return vPos;
}

CVector CRoute::GetPlayAreaElementRoutePoint(CVector vPlayAreaElementPos,SRoutePoint *pPoint)
{
	CVector vPos;
	CVector vMins,vMaxs;
	g_PlayAreaManagerWrapper.AddRef();
	g_PlayAreaManagerWrapper.m_piInterface->GetVisibleAirPlayPlane(&vMins,&vMaxs);
	CVector vSize=vMaxs-vMins;
	CVector vTempPos;
	vTempPos=g_PlayAreaManagerWrapper.m_piInterface->GetPlayMovementRight()*pPoint->vPosition.c[0];
	vTempPos+=g_PlayAreaManagerWrapper.m_piInterface->GetPlayMovementForward()*(pPoint->vPosition.c[1]-1.0);
	for(int x=0;x<3;x++)
	{   
		vPos.c[x]=vPlayAreaElementPos.c[x]+vTempPos.c[x]*vSize.c[x]*0.5;
	}
	g_PlayAreaManagerWrapper.Release();
	return vPos;
}

CVector	CRoute::GetPlayAreaElementPoint(CVector vPlayAreaElementPos,unsigned nIndex)
{
	if(nIndex>=m_vPoints.size()){return Origin;}
	return GetPlayAreaElementRoutePoint(vPlayAreaElementPos,&m_vPoints[nIndex]);
}

CVector CRoute::GetEstimatedAbsolutePoint(unsigned int nIndex,double dTime)
{
	CVector vPoint=GetAbsolutePoint(nIndex);
	IPlayAreaDesign *piDesign=QI(IPlayAreaDesign,g_PlayAreaManagerWrapper.m_piInterface);
	if(piDesign)
	{
		SPlayAreaConfig sConfig;
		piDesign->GetPlayAreaConfig(&sConfig);
		vPoint.c[0]+=sConfig.dCameraSpeed*dTime;
	}
	REL(piDesign);
	return vPoint;
}

CVector CRoute::GetAbsolutePoint(unsigned nIndex)
{
    CVector vReturnPoint;
    unsigned nCount=(unsigned)m_vPoints.size();
    if(nCount==0){return Origin;}
    if(nIndex>=nCount)
    {
        vReturnPoint=GetAbsoluteRoutePoint(&m_vPoints[nCount-1]);
    }
    else
    {
        vReturnPoint=GetAbsoluteRoutePoint(&m_vPoints[nIndex]);
    }
    return vReturnPoint;
}

CVector CRoute::GetDirection(unsigned nSection)
{
    CVector p1=GetAbsolutePoint(nSection),p2=GetAbsolutePoint(nSection+1);
    CVector vDir=p2-p1;
    vDir.N();
    return vDir;
}
unsigned CRoute::GetNextPointIndex(unsigned nIndex)
{
    unsigned nCount=(unsigned)m_vPoints.size();
    if(nIndex<(nCount-1)){return nIndex+1;}
    if(m_bCyclic && nIndex==(nCount-1)){return 0;}
    return nIndex;// ultimo punto de una ruta no ciclica o esta fuera de rango
}

void CRoute::ProcessPoint(IEntity *piEntity,unsigned int dwCurrentFrame,double dTimeFraction){}

CVector CRoute::GetRelativePoint(unsigned int nIndex)
{
	if(nIndex>=m_vPoints.size()){return Origin;}
	return m_vPoints[nIndex].bAbsolutePoint?Origin:m_vPoints[nIndex].vPosition;
}

bool CRoute::GetPoint(unsigned int nIndex,SRoutePoint *psPoint)
{
	if(nIndex>=m_vPoints.size()){return false;}
	*psPoint=m_vPoints[nIndex];
	return true;
}

bool CRoute::AddPoint(unsigned int nIndex,const SRoutePoint &sPoint)
{
	if(nIndex>m_vPoints.size()){return false;}
	if(nIndex==m_vPoints.size()){m_vPoints.push_back(sPoint);return true;}
	vector<SRoutePoint>::iterator i=m_vPoints.begin();
	for(unsigned int x=0;x<m_vPoints.size();x++,i++)
	{
		if(x==nIndex)
		{
			m_vPoints.insert(i,sPoint);
			return true;
		}

	}
	return false;
}
bool CRoute::SetPoint(unsigned int nIndex,const SRoutePoint &sPoint)
{
	if(nIndex>=m_vPoints.size()){return false;}
	m_vPoints[nIndex]=sPoint;
	return true;
}

void CRoute::RemovePoint(unsigned int nIndex)
{
	vector<SRoutePoint>::iterator i=m_vPoints.begin();
	for(unsigned int x=0;x<m_vPoints.size();x++,i++)
	{
		if(x==nIndex){m_vPoints.erase(i);break;}
	}
}

void CRoute::Clear()
{
	m_vPoints.clear();
}