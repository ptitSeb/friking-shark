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
