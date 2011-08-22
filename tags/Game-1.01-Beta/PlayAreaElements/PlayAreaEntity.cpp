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
#include "GameGraphics.h"
#include "PlayAreaElementBase.h"
#include "PlayAreaEntity.h"

CPlayAreaEntity::CPlayAreaEntity()
{
	m_dRTRadius=0;
	m_dRTActivationMin=0;
	m_dRTActivationMax=0;
	m_dRTShadowPreactivation=0;
    m_nDelay=0;
	m_nRouteDelay=0;
	m_nInterval=0;
	m_nEntityCount=1;
	m_nLastEntityTime=0;
	m_nCreatedEntities=0;
	m_nKilledEntities=0;
	m_nBonusOnChild=-1;
	m_bFirstFrame=true;
	m_bDoNotActivate=false;
	m_bDynamic=false;
	g_FrameManagerSingleton.AddRef();
	g_PlayAreaManagerWrapper.AddRef();
	g_WorldManagerWrapper.AddRef();
}

CPlayAreaEntity::~CPlayAreaEntity()
{
    m_EntityType.Detach();
	g_FrameManagerSingleton.Release();
	g_PlayAreaManagerWrapper.Release();
	g_WorldManagerWrapper.Release();
}
void CPlayAreaEntity::Start()
{
	CPlayAreaElementBase::Start();
	if(m_EntityType.m_piEntityType)
	{
		/* 
		Shadow preactivation is simply the estimated shadow lenght in the X axis. 
		Shadow lenght is computed by intersecting the XZ plane at the entity min Y with a line from the sun passing throught
		the entity highest height, that gaves us the ammount of X we have to show the entity in advance.
		It can only be negative or zero, shadows lengths>0 are clamped to 0 as it means that the shadow falls below the
		entity start (azimuth 90 - 270).
		This computation takes into account both elevation and azimuth.
		 
		This computation fails when the entity shadow is projected below its min Y but works most of the times.
		This problem can be fixed by tracing against the world, but it also creates problems in some other cases.
		 
		So, to keep things as simple as possible the cases not covered by this computation are
		required to be fine tuned using a negative delay
		*/
		 
		CVector pvRTVolume[8];
		m_dRTRadius=m_EntityType.m_piEntityType->DesignGetRadius();
		m_EntityType.m_piEntityType->DesignGetAABBox(Origin,Origin,&m_vRTMins,&m_vRTMaxs);
		CalcBBoxVolume(Origin,m_vAngles,m_vRTMins,m_vRTMaxs,pvRTVolume);
		
		STerrainSun sSun;
		CVector vSunDir;
		
		g_WorldManagerWrapper.m_piInterface->GetTerrainSun(&sSun);
		VectorsFromAngles(CVector(sSun.dAzimuth,sSun.dElevation,0),&vSunDir,NULL,NULL);

		CVector vSunLine[2];
		vSunLine[0].c[1]=(m_vRTMaxs.c[1]-m_vRTMins.c[1]);
		vSunLine[1].c[1]=(m_vRTMaxs.c[1]-m_vRTMins.c[1]);
		vSunLine[0]+=vSunDir*10000.0;
		vSunLine[1]-=vSunDir*10000.0;
		
		m_dRTShadowPreactivation=0;
		
		CVector vSunCutWithPlane;
		CPlane plane(AxisPosY,0);
		if(plane.Cut(vSunLine[0],vSunLine[1],&vSunCutWithPlane) && vSunCutWithPlane.c[0]<0)
		{
			m_dRTShadowPreactivation=vSunCutWithPlane.c[0];
		}		
		
		double dMinx=pvRTVolume[0].c[0];
		double dMaxx=pvRTVolume[0].c[0];
		for(unsigned int x=1;x<8;x++)
		{
			if(pvRTVolume[x].c[0]<dMinx){dMinx=pvRTVolume[x].c[0];}
			if(pvRTVolume[x].c[0]>dMaxx){dMaxx=pvRTVolume[x].c[0];}
		}
		
		m_dRTActivationMin=dMinx;
		m_dRTActivationMax=dMaxx;
	}
}

bool CPlayAreaEntity::ProcessFrame(CVector vPlayPosition,SPlayAreaInfo *pAreaInfo,unsigned int dwCurrentTime,double dInterval)
{	
	
	CVector vActivationPosition=m_vPosition;
	vActivationPosition.c[0]+=(m_nDelay && g_PlayAreaManagerWrapper.m_piInterface)?g_PlayAreaManagerWrapper.m_piInterface->GetCameraSpeed()*((double)m_nDelay)/1000.0:0;
	
	if(m_bFirstFrame)
	{
		m_bFirstFrame=false;
		if(!m_bDynamic && Util_IsInPlayArea(vActivationPosition+CVector(0,m_vRTMins.c[1],0),m_dRTActivationMin+m_dRTShadowPreactivation,m_dRTActivationMax,pAreaInfo))
		{
			 SEntityTypeConfig sConfig;
			IEntityTypeDesign *piDesign=QI(IEntityTypeDesign,m_EntityType.m_piEntityType);
			if(piDesign){piDesign->GetEntityTypeConfig(&sConfig);}
			REL(piDesign);
			m_bDoNotActivate=(sConfig.nAlignment==ENTITY_ALIGNMENT_ENEMIES && m_Route.GetPointCount()>0);
		}
	}
	   
	if(!m_bActive && !m_bDoNotActivate)
	{
		bool bCurrentlyInPlayArea=Util_IsInPlayArea(vActivationPosition+CVector(0,m_vRTMins.c[1],0),m_dRTActivationMin+m_dRTShadowPreactivation,m_dRTActivationMax,pAreaInfo);
		if(bCurrentlyInPlayArea)
		{
			Activate(dwCurrentTime);
		}
	}
	// Comprobar si la entidad esta activa otra vez para procesar el frame en caso de que se acabe de activar,
	if(m_bActive)
	{
		if(m_EntityType.m_piEntityType!=NULL && 
			((m_nCreatedEntities==0) 
			|| (m_nCreatedEntities>0 && m_nCreatedEntities<m_nEntityCount && dwCurrentTime>=(m_nLastEntityTime+m_nInterval))))
		{
			IEntity *piEntity=m_EntityType.m_piEntityType->CreateInstance(NULL,dwCurrentTime);
			piEntity->GetPhysicInfo()->vPosition=m_vPosition;
			piEntity->GetPhysicInfo()->vAngles=m_vAngles;
			if(m_Route.GetPointCount())
			{
				piEntity->SetRoute(&m_Route);
				piEntity->SetRouteDelay(m_nRouteDelay);
				piEntity->GetPhysicInfo()->vAngles=AnglesFromVector(m_Route.GetAbsolutePoint(0)-m_vPosition);
			}
			SUBSCRIBE_TO(piEntity,IEntityEvents);
			m_mEntities.insert(std::pair<IEntity*,bool>(piEntity,false));
			m_nCreatedEntities++;
			m_nLastEntityTime=dwCurrentTime;
		}
		
		map<IEntity*,bool>::iterator i;
		for(i=m_mEntities.begin();i!=m_mEntities.end();)
		{
			IEntity *piEntity=i->first;
			bool bHasBeenVisible=i->second;
			bool bEntityCurrentlyInPlayArea=Util_IsInPlayArea(piEntity->GetPhysicInfo()->vPosition+CVector(0,m_vRTMins.c[1],0),m_dRTActivationMin+m_dRTShadowPreactivation,m_dRTActivationMax,pAreaInfo);
			if(!bEntityCurrentlyInPlayArea && bHasBeenVisible &&  piEntity->HasFinishedRoute() )
			{
				m_mEntities.erase(i++);
				UNSUBSCRIBE_FROM_CAST(piEntity,IEntityEvents);
				piEntity->Remove();
				piEntity=NULL;
			}
			else
			{
				if(bEntityCurrentlyInPlayArea){i->second=true;}
				i++;
			}
		}
		if(m_nCreatedEntities==m_nEntityCount && m_mEntities.size()==0)
		{
			bool bCurrentlyInPlayArea=Util_IsInPlayArea(vActivationPosition+CVector(0,m_vRTMins.c[1],0),m_dRTActivationMin+m_dRTShadowPreactivation,m_dRTActivationMax,pAreaInfo);
			if(!bCurrentlyInPlayArea){Deactivate();}
		}
	}
	return m_bActive;
}

bool CPlayAreaEntity::Init(std::string sClass,std::string sName,ISystem *piSystem)
{
    bool bOk=CPlayAreaElementBase::Init(sClass,sName,piSystem);
    return bOk;
}

void CPlayAreaEntity::Destroy()
{
	Deactivate();
	m_dRTRadius=0;
	m_EntityType.Detach();
	CPlayAreaElementBase::Destroy();
}

void CPlayAreaEntity::Activate(unsigned int dwCurrentTime)
{
    CPlayAreaElementBase::Activate(dwCurrentTime);
}

void CPlayAreaEntity::Stop()
{
	CPlayAreaElementBase::Stop();
	m_bFirstFrame=true;
	m_bDoNotActivate=false;
	m_dRTRadius=0;
	m_dRTActivationMin=0;
	m_dRTActivationMax=0;
}

void CPlayAreaEntity::Deactivate()
{
	map<IEntity*,bool>::iterator i;
	for(i=m_mEntities.begin();i!=m_mEntities.end();i++)
    {
		IEntity *piEntity=i->first;
        UNSUBSCRIBE_FROM_CAST(piEntity,IEntityEvents);
        piEntity->Remove();
        piEntity=NULL;
    }
	m_mEntities.clear();
	m_nCreatedEntities=0;
	m_nLastEntityTime=0;
	m_nKilledEntities=0;
    CPlayAreaElementBase::Deactivate();
}

void CPlayAreaEntity::OnChildKilled(IEntity *piEntity,unsigned int nChildId,IEntity *piChildEntity)
{
	if(m_nBonusOnChild!=-1)
	{
		if((int)nChildId==m_nBonusOnChild && m_BonusType.m_piEntityType)
		{
			m_BonusType.m_piEntityType->CreateInstance(piChildEntity,g_FrameManagerSingleton.m_piInterface->GetCurrentTime());
		}
	}
}

void CPlayAreaEntity::OnKilled(IEntity *piEntity)
{
	if(m_nBonusOnChild==-1)
	{
		m_nKilledEntities++;
		if(m_nKilledEntities==m_nEntityCount && m_BonusType.m_piEntityType)
		{
			m_BonusType.m_piEntityType->CreateInstance(piEntity,g_FrameManagerSingleton.m_piInterface->GetCurrentTime());
		}
	}
}

void CPlayAreaEntity::OnRemoved(IEntity *piEntity)
{
	UNSUBSCRIBE_FROM_CAST(piEntity,IEntityEvents);
	m_mEntities.erase(piEntity);
}

void CPlayAreaEntity::SetPosition(const CVector &vPosition)
{
	m_vPosition=vPosition;
	if(m_bActive && m_Route.GetPointCount()==0)
	{
		map<IEntity*,bool>::iterator i;
		for(i=m_mEntities.begin();i!=m_mEntities.end();i++)
	    {
			IEntity *piEntity=i->first;
			piEntity->GetPhysicInfo()->vPosition=m_vPosition;
		}
	}
}

void CPlayAreaEntity::SetAngles(const CVector &vAngles)
{
	m_vAngles=vAngles;
	if(m_bActive && m_Route.GetPointCount()==0)
	{
		map<IEntity*,bool>::iterator i;
		for(i=m_mEntities.begin();i!=m_mEntities.end();i++)
	    {
			IEntity *piEntity=i->first;
			piEntity->GetPhysicInfo()->vAngles=m_vAngles;
		}
	}
}
void CPlayAreaEntity::SetEntityType(IEntityType *piEntityType)
{
	m_EntityType.Attach(piEntityType);
	if(m_EntityType.m_piEntityType){m_dRTRadius=m_EntityType.m_piEntityType->DesignGetRadius();}
}
void CPlayAreaEntity::SetCount(unsigned int nCount){m_nEntityCount=nCount;}
void CPlayAreaEntity::SetDelay(int nDelay){m_nDelay=nDelay;}
void CPlayAreaEntity::SetRouteDelay(unsigned int nDelay){m_nRouteDelay=nDelay;}
void CPlayAreaEntity::SetInterval(unsigned int nInterval){m_nInterval=nInterval;}
void CPlayAreaEntity::SetDynamic(bool bDynamic){m_bDynamic=bDynamic;}

CVector CPlayAreaEntity::GetPosition(){return m_vPosition;}
CVector CPlayAreaEntity::GetAngles(){return m_vAngles;}
void	CPlayAreaEntity::GetEntityType(IEntityType **ppiEntityType){if(ppiEntityType){*ppiEntityType=ADD(m_EntityType.m_piEntityType);}	}

unsigned int CPlayAreaEntity::GetCount(){return m_nEntityCount;}
int          CPlayAreaEntity::GetDelay(){return m_nDelay;}
unsigned int CPlayAreaEntity::GetRouteDelay(){return m_nRouteDelay;}
unsigned int CPlayAreaEntity::GetInterval(){return m_nInterval;}

void CPlayAreaEntity::DesignRender( IGenericRender *piRender,bool bSelected )
{
	if(m_EntityType.m_piEntityType==NULL){return;}

	CVector vAngles=m_vAngles;
	if(m_Route.GetPointCount()){vAngles=AnglesFromVector(m_Route.GetAbsolutePoint(0)-m_vPosition);}
	m_EntityType.m_piEntityType->DesignRender(piRender,m_vPosition,vAngles,bSelected);

	/*
	 * Draws hits on the estimated shadow length for shadow preactivation
	CVector p1=m_vPosition+CVector(0+m_dRTActivationMin+m_dRTShadowPreactivation,m_vRTMins.c[1],0);
	CVector p2=m_vPosition+CVector(0+m_dRTActivationMin,m_vRTMins.c[1],0);
	piRender->RenderLine(p1,p2,CVector(1,0,0),0x8888);
	p1=m_vPosition+CVector(0+m_dRTActivationMin,m_vRTMins.c[1],0);
	p2=m_vPosition+CVector(0+m_dRTActivationMax,m_vRTMins.c[1],0);
	piRender->RenderLine(p1,p2,CVector(0,1,0),0xFFFF);
	*/
}

CTraceInfo CPlayAreaEntity::DesignGetTrace( const CVector &p1,const CVector &p2 )
{
	CVector vAngles=m_vAngles;
	if(m_Route.GetPointCount()){vAngles=AnglesFromVector(m_Route.GetAbsolutePoint(0)-m_vPosition);}
	
	if(m_EntityType.m_piEntityType==NULL)
	{
		CTraceInfo info;
		info.m_dTraceFraction=1.0;
		info.m_vTracePos=p2;
		return info;
	}
	else
	{
		return m_EntityType.m_piEntityType->DesignGetTrace(m_vPosition,vAngles,p1,p2);
	}
}

unsigned int CPlayAreaEntity::GetRoutePoints(){return m_Route.GetPointCount();}
bool CPlayAreaEntity::GetRoutePoint( unsigned int nIndex,SRoutePoint *psPoint ){return m_Route.GetPoint(nIndex,psPoint);}
bool CPlayAreaEntity::AddRoutePoint( unsigned int nIndex,const SRoutePoint &sPoint ){return m_Route.AddPoint(nIndex,sPoint);}
bool CPlayAreaEntity::SetRoutePoint( unsigned int nIndex,const SRoutePoint &sPoint ){return m_Route.SetPoint(nIndex,sPoint);}
void CPlayAreaEntity::RemoveRoutePoint( unsigned int nIndex ){m_Route.RemovePoint(nIndex);}
void CPlayAreaEntity::ClearRoute(){return m_Route.Clear();}
void CPlayAreaEntity::SetBonusType(IEntityType *piBonusType){m_BonusType.Attach(piBonusType);}
void CPlayAreaEntity::GetBonusType(IEntityType **ppiBonusType){if(ppiBonusType){*ppiBonusType=ADD(m_BonusType.m_piEntityType);}}
void CPlayAreaEntity::SetBonusOnChild(int nChildIndex){m_nBonusOnChild=nChildIndex;}
int  CPlayAreaEntity::GetBonusOnChild(){return m_nBonusOnChild;}

bool CPlayAreaEntity::PrepareResources()
{
	bool bOk=true;
	if(m_BonusType.m_piEntityType){bOk=bOk&&m_BonusType.m_piEntityType->PrepareResources();}
	if(m_EntityType.m_piEntityType){bOk=bOk&&m_EntityType.m_piEntityType->PrepareResources();}
	return bOk;
}
