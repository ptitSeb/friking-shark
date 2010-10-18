#include "StdAfx.h"
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

CVector COpenGLLight::GetColor(){return m_vColor;}
CVector COpenGLLight::GetPosition(){return m_vPosition;}
void COpenGLLight::SetColor( CVector cColor ){m_vColor=cColor;}
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
