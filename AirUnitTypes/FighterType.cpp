#include "./stdafx.h"
#include "FighterType.h"

CFighterType::CFighterType()
{
  m_dMaxSpeed=0;
  m_dMaxHealth=0;
  m_dMaxRoll=0;
  m_dMaxAngularSpeed=0;
  m_dTimeFirstShotMin=0;
  m_dTimeFirstShotMax=0;
  m_dTimeBetweenShotsMin=0;
  m_dTimeBetweenShotsMax=0;
}

CFighterType::~CFighterType(){}

IEntity *CFighterType::CreateInstance(IEntity *piParent,DWORD dwCurrentTime)
{
  CFighter *piEntity=new CFighter(this,dwCurrentTime);
  InitializeEntity(piEntity,dwCurrentTime);
  piEntity->SetCurrentAnimation(0);
  return piEntity;
}

CFighter::CFighter(CFighterType *pType,DWORD dwCurrentTime)
{
  m_sClassName="CFighter";
  m_pType=pType;
  m_nRoutePoint=0;
  m_dwDamageType=DAMAGE_TYPE_NORMAL;
  m_dMaxHealth=m_dHealth=pType->m_dMaxHealth;
  m_dwNextProcessFrame=dwCurrentTime+10;
  m_dwNextShotTime=dwCurrentTime+drand()*(m_pType->m_dTimeFirstShotMax-m_pType->m_dTimeFirstShotMin)+m_pType->m_dTimeFirstShotMin;
}

void CFighter::OnKilled()
{
  bool bRemove=false;
  if(m_dAnimations.size()>1)
  {
    if(GetCurrentAnimation()!=1)
    {
      m_PhysicInfo.vAngleVelocity.c[2]+=drand()*300.0-150.0;
      SetCurrentAnimation(1);
      m_PhysicInfo.dwMoveType=PHYSIC_MOVE_TYPE_NORMAL;
    }
  }
  else
  {
    m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
    bRemove=true;
  }
  CEntityBase::OnKilledInternal(bRemove);
}

bool CFighter::OnCollision(IEntity *piOther,CVector &vCollisionPos)
{
	if(GetCurrentAnimation()==1 && piOther->GetAlignment()!=ENTITY_ALIGNMENT_PLAYER)
	{
		if(GetCurrentAnimation()!=2 && m_dAnimations.size()>2)
		{
			SetCurrentAnimation(2);
		}
		Remove();
	} 
	else if(m_dHealth>0 && piOther->GetAlignment()!=m_dwAlignment)
	{
		piOther->OnDamage(m_dHealth,this);
	}
  return false;
}

void CFighter::ProcessFrame(DWORD dwCurrentTime,double dTimeFraction)
{
	CEntityBase::ProcessFrame(dwCurrentTime,dTimeFraction);

  if(!m_piRoute){return;}
  if(m_dHealth<=0)
  {
    if(GetCurrentAnimation()==1)
    {
      ApproachAngle(m_PhysicInfo.vAngles.c[1],40,30);
    }
    return;
  }

  m_dwNextProcessFrame=dwCurrentTime+10;

  CVector vDest=m_piRoute->GetAbsolutePoint(m_piRoute->GetNextPointIndex(m_nRoutePoint));
  CVector vDir=vDest-m_PhysicInfo.vPosition;
  double dDist=vDir.N();
  if(dDist<m_pType->m_dMaxSpeed/4.0)
  {
    int nNext=m_piRoute->GetNextPointIndex(m_nRoutePoint);
    if(nNext==m_nRoutePoint)
    {
      Remove();
    }
    else
    {
      m_nRoutePoint=nNext;
    }

  }
  else
  {
    CVector vAngles,vForward,vRight,vTempAngles=m_PhysicInfo.vAngles;
    AnglesFromVector(vDir,&vAngles);
    VectorsFromAngles(m_PhysicInfo.vAngles,&vForward,&vRight);

    if(vForward!=vDir)
    {
      CPlane plane(vRight,m_PhysicInfo.vPosition);
      if(plane.GetSide(vDest)>0)
      {
        m_PhysicInfo.vAngles.c[ROLL]+=m_pType->m_dMaxRoll*dTimeFraction;
        if(m_PhysicInfo.vAngles.c[ROLL]>m_pType->m_dMaxRoll){m_PhysicInfo.vAngles.c[ROLL]=m_pType->m_dMaxRoll;}
      }
      else
      {
        m_PhysicInfo.vAngles.c[ROLL]-=m_pType->m_dMaxRoll*dTimeFraction;
        if(m_PhysicInfo.vAngles.c[ROLL]<-m_pType->m_dMaxRoll){m_PhysicInfo.vAngles.c[ROLL]=-m_pType->m_dMaxRoll;}
      }
    }
    else
    {
      if(m_PhysicInfo.vAngles.c[ROLL]>0)
      {
        m_PhysicInfo.vAngles.c[ROLL]-=m_pType->m_dMaxAngularSpeed*dTimeFraction;
        if(m_PhysicInfo.vAngles.c[ROLL]<0){m_PhysicInfo.vAngles.c[ROLL]=0;}
      }
      else
      {
        m_PhysicInfo.vAngles.c[ROLL]+=m_pType->m_dMaxAngularSpeed*dTimeFraction;
        if(m_PhysicInfo.vAngles.c[ROLL]>0){m_PhysicInfo.vAngles.c[ROLL]=0;}
      }
    }
    double dCurrentAngularSpeed=(m_PhysicInfo.vAngles.c[ROLL]/m_pType->m_dMaxRoll)*m_pType->m_dMaxAngularSpeed;
    m_PhysicInfo.vAngles.c[YAW]=ApproachAngle(m_PhysicInfo.vAngles.c[YAW],vAngles.c[YAW],-dCurrentAngularSpeed*dTimeFraction);
    VectorsFromAngles(m_PhysicInfo.vAngles,&m_PhysicInfo.vVelocity);
    m_PhysicInfo.vVelocity*=m_pType->m_dMaxSpeed;
    m_dwNextProcessFrame=dwCurrentTime+10;
  }

  if(dwCurrentTime>m_dwNextShotTime)
  {
    FireWeapon(0,dwCurrentTime);
    m_dwNextShotTime=dwCurrentTime+drand()*(m_pType->m_dTimeBetweenShotsMax-m_pType->m_dTimeBetweenShotsMin)+m_pType->m_dTimeBetweenShotsMin;
  }
}

void CFighter::Render(IGenericRender *piRender,IGenericCamera *piCamera)
{
  CEntityBase::Render(piRender,piCamera);
}

IEntity *CFighter::GetTarget()
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
 
