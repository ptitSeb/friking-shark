#pragma once

#include "GameGraphics.h"

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
    float       	m_dFps;
    unsigned int	m_nStartTime;
    unsigned int	m_nEndTime;
    bool        	m_bLoop;
    bool        	m_bCastShadow;

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
		PROP_VALUE_FLAGS(m_bCastShadow,"ShadowEmitter",true,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_dFps,"Fps",50.0,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_bLoop,"Loop",false,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_nStartTime,"StartTime",0,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_nEndTime,"EndTime",0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_vPosition,"Position",Origin,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_vAngles,"Angles",Origin,MRPF_NORMAL|MRPF_OPTIONAL)
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

    void Activate(unsigned int dwCurrentTime);
    void UpdateVisibility(unsigned int dwCurrentTime);
    void Render(IGenericRender *piRender,IGenericCamera *piCamera);

public:

    bool ProcessFrame(IPhysicManager *pPhysicManager,unsigned int dwCurrentTime,double dInterval);
    void CustomRender(IGenericRender *piRender,IGenericCamera *piCamera);

	CTraceInfo GetTrace(const CVector &vOrigin,const CVector &vAngles,const CVector &p1,const CVector &p2 );

    CModelAnimationObject(CModelAnimationObjectType *pType,IAnimation *piAnimation);
};
