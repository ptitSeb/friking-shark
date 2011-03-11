#include "./stdafx.h"
#include "OpenGLCamera.h"

COpenGLCamera::COpenGLCamera()
{
	PersistencyDefaultValue();
}

CVector &COpenGLCamera::GetPosition(){return m_vPosition;}
CVector &COpenGLCamera::GetAngles(){return m_vAngles;}

void	COpenGLCamera::SetPosition(CVector &vPosition){m_vPosition=vPosition;}
void	COpenGLCamera::SetAngles(CVector &vAngles){m_vAngles=vAngles;VectorsFromAngles(m_vAngles,&m_vForward,&m_vRight,&m_vUp);}

void	COpenGLCamera::SetClippingPlanes(double dNear,double dFar){m_dNearClippingPlane=dNear;m_dFarClippingPlane=dFar;}
void	COpenGLCamera::GetClippingPlanes(double &dNear,double &dFar){dNear=m_dNearClippingPlane;dFar=m_dFarClippingPlane;}

void	COpenGLCamera::SetViewAngle(double dAngle){m_dViewAngle=dAngle;}
double	COpenGLCamera::GetViewAngle(){return m_dViewAngle;}

CVector	&COpenGLCamera::GetUpVector(){return m_vUp;}
CVector	&COpenGLCamera::GetRightVector(){return m_vRight;}
CVector	&COpenGLCamera::GetForwardVector(){return  m_vForward;}

void	COpenGLCamera::SetAspectRatio(double dRatio){m_dAspectRatio=dRatio;}
double COpenGLCamera::GetAspectRatio(){return m_dAspectRatio;}
