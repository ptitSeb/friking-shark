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
#include "VehicleType.h"
#include "GameGraphics.h"

CVehicleType::CVehicleType()
{
  PersistencyInitialize();
}

CVehicleType::~CVehicleType()
{
}

IEntity *CVehicleType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
    CVehicle *piEntity=new CVehicle(this);
    InitializeEntity(piEntity,dwCurrentTime);
	// Set damage type to none till first frame to let the unit know if it is inside a building.
	piEntity->SetDamageType(DAMAGE_TYPE_NONE);	
    piEntity->SetState(eVehicleState_Normal);
	piEntity->GetPhysicInfo()->dMass=1;
	piEntity->GetPhysicInfo()->dwCollisionType=PHYSIC_COLLISION_TYPE_SLIDE;
    return piEntity;
}

CVehicle::CVehicle(CVehicleType *pType)
{
	m_nNextCheckContainerBuilding=0;
	m_piContainerBuilding=NULL;
	m_piTarget=NULL;
    m_sClassName="CVehicle";
    m_pType=pType;
	m_nRoutePoint=0;
	m_piContainerBuilding=NULL;
	m_bRouteFinished=false;
	SEntityTypeConfig sconfig;
	m_pType->GetEntityTypeConfig(&sconfig);
	m_nConfiguredDamageType=sconfig.nDamageType;
}

void CVehicle::Render(IGenericRender *piRender,IGenericCamera *piCamera)
{
	//piRender->RenderBBox(m_PhysicInfo.vPosition,Origin,m_PhysicInfo.vMins,m_PhysicInfo.vMaxs,CVector(1,1,1),0x8888);
	
	CEntityBase::Render(piRender,piCamera);	
}

void CVehicle::OnKilled()
{
  bool bRemove=false;
  if(m_pTypeBase->GetStateAnimations(eVehicleState_Destroyed))
  {
    m_dwDamageType=DAMAGE_TYPE_NONE;

	if(!m_pType->m_bFollowRouteDestroyed || !m_piRoute)
	{   
		m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
		m_PhysicInfo.dwMoveType=PHYSIC_MOVE_TYPE_NONE;
		m_PhysicInfo.dwCollisionType=PHYSIC_COLLISION_TYPE_STUCK;
	}
	
	SetState(eVehicleState_Destroyed);
  }
  else
  {
    bRemove=true;
  }
  
  CEntityBase::OnKilledInternal(bRemove);

  for(unsigned int x=0;x<m_vChildren.size();x++){m_vChildren[x].piEntity->Remove();}
}

void CVehicle::AcquireTarget()
{
	IEntity 		*piTarget=NULL;
	IEntityManager 	*piManager=GetEntityManager();
	if(piManager){piTarget=piManager->FindEntity("Player");}
	SetTarget(piTarget);
}

bool CVehicle::IsInsideBuilding(IStaticStructure *piStaticStructure)
{
	const std::vector<SBBox> &vBBoxes=piStaticStructure->GetProtectiveRegions();
	if(vBBoxes.size()==0){return false;}
	
	
	CVector vFake1,vFake2;
	CVector vForward,vRight,vUp;
	ComputeReferenceSystem(piStaticStructure->GetPhysicInfo()->vPosition,piStaticStructure->GetPhysicInfo()->vAngles,Origin,Origin,&vFake1,&vFake2,&vForward,&vUp,&vRight);

	CVector vMyCenter=m_PhysicInfo.vPosition;
	vMyCenter-=piStaticStructure->GetPhysicInfo()->vPosition;
	CMatrix m;
	m.Ref(vForward,vUp,vRight);
	vMyCenter*=m;
	
	bool bInside=false;
	for(unsigned int b=0;!bInside && b<vBBoxes.size();b++)
	{
		bInside=true;
		for(int c=0;c<3;c++)
		{
			if(vMyCenter.c[c]<vBBoxes[b].vMins.c[c]){bInside=false;break;}
			if(vMyCenter.c[c]>vBBoxes[b].vMaxs.c[c]){bInside=false;break;}
		}
	}
	return bInside;
}

void CVehicle::FindBuilding(IEntity *piEntity,void *pParam1,void *pParam2)
{
	CVehicle *pThis=(CVehicle *)pParam1;
	if(pThis->m_piContainerBuilding){return;}
	if(piEntity->IsRemoved()){return;}
	if(*piEntity->GetEntityClass()!="CStaticStructure"){return;}
	
	IStaticStructure *piStructure=dynamic_cast<IStaticStructure*>(piEntity);
	if(piStructure==NULL){return;}
	
	bool bInside=pThis->IsInsideBuilding(piStructure);
	if(bInside){pThis->m_piContainerBuilding=piStructure;}
}

	
	
void CVehicle::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	CEntityBase::ProcessFrame(dwCurrentTime,dTimeFraction);
	
	if(GetState()==eVehicleState_Destroyed)
	{
		if(m_vActiveAnimations.size()==0){Remove();}
		if(!m_pType->m_bFollowRouteDestroyed){return;}
	}	
	
	if(m_nNextCheckContainerBuilding<dwCurrentTime)
	{
		if(m_piContainerBuilding)
		{
			if(!IsInsideBuilding(m_piContainerBuilding))
			{
				UNSUBSCRIBE_FROM_CAST(m_piContainerBuilding,IEntityEvents);
				m_piContainerBuilding=NULL;			
			}
		}
		else
		{
			m_piContainerBuilding=NULL;
			GetEntityManager()->PerformUnaryOperation(FindBuilding,this,0);
			if(m_piContainerBuilding){SUBSCRIBE_TO_CAST(m_piContainerBuilding,IEntityEvents);}
		}
		m_nNextCheckContainerBuilding=dwCurrentTime+100;
	}

	if(m_piContainerBuilding)
	{
		m_dwDamageType=DAMAGE_TYPE_NONE;
	}
	else
	{
		bool bAllChildDead=true;
		for(unsigned int x=0;x<m_vChildren.size();x++){if(m_vChildren[x].piEntity->GetHealth()>0){bAllChildDead=false;}}
		m_dwDamageType=(bAllChildDead?m_nConfiguredDamageType:DAMAGE_TYPE_NONE);
		if(m_piTarget==NULL){AcquireTarget();}
	}
	
	if(m_bRouteFinished)
	{
		double dVel=m_PhysicInfo.vVelocity.N();
		if(dVel!=0)
		{
			double dNewVel=dVel-m_PhysicInfo.dMaxVelocity*2.0*dTimeFraction;
			if(dNewVel<0){dNewVel=0;}
			m_PhysicInfo.vVelocity*=dNewVel;
		}
	}
	else if(m_piRoute && dwCurrentTime>=(m_dwCreationTime+m_nRouteDelay))
	{
		CVector vDest,vDir;
		// Just follow the configured route
		double dCurrentAngularSpeed=m_pType->m_dMaxAngularSpeed;
		vDest=m_piRoute->GetAbsolutePoint(m_nRoutePoint);
		vDir=vDest-m_PhysicInfo.vPosition;
		double dDist=vDir.N();  
		
		bool bNext=false;
		int nNext=m_piRoute->GetNextPointIndex(m_nRoutePoint);
		if(nNext!=m_nRoutePoint)
		{
			CVector vDirNext=m_piRoute->GetAbsolutePoint(m_piRoute->GetNextPointIndex(nNext))-vDest;
			vDirNext.N();
			
			double dCirclePerimeter=(m_PhysicInfo.dMaxVelocity*360.0/m_pType->m_dMaxAngularSpeed);
			double dCapableRadius=(dCirclePerimeter/(2*PI));
			
			CVector vPerpB=vDirNext^m_PhysicInfo.vOwnY;
			CVector vPB1=vDest+vDirNext*dDist;
			CPlane  vPlaneA=CPlane(vDir,m_PhysicInfo.vPosition);
			
			double dSide1=vPlaneA.GetSide(vPB1);
			double dSide2=vPlaneA.GetSide(vPB1+vPerpB*10000.0);
			double dLength=(dSide1-dSide2);
			double dFraction=dLength?dSide1/dLength:0;
			double dFinalRadius=fabs(10000.0*dFraction);
			
			bNext=(dFinalRadius<dCapableRadius);
		}
		bNext=(bNext || dDist<m_PhysicInfo.dMaxVelocity*0.1);
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
		m_PhysicInfo.vAngles.c[YAW]=ApproachAngle(m_PhysicInfo.vAngles.c[YAW],dDesiredYaw,-dCurrentAngularSpeed*dTimeFraction);
		VectorsFromAngles(m_PhysicInfo.vAngles,&m_PhysicInfo.vVelocity);
		m_PhysicInfo.vVelocity*=m_PhysicInfo.dMaxVelocity;
		m_dwNextProcessFrame=dwCurrentTime+10;
	}
}

void CVehicle::SetRoute( IRoute *piRoute )
{
	CEntityBase::SetRoute(piRoute);
}

bool CVehicle::HasFinishedRoute()
{
	return m_piRoute==NULL || m_bRouteFinished || m_dHealth==0;
}

void CVehicle::OnRemoved(IEntity *piEntity)
{
	CEntityBase::OnRemoved(piEntity);
	
	if(piEntity==m_piContainerBuilding)
	{
		UNSUBSCRIBE_FROM_CAST(m_piContainerBuilding,IEntityEvents);
		m_piContainerBuilding=NULL;
	}
}

void CVehicle::OnKilled(IEntity *piEntity)
{
	CEntityBase::OnKilled(piEntity);
	
	if(piEntity==m_piContainerBuilding)
	{
		UNSUBSCRIBE_FROM_CAST(m_piContainerBuilding,IEntityEvents);
		m_piContainerBuilding=NULL;
	}
}