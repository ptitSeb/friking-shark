#include "./stdafx.h"
#include "BomberType.h"

CBomberType::CBomberType()
{
  m_dMaxHealth=20;
  m_dMaxVelocity=20;
  m_dTimeFirstShotMin=0;
  m_dTimeFirstShotMax=0;
  m_dTimeBetweenShotsMin=0;
  m_dTimeBetweenShotsMax=0;
  m_nDamageType=DAMAGE_TYPE_NORMAL;
  m_nMovementType=PHYSIC_MOVE_TYPE_FLY;
}

CBomberType::~CBomberType(){}

IEntity *CBomberType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
  CBomber *piEntity=new CBomber(this,dwCurrentTime);
  InitializeEntity(piEntity,dwCurrentTime);
  piEntity->SetState(ENTITY_STATE_BASE,0);
  return piEntity;
}

CBomber::CBomber(CBomberType *pType,unsigned int dwCurrentTime)
{
  m_sClassName="CBomber";
  m_pType=pType;
  m_nRoutePoint=0;
  m_PhysicInfo.fOwnForce.dForce=0;
  m_PhysicInfo.fOwnForce.dMaxVelocity=m_pType->m_dMaxVelocity;
  m_dwNextProcessFrame=dwCurrentTime+100;
  m_dwNextShotTime=dwCurrentTime+drand()*(m_pType->m_dTimeFirstShotMax-m_pType->m_dTimeFirstShotMin)+m_pType->m_dTimeFirstShotMin;
}

void CBomber::OnKilled()
{
  m_PhysicInfo.dwMoveType=PHYSIC_MOVE_TYPE_NORMAL;
  CEntityBase::OnKilledInternal(false);
}

bool CBomber::OnCollision(IEntity *piOther,CVector &vCollisionPos)
{
  if(*piOther->GetEntityClass()=="CWorldEntity")
	{
		Remove();
	}
	else if(m_dHealth>0 && piOther->GetAlignment()!=m_dwAlignment)
	{
		piOther->OnDamage(m_dHealth,this);
	}
	return false;
}

void CBomber::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
  size_t nAnimationToSet=0;

	CEntityBase::ProcessFrame(dwCurrentTime,dTimeFraction);

  double dMaxHealth=GetMaxHealth();
  if(dMaxHealth)
  {
	unsigned int nAnimations=m_pTypeBase->GetStateAnimations(ENTITY_STATE_BASE);
	nAnimationToSet=(size_t)(((dMaxHealth-m_dHealth)/dMaxHealth)*((double)nAnimations));
	if(nAnimationToSet>nAnimations-1){nAnimationToSet=nAnimations-1;}
    SetState(ENTITY_STATE_BASE,(int)nAnimationToSet);
  }
  if(m_dHealth>0)
  {
    if(!m_piRoute){return;}
    if(m_nRoutePoint>=m_piRoute->GetPointCount()-1){return;}

    CVector vDir=m_piRoute->GetAbsolutePoint(m_nRoutePoint+1)-m_PhysicInfo.vPosition;
    double dDist=vDir.N();
    if(dDist/2.0<m_PhysicInfo.vVelocity)
    {
      if(m_nRoutePoint==m_piRoute->GetPointCount()-2){Remove();}
      m_nRoutePoint++;
    }
    else
    {
      m_PhysicInfo.fOwnForce.vDir=vDir;
      m_PhysicInfo.fOwnForce.dForce=m_pType->m_dMaxVelocity;
      m_PhysicInfo.fOwnForce.dMaxVelocity=vDir*m_pType->m_dMaxVelocity;
      m_dwNextProcessFrame=dwCurrentTime+10;
    }
    if(dwCurrentTime>m_dwNextShotTime)
    {
      FireWeapon(0,dwCurrentTime);
      m_dwNextShotTime=dwCurrentTime+drand()*(m_pType->m_dTimeBetweenShotsMax-m_pType->m_dTimeBetweenShotsMin)+m_pType->m_dTimeBetweenShotsMin;
    }
  }
}

IEntity *CBomber::GetTarget()
{
  if(m_piTarget==NULL)
  {
    IEntityManager *piManager=GetEntityManager();
    if(piManager)
    {
      m_piTarget=piManager->FindEntity("Player");
    }
  }
  return m_piTarget;
}
