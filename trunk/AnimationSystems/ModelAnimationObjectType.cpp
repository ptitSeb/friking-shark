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
#include "AnimationSystems.h"
#include "AnimationObjectBase.h"
#include "ModelAnimationObjectType.h"

CModelAnimationObjectType::CModelAnimationObjectType()
{
    m_bCastShadow=true;
    m_dFps=50.0;
    m_bLoop=false;
	m_bReceiveShadows=true;
	m_bLighting=true;
    m_nStartTime=0;
    m_nEndTime=0;
}

CModelAnimationObjectType::~CModelAnimationObjectType()
{
}

IAnimationObject *CModelAnimationObjectType::CreateInstance(IAnimation *piAnimation,unsigned int dwCurrentTime)
{
    CModelAnimationObject *pParticle=new CModelAnimationObject(this,piAnimation);
    return pParticle;
}

bool CModelAnimationObjectType::PrepareResources()
{
	if(!m_ModelWrapper.m_piModel){return false;}
	return m_ModelWrapper.m_piModel->Prepare();
}

void CModelAnimationObjectType::DesignRender( IGenericRender *piRender,CVector &vPosition,CVector &vAngles ,bool bSelected)
{
	if(m_ModelWrapper.m_piModel)
	{
		if(bSelected)
		{
			CVector vMins,vMaxs;
			piRender->PushState();
			piRender->DeactivateShadowReception();

			CVector vTempPos,vTempAngles;
			ComputeReferenceSystem(vPosition,vAngles,m_vPosition,m_vAngles,&vTempPos,&vTempAngles);
			m_ModelWrapper.m_piModel->GetFrameBBox(0,0,&vMins,&vMaxs);
			piRender->RenderBBox(vTempPos,vTempAngles,vMins,vMaxs,CVector(1,1,1),0x8888);
			piRender->PopState();
		}
		else
		{
			CVector vTempPos,vTempAngles;
			ComputeReferenceSystem(vPosition,vAngles,m_vPosition,m_vAngles,&vTempPos,&vTempAngles);
			
			piRender->PushState();
			if(!m_bCastShadow){piRender->DeactivateShadowEmission();}
			if(!m_bReceiveShadows){piRender->DeactivateShadowReception();}
			if(m_bLighting){piRender->ActivateLighting();}
			if(m_ShaderWrapper.m_piShader){m_ShaderWrapper.m_piShader->Activate();}
			piRender->RenderModel(vTempPos,vTempAngles,m_ModelWrapper.m_piModel);
			if(m_ShaderWrapper.m_piShader){m_ShaderWrapper.m_piShader->Deactivate();}
			piRender->PopState();
		}
	}
}

void CModelAnimationObjectType::DesignGetAABBox(CVector &vPosition,CVector &vAngles,CVector *pvMins,CVector *pvMaxs )
{
	if(m_ModelWrapper.m_piModel)
	{
		CVector vTempPos,vTempAngles;
		ComputeReferenceSystem(vPosition,vAngles,m_vPosition,m_vAngles,&vTempPos,&vTempAngles);
		
		CVector pvVolume[8];
		CVector vLocalMins,vLocalMaxs;
		m_ModelWrapper.m_piModel->GetFrameBBox(0,0,&vLocalMins,&vLocalMaxs);
		CalcBBoxVolume(vTempPos,vTempAngles,vLocalMins,vLocalMaxs,pvVolume);
		
		if(pvMins){*pvMins=pvVolume[0];}
		if(pvMaxs){*pvMaxs+=pvVolume[0];}
		for(int x=1;x<8;x++)
		{
			if(pvMins)
			{
				CVector vCurrentMins=*pvMins;
				pvMins->Mins(vCurrentMins,pvVolume[x]);
			}
			if(pvMaxs)
			{
				CVector vCurrentMaxs=*pvMaxs;
				pvMaxs->Maxs(vCurrentMaxs,pvVolume[x]);
			}
		}
	}
}

double CModelAnimationObjectType::DesignGetRadius()
{
	CVector vMins,vMaxs;
	DesignGetAABBox(Origin,Origin,&vMins,&vMaxs);
	return GetBBoxRadius(vMins,vMaxs);
}

CTraceInfo CModelAnimationObjectType::DesignGetTrace( const CVector &vOrigin,const CVector &vAngles,const CVector &p1,const CVector &p2 )
{
	if(m_ModelWrapper.m_piModel)
	{
		CVector vTempPos,vTempAngles;
		ComputeReferenceSystem(vOrigin,vAngles,m_vPosition,m_vAngles,&vTempPos,&vTempAngles);
		return m_ModelWrapper.m_piModel->GetTrace(vTempPos,vTempAngles,p1,p2);
	}
	else
	{
		CTraceInfo info;
		info.m_dTraceFraction=1.0;
		info.m_vTracePos=p2;
		return info;
	}
}

void CModelAnimationObjectType::GetConfig(SModelAnimationObjectTypeConfig *pConfig)
{
	pConfig->nStartTime=m_nStartTime;
	pConfig->nEndTime=m_nEndTime;
	pConfig->bCastShadow=m_bCastShadow;
	pConfig->bReceiveShadows=m_bReceiveShadows;
	pConfig->bLoop=m_bLoop;
	pConfig->dFps=m_dFps;
	pConfig->vKeyFrames=m_vKeyFrames;
}
void CModelAnimationObjectType::SetConfig(SModelAnimationObjectTypeConfig *pConfig)
{
	m_nStartTime=pConfig->nStartTime;
	m_nEndTime=pConfig->nEndTime;
	m_bCastShadow=pConfig->bCastShadow;
	m_bReceiveShadows=pConfig->bReceiveShadows;
	m_bLoop=pConfig->bLoop;
	m_dFps=pConfig->dFps;
	m_vKeyFrames=pConfig->vKeyFrames;	
}

void CModelAnimationObjectType::SetModel(IGenericModel *piModel){m_ModelWrapper.Attach(piModel);}
void CModelAnimationObjectType::GetModel(IGenericModel **ppiModel){(*ppiModel)=ADD(m_ModelWrapper.m_piModel);}
std::string CModelAnimationObjectType::GetAnimationObjectDescription(){return m_ModelWrapper.m_piObject?m_ModelWrapper.m_piObject->GetName():"Unknown";}


CVector CModelAnimationObjectType::GetPosition(){return m_vPosition;}
void	CModelAnimationObjectType::SetPosition(CVector vPosition){m_vPosition=vPosition;}
CVector CModelAnimationObjectType::GetAngles(){return m_vAngles;}
void	CModelAnimationObjectType::SetAngles(CVector vAngles){m_vAngles=vAngles;}

CModelAnimationObject::CModelAnimationObject(CModelAnimationObjectType *pType,IAnimation *piAnimation)
:CAnimationObjectBase(pType,piAnimation)
{
    m_nCurrentFrame=0;
    m_pType=pType;
    m_bVisible=false;
}

void CModelAnimationObject::Activate(unsigned int dwCurrentTime)
{
    CAnimationObjectBase::Activate(dwCurrentTime);
	m_vPosition=m_pType->m_vPosition;
	m_vAngles=m_pType->m_vAngles;
    UpdateVisibility(dwCurrentTime);
}

void CModelAnimationObject::Render(IGenericRender *piRender,IGenericCamera *piCamera)
{
	if(!m_bActive){return;}
	if(!m_bVisible){return;}
	CVector vPosition,vAngles;
	
	IEntity *piEntity=m_piAnimation->GetEntity();
	if(piEntity)
	{
		SPhysicInfo *pPhysicInfo=piEntity->GetPhysicInfo();
		vPosition=pPhysicInfo->vPosition;
		vAngles=pPhysicInfo->vAngles;
		
	}
	
	CVector vTempPos,vTempAngles;
	ComputeReferenceSystem(vPosition,vAngles,m_vPosition,m_vAngles,&vTempPos,&vTempAngles);
	
	piRender->PushState();
	if(!m_pType->m_bCastShadow){piRender->DeactivateShadowEmission();}
	if(!m_pType->m_bReceiveShadows){piRender->DeactivateShadowReception();}
	if(m_pType->m_bLighting){piRender->ActivateLighting();}
	if(m_pType->m_ShaderWrapper.m_piShader){m_pType->m_ShaderWrapper.m_piShader->Activate();}
	piRender->RenderModel(vTempPos,vTempAngles,m_pType->m_ModelWrapper.m_piModel);
	if(m_pType->m_ShaderWrapper.m_piShader){m_pType->m_ShaderWrapper.m_piShader->Deactivate();}
	piRender->PopState();
}
void CModelAnimationObject::CustomRender(IGenericRender *piRender,IGenericCamera *piCamera)
{
	Render(piRender,piCamera);
}

void CModelAnimationObject::UpdateVisibility(unsigned int dwCurrentTime)
{
    unsigned int dwRelativeTime=dwCurrentTime-m_piAnimation->GetCurrentTimeBase();
    m_bVisible=true;
    if(dwRelativeTime<m_pType->m_nStartTime){m_bVisible=false;}
    if(dwRelativeTime>m_pType->m_nEndTime && m_pType->m_nEndTime){m_bVisible=false;}
}

bool CModelAnimationObject::ProcessFrame(IPhysicManager *pPhysicManager,unsigned int dwCurrentTime,double dInterval)
{
    if(!m_bActive){return false;}
    UpdateVisibility(dwCurrentTime);

	unsigned long nFrames=m_pType->m_ModelWrapper.m_piModel?m_pType->m_ModelWrapper.m_piModel->GetAnimationFrames(0):0;

	if(m_pType->m_vAngularVelocity.c[0]==0 && m_pType->m_vAngularVelocity.c[1]==0 && m_pType->m_vAngularVelocity.c[2]==0)
	{
		m_vAngles=m_pType->m_vAngles;
	}
	else
	{
		m_vAngles+=m_pType->m_vAngularVelocity*dInterval;
	}
	
	if(m_pType->m_vKeyFrames.size())
	{
		unsigned int dwTime=dwCurrentTime-m_piAnimation->GetCurrentTimeBase();
		
		int nNextKeyFrame=-1;
		for(unsigned int x=0;x<m_pType->m_vKeyFrames.size();x++)
		{
			nNextKeyFrame=(int)x;
			if(m_pType->m_vKeyFrames[x].nTime>dwTime){break;}
		}
		if(nNextKeyFrame!=-1)
		{
			unsigned int dwPrevTime=(nNextKeyFrame==0)?0:m_pType->m_vKeyFrames[nNextKeyFrame-1].nTime;
			CVector vPrevAngles=(nNextKeyFrame==0)?m_pType->m_vAngles:m_pType->m_vKeyFrames[nNextKeyFrame-1].vAngles;
			CVector vPrevPosition=(nNextKeyFrame==0)?m_pType->m_vPosition:m_pType->m_vKeyFrames[nNextKeyFrame-1].vPosition;
			double dKeyFraction=((double)(dwTime-dwPrevTime))/((double)(m_pType->m_vKeyFrames[nNextKeyFrame].nTime-dwPrevTime));
			if(dKeyFraction>1.0){dKeyFraction=1.0;}
			m_vAngles=vPrevAngles+(m_pType->m_vKeyFrames[nNextKeyFrame].vAngles-vPrevAngles)*dKeyFraction;
			m_vPosition=vPrevPosition+(m_pType->m_vKeyFrames[nNextKeyFrame].vPosition-vPrevPosition)*dKeyFraction;
		}
			
	}
	else
	{
		m_vPosition=m_pType->m_vPosition;
	}
	
	
	
    if(nFrames) 
    {
        unsigned int dwRelativeTime=dwCurrentTime-m_piAnimation->GetCurrentTimeBase()-m_pType->m_nStartTime;
        m_nCurrentFrame=(int)((((double)dwRelativeTime)*m_pType->m_dFps)/1000.0);
        if(m_nCurrentFrame>(nFrames-1))
        {
            if(m_pType->m_bLoop)
            {
                m_nCurrentFrame=m_nCurrentFrame%nFrames;
            }
            else
            {
                m_nCurrentFrame=nFrames-1;
            }
        }
    }
    else
    {
        m_nCurrentFrame=0;
    }
    return m_bActive;
}

CTraceInfo CModelAnimationObject::GetTrace(const CVector &vOrigin,const CVector &vAngles,const CVector &p1,const CVector &p2 )
{
	if(m_pType->m_ModelWrapper.m_piModel)
	{
		CVector vTempPos,vTempAngles;
		ComputeReferenceSystem(vOrigin,vAngles,m_vPosition,m_vAngles,&vTempPos,&vTempAngles);		
		return m_pType->m_ModelWrapper.m_piModel->GetTrace(vTempPos,vTempAngles,p1,p2);
	}
	else
	{
		CTraceInfo info;
		info.m_dTraceFraction=1.0;
		info.m_vTracePos=p2;
		return info;
	}
}