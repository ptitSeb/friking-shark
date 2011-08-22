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
#include "GameGraphics.h"

class COpenGLLight : virtual public CSystemObjectBase,virtual public IGenericLight
{
	bool Unserialize(ISystemPersistencyNode *piNode);

	eGenericLightType  m_eType;

	CVector m_vAmbientColor;
	CVector m_vDiffuseColor;
	CVector m_vSpecularColor;

	CVector m_vPosition;

	// only for omni sources	
	double m_dOmniRadius;

	// only for spot sources
	CVector m_vSpotDirection;
	double m_dSpotAngle;
	double m_dSpotExponent;
	double m_dSpotAttenuationConstant;
	double m_dSpotAttenuationLinear;
	double m_dSpotAttenuationQuadratic;

	// only for directional sources
	CVector m_vDirectionalDirection;

public:
	
	BEGIN_PROP_MAP(COpenGLLight)
		PROP(m_eType,"Type");
		PROP(m_vPosition,"Position");
		PROP_VALUE(m_vAmbientColor,"Ambient",CVector(1,1,1));
		PROP_VALUE(m_vDiffuseColor,"Diffuse",CVector(1,1,1));
		PROP_VALUE(m_vSpecularColor,"Specular",CVector(1,1,1));
		// only for omni sources	
		PROP_VALUE_FLAGS(m_dOmniRadius,"OmniRadius",0,MRPF_NORMAL|MRPF_OPTIONAL);
		// only for spot sources
		PROP_VALUE_FLAGS(m_vSpotDirection,"SpotDirection",AxisPosX,MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_VALUE_FLAGS(m_dSpotAngle,"SpotAngle",0,MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_VALUE_FLAGS(m_dSpotExponent,"SpotExponent",0,MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_VALUE_FLAGS(m_dSpotAttenuationConstant,"SpotAttenuationConstant",1,MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_VALUE_FLAGS(m_dSpotAttenuationLinear,"SpotAttenuationLinear",0,MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_VALUE_FLAGS(m_dSpotAttenuationQuadratic,"SpotAttenuationQuadratic",0,MRPF_NORMAL|MRPF_OPTIONAL);
		// only for directional sources
		PROP_VALUE_FLAGS(m_vDirectionalDirection,"DirectionalDirection",AxisNegY,MRPF_NORMAL|MRPF_OPTIONAL);
	END_PROP_MAP();

	// IGenericLight
	eGenericLightType GetType();

	CVector GetAmbientColor();
	CVector GetDiffuseColor();
	CVector GetSpecularColor();
	CVector GetPosition();

	void SetAmbientColor(CVector cColor);
	void SetDiffuseColor(CVector cColor);
	void SetSpecularColor(CVector cColor);
	void SetPosition(CVector vPosition);

	void SetOmni(double dRadius);
	void SetSpot(CVector vDirection,double dAngle,double dExponent,double dAttenuationConstant,double dAttenuationLinear,double dAttenuationQuadratic);
	void SetDirectional(CVector vDirection);

	// only for omni sources	
	double GetOmniRadius();

	// only for spot sources
	CVector GetSpotDirection();
	double	GetSpotAngle();
	void	GetSpotParameters(double *pdExponent,double *pdAttenuationConstant,double *pdAttenuationLinear,double *pdAttenuationQuadratic);

	// only for directional sources
	CVector GetDirectionalDirection();

	COpenGLLight(void);
	~COpenGLLight(void);
};
