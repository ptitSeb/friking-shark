#include "./stdafx.h"
#include "PlayerType.h"

CPlayerType::CPlayerType()
{
	m_nDamageType=DAMAGE_TYPE_NORMAL;
	m_nMovementType=PHYSIC_MOVE_TYPE_CUSTOM;
}

CPlayerType::~CPlayerType()
{
	
}

IEntity *CPlayerType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
    CPlayer *piEntity=new CPlayer(this);
    InitializeEntity(piEntity,dwCurrentTime);
	piEntity->SetState(ePlayerState_Normal);
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
}

unsigned int CPlayer::GetPoints(){return m_dwPoints;}
void  CPlayer::SetPoints(unsigned int dwPoints){m_dwPoints=dwPoints;}
void  CPlayer::AddPoints(unsigned int dwPoints){m_dwPoints+=dwPoints;}

unsigned int CPlayer::GetLivesLeft(){return m_dwLivesLeft;}
void  CPlayer::SetLivesLeft(unsigned int dwLivesLeft){m_dwLivesLeft=dwLivesLeft;}
void  CPlayer::AddLivesLeft(unsigned int dwLivesLeft){m_dwLivesLeft+=dwLivesLeft;}

double CPlayer::GetSpeed(){return m_dSpeed;}
void   CPlayer::SetSpeed(double dSpeed){m_dSpeed=dSpeed;}

void  CPlayer::GetWeaponsOnSlot(unsigned int dwWeaponSlot,vector<IWeapon*> *pWeapons)
{
  size_t x;
  for(x=0;x<m_vWeapons.size();x++)
  {
    IWeapon *piWeapon=m_vWeapons[x];
    if(piWeapon->GetSlot()==dwWeaponSlot)
    {
      pWeapons->push_back(piWeapon);
    }
  }
}

void  CPlayer::GetWeapons(vector<IWeapon*> *pWeapons)
{
  *pWeapons=m_vWeapons;
}

void  CPlayer::FireWeaponsOnSlot(unsigned int dwWeaponSlot,unsigned int dwCurrentTime)
{
  CEntityBase::FireWeapon(dwWeaponSlot,dwCurrentTime);
}

void CPlayer::OnKilled()
{
	if(m_pTypeBase->GetStateAnimations(ePlayerState_Falling))
	{
		if(GetState()!=ePlayerState_Falling)
		{
			m_PhysicInfo.vAngleVelocity.c[2]+=drand()*300.0-150.0;
			SetState(ePlayerState_Crashed);
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
		if(GetState()!=ePlayerState_Crashed && m_pTypeBase->GetStateAnimations(ePlayerState_Crashed))
		{
			SetState(ePlayerState_Crashed);
		}
		CEntityBase::OnKilledInternal(true);
	}
	if(m_dHealth>0 && *piOther->GetEntityClass()!="CWorldEntity" && piOther->GetAlignment()!=m_dwAlignment)
	{
		piOther->OnDamage(m_dHealth,this);
	}
	return false;
}
