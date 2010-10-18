#include "StdAfx.h"
#include ".\playertype.h"

CPlayerType::CPlayerType(){}
CPlayerType::~CPlayerType(){}

IEntity *CPlayerType::CreateInstance(IEntity *piParent,DWORD dwCurrentTime)
{
    CPlayer *piEntity=new CPlayer(this);
    InitializeEntity(piEntity,dwCurrentTime);
    piEntity->GetPhysicInfo()->dwMoveType=PHYSIC_MOVE_TYPE_CUSTOM;
    piEntity->SetCurrentAnimation(0);
    piEntity->SetSpeed(m_dMaxSpeed);
    return piEntity;
}

CPlayer::CPlayer(CPlayerType *pType)
{
  m_dHealth=10000000;
  m_dwPoints=0;
  m_dwLivesLeft=1;
  m_dSpeed=80;
  m_sClassName="CPlayer";
  m_sName="Player";
  m_pType=pType;
	m_dwDamageType=DAMAGE_TYPE_NORMAL;
}

DWORD CPlayer::GetPoints(){return m_dwPoints;}
void  CPlayer::SetPoints(DWORD dwPoints){m_dwPoints=dwPoints;}
void  CPlayer::AddPoints(DWORD dwPoints){m_dwPoints+=dwPoints;}

DWORD CPlayer::GetLivesLeft(){return m_dwLivesLeft;}
void  CPlayer::SetLivesLeft(DWORD dwLivesLeft){m_dwLivesLeft=dwLivesLeft;}
void  CPlayer::AddLivesLeft(DWORD dwLivesLeft){m_dwLivesLeft+=dwLivesLeft;}

double CPlayer::GetSpeed(){return m_dSpeed;}
void   CPlayer::SetSpeed(double dSpeed){m_dSpeed=dSpeed;}

void  CPlayer::GetWeaponsOnSlot(DWORD dwWeaponSlot,vector<IWeapon*> *pWeapons)
{
  size_t x;
  for(x=0;x<m_dWeapons.size();x++)
  {
    IWeapon *piWeapon=m_dWeapons[x];
    if(piWeapon->GetSlot()==dwWeaponSlot)
    {
      pWeapons->push_back(piWeapon);
    }
  }
}

void  CPlayer::GetWeapons(vector<IWeapon*> *pWeapons)
{
  *pWeapons=m_dWeapons;
}

void  CPlayer::FireWeaponsOnSlot(DWORD dwWeaponSlot,DWORD dwCurrentTime)
{
  CEntityBase::FireWeapon(dwWeaponSlot,dwCurrentTime);
}

void CPlayer::OnKilled()
{
	if(m_dAnimations.size()>1)
	{
		if(GetCurrentAnimation()!=1)
		{
			m_PhysicInfo.vAngleVelocity.c[2]+=drand()*300.0-150.0;
			SetCurrentAnimation(1);
			m_PhysicInfo.dwMoveType=PHYSIC_MOVE_TYPE_NORMAL;
			if(m_dwLivesLeft){m_dwLivesLeft--;}
		}
	}
	else
	{
		if(m_dwLivesLeft){m_dwLivesLeft--;}
		m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
		CEntityBase::OnKilledInternal(true);
	}
}

bool CPlayer::OnCollision(IEntity *piOther,CVector &vCollisionPos)
{
	if(m_dHealth<=0 && *piOther->GetEntityClass()=="CWorldEntity")
	{
		if(GetCurrentAnimation()!=2 && m_dAnimations.size()>2)
		{
			SetCurrentAnimation(2);
		}
		CEntityBase::OnKilledInternal(true);
	}
	if(m_dHealth>0 && *piOther->GetEntityClass()!="CWorldEntity" && piOther->GetAlignment()!=m_dwAlignment)
	{
		piOther->OnDamage(m_dHealth,this);
	}
	return false;
}
