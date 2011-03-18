#include "./stdafx.h"
#include "GameEntityLib.h"
#include "EntityBase.h"
#include "EntityTypeBase.h"


CEntityTypeBase::CEntityTypeBase(void)
{
	m_dMaxVelocity=0;
	m_dMaxHealth=1;
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
	CAnimationTypeWrapper *pAnimation=GetStateAnimation(ENTITY_STATE_BASE,0);
	if(pAnimation){return pAnimation->m_piAnimationType->DesignGetRadius();}
	return 0;
}

void CEntityTypeBase::DesignRender(IGenericRender *piRender,CVector &vPosition,CVector &vAngles, bool bSelected)
{
	CAnimationTypeWrapper *pAnimation=GetStateAnimation(ENTITY_STATE_BASE,0);
	if(pAnimation){pAnimation->m_piAnimationType->DesignRender(piRender,vPosition,vAngles,bSelected);}
}

void CEntityTypeBase::DesignGetBBox( CVector *pvMins,CVector *pvMaxs )
{
	CAnimationTypeWrapper *pAnimation=GetStateAnimation(ENTITY_STATE_BASE,0);
	if(pAnimation){pAnimation->m_piAnimationType->DesignGetBBox(pvMins,pvMaxs);}
}

CTraceInfo CEntityTypeBase::DesignGetTrace( const CVector &vPosition,const CVector &vAngles,const CVector &p1,const CVector &p2 )
{
	CTraceInfo traceInfo;
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
	pConfig->vBBoxMins=m_vBBoxMins;
	pConfig->vBBoxMaxs=m_vBBoxMaxs;
	pConfig->nMovementType=m_nMovementType;
	pConfig->nCollisionType=m_nCollisionType;
	pConfig->nDamageType=m_nDamageType;
	pConfig->nBoundsType=m_nBoundsType;
	pConfig->nAlignment=m_nAlignment;
}

void CEntityTypeBase::SetEntityTypeConfig(SEntityTypeConfig *pConfig)
{
	m_vBBoxMins=pConfig->vBBoxMins;
	m_vBBoxMaxs=pConfig->vBBoxMaxs;
	m_nMovementType=pConfig->nMovementType;
	m_nCollisionType=pConfig->nCollisionType;
	m_nDamageType=pConfig->nDamageType;
	m_nBoundsType=pConfig->nBoundsType;
	m_nAlignment=pConfig->nAlignment;
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
		if(x==nAnimation){pState->vAnimations.erase(i);return true;}
	}
	for(x=0,i=m_mStateAnimations[pState->sName].begin();x<m_mStateAnimations[pState->sName].size();x++,i++)
	{
		if(x==nAnimation){m_mStateAnimations[pState->sName].erase(i);return true;}
	}
	return false;
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
