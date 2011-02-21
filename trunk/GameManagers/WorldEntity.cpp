#include "./stdafx.h"
#include "WorldEntity.h"
#include "WorldManager.h"

CWorldEntity::CWorldEntity(CWorldManager *pManager)
{
    m_sName="World";
    m_sClassName="CWorldEntity";
    m_pManager=pManager;

	m_dHealth=100000000;
	m_dwDamageType=DAMAGE_TYPE_NORMAL;
    m_PhysicInfo.dwMoveType=PHYSIC_MOVE_TYPE_NONE;
    m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_BSP;
}

CWorldEntity::~CWorldEntity(void)
{
}

void CWorldEntity::Render(IGenericRender *piRender,IGenericCamera *piCamera)
{
    m_pManager->Render(piRender,piCamera);
}

void CWorldEntity::OnDamage(double dDamage,IEntity *pAggresor)
{
}

CTraceInfo CWorldEntity::GetTrace( const CVector &p1,const CVector &p2 )
{
	CTraceInfo info;
	if(m_pManager->m_pTerrainBSP)
	{
		info=m_pManager->m_pTerrainBSP->GetTrace(p1,p2,p1,p2);
	}
	else
	{
		info.m_dTraceFraction=1.0;
		info.m_vTracePos=p2;
	}
	return info;
}