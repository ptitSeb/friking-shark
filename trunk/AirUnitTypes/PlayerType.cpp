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
#include "PlayerType.h"
//#include "GameGraphics.h"

CPlayerType::CPlayerType()
{
	m_nDamageType=DAMAGE_TYPE_NORMAL;
	m_nMovementType=PHYSIC_MOVE_TYPE_CUSTOM;
	m_dMaxAngularSpeed=false;
}

CPlayerType::~CPlayerType()
{
	
}

IEntity *CPlayerType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
    CPlayer *piEntity=new CPlayer(this);
    InitializeEntity(piEntity,dwCurrentTime);
    piEntity->SetState(ePlayerState_Normal);
    piEntity->SetSpeed(m_dMaxVelocity);
    piEntity->SetHealth(1);
    return piEntity;
}

CPlayer::CPlayer(CPlayerType *pType)
{
  m_dwPoints=0;
  m_dwLivesLeft=1;
  m_dSpeed=80;
  m_sClassName="CPlayer";
  m_sName="Player";
  m_pType=pType;
  m_bRouteFinished=false;
  m_nRoutePoint=0;
  m_bGodMode=false;
  m_nCurrentTime=0;
}

unsigned int CPlayer::GetPoints(){return m_dwPoints;}
void  CPlayer::SetPoints(unsigned int dwPoints){m_dwPoints=dwPoints;}
void  CPlayer::AddPoints(unsigned int dwPoints){m_dwPoints+=dwPoints;}

unsigned int CPlayer::GetLivesLeft(){return m_dwLivesLeft;}
void  CPlayer::SetLivesLeft(unsigned int dwLivesLeft){m_dwLivesLeft=dwLivesLeft;}
void  CPlayer::AddLivesLeft(unsigned int dwLivesLeft){m_dwLivesLeft+=dwLivesLeft;}

double CPlayer::GetSpeed(){return m_dSpeed;}
void   CPlayer::SetSpeed(double dSpeed){m_dSpeed=dSpeed;}
void   CPlayer::SetGodMode(bool bGod){m_bGodMode=bGod;}

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
	bool bRemove=false;
	if(m_pTypeBase->GetStateAnimations(ePlayerState_Falling))
	{
		if(GetState()!=ePlayerState_Falling)
		{
			m_PhysicInfo.vAngleVelocity.c[2]+=drand()*300.0-150.0;
			SetState(ePlayerState_Falling);
			m_PhysicInfo.dwMoveType=PHYSIC_MOVE_TYPE_NORMAL;
			m_dwDamageType=DAMAGE_TYPE_NONE;
			if(m_dwLivesLeft){m_dwLivesLeft--;}
		}
		bRemove=false;
	}
	else
	{
		if(m_dwLivesLeft){m_dwLivesLeft--;}
		m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
		bRemove=true;
	}
	CEntityBase::OnKilledInternal(bRemove);
}

bool CPlayer::OnCollision(IEntity *piOther,CVector &vCollisionPos)
{
	if(GetState()==ePlayerState_Falling && (piOther->GetPlacement()!=ENTITY_PLACEMENT_AIR || piOther->GetPhysicInfo()->dwBoundsType==PHYSIC_BOUNDS_TYPE_BSP))
	{
		if(GetState()!=ePlayerState_Crashed && m_pTypeBase->GetStateAnimations(ePlayerState_Crashed))
		{
			SetState(ePlayerState_Crashed);
		}
		Remove();
	}
	if(m_dHealth>0 && piOther->GetAlignment()==ENTITY_ALIGNMENT_ENEMIES)
	{
		double dMyDamage=piOther->GetHealth();
		piOther->OnDamage(m_dHealth,this);
		OnDamage(dMyDamage,piOther);
	}
	return false;
}

void CPlayer::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	CEntityBase::ProcessFrame(dwCurrentTime,dTimeFraction);
	
	m_nCurrentTime=dwCurrentTime;
	
	if(m_piRoute)
	{
		CVector vDest,vDir;
		vDest=m_piRoute->GetAbsolutePoint(m_nRoutePoint);
		vDir=vDest-m_PhysicInfo.vPosition;
		double dDist=vDir.N();  
		
		if(m_nRoutePoint==0 && dDist>m_PhysicInfo.dMaxVelocity*0.1)
		{
			m_PhysicInfo.vVelocity=vDir*m_PhysicInfo.dMaxVelocity;
		}
		else
		{
			// Just follow the configured route
			double dCurrentAngularSpeed=m_pType->m_dMaxAngularSpeed;
			
			bool bNext=false;
			int nNext=m_piRoute->GetNextPointIndex(m_nRoutePoint);
			if(nNext!=m_nRoutePoint)
			{
				CVector vDirNext=m_piRoute->GetAbsolutePoint(nNext)-vDest;
				vDirNext.N();
				
				double dCirclePerimeter=(m_PhysicInfo.dMaxVelocity*360.0/m_pType->m_dMaxAngularSpeed);
				double dCapableRadius=(dCirclePerimeter/(2*PI));
				
				CVector vPerpB=vDirNext^m_PhysicInfo.vOwnZ;
				CVector vPB1=vDest+vDirNext*dDist;
				CPlane  vPlaneA=CPlane(vDir,m_PhysicInfo.vPosition);
				
				double dSide1=vPlaneA.GetSide(vPB1);
				double dSide2=vPlaneA.GetSide(vPB1+vPerpB*10000.0);
				double dLength=(dSide1-dSide2);
				double dFraction=dLength?dSide1/dLength:0;
				double dFinalRadius=fabs(10000.0*dFraction);
				
				bNext=(dFinalRadius<dCapableRadius);
			}
			bNext=bNext || dDist<m_PhysicInfo.dMaxVelocity*0.1;
			if(bNext)
			{
				if(nNext==m_nRoutePoint)
				{
					m_bRouteFinished=true;
				}
				else
				{
					m_nRoutePoint=nNext;
				}
			}
			// Head to the desired direction
			double dDesiredYaw=0,dDesiredPitch=0;
			AnglesFromVector(vDir,dDesiredYaw,dDesiredPitch);
			m_PhysicInfo.vAngles.c[PITCH]=ApproachAngle(m_PhysicInfo.vAngles.c[PITCH],dDesiredPitch,-dCurrentAngularSpeed*dTimeFraction);
			VectorsFromAngles(m_PhysicInfo.vAngles,&m_PhysicInfo.vVelocity);
			m_PhysicInfo.vVelocity*=m_PhysicInfo.dMaxVelocity;
			m_dwNextProcessFrame=dwCurrentTime+10;
			
			if(m_bRouteFinished)
			{
				m_PhysicInfo.vAngles.c[YAW]=0;
				m_PhysicInfo.vAngles.c[PITCH]=0;
				m_PhysicInfo.vVelocity=Origin;
				SetRoute(NULL);
			}
		}
	}
}
void CPlayer::Render(IGenericRender *piRender,IGenericCamera *piCamera)
{
	CEntityBase::Render(piRender,piCamera);	
}

void CPlayer::SetRoute( IRoute *piRoute )
{
	SEntityTypeConfig sConfig;
	m_pType->GetEntityTypeConfig(&sConfig);
	m_dwDamageType=piRoute?DAMAGE_TYPE_NONE:sConfig.nDamageType;
	m_PhysicInfo.dwMoveType=piRoute?PHYSIC_MOVE_TYPE_FLY:sConfig.nMovementType;
	m_PhysicInfo.dwBoundsType=piRoute?PHYSIC_BOUNDS_TYPE_NONE:sConfig.nBoundsType;
	
	m_nRoutePoint=0;
	m_bRouteFinished=false;
	CEntityBase::SetRoute(piRoute);
}

bool CPlayer::HasFinishedRoute()
{
	return m_piRoute==NULL || m_bRouteFinished || m_dHealth==0;
}

void CPlayer::OnDamage(double dDamage,IEntity *pAggresor)
{
	if(m_bGodMode){return;}
	CEntityBase::OnDamage(dDamage,pAggresor);
}
