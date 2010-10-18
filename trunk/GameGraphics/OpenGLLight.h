#pragma once
#include "gamegraphics.h"

class COpenGLLight : virtual public CSystemObjectBase,virtual public IGenericLight
{
	bool Unserialize(ISystemPersistencyNode *piNode);

	eGenericLightType  m_eType;

	CVector m_vColor;
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
		PROP_VALUE(m_vColor,"Color",CVector(1,1,1));
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

	CVector GetColor();
	CVector GetPosition();

	void SetColor(CVector cColor);
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
