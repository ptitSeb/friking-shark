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
#include "FormationType.h"
#include "GameGraphics.h"

CFormationType::CFormationType(){}
CFormationType::~CFormationType(){}

IFormation *CFormationType::CreateInstance(CVector vPosition,unsigned int dwCurrentTime)
{

	CFormation *pFormation=new CFormation(this,vPosition);
	return pFormation;
}

bool CFormationType::PrepareResources()
{
	bool bOk=true;
	for(unsigned int x=0;x<m_vElements.size();x++)
	{
		SFormationElement *pElement=&m_vElements[x];
		if(pElement->m_EntityType.m_piEntityType){bOk=bOk&&pElement->m_EntityType.m_piEntityType->PrepareResources();}
	}
	return bOk;
}

void CFormationType::DesignRender( IGenericRender *piRender,CVector &vPosition,CVector &vAngles ,bool bSelected)
{
	if(bSelected)
	{
		CVector vMins,vMaxs;
		g_PlayAreaManagerWrapper.AddRef();
		g_PlayAreaManagerWrapper.m_piInterface->GetPlayAreaPlaneAt(vPosition,&vMins,&vMaxs);
		g_PlayAreaManagerWrapper.Release();
		CVector vSize=vMaxs-vMins;
		// Centramos el bbox que representa el play area justo delante del punto de activacion
		// y lo hacemos plano
		vMins.c[0]-=vSize.c[0]*0.5;
		vMaxs.c[0]-=vSize.c[0]*0.5;
		piRender->PushState();
		piRender->DeactivateShadowReception();
		piRender->DeactivateHeightFog();
		piRender->RenderBBox(vMins,vMaxs,CVector(0.7,0.7,0.7),0xFFFF);
		piRender->PopState();
	}

	for(unsigned int x=0;x<m_vElements.size();x++)
	{
		SFormationElement *pElement=&m_vElements[x];
		if(pElement->m_EntityType.m_piEntityType)
		{		
			piRender->PushState();
			piRender->DeactivateShadowReception();
			piRender->DeactivateHeightFog();
			for(unsigned int y=0;y<m_vElements[x].m_Route.GetPointCount()-1;y++)
			{
				CVector vPos1=m_vElements[x].m_Route.GetPlayAreaElementPoint(vPosition,y);
				CVector vPos2=m_vElements[x].m_Route.GetPlayAreaElementPoint(vPosition,y+1);
				if(bSelected)
				{
					piRender->RenderLine(vPos1,vPos2,CVector(1,1,1),0x8888);
				}
				else
				{
					piRender->RenderLine(vPos1,vPos2,CVector(0.5,0.5,0.5),0x8888);
				}
			}
			piRender->PopState();
			
			CVector vPos=m_vElements[x].m_Route.GetPlayAreaElementPoint(vPosition,0);
			CVector vAngles=AnglesFromVector(m_vElements[x].m_Route.GetDirection(0));
			pElement->m_EntityType.m_piEntityType->DesignRender(piRender,vPos,vAngles,bSelected);
		}
	}
}

void CFormationType::DesignGetBBox( CVector *pvMins,CVector *pvMaxs )
{
	if(m_vElements.size())
	{
		// Se deducir las coordenadas a partir de los puntos de ruta con el minimo en -1,-1,1,1
		// De esta forma se incluyen todos los puntos de ruta que esten fuera del area de juego
		// pero como minimo siempre se muestra el area de juego

		SRoutePoint routeMins,routeMaxs;
		routeMins.bAbsolutePoint=false;
		routeMaxs.bAbsolutePoint=false;
		routeMins.vPosition=CVector(-1,-1,0);
		routeMaxs.vPosition=CVector(1,1,0);
		for(unsigned int x=0;x<m_vElements.size();x++)
		{
			for(unsigned int y=0;y<m_vElements[x].m_Route.GetPointCount();y++)
			{
				CVector vTempMins=routeMins.vPosition;
				CVector vTempMaxs=routeMaxs.vPosition;
				routeMins.vPosition.Mins(vTempMins,m_vElements[x].m_Route.GetRelativePoint(y));
				routeMaxs.vPosition.Maxs(vTempMaxs,m_vElements[x].m_Route.GetRelativePoint(y));
			}
		}
		CVector vMins=m_vElements[0].m_Route.GetPlayAreaElementRoutePoint(Origin,&routeMins);
		CVector vMaxs=m_vElements[0].m_Route.GetPlayAreaElementRoutePoint(Origin,&routeMaxs);
		pvMins->Mins(vMins,vMaxs);
		pvMaxs->Maxs(vMins,vMaxs);
	}
}

double CFormationType::DesignGetRadius()
{
	CVector vMins,vMaxs;
	DesignGetBBox(&vMins,&vMaxs);
	return GetBBoxRadius(vMins,vMaxs);
}

CTraceInfo CFormationType::DesignGetTrace( const CVector &vPosition,const CVector &vAngles,const CVector &p1,const CVector &p2 )
{
	CTraceInfo info;
	info.m_dTraceFraction=1.0;
	info.m_vTracePos=p2;
	return info;
}

unsigned long CFormationType::AddElement()
{
	unsigned long nElement=m_vElements.size();
	
	SFormationElement element;
	m_vElements.push_back(element);
	return nElement;
}

void CFormationType::RemoveElement(unsigned int nIndex)
{
	if(nIndex>=m_vElements.size()){return;}
	unsigned int x;
	std::vector<SFormationElement>::iterator i;
	for(x=0,i=m_vElements.begin();i!=m_vElements.end();i++,x++)
	{
		if(x==nIndex)
		{
			m_vElements.erase(i);
			break;
		}
	}
}
unsigned int CFormationType::GetElements(){return m_vElements.size();}

void 		 CFormationType::SetElementEntityType(unsigned int nElement,IEntityType *piEntityType){if(nElement>=m_vElements.size()){return;}m_vElements[nElement].m_EntityType.Attach(piEntityType);}
void		 CFormationType::GetElementEntityType(unsigned int nElement,IEntityType **ppiEntityType){if(nElement>=m_vElements.size()){return;}if(ppiEntityType){*ppiEntityType=ADD(m_vElements[nElement].m_EntityType.m_piEntityType);}}
unsigned int CFormationType::GetElementRoutePoints(unsigned int nElement){if(nElement>=m_vElements.size()){return 0;} return m_vElements[nElement].m_Route.GetPointCount();}
bool 		 CFormationType::GetElementRoutePoint(unsigned int nElement,unsigned int nIndex,SRoutePoint *psPoint){if(nElement>=m_vElements.size()){return false;} return m_vElements[nElement].m_Route.GetPoint(nIndex,psPoint);}
bool 		 CFormationType::AddElementRoutePoint(unsigned int nElement,unsigned int nIndex,const SRoutePoint &sPoint){if(nElement>=m_vElements.size()){return false;} return m_vElements[nElement].m_Route.AddPoint(nIndex,sPoint);}
bool 		 CFormationType::SetElementRoutePoint(unsigned int nElement,unsigned int nIndex,const SRoutePoint &sPoint){if(nElement>=m_vElements.size()){return false;} return m_vElements[nElement].m_Route.SetPoint(nIndex,sPoint);}
void		 CFormationType::RemoveElementRoutePoint(unsigned int nElement,unsigned int nIndex){if(nElement>=m_vElements.size()){return;} m_vElements[nElement].m_Route.RemovePoint(nIndex);}
void 		 CFormationType::ClearElementRoute(unsigned int nElement){if(nElement>=m_vElements.size()){return;} m_vElements[nElement].m_Route.Clear();}
void 		 CFormationType::SetElementEntityCount(unsigned int nElement,unsigned int nCount){if(nElement>=m_vElements.size()){return;} m_vElements[nElement].m_nEntityCount=nCount;}
unsigned int CFormationType::GetElementEntityCount(unsigned int nElement){if(nElement>=m_vElements.size()){return 0;} return m_vElements[nElement].m_nEntityCount;}
void 		 CFormationType::SetElementEntityInterval(unsigned int nElement,unsigned int nMilliseconds){if(nElement>=m_vElements.size()){return;} m_vElements[nElement].m_nInterval=nMilliseconds;}
unsigned int CFormationType::GetElementEntityInterval(unsigned int nElement){if(nElement>=m_vElements.size()){return 0;} return m_vElements[nElement].m_nInterval;}
void 		 CFormationType::SetElementEntityDelay(unsigned int nElement,unsigned int nMilliseconds){if(nElement>=m_vElements.size()){return;} m_vElements[nElement].m_nDelay=nMilliseconds;}
unsigned int CFormationType::GetElementEntityDelay(unsigned int nElement){if(nElement>=m_vElements.size()){return 0;} return m_vElements[nElement].m_nDelay;}

CFormation::CFormation(CFormationType *pType,CVector vPosition)
{
	g_PlayAreaManagerWrapper.AddRef();
	m_bAllUnitsCreated=true;
	m_pType=pType;
	m_vPosition=vPosition;
	m_nActivationTime=0;
	
	m_vElementRunTimeInfo.resize(m_pType->m_vElements.size());
	for(unsigned x=0;x<m_pType->m_vElements.size();x++)
	{
		SFormationElementRunTimeInfo runtimeElement;
		runtimeElement.m_dwCreatedEntities=0;
		runtimeElement.m_dwLastEntityTime=0;
		runtimeElement.m_pFormationTypeElement=&pType->m_vElements[x];        
		m_vElementRunTimeInfo[x]=runtimeElement;
		if(runtimeElement.m_pFormationTypeElement->m_nEntityCount){m_bAllUnitsCreated=false;}
	}
}

CFormation::~CFormation()
{
	set<IEntity *>::iterator i;
	for(i=m_sEntities.begin();i!=m_sEntities.end();i++)
	{
		IEntity *piEntity=*i;
		UNSUBSCRIBE_FROM_CAST(piEntity,IEntityEvents);
	}
	m_sEntities.clear();
	g_PlayAreaManagerWrapper.Release();
}

void CFormation::AddEntity(IEntity *piEntity)
{
	m_sEntities.insert(piEntity);
	SUBSCRIBE_TO_CAST(piEntity,IEntityEvents);
}
void CFormation::OnRemoved(IEntity *piEntity)
{
	UNSUBSCRIBE_FROM_CAST(piEntity,IEntityEvents);
	m_sEntities.erase(piEntity);
	// La referencia de mas esta para evitar que se borre este objeto al liberarlo el CPlayAreaFormation
	if(m_sEntities.size()==0 && m_bAllUnitsCreated){AddReference();NOTIFY_EVENT(IFormationEvents,OnFormationRemoved(this));ReleaseReference();}
}
void CFormation::OnKilled(IEntity *piEntity)
{
	UNSUBSCRIBE_FROM_CAST(piEntity,IEntityEvents);
	m_sEntities.erase(piEntity);
	// La referencia de mas esta para evitar que se borre este objeto al liberarlo el CPlayAreaFormation
	if(m_sEntities.size()==0 && m_bAllUnitsCreated){AddReference();NOTIFY_EVENT(IFormationEvents,OnFormationKilled(this,piEntity));ReleaseReference();}
}

bool CFormation::ProcessFrame(unsigned int dwCurrentTime,double dInterval)
{
    bool bFinished=false;
	if(m_nActivationTime==0){m_nActivationTime=dwCurrentTime;}

    m_bAllUnitsCreated=true;
    for(unsigned x=0;x<m_vElementRunTimeInfo.size();x++)
    {
        SFormationElementRunTimeInfo *pElement=&m_vElementRunTimeInfo[x];
		if(pElement->m_pFormationTypeElement->m_EntityType.m_piEntityType==NULL){continue;}
        if(pElement->m_dwCreatedEntities==pElement->m_pFormationTypeElement->m_nEntityCount){continue;}

		// Activar la siguiente entidad:
		// Si es la primera entidad y ha pasado el delay o
		// No es la primera entidad y ha pasado el intervalo entre entidades
        if(    (pElement->m_dwCreatedEntities==0 && dwCurrentTime>=m_nActivationTime+pElement->m_pFormationTypeElement->m_nDelay) 
			|| (pElement->m_dwCreatedEntities!=0  && dwCurrentTime>=(pElement->m_dwLastEntityTime+pElement->m_pFormationTypeElement->m_nInterval)))
        {
			
			CVector vPlayerStart,vPlayerEnd;
			g_PlayAreaManagerWrapper.m_piInterface->GetCameraRoute(&vPlayerStart,&vPlayerEnd);
			
			SPhysicForce force;
			force.dwForceType=PHYSIC_FORCE_CONSTANT_VELOCITY;
			force.vDir=vPlayerEnd-vPlayerStart;
			force.vDir.N();
			force.dConstantVel=g_PlayAreaManagerWrapper.m_piInterface->GetCameraSpeed();
			
			IEntity *piEntity=pElement->m_pFormationTypeElement->m_EntityType.m_piEntityType->CreateInstance(NULL,dwCurrentTime);
            piEntity->GetPhysicInfo()->vPosition=pElement->m_pFormationTypeElement->m_Route.GetAbsolutePoint(0);
			piEntity->GetPhysicInfo()->vForces.push_back(force);
			AnglesFromVector(pElement->m_pFormationTypeElement->m_Route.GetDirection(0),&piEntity->GetPhysicInfo()->vAngles);
			piEntity->SetRoute(&pElement->m_pFormationTypeElement->m_Route);
			AddEntity(piEntity);
			pElement->m_dwLastEntityTime=dwCurrentTime;
			pElement->m_dwCreatedEntities++;
		}
		if(pElement->m_dwCreatedEntities<pElement->m_pFormationTypeElement->m_nEntityCount)
		{
			m_bAllUnitsCreated=false;
		}
	}
	bFinished=(m_sEntities.size()==0 && m_bAllUnitsCreated);
	// La referencia de mas esta para evitar que se borre este objeto al liberarlo el CPlayAreaFormation
	if(bFinished){AddReference();NOTIFY_EVENT(IFormationEvents,OnFormationRemoved(this));ReleaseReference();}
	return !bFinished;
}
