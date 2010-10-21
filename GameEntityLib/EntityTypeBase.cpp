#include "StdAfx.h"
#include "GameEntityLib.h"
#include ".\entitybase.h"
#include ".\entitytypebase.h"


CEntityTypeBase::CEntityTypeBase(void)
{
    m_bSolid=false;
	m_bUseBSP=false;
    m_bFlyMove=true;
    m_dwAlignment=ENTITY_ALIGNMENT_NEUTRAL;
}
CEntityTypeBase::~CEntityTypeBase(void){}

void CEntityTypeBase::InitializeEntity(CEntityBase *pEntity,DWORD dwCurrentTime)
{
	unsigned long x;
    for(x=0;x<m_dAnimations.size();x++)
    {
        IAnimationType  *piAnimationType=m_dAnimations[x].m_piAnimationType;
        IAnimation      *piAnimation=piAnimationType->CreateInstance(pEntity,dwCurrentTime);
        if(piAnimation){pEntity->AddAnimation(x,piAnimation);}
    }
	
    for(x=0;x<m_dWeapons.size();x++)
    {
      IWeaponType  *piWeaponType=m_dWeapons[x].m_piWeaponType;
      IWeapon      *piWeapon=piWeaponType->CreateInstance(pEntity,dwCurrentTime);
      if(piWeapon){pEntity->AddWeapon(piWeapon);}
    }

    SPhysicInfo *pPhysicInfo=pEntity->GetPhysicInfo();
    pPhysicInfo->dwMoveType=m_bFlyMove?PHYSIC_MOVE_TYPE_FLY:PHYSIC_MOVE_TYPE_NORMAL;
	pPhysicInfo->dwBoundsType=m_bSolid?PHYSIC_BOUNDS_TYPE_BBOX:PHYSIC_BOUNDS_TYPE_NONE;
	if(m_bUseBSP){pPhysicInfo->dwBoundsType=PHYSIC_BOUNDS_TYPE_BSP;}
    pPhysicInfo->vMins=m_vBBoxMins;
    pPhysicInfo->vMaxs=m_vBBoxMaxs;
    pEntity->SetAlignment(m_dwAlignment);
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
	if(m_dAnimations.size())
	{
		return m_dAnimations[0].m_piAnimationType->DesignGetRadius();
	}
	return 0;
}

void CEntityTypeBase::DesignRender(IGenericRender *piRender,CVector &vPosition,CVector &vAngles, bool bSelected)
{
	if(m_dAnimations.size())
	{
		m_dAnimations[0].m_piAnimationType->DesignRender(piRender,vPosition,vAngles,bSelected);
	}
}

void CEntityTypeBase::DesignGetBBox( CVector *pvMins,CVector *pvMaxs )
{
	if(m_dAnimations.size())
	{
		m_dAnimations[0].m_piAnimationType->DesignGetBBox(pvMins,pvMaxs);
	}
}

CTraceInfo CEntityTypeBase::DesignGetTrace( const CVector &vPosition,const CVector &vAngles,const CVector &p1,const CVector &p2 )
{
	CTraceInfo traceInfo;
	if(m_dAnimations.size())
	{
		traceInfo=m_dAnimations[0].m_piAnimationType->DesignGetTrace(vPosition,vAngles,p1,p2);
	}
	else
	{
		traceInfo.m_dTraceFraction=1.0;
		traceInfo.m_vTracePos=p2;
	}
	return traceInfo;
}