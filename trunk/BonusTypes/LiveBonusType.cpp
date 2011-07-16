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
#include "LiveBonusType.h"
#include "GameGraphics.h"

CLiveBonusType::CLiveBonusType()
{
	m_PlayAreaManager.Attach("GameSystem","PlayAreaManager");
	
	m_dAngularVelocity=60;
	m_dForwardVelocity=20;
	m_dExitVelocity=20;
	m_nMovementType=PHYSIC_MOVE_TYPE_FLY;
	m_nCollisionType=PHYSIC_COLLISION_TYPE_THROUGH;
}

CLiveBonusType::~CLiveBonusType()
{
}

IEntity *CLiveBonusType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
	CLiveBonus *piEntity=new CLiveBonus(this);
	SPhysicInfo *pPhysicInfo=piEntity->GetPhysicInfo();
	InitializeEntity(piEntity,dwCurrentTime);
	
	// Se posiciona el bonus en el plano aereo, en el juego no hay bonus de armas entregados por unidades
	// de tierra, pero just in case
	CVector vPosition=pPhysicInfo->vPosition=piParent?piParent->GetPhysicInfo()->vPosition:Origin;
	CVector vStart,vEnd;
	CVector vCameraPos;
	if(m_PlayAreaManager.m_piPlayAreaManager)
	{
		m_PlayAreaManager.m_piPlayAreaManager->GetCameraRoute(&vStart,&vEnd);
		IGenericCamera *piCamera=m_PlayAreaManager.m_piPlayAreaManager->GetCamera();
		if(piCamera){vCameraPos=piCamera->GetPosition();}
		REL(piCamera);	
	}
	CPlane playAreaPlane=CPlane(AxisPosY,vStart);
	CVector vCut;
	if(playAreaPlane.Cut(vPosition,vCameraPos,&vCut))
	{
		pPhysicInfo->vPosition=vCut;
	}
	
	piEntity->SetState(eLiveBonusState_Normal,ANIMATION_RANDOM);
	piEntity->SetInitialVelocity();
	return piEntity;
}

CLiveBonus::CLiveBonus(CLiveBonusType *pType)
{
	m_sClassName="CLiveBonus";
	m_sName="LiveBonus";
	m_pType=pType;
	m_dwDamageType=DAMAGE_TYPE_NONE;
	m_dCurrentAngularVelocity=0;
	m_dRadius=pType->DesignGetRadius();
}

bool CLiveBonus::OnCollision(IEntity *pOther,CVector &vCollisionPos)
{
	IPlayer *piPlayer=dynamic_cast<IPlayer*>(pOther);
	if(piPlayer)
	{
		SEntityTypeConfig sConfig;
		m_pType->GetEntityTypeConfig(&sConfig);
		if(sConfig.nPoints){piPlayer->AddPoints(sConfig.nPoints);}
		if(sConfig.nPoints){piPlayer->AddLivesLeft(1);}
		
		SetState(eLiveBonusState_Taken);
		Remove();
	}
	return false;
}

void CLiveBonus::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	CEntityBase::ProcessFrame(dwCurrentTime,dTimeFraction);
	if(m_pType->m_PlayAreaManager.m_piPlayAreaManager==NULL){return;}
	
	CVector vMins,vMaxs;
	m_pType->m_PlayAreaManager.m_piPlayAreaManager->GetCurrentVisibleArea(&vMins,&vMaxs);
	
	if(m_PhysicInfo.vPosition.c[0]+m_dRadius<vMins.c[0])
	{
		Remove();
		return;
	}
	
	if(m_PhysicInfo.vPosition.c[2]-m_dRadius<vMins.c[2] && m_vCurrentForwardDirection.c[2]<0)
	{
		CMatrix m;
		double dInitialAngle=drand()*(-25.0)-20.0;
		m.R(AxisPosY,DegreesToRadians(dInitialAngle));		
		m_dCurrentAngularVelocity=0-m_pType->m_dAngularVelocity;
		m_vCurrentForwardDirection=AxisPosX;
		m_vCurrentForwardDirection*=m;
	}
	if(m_PhysicInfo.vPosition.c[2]+m_dRadius>vMaxs.c[2] && m_vCurrentForwardDirection.c[2]>0)
	{
		CMatrix m;
		double dInitialAngle=drand()*25.0+20.0;
		m.R(AxisPosY,DegreesToRadians(dInitialAngle));		
		m_dCurrentAngularVelocity=m_pType->m_dAngularVelocity;
		m_vCurrentForwardDirection=AxisPosX;
		m_vCurrentForwardDirection*=m;
	}
	
	CMatrix m;
	m.R(AxisPosY,DegreesToRadians(m_dCurrentAngularVelocity*dTimeFraction));
	m_vCurrentForwardDirection*=m;
	m_vCurrentForwardDirection.N();
	
	double dCameraFollow=(m_pType->m_PlayAreaManager.m_piPlayAreaManager->GetCameraSpeed()-m_pType->m_dExitVelocity);
	m_PhysicInfo.vVelocity=m_vCurrentForwardDirection*m_pType->m_dForwardVelocity+AxisPosX*dCameraFollow;
}

void CLiveBonus::SetInitialVelocity()
{
	if(m_pType->m_PlayAreaManager.m_piPlayAreaManager==NULL){return;}
	
	CMatrix m;
	double dInitialAngle=45.0-drand()*90.0;
	m_dCurrentAngularVelocity=dInitialAngle>0?m_pType->m_dAngularVelocity:0-m_pType->m_dAngularVelocity;
	m.R(AxisPosY,DegreesToRadians(dInitialAngle));
	m_vCurrentForwardDirection=AxisPosX;
	m_vCurrentForwardDirection*=m;	
}

