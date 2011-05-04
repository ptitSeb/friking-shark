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


#pragma once


class COpenGLCamera: virtual public CSystemObjectBase,virtual public IGenericCamera
{
	CVector m_vPosition;
	CVector m_vAngles;
	double  m_dViewAngle;
	double  m_dFarClippingPlane;
	double  m_dNearClippingPlane;

	CVector m_vUp;
	CVector m_vRight;
	CVector m_vForward;
	double  m_dAspectRatio;

public:

	COpenGLCamera();

	CVector &GetPosition();
	CVector &GetAngles();

	void	SetPosition(CVector &vPosition);
	void	SetAngles(CVector &vAngles);

	void	SetClippingPlanes(double dNear,double dFar);
	void	GetClippingPlanes(double &dNear,double &dFar);

	void	SetViewAngle(double dAngle);
	double	GetViewAngle();

	CVector	&GetUpVector();
	CVector	&GetRightVector();
	CVector	&GetForwardVector();

	void	 SetAspectRatio(double dRatio);
	double	 GetAspectRatio();

	BEGIN_PROP_MAP(COpenGLCamera)
		PROP_VALUE_FLAGS(m_vPosition,"Posicion",Origin,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_vAngles,"Angulos",Origin,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dViewAngle,"AnguloVisualizacion",50,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dNearClippingPlane,"NearPlane",10,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dFarClippingPlane,"FarPlane",10000,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dAspectRatio,"AspectRatio",1,MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP();

};