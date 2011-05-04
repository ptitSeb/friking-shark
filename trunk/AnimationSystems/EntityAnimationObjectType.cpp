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
#include "AnimationSystems.h"
#include "AnimationObjectBase.h"
#include "EntityAnimationObjectType.h"

DECLARE_CUSTOM_WRAPPER1(CPlayAreaManagerWrapper,IPlayAreaManager,m_piPlayAreaManager)

CEntityAnimationObjectType::CEntityAnimationObjectType()
{
    m_nTime=0;
}

CEntityAnimationObjectType::~CEntityAnimationObjectType()
{
}

IAnimationObject *CEntityAnimationObjectType::CreateInstance(IAnimation *piAnimation,unsigned int dwCurrentTime)
{
    CEntityAnimationObject *pEntityAnimationObject=new CEntityAnimationObject(this,piAnimation);
	return pEntityAnimationObject;
}

void CEntityAnimationObjectType::GetConfig(SEntityAnimationObjectTypeConfig *pConfig)
{
	pConfig->nTime=m_nTime;
}

void CEntityAnimationObjectType::SetConfig(SEntityAnimationObjectTypeConfig *pConfig)
{
	m_nTime=pConfig->nTime;
}

void CEntityAnimationObjectType::SetEntityType(IEntityType *piEntity){m_EntityType.Attach(piEntity);}
void CEntityAnimationObjectType::GetEntityType(IEntityType **ppiEntity){(*ppiEntity)=ADD(m_EntityType.m_piEntityType);}
std::string CEntityAnimationObjectType::GetAnimationObjectDescription(){return m_EntityType.m_piObject?m_EntityType.m_piObject->GetName():"Unknown";}

CVector CEntityAnimationObjectType::GetPosition(){return m_vPosition;}
void	CEntityAnimationObjectType::SetPosition(CVector vPosition){m_vPosition=vPosition;}

CVector CEntityAnimationObjectType::GetAngles(){return m_vAngles;}
void	CEntityAnimationObjectType::SetAngles(CVector vAngles){m_vAngles=vAngles;}

CEntityAnimationObject::CEntityAnimationObject(CEntityAnimationObjectType *pType,IAnimation *piAnimation)
:CAnimationObjectBase(pType,piAnimation)
{
    m_pType=pType;
	m_bEntityCreated=false;
}
CEntityAnimationObject::~CEntityAnimationObject()
{
}

void CEntityAnimationObject::Activate(unsigned int dwCurrentTime)
{
    CAnimationObjectBase::Activate(dwCurrentTime);
}

void CEntityAnimationObject::Deactivate()
{
	m_bEntityCreated=false;
    CAnimationObjectBase::Deactivate();
}

void CEntityAnimationObject::CustomRender(IGenericRender *piRender,IGenericCamera *piCamera){}
bool CEntityAnimationObject::ProcessFrame(IPhysicManager *pPhysicManager,unsigned int dwCurrentTime,double dInterval)
{
	if(!m_bActive){return false;}
	if(m_pType->m_EntityType.m_piEntityType==NULL){return false;}
	if(m_bEntityCreated){return false;}
	if(dwCurrentTime>m_pType->m_nTime)
	{
		CVector vTempPos,vTempAngles;
		CVector vPosition,vAngles;
		IEntity *piAnimationEntity=m_piAnimation->GetEntity();
		if(piAnimationEntity)
		{
			vTempPos=piAnimationEntity->GetPhysicInfo()->vPosition;
			vTempAngles=piAnimationEntity->GetPhysicInfo()->vAngles;
		}
		ComputeReferenceSystem(vTempPos,vTempAngles,m_pType->m_vPosition,m_pType->m_vAngles,&vPosition,&vAngles);
		
		CPlayAreaManagerWrapper playAreaManager;
		playAreaManager.Attach("GameSystem","PlayAreaManager");
		if(playAreaManager.m_piPlayAreaManager)
		{
			playAreaManager.m_piPlayAreaManager->CreateDynamicEntityElement(m_pType->m_EntityType.m_piEntityType,vPosition,vAngles,NULL);
		}
		m_bEntityCreated=true;
	}
	return false;
}

void CEntityAnimationObjectType::DesignRender( IGenericRender *piRender,CVector &vPosition,CVector &vAngles ,bool bSelected)
{
	if(m_EntityType.m_piEntityType)
	{
		CVector vTempPos,vTempAngles;
		ComputeReferenceSystem(vPosition,vAngles,m_vPosition,m_vAngles,&vTempPos,&vTempAngles);
		m_EntityType.m_piEntityType->DesignRender(piRender,vTempPos,vTempAngles,bSelected);
	}
}