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

BEGIN_STRUCT_PROPS(SModelAnimationKeyFrame)
	PROP_VALUE_FLAGS(vPosition,"Position",Origin,MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(vAngles,"Angles",Origin,MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(nTime,"Time",0,MRPF_NORMAL|MRPF_OPTIONAL)
END_STRUCT_PROPS()

class CModelAnimationObjectType: public CAnimationObjectTypeBase,
virtual public IModelAnimationObjectTypeDesign,
virtual public IAnimationObjectTypePositionDesign,
virtual public IAnimationObjectTypeOrientationDesign
{
public:

	CGenericShaderWrapper   m_ShaderWrapper;
	CGenericModelWrapper    m_ModelWrapper;

	CVector 		m_vPosition;
	CVector 		m_vAngles;	
	CVector			m_vAngularVelocity;
    float       	m_dFps;
    unsigned int	m_nStartTime;
    unsigned int	m_nEndTime;
    bool        	m_bLoop;
    bool        	m_bCastShadow;
	bool        	m_bReceiveShadows;
	bool			m_bLighting;

	std::vector<SModelAnimationKeyFrame> m_vKeyFrames;
	
    IAnimationObject *CreateInstance(IAnimation *piAnimation,unsigned int dwCurrentTime);
	std::string 	  GetAnimationObjectDescription();
	
	void DesignRender(IGenericRender *pRender,CVector &vPosition,CVector &vAngles,bool bSelected);
	void DesignGetBBox(CVector *pvMins,CVector *pvMaxs);
	double DesignGetRadius();
	CTraceInfo DesignGetTrace(const CVector &vOrigin,const CVector &vAngles,const CVector &p1,const CVector &p2 );

    BEGIN_PROP_MAP(CModelAnimationObjectType)
        PROP_CLASS_CHAIN(CAnimationObjectTypeBase)
        PROP(m_ModelWrapper,"Model")
        PROP_FLAGS(m_ShaderWrapper,"Shader",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_bReceiveShadows,"ShadowReception",true,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_bCastShadow,"ShadowEmitter",true,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_dFps,"Fps",50.0,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_bLoop,"Loop",false,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_nStartTime,"StartTime",0,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_nEndTime,"EndTime",0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_vPosition,"Position",Origin,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_vAngles,"Angles",Origin,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_vAngularVelocity,"AngularVelocity",Origin,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_bLighting,"Lighting",true,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_vKeyFrames,"KeyFrames",MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP();

   // IModelAnimationObjectTypeDesign
   void GetConfig(SModelAnimationObjectTypeConfig *pConfig);
   void SetConfig(SModelAnimationObjectTypeConfig *pConfig);
   
   void SetModel(IGenericModel *piModel);
   void GetModel(IGenericModel **ppiModel);
   
   // IAnimationObjectTypePositionDesign
   
   CVector GetPosition();
   void	SetPosition(CVector vPosition);
   
   // IAnimationObjectTypeOrientaitonDesign
   
   CVector GetAngles();
   void	SetAngles(CVector vAngles);
   
   CModelAnimationObjectType();
    ~CModelAnimationObjectType();
};


class CModelAnimationObject: public CAnimationObjectBase
{
    CModelAnimationObjectType  *m_pType;
    unsigned int                m_nCurrentFrame;    
    bool                        m_bVisible;
	CVector                     m_vAngles;
	CVector                     m_vPosition;
	
    void Activate(unsigned int dwCurrentTime);
    void UpdateVisibility(unsigned int dwCurrentTime);
    void Render(IGenericRender *piRender,IGenericCamera *piCamera);

public:

    bool ProcessFrame(IPhysicManager *pPhysicManager,unsigned int dwCurrentTime,double dInterval);
    void CustomRender(IGenericRender *piRender,IGenericCamera *piCamera);

	CTraceInfo GetTrace(const CVector &vOrigin,const CVector &vAngles,const CVector &p1,const CVector &p2 );

    CModelAnimationObject(CModelAnimationObjectType *pType,IAnimation *piAnimation);
};
