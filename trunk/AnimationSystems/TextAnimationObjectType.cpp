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
#include "TextAnimationObjectType.h"

CTextAnimationObjectType::CTextAnimationObjectType()
{
    m_nStartTime=0;
    m_nEndTime=0;
	m_dFontSize=10;
}

CTextAnimationObjectType::~CTextAnimationObjectType()
{
}

IAnimationObject *CTextAnimationObjectType::CreateInstance(IAnimation *piAnimation,unsigned int dwCurrentTime)
{
    CTextAnimationObject *pParticle=new CTextAnimationObject(this,piAnimation);
    return pParticle;
}


void CTextAnimationObjectType::DesignRender( IGenericRender *piRender,CVector &vPosition,CVector &vAngles ,bool bSelected)
{
	if(m_FontWrapper.m_piFont)
	{
		if(!bSelected)
		{		
			CVector vTempPos,vTempAngles;
			ComputeReferenceSystem(vPosition,vAngles,m_vPosition,Origin,&vTempPos,&vTempAngles);
			
			double dWidth=0,dHeight=0;
			m_FontWrapper.m_piFont->CalcTextSize(m_dFontSize,m_sText.c_str(),&dWidth,&dHeight);
			vTempPos-=piRender->GetCameraRight()*(dWidth*0.5);
			vTempPos-=piRender->GetCameraUp()*(dHeight*0.5);
			m_FontWrapper.m_piFont->RenderText(piRender,m_dFontSize,vTempPos,m_sText.c_str());
		}
	}
}

void CTextAnimationObjectType::DesignGetBBox( CVector *pvMins,CVector *pvMaxs )
{
	double dSize=DesignGetRadius();
	if(pvMins){*pvMins=CVector(-dSize*0.5,-dSize*0.5,-dSize*0.5);}
	if(pvMaxs){*pvMaxs=CVector(dSize*0.5,dSize*0.5,dSize*0.5);}
}

double CTextAnimationObjectType::DesignGetRadius()
{
	double dWidth=0,dHeight=0;
	if(m_FontWrapper.m_piFont){m_FontWrapper.m_piFont->CalcTextSize(m_dFontSize,m_sText.c_str(),&dWidth,&dHeight);}
	return std::max(dWidth,dHeight);
}

CTraceInfo CTextAnimationObjectType::DesignGetTrace( const CVector &vOrigin,const CVector &vAngles,const CVector &p1,const CVector &p2 )
{
	CTraceInfo info;
	info.m_dTraceFraction=1.0;
	info.m_vTracePos=p2;
	return info;
}

void CTextAnimationObjectType::GetConfig(STextAnimationObjectTypeConfig *pConfig)
{
	pConfig->nStartTime=m_nStartTime;
	pConfig->nEndTime=m_nEndTime;
	pConfig->sText=m_sText;
	pConfig->dFontSize=m_dFontSize;
}
void CTextAnimationObjectType::SetConfig(STextAnimationObjectTypeConfig *pConfig)
{
	m_nStartTime=pConfig->nStartTime;
	m_nEndTime=pConfig->nEndTime;
	m_sText=pConfig->sText;
	m_dFontSize=pConfig->dFontSize;
}

void CTextAnimationObjectType::SetFont(IGenericFont *piFont){m_FontWrapper.Attach(piFont);}
void CTextAnimationObjectType::GetFont(IGenericFont **ppiFont){(*ppiFont)=ADD(m_FontWrapper.m_piFont);}
std::string CTextAnimationObjectType::GetAnimationObjectDescription(){return m_FontWrapper.m_piObject?m_FontWrapper.m_piObject->GetName():"Unknown";}


CVector CTextAnimationObjectType::GetPosition(){return m_vPosition;}
void	CTextAnimationObjectType::SetPosition(CVector vPosition){m_vPosition=vPosition;}

CTextAnimationObject::CTextAnimationObject(CTextAnimationObjectType *pType,IAnimation *piAnimation)
:CAnimationObjectBase(pType,piAnimation)
{
    m_pType=pType;
    m_bVisible=false;
}

void CTextAnimationObject::Activate(unsigned int dwCurrentTime)
{
    CAnimationObjectBase::Activate(dwCurrentTime);
    UpdateVisibility(dwCurrentTime);
}

void CTextAnimationObject::Render(IGenericRender *piRender,IGenericCamera *piCamera)
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
	if(m_pType){m_pType->DesignRender(piRender,vPosition,vAngles,false);}
}
void CTextAnimationObject::CustomRender(IGenericRender *piRender,IGenericCamera *piCamera)
{
	Render(piRender,piCamera);
}

void CTextAnimationObject::UpdateVisibility(unsigned int dwCurrentTime)
{
    unsigned int dwRelativeTime=dwCurrentTime-m_piAnimation->GetCurrentTimeBase();
    m_bVisible=true;
    if(dwRelativeTime<m_pType->m_nStartTime){m_bVisible=false;}
    if(dwRelativeTime>m_pType->m_nEndTime && m_pType->m_nEndTime){m_bVisible=false;}
}

bool CTextAnimationObject::ProcessFrame(IPhysicManager *pPhysicManager,unsigned int dwCurrentTime,double dInterval)
{
    if(!m_bActive){return false;}
    UpdateVisibility(dwCurrentTime);
    return m_bActive;
}
