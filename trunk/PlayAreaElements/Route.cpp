#include "./stdafx.h"
#include "Route.h"

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

void CRoute::ProcessPoint(IEntity *piEntity,DWORD dwCurrentFrame,double dTimeFraction){}

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