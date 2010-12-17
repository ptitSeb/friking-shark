#include "StdAfx.h"
#include ".\AnimationSystems.h"
#include ".\AnimationObjectBase.h"
#include ".\ModelAnimationObjecttype.h"

CModelAnimationObjectType::CModelAnimationObjectType()
{
    m_bCastShadow=true;
    m_dFps=50.0;
    m_bCyclic=false;
    m_dwStartTime=0;
    m_dwEndTime=0;
}

CModelAnimationObjectType::~CModelAnimationObjectType()
{
}

IAnimationObject *CModelAnimationObjectType::CreateInstance(IAnimation *piAnimation,DWORD dwCurrentTime)
{
    CModelAnimationObject *pParticle=new CModelAnimationObject(this,piAnimation);
    return pParticle;
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
			piRender->DeactivateHeightFog();
			m_ModelWrapper.m_piModel->GetFrameBBox(0,0,&vMins,&vMaxs);
			piRender->RenderBBox(vPosition,vAngles,vMins,vMaxs,CVector(1,1,1),0x8888);
			piRender->PopState();
		}
		else
		{
			piRender->PushState();
			piRender->ActivateLighting();
			if(m_ShaderWrapper.m_piShader){m_ShaderWrapper.m_piShader->Activate();}
			piRender->RenderModel(vPosition,vAngles,m_ModelWrapper.m_piModel);
			if(m_ShaderWrapper.m_piShader){m_ShaderWrapper.m_piShader->Deactivate();}
			piRender->PopState();
		}
	}
}

void CModelAnimationObjectType::DesignGetBBox( CVector *pvMins,CVector *pvMaxs )
{
	if(m_ModelWrapper.m_piModel){m_ModelWrapper.m_piModel->GetFrameBBox(0,0,pvMins,pvMaxs);}
}

double CModelAnimationObjectType::DesignGetRadius()
{
	CVector vMins,vMaxs;
	DesignGetBBox(&vMins,&vMaxs);
	return GetBBoxRadius(vMins,vMaxs);
}

CTraceInfo CModelAnimationObjectType::DesignGetTrace( const CVector &vOrigin,const CVector &vAngles,const CVector &p1,const CVector &p2 )
{
	if(m_ModelWrapper.m_piModel)
	{
		return m_ModelWrapper.m_piModel->GetTrace(vOrigin,vAngles,p1,p2);
	}
	else
	{
		CTraceInfo info;
		info.m_dTraceFraction=1.0;
		info.m_vTracePos=p2;
		return info;
	}
}

CModelAnimationObject::CModelAnimationObject(CModelAnimationObjectType *pType,IAnimation *piAnimation)
:CAnimationObjectBase(pType,piAnimation)
{
    m_nCurrentFrame=0;
    m_pType=pType;
    m_bVisible=false;
}

void CModelAnimationObject::Activate(DWORD dwCurrentTime)
{
    CAnimationObjectBase::Activate(dwCurrentTime);
    UpdateVisibility(dwCurrentTime);
}

void CModelAnimationObject::Render(IGenericRender *piRender,IGenericCamera *piCamera)
{
	if(!m_bActive){return;}
	if(!m_bVisible){return;}
	IEntity *piEntity=m_piAnimation->GetEntity();
	if(m_pType->m_ModelWrapper.m_piModel && piEntity)
	{
		SPhysicInfo *pPhysicInfo=piEntity->GetPhysicInfo();
		piRender->ActivateLighting();
		if(m_pType->m_ShaderWrapper.m_piShader){m_pType->m_ShaderWrapper.m_piShader->Activate();}
		piRender->RenderModel(pPhysicInfo->vPosition,pPhysicInfo->vAngles,m_pType->m_ModelWrapper.m_piModel,0,m_nCurrentFrame);
		if(m_pType->m_ShaderWrapper.m_piShader){m_pType->m_ShaderWrapper.m_piShader->Deactivate();}
		piRender->DeactivateLighting();
	}
}
void CModelAnimationObject::CustomRender(IGenericRender *piRender,IGenericCamera *piCamera)
{
	Render(piRender,piCamera);
}

void CModelAnimationObject::UpdateVisibility(DWORD dwCurrentTime)
{
    DWORD dwRelativeTime=dwCurrentTime-m_piAnimation->GetCurrentTimeBase();
    m_bVisible=true;
    if(dwRelativeTime<m_pType->m_dwStartTime){m_bVisible=false;}
    if(dwRelativeTime>m_pType->m_dwEndTime && m_pType->m_dwEndTime){m_bVisible=false;}
}

bool CModelAnimationObject::ProcessFrame(IPhysicManager *pPhysicManager,DWORD dwCurrentTime,double dInterval)
{
    if(!m_bActive){return false;}
    UpdateVisibility(dwCurrentTime);

	unsigned long nFrames=m_pType->m_ModelWrapper.m_piModel?m_pType->m_ModelWrapper.m_piModel->GetAnimationFrames(0):0;

    if(nFrames) 
    {
        DWORD dwRelativeTime=dwCurrentTime-m_piAnimation->GetCurrentTimeBase()-m_pType->m_dwStartTime;
        m_nCurrentFrame=(int)((((double)dwRelativeTime)*m_pType->m_dFps)/1000.0);
        if(m_nCurrentFrame>(nFrames-1))
        {
            if(m_pType->m_bCyclic)
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
		return m_pType->m_ModelWrapper.m_piModel->GetTrace(vOrigin,vAngles,p1,p2);
	}
	else
	{
		CTraceInfo info;
		info.m_dTraceFraction=1.0;
		info.m_vTracePos=p2;
		return info;
	}
}