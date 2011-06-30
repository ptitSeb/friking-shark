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
#include "OpenGLGraphics.h"
#include "OpenGLLight.h"

COpenGLLight::COpenGLLight(void)
{
	m_eType=eGenericLightType_UNKNOWN;
	PersistencyInitialize();
}


COpenGLLight::~COpenGLLight(void)
{
}

bool COpenGLLight::Unserialize(ISystemPersistencyNode *piNode)
{
	bool bResult=CSystemObjectBase::Unserialize(piNode);
	return bResult;
}

CVector COpenGLLight::GetAmbientColor(){return m_vAmbientColor;}
CVector COpenGLLight::GetDiffuseColor(){return m_vDiffuseColor;}
CVector COpenGLLight::GetSpecularColor(){return m_vSpecularColor;}
CVector COpenGLLight::GetPosition(){return m_vPosition;}

void COpenGLLight::SetAmbientColor(CVector cColor){m_vAmbientColor=cColor;}
void COpenGLLight::SetDiffuseColor(CVector cColor){m_vDiffuseColor=cColor;}
void COpenGLLight::SetSpecularColor(CVector cColor){m_vSpecularColor=cColor;}
void COpenGLLight::SetPosition( CVector vPosition ){m_vPosition=vPosition;}

void COpenGLLight::SetOmni( double dRadius )
{
	m_eType=eGenericLightType_Omni;
	m_dOmniRadius=dRadius;
}

void COpenGLLight::SetSpot( CVector vDirection,double dAngle,double dExponent,double dAttenuationConstant,double dAttenuationLinear,double dAttenuationQuadratic )
{
	m_eType=eGenericLightType_Spot;
	m_vSpotDirection=vDirection;
	m_dSpotAngle=dAngle;
	m_dSpotExponent=dExponent;
	m_dSpotAttenuationConstant=dAttenuationConstant;
	m_dSpotAttenuationLinear=dAttenuationLinear;
	m_dSpotAttenuationQuadratic=dAttenuationQuadratic;
}


void COpenGLLight::SetDirectional( CVector vDirection )
{
	m_eType=eGenericLightType_Directional;
	m_vDirectionalDirection=vDirection;
}

eGenericLightType COpenGLLight::GetType(){return m_eType;}
double COpenGLLight::GetOmniRadius()
{
	return m_dOmniRadius;
}
CVector COpenGLLight::GetSpotDirection(){return m_vSpotDirection;}
double COpenGLLight::GetSpotAngle(){return m_dSpotAngle;}

void COpenGLLight::GetSpotParameters( double *pdExponent,double *pdAttenuationConstant,double *pdAttenuationLinear,double *pdAttenuationQuadratic )
{
	if(pdExponent){*pdExponent=m_dSpotExponent;}
	if(pdAttenuationConstant){*pdAttenuationConstant=m_dSpotAttenuationConstant;}
	if(pdAttenuationLinear){*pdAttenuationLinear=m_dSpotAttenuationLinear;}
	if(pdAttenuationQuadratic){*pdAttenuationQuadratic=m_dSpotAttenuationQuadratic;}
}
CVector COpenGLLight::GetDirectionalDirection(){return m_vDirectionalDirection;}
