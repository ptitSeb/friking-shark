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
#include "GameRunTimeLib.h"
#include "PhysicsManager.h"

CPhysicManager::CPhysicManager()
{
	SPhysicForce		 gravity;
	gravity.dwForceType=PHYSIC_FORCE_CONSTANT_ACCELERATION;
	gravity.dConstantAccel=29.8;
	gravity.dMaxVelocity=200;
	gravity.vDir=CVector(0,-1,0);
	m_vGlobalForces.push_back(gravity);	
}

CPhysicManager::~CPhysicManager()
{

}

bool CPhysicManager::Init(std::string sClass,std::string sName,ISystem *piSystem)
{
    bool bOk=CSystemObjectBase::Init(sClass,sName,piSystem);
    if(bOk){bOk=m_EntityManagerWrapper.Attach("GameSystem","EntityManager");}
    if(bOk){bOk=m_GameControllerWrapper.Attach("GameSystem","GameController");}
    if(bOk){m_GameControllerWrapper.m_piGameController->RegisterManager(300,this);}
    return bOk;
}

void CPhysicManager::Destroy()
{
    if(m_GameControllerWrapper.m_piGameController)
    {
        m_GameControllerWrapper.m_piGameController->UnregisterManager(this);
    }
    m_EntityManagerWrapper.Detach();
    CSystemObjectBase::Destroy();
}

void CPhysicManager::CreateScenario(){}
void CPhysicManager::LoadScenario(ISystemPersistencyNode *piNode){}
void CPhysicManager::SaveScenario(ISystemPersistencyNode *piNode){}
void CPhysicManager::CloseScenario(){}

void CPhysicManager::Start(){}
void CPhysicManager::Stop(){}

CVector CPhysicManager::ProcessPhysicInfo(SPhysicInfo *pInfo,double dInterval)
{    
	pInfo->bOnSurface=false;

   // Se aplica la velocidad angular
    pInfo->vAngles+=pInfo->vAngleVelocity*dInterval;
    for(int c=0;c<3;c++)
    {
        if(pInfo->vAngles.c[c]>360){pInfo->vAngles.c[c]-=360;}
        if(pInfo->vAngles.c[c]<-360){pInfo->vAngles.c[c]+=360;}
    }

	if(pInfo->dwMoveType==PHYSIC_MOVE_TYPE_NONE){return pInfo->vPosition;}
	if(pInfo->dwMoveType==PHYSIC_MOVE_TYPE_CUSTOM){return pInfo->vPosition;}

    // Se aplican las fuerzas globales
    if(pInfo->dwMoveType==PHYSIC_MOVE_TYPE_NORMAL)
    {
        for(unsigned f=0;f<m_vGlobalForces.size();f++)
        {
            ApplyForce(pInfo,&m_vGlobalForces[f],dInterval);
        }
    }
    // Se aplican las fuerzas sobre la entidad
    for(unsigned f=0;f<pInfo->vForces.size();f++)
    {
        ApplyForce(pInfo,&pInfo->vForces[f],dInterval);
    }
    // Se aplica la fuerza propia
    ApplyForce(pInfo,&pInfo->fOwnForce,dInterval);


    // Se calcula la nueva posicion
    return pInfo->vPosition+(pInfo->vVelocity)*dInterval;
}
struct SCheckCollisionInfo
{
    // Se construye una caja que contenga todo el movimiento para utilizarla en la colision con las entidades 
    // con bounding box.
 
	IEntity *piAncestor;
    CVector vTotalMins;
    CVector vTotalMaxs;
    CVector vTotalMinsOrigin;
    CVector vTotalMaxsOrigin;
    CVector vTotalMinsDestination;
    CVector vTotalMaxsDestination;

    CVector     vOrigin;
    CVector     vDestination;
    CTraceInfo  traceInfo;
	
	SCheckCollisionInfo(){piAncestor=NULL;}
};

void EntityOperation_CheckCollision(IEntity *piOther,void *pParam1,void *pParam2)
{
    IEntity             *piEntity=(IEntity *)pParam1;
    SPhysicInfo         *pPhysicInfo=piEntity->GetPhysicInfo();
    SPhysicInfo         *pOtherPhysicInfo=piOther->GetPhysicInfo();
    SCheckCollisionInfo *pInfo=(SCheckCollisionInfo *)pParam2;

    if(piOther==piEntity){return;}
    if(pOtherPhysicInfo->dwBoundsType==PHYSIC_BOUNDS_TYPE_NONE){return;}
    

	IEntity *piParent=piOther,*piAncestor=piOther;
	while(piParent){piParent=piParent->GetParent();if(piParent){piAncestor=piParent;}}
	if(piAncestor==pInfo->piAncestor){return;}

	if(pOtherPhysicInfo->dwBoundsType==PHYSIC_BOUNDS_TYPE_BBOX)
    {
        bool bInMovementBox=true;
        for(int x=0;x<3;x++)
        {
            if((pOtherPhysicInfo->vPosition.c[x]+pOtherPhysicInfo->vMaxs.c[x])<pInfo->vTotalMins.c[x]){bInMovementBox=false;break;}
            if((pOtherPhysicInfo->vPosition.c[x]+pOtherPhysicInfo->vMins.c[x])>pInfo->vTotalMaxs.c[x]){bInMovementBox=false;break;}
        }
        if(bInMovementBox)
        {

            // Si hay colision en el area de movimiento se ajusta la colision haciendo avanzar el bbox del objeto
            // en la direccion del movimiento, de momento se usa un tamaño de paso fijo, despues se podria ajustar
            // dependiendo del tamaño del bbox del objeto en la direccion del movimiento.

            CVector vDir=pInfo->vDestination-pInfo->vOrigin;
            CVector vTempPos=pInfo->vOrigin;
            double  dLen=vDir.N();
            double  dStepLength=5.0;
            CVector vStep=vDir*dStepLength;

            bool bInStepBox=false;
            double dSteps=dLen/dStepLength;
            if(dSteps<1.0)
            {
              bInStepBox=true;
            }

            for(int y=0;y<dSteps;y++)
            {
                bInStepBox=true;
                for(int x=0;x<3;x++)
                {
                    if((pOtherPhysicInfo->vPosition.c[x]+pOtherPhysicInfo->vMaxs.c[x])<pPhysicInfo->vMins.c[x]+vTempPos.c[x]){bInStepBox=false;break;}
                    if((pOtherPhysicInfo->vPosition.c[x]+pOtherPhysicInfo->vMins.c[x])>pPhysicInfo->vMaxs.c[x]+vTempPos.c[x]){bInStepBox=false;break;}
                }
                if(bInStepBox){break;}
                vTempPos+=vStep;
            }

            if(bInStepBox)
            {
                if(piEntity->OnCollision(piOther,pInfo->traceInfo.m_vTracePos))
                {
                    if( pPhysicInfo->dwCollisionType==PHYSIC_COLLISION_TYPE_THROUGH || 
						pOtherPhysicInfo->dwCollisionType==PHYSIC_COLLISION_TYPE_THROUGH
					)
                    {
                      pInfo->traceInfo.m_vTracePos=pInfo->vDestination;
                    }
                    else
                    {
                      pInfo->traceInfo.m_vTracePos=vTempPos;
                    }
                }
                if(!piOther->IsRemoved()){piOther->OnCollision(piEntity,pInfo->traceInfo.m_vTracePos);}
                pInfo->traceInfo.m_bTraceHit=true;
            }
        }
    }
    else if(pOtherPhysicInfo->dwBoundsType==PHYSIC_BOUNDS_TYPE_BSP)
    {
        pInfo->traceInfo=piOther->GetTrace(pInfo->vOrigin,pInfo->vDestination);
        if(pInfo->traceInfo.m_bTraceHit)
        {
			//RTTRACE("%s Hit! fraction %f",piEntity->GetEntityClass()->c_str(),pInfo->traceInfo.m_dTraceFraction);
            CVector vDir=pInfo->vDestination-pInfo->vOrigin;
            double vlen=vDir.N();
            double dStart=pInfo->traceInfo.m_vTracePlane.GetSide(pInfo->vOrigin);
            bool bWasOnSurface=dStart<FP_PRECISION*11.0;
            //RTTRACE("%s dStart %f, bWasOnSurface %d",piEntity->GetEntityClass()->c_str(),dStart,bWasOnSurface);
            if(pPhysicInfo->dwCollisionType==PHYSIC_COLLISION_TYPE_STUCK)
            {
				if(!bWasOnSurface)
				{
					pInfo->vDestination=pInfo->traceInfo.m_vTracePos=pInfo->traceInfo.m_vTracePos-vDir*(FP_PRECISION*2.0);
				}
				else
				{
					pInfo->vDestination=pInfo->traceInfo.m_vTracePos=pInfo->vOrigin;
				}

				pPhysicInfo->bOnSurface=true;
				pPhysicInfo->surfacePlane=pInfo->traceInfo.m_vTracePlane;
				pPhysicInfo->vVelocity=Origin;
				
            }
            else if(pPhysicInfo->dwCollisionType==PHYSIC_COLLISION_TYPE_SLIDE)
            {
				int nIterations=0;
                CVector vImpact=pInfo->vOrigin;
                CTraceInfo tempTrace;
                tempTrace=pInfo->traceInfo;
                while(tempTrace.m_bTraceHit)
                {
//					RTTRACE("Iteration %d, HIT: normal %.05f,%.05f,%.05f, d:%.05f     Orig:%.05f,%.05f,%.05f    Dest:%.05f,%.05f,%.05f    Hit:%.05f,%.05f,%.05f",nIterations,tempTrace.m_vTracePlane.c[0],tempTrace.m_vTracePlane.c[1],tempTrace.m_vTracePlane.c[2],tempTrace.m_vTracePlane.d,vImpact.c[0],vImpact.c[1],vImpact.c[2],pInfo->vDestination.c[0],pInfo->vDestination.c[1],pInfo->vDestination.c[2],tempTrace.m_vTracePos.c[0],tempTrace.m_vTracePos.c[1],tempTrace.m_vTracePos.c[2]);
		            double dEnd=-tempTrace.m_vTracePlane.GetSide(pInfo->vDestination);
                    vImpact=tempTrace.m_vTracePos-vDir*(dEnd+FP_PRECISION*2.0);

                    vDir=pInfo->vDestination-vImpact;
                    vlen=vDir.N();

                    pInfo->vDestination=pInfo->vDestination+tempTrace.m_vTracePlane*(dEnd+FP_PRECISION*2.0);
                    tempTrace.m_vTracePos=pInfo->vDestination;

					CVector vPlaneNormal=tempTrace.m_vTracePlane;
					double dDistance=(vPlaneNormal*pPhysicInfo->vVelocity);
					pPhysicInfo->vVelocity-=vPlaneNormal*dDistance;
					pPhysicInfo->bOnSurface=true;
					pPhysicInfo->surfacePlane=tempTrace.m_vTracePlane;

                    tempTrace=piOther->GetTrace(vImpact,pInfo->vDestination);
                    pInfo->traceInfo.m_vTracePos=tempTrace.m_vTracePos;
					nIterations++;

                }

				/*
                double dFinalVel=pPhysicInfo->vVelocity;
                if(bWasOnSurface)
                {
                    //dFinalVel*=pPhysicInfo->dSlideFactor;
					double dSize=tempTrace.m_vTracePlane.GetSide(pInfo->vDestination);
					if(dSize<0)
					{
						pPhysicInfo->vVelocity+=tempTrace.m_vTracePlane*dSize;
					}
                }*/
            }
            else if(pPhysicInfo->dwCollisionType==PHYSIC_COLLISION_TYPE_BOUNCE)
            {
                double vVel=pPhysicInfo->vVelocity;
                pInfo->traceInfo.m_vTracePos-=vDir*(FP_PRECISION*10.0);

                CVector vNewDir=(vDir+pInfo->traceInfo.m_vTracePlane*2.0);
                vNewDir.N();
                pPhysicInfo->vVelocity=vNewDir*(vVel*pPhysicInfo->dBounceFactor);
            }

            if(pInfo->traceInfo.m_bTraceHit)
            {
                if(!piEntity->OnCollision(piOther,pInfo->traceInfo.m_vTracePos) && !bWasOnSurface)
                {
                    pInfo->traceInfo.m_vTracePos=pInfo->vDestination;
                }
                if(!piOther->IsRemoved()){piOther->OnCollision(piEntity,pInfo->traceInfo.m_vTracePos);}
            }
        }
    }
}

void ProcessChildren(IEntity *piParent)
{
	SPhysicInfo *pParentInfo=piParent->GetPhysicInfo();
	unsigned int nChildren=piParent->GetChildren();
	for(unsigned int x=0;x<nChildren;x++)
	{
		IEntity *piChild=piParent->GetChild(x);
		SPhysicInfo *pChildInfo=piChild->GetPhysicInfo();
		
		CVector vPositionInParent,vAnglesInParent;
		piParent->GetChildLocation(piChild,vPositionInParent,vAnglesInParent);
		
		CVector vPos,vAngles;
		ComputeReferenceSystem(pParentInfo->vPosition,pParentInfo->vAngles,vPositionInParent,vAnglesInParent,&vPos,&vAngles);
		
		
		CVector vFinalPos,vFinalAngles;
		ComputeReferenceSystem(vPos,vAngles,pChildInfo->vLocalPosition,pChildInfo->vLocalAngles,&vFinalPos,&vFinalAngles);
		VectorsFromAngles(vAngles,&pChildInfo->vRefSysX,&pChildInfo->vRefSysZ,&pChildInfo->vRefSysY);
		
		pChildInfo->vPosition=vFinalPos;
		pChildInfo->vAngles=vFinalAngles;
		ProcessChildren(piChild);
	} 
}

void EntityOperation_ProcessPhysicFrame(IEntity *piEntity,void *pParam1,void *pParam2)
{
	IEntity *piParent=piEntity->GetParent();
	if(piParent){return;}
	if(piEntity->IsRemoved()){return;}

    double dTimeFraction=*((double *)pParam2);
    CPhysicManager *pThis=(CPhysicManager *)pParam1;
    SPhysicInfo *pPhysicInfo=piEntity->GetPhysicInfo();
	
	CVector vNewPos=pThis->ProcessPhysicInfo(pPhysicInfo,dTimeFraction);
	
	if( (vNewPos!=pPhysicInfo->vPosition || pPhysicInfo->dwMoveType==PHYSIC_MOVE_TYPE_CUSTOM) && 
        pPhysicInfo->dwBoundsType!=PHYSIC_BOUNDS_TYPE_NONE && 
        pPhysicInfo->dwBoundsType!=PHYSIC_BOUNDS_TYPE_BSP)
    {
        SCheckCollisionInfo info;
        info.vOrigin=pPhysicInfo->vPosition;
        info.vDestination=vNewPos;
        info.vTotalMinsOrigin=pPhysicInfo->vMins+info.vOrigin;
        info.vTotalMaxsOrigin=pPhysicInfo->vMaxs+info.vOrigin;
        info.vTotalMinsDestination=pPhysicInfo->vMins+info.vDestination;
        info.vTotalMaxsDestination=pPhysicInfo->vMaxs+info.vDestination;
        info.traceInfo.m_vTracePos=info.vDestination;

        for(int x=0;x<3;x++)
        {
            info.vTotalMins.c[x]=min(info.vTotalMinsOrigin.c[x],info.vTotalMinsDestination.c[x]);
            info.vTotalMaxs.c[x]=max(info.vTotalMaxsOrigin.c[x],info.vTotalMaxsDestination.c[x]);
        }
        info.piAncestor=piEntity;
		IEntity *piParent=piEntity;
		while(piParent){piParent=piParent->GetParent();if(piParent){info.piAncestor=piParent;}}

		pThis->m_EntityManagerWrapper.m_piEntityManager->PerformUnaryOperation(EntityOperation_CheckCollision,piEntity,&info);
		pPhysicInfo->vPosition=info.traceInfo.m_vTracePos;
    }
    else
    {
		pPhysicInfo->vPosition=vNewPos;
    }
    ProcessChildren(piEntity);
//	RTTRACE("%s ------ Finished : %f",piEntity->GetEntityClass()->c_str(),pPhysicInfo->vPosition.c[1]);
}

void CPhysicManager::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	m_EntityManagerWrapper.m_piEntityManager->PerformUnaryOperation(EntityOperation_ProcessPhysicFrame,this,&dTimeFraction);
}

void CPhysicManager::ApplyForce(SPhysicInfo *pInfo,SPhysicForce *pForce,double dInterval)
{
	CVector vForceVelocity;
	if(pForce->dwForceType==PHYSIC_FORCE_CONSTANT_ACCELERATION)
	{
		vForceVelocity=pForce->vDir*(pForce->dConstantAccel*(dInterval));
	}
	else if(pForce->dwForceType==PHYSIC_FORCE_CONSTANT_VELOCITY)
	{
		pInfo->vPosition+=pForce->vDir*pForce->dConstantVel*dInterval;
	}
	else
	{
		double dEffectiveAccel=pForce->dForce/pInfo->dMass;
		vForceVelocity+=pForce->vDir*(dEffectiveAccel*(dInterval));
	}
	if(pForce->dMaxVelocity!=0.0)
	{
		int x;
		for(x=0;x<3;x++)
		{
			if(pForce->vDir.c[x]>0)
			{
				if(pInfo->vVelocity.c[x]<pForce->vDir.c[x]*pForce->dMaxVelocity)
				{
					pInfo->vVelocity.c[x]+=vForceVelocity.c[x];
					if(pInfo->vVelocity.c[x]>pForce->vDir.c[x]*pForce->dMaxVelocity)
					{
						pInfo->vVelocity.c[x]=pForce->vDir.c[x]*pForce->dMaxVelocity;
					}
				}
			}
			else if(pForce->vDir.c[x]<0)
			{
				if(pInfo->vVelocity.c[x]>pForce->vDir.c[x]*pForce->dMaxVelocity)
				{
					pInfo->vVelocity.c[x]+=vForceVelocity.c[x];
					if(pInfo->vVelocity.c[x]<pForce->vDir.c[x]*pForce->dMaxVelocity)
					{
						pInfo->vVelocity.c[x]=pForce->vDir.c[x]*pForce->dMaxVelocity;
					}
				}
			}
		}
	}
	else
	{
		pInfo->vVelocity+=vForceVelocity;
	}
}
