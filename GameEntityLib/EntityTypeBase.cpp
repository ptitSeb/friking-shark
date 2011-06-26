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
#include "GameEntityLib.h"
#include "EntityBase.h"
#include "EntityTypeBase.h"


CEntityTypeBase::CEntityTypeBase(void)
{
	m_dMaxVelocity=0;
	m_dMaxHealth=1;
	m_nPoints=0;
	m_nDamageType=DAMAGE_TYPE_NONE;
	m_nBoundsType=PHYSIC_BOUNDS_TYPE_BBOX;
	m_nMovementType=PHYSIC_MOVE_TYPE_NORMAL;
	m_nCollisionType=PHYSIC_COLLISION_TYPE_STUCK;
	m_nAlignment=ENTITY_ALIGNMENT_NEUTRAL;
}

CEntityTypeBase::~CEntityTypeBase(void){}

bool CEntityTypeBase::Init(std::string sClass,std::string sName,ISystem *piSystem)
{
	bool bOk=CSystemObjectBase::Init(sClass,sName,piSystem);
	if(bOk){InitializeStates();}
	return bOk;
}

bool CEntityTypeBase::Unserialize(ISystemPersistencyNode *piNode)
{
	bool bOk=CSystemObjectBase::Unserialize(piNode);
	if(bOk){MapStateAnimations();}
	return bOk;
}

void CEntityTypeBase::InitializeStates(){m_vStates.clear();RegisterStates();}

void CEntityTypeBase::InitializeEntity(CEntityBase *pEntity,unsigned int dwCurrentTime)
{
    for(unsigned int x=0;x<m_vWeapons.size();x++)
    {
      IWeaponType  *piWeaponType=m_vWeapons[x].m_piWeaponType;
      IWeapon      *piWeapon=piWeaponType->CreateInstance(pEntity,dwCurrentTime);
      if(piWeapon){pEntity->AddWeapon(piWeapon);}
    }

    SPhysicInfo *pPhysicInfo=pEntity->GetPhysicInfo();
    pPhysicInfo->dwMoveType=m_nMovementType;
	pPhysicInfo->dwBoundsType=m_nBoundsType;
	pPhysicInfo->dwCollisionType=m_nCollisionType;
    pPhysicInfo->vMins=m_vBBoxMins;
    pPhysicInfo->vMaxs=m_vBBoxMaxs;
	pPhysicInfo->dMaxVelocity=m_dMaxVelocity;
	pPhysicInfo->dMaxForce=m_dMaxVelocity;

	pEntity->SetEntityTypeBase(this);
    pEntity->SetAlignment(m_nAlignment);
	pEntity->SetDamageType(m_nDamageType);
	pEntity->SetHealth(m_dMaxHealth);
	pEntity->SetMaxHealth(m_dMaxHealth);
	pEntity->SetPlacement(m_nPlacement);
	
	for(unsigned int x=0;x<m_vChildren.size();x++)
	{
		if(m_vChildren[x].entityType.m_piEntityType==NULL){continue;}
		IEntity *piEntity=m_vChildren[x].entityType.m_piEntityType->CreateInstance(NULL,dwCurrentTime);
		if(piEntity){pEntity->AddChild(piEntity,m_vChildren[x].vPosition,m_vChildren[x].vAngles);}
	}
}

void CEntityTypeBase::GetBBox( CVector *pvMins,CVector *pvMaxs )
{
	if(pvMins){*pvMins=m_vBBoxMins;}
	if(pvMaxs){*pvMaxs=m_vBBoxMaxs;}
}

CVector CEntityTypeBase::GetSize()
{
	return m_vBBoxMaxs-m_vBBoxMins;
}

double CEntityTypeBase::DesignGetRadius()
{
	CVector vMins,vMaxs;
	DesignGetBBox(&vMins,&vMaxs);
	return GetBBoxRadius(vMins,vMaxs);
}

void CEntityTypeBase::DesignRender(IGenericRender *piRender,CVector &vPosition,CVector &vAngles, bool bSelected)
{
	CAnimationTypeWrapper *pAnimation=GetStateAnimation(ENTITY_STATE_BASE,0);
	if(pAnimation){pAnimation->m_piAnimationType->DesignRender(piRender,vPosition,vAngles,bSelected);}

	for(unsigned int x=0;x<m_vChildren.size();x++)
	{
		if(!m_vChildren[x].entityType.m_piEntityType){continue;}

		CVector vTempPos,vTempAngles;
		ComputeReferenceSystem(vPosition,vAngles,m_vChildren[x].vPosition,m_vChildren[x].vAngles,&vTempPos,&vTempAngles);
		m_vChildren[x].entityType.m_piEntityType->DesignRender(piRender,vTempPos,vTempAngles,bSelected);
	}
}

void CEntityTypeBase::DesignGetBBox( CVector *pvMins,CVector *pvMaxs )
{
	CVector vFakeMins(1000,1000,1000),vFakeMaxs(-1000,-1000,-1000);
	CVector vMins(1000,1000,1000),vMaxs(-1000,-1000,-1000);

	CAnimationTypeWrapper *pAnimation=GetStateAnimation(ENTITY_STATE_BASE,0);
	if(pAnimation){pAnimation->m_piAnimationType->DesignGetBBox(&vMins,&vMaxs);}

	for(unsigned int x=0;x<m_vChildren.size();x++)
	{
		if(!m_vChildren[x].entityType.m_piEntityType){continue;}

		CVector vTempMins,vTempMaxs;
		m_vChildren[x].entityType.m_piEntityType->DesignGetBBox(&vTempMins,&vTempMaxs);
		vTempMins+=m_vChildren[x].vPosition;
		vTempMaxs+=m_vChildren[x].vPosition;
		for(unsigned int c=0;c<3;c++)
		{
			if(vTempMins.c[c]<vMins.c[c]){vMins.c[c]=vTempMins.c[c];}
			if(vTempMaxs.c[c]>vMaxs.c[c]){vMaxs.c[c]=vTempMaxs.c[c];}
		}
	}
	if(vFakeMins!=vMins && vFakeMaxs!=vMaxs)
	{
		if(pvMins){*pvMins=vMins;}
		if(pvMaxs){*pvMaxs=vMaxs;}
	}
}

CTraceInfo CEntityTypeBase::DesignGetTrace( const CVector &vPosition,const CVector &vAngles,const CVector &p1,const CVector &p2 )
{
	CTraceInfo traceInfo;
	traceInfo.m_dTraceFraction=1.0;
	traceInfo.m_vTracePos=p2;
	CAnimationTypeWrapper *pAnimation=GetStateAnimation(ENTITY_STATE_BASE,0);
	if(pAnimation)
	{
		traceInfo=pAnimation->m_piAnimationType->DesignGetTrace(vPosition,vAngles,p1,p2);
	}
	else
	{
		traceInfo.m_dTraceFraction=1.0;
		traceInfo.m_vTracePos=p2;
	}
	for(unsigned int x=0;x<m_vChildren.size();x++)
	{
		if(!m_vChildren[x].entityType.m_piEntityType){continue;}
		CVector vTempPos,vTempAngles;
		ComputeReferenceSystem(vPosition,vAngles,m_vChildren[x].vPosition,m_vChildren[x].vAngles,&vTempPos,&vTempAngles);

		CTraceInfo temp=m_vChildren[x].entityType.m_piEntityType->DesignGetTrace(vTempPos,vTempAngles,p1,p2);
		if(temp.m_dTraceFraction<traceInfo.m_dTraceFraction)
		{
			traceInfo=temp;
		}
	}
	return traceInfo;
}

CAnimationTypeWrapper *CEntityTypeBase::GetStateAnimation(unsigned int nState,unsigned int nIndex)
{
	if(nState>=m_vStates.size()){return NULL;}
	SEntityState *pState=&m_vStates[nState];
	return (nIndex<pState->vAnimations.size())?&pState->vAnimations[nIndex]:NULL;
}

void CEntityTypeBase::MapStateAnimations()
{
	for(unsigned int x=0;x<m_vStates.size();x++)
	{
		m_vStates[x].vAnimations.clear();
		map<std::string,vector<CAnimationTypeWrapper> >::iterator i=m_mStateAnimations.find(m_vStates[x].sName);
		if(i!=m_mStateAnimations.end())
		{
			for(unsigned int y=0;y<i->second.size();y++)
			{
				CAnimationTypeWrapper wrapper;
				wrapper.Attach(i->second[y].m_piAnimationType);
				m_vStates[x].vAnimations.push_back(wrapper);
			}
		}
	}
}

IAnimation  *CEntityTypeBase::CreateStateAnimation(CEntityBase *pEntity,unsigned int nState,unsigned int nIndex,unsigned int nCurrentTime)
{
	if(nIndex==ANIMATION_RANDOM )
	{
		unsigned int nAnimations=GetStateAnimations(nState);
		if(nAnimations)
		{
			nIndex=(unsigned int)(((double)rand()/(double)RAND_MAX)*(double)nAnimations);
			if(nIndex==nAnimations){nIndex=nAnimations-1;}
		}
	}
	CAnimationTypeWrapper *pWrapper=GetStateAnimation(nState,nIndex);
	return pWrapper?pWrapper->m_piAnimationType->CreateInstance(pEntity,nCurrentTime):NULL;
}

unsigned int CEntityTypeBase::GetStateAnimations(unsigned int nState)
{
	if(nState>=m_vStates.size()){return 0;}
	SEntityState *pState=&m_vStates[nState];
	return pState->vAnimations.size();
}

void CEntityTypeBase::GetEntityTypeConfig(SEntityTypeConfig *pConfig)
{
	pConfig->dMaxHealth=m_dMaxHealth;
	pConfig->dMaxVelocity=m_dMaxVelocity;
	pConfig->vBBoxMins=m_vBBoxMins;
	pConfig->vBBoxMaxs=m_vBBoxMaxs;
	pConfig->nMovementType=m_nMovementType;
	pConfig->nCollisionType=m_nCollisionType;
	pConfig->nDamageType=m_nDamageType;
	pConfig->nBoundsType=m_nBoundsType;
	pConfig->nAlignment=m_nAlignment;
	pConfig->nPoints=m_nPoints;
	pConfig->nPlacement=m_nPlacement;
}

void CEntityTypeBase::SetEntityTypeConfig(SEntityTypeConfig *pConfig)
{
	m_dMaxHealth=pConfig->dMaxHealth;
	m_dMaxVelocity=pConfig->dMaxVelocity;
	m_vBBoxMins=pConfig->vBBoxMins;
	m_vBBoxMaxs=pConfig->vBBoxMaxs;
	m_nMovementType=pConfig->nMovementType;
	m_nCollisionType=pConfig->nCollisionType;
	m_nDamageType=pConfig->nDamageType;
	m_nBoundsType=pConfig->nBoundsType;
	m_nAlignment=pConfig->nAlignment;
	m_nPoints=pConfig->nPoints;
	m_nPlacement=pConfig->nPlacement;
}

// State Management

unsigned int CEntityTypeBase::GetStateCount(){return m_vStates.size();}
bool CEntityTypeBase::GetStateName(unsigned int nIndex,std::string *psName)
{
	*psName="";
	if(nIndex>=m_vStates.size()){return false;}
	*psName=m_vStates[nIndex].sName;
	return true;
}

// Animation management

unsigned int CEntityTypeBase::AddStateAnimation(unsigned int nState)
{
	if(nState>=m_vStates.size()){return ANIMATION_INVALID;}
	SEntityState *pState=&m_vStates[nState];
	CAnimationTypeWrapper wrapper;
	int nIndex=ANIMATION_INVALID;
	bool bOk=wrapper.Create("Animations","AnimationType","");
	if(bOk)
	{
		nIndex=pState->vAnimations.size();
		pState->vAnimations.push_back(wrapper);
		m_mStateAnimations[pState->sName].push_back(wrapper);
	}
	return nIndex;
}

bool CEntityTypeBase::RemoveStateAnimation(unsigned int nState,unsigned int nAnimation)
{
	if(nState>=m_vStates.size()){return false;}
	SEntityState *pState=&m_vStates[nState];
	if(nAnimation>=pState->vAnimations.size()){return false;}
	unsigned int x=0;
	vector<CAnimationTypeWrapper>::iterator i;
	for(x=0,i=pState->vAnimations.begin();x<pState->vAnimations.size();x++,i++)
	{
		if(x==nAnimation){pState->vAnimations.erase(i);break;}
	}
	for(x=0,i=m_mStateAnimations[pState->sName].begin();x<m_mStateAnimations[pState->sName].size();x++,i++)
	{
		if(x==nAnimation){m_mStateAnimations[pState->sName].erase(i);break;}
	}
	return true;
}

bool CEntityTypeBase::GetStateAnimation(unsigned int nState,unsigned int nAnimation,IAnimationType **ppiAnimation)
{
	*ppiAnimation=NULL;
	if(nState>=m_vStates.size()){return false;}
	SEntityState *pState=&m_vStates[nState];
	if(nAnimation>=pState->vAnimations.size()){return false;}
	*ppiAnimation=ADD(m_vStates[nState].vAnimations[nAnimation].m_piAnimationType);
	return true;
}

unsigned int CEntityTypeBase::GetStateAnimationCount(unsigned int nState)
{
	if(nState>=m_vStates.size()){return 0;}
	return m_vStates[nState].vAnimations.size();	
}

// Weapon management

unsigned int CEntityTypeBase::AddWeapon(std::string sWeaponType)
{
	CWeaponTypeWrapper wrapper;
	int nIndex=WEAPON_INVALID;
	bool bOk=wrapper.Create("Weapons",sWeaponType,"");
	if(bOk)
	{
		nIndex=m_vWeapons.size();
		m_vWeapons.push_back(wrapper);
	}
	return nIndex;
}

bool CEntityTypeBase::RemoveWeapon(unsigned int nWeapon)
{
	if(nWeapon>=m_vWeapons.size()){return false;}
	unsigned int x=0;
	vector<CWeaponTypeWrapper>::iterator i;
	for(x=0,i=m_vWeapons.begin();x<m_vWeapons.size();x++,i++)
	{
		if(x==nWeapon){m_vWeapons.erase(i);return true;}
	}
	return false;
}

bool CEntityTypeBase::GetWeapon(unsigned int nWeapon,IWeaponType **ppiWeapon)
{
	*ppiWeapon=NULL;
	if(nWeapon>=m_vWeapons.size()){return false;}
	*ppiWeapon=ADD(m_vWeapons[nWeapon].m_piWeaponType);
	return true;
}

unsigned int CEntityTypeBase::GetWeaponCount(unsigned int nState){return m_vWeapons.size();}

// Child management

unsigned int CEntityTypeBase::AddChild(std::string sEntityType)
{
	SChildEntityType child;
	int nIndex=WEAPON_INVALID;
	bool bOk=child.entityType.Attach("EntityTypes",sEntityType);
	if(bOk)
	{
		nIndex=m_vChildren.size();
		m_vChildren.push_back(child);
	}
	return nIndex;
}

bool CEntityTypeBase::RemoveChild(unsigned int nChild)
{
	if(nChild>=m_vChildren.size()){return false;}
	unsigned int x=0;
	vector<SChildEntityType>::iterator i;
	for(x=0,i=m_vChildren.begin();x<m_vChildren.size();x++,i++)
	{
		if(x==nChild){m_vChildren.erase(i);return true;}
	}
	return false;
}

bool CEntityTypeBase::GetChild(unsigned int nChild,IEntityType **ppiChild)
{
	*ppiChild=NULL;
	if(nChild>=m_vChildren.size()){return false;}
	*ppiChild=ADD(m_vChildren[nChild].entityType.m_piEntityType);
	return true;
}

unsigned int CEntityTypeBase::GetChildren(){return m_vChildren.size();}

void CEntityTypeBase::SetChildLocation(unsigned int nChild,CVector vPosition,CVector vAngles)
{
	if(nChild>=m_vChildren.size()){return;}
	m_vChildren[nChild].vPosition=vPosition;
	m_vChildren[nChild].vAngles=vAngles;
}

void CEntityTypeBase::GetChildLocation(unsigned int nChild,CVector &vPosition,CVector &vAngles)
{
	if(nChild>=m_vChildren.size()){return;}
	vPosition=m_vChildren[nChild].vPosition;
	vAngles=m_vChildren[nChild].vAngles;
}