#pragma once

#include "GameGraphics.h"

class CTextAnimationObjectType: public CAnimationObjectTypeBase,
virtual public ITextAnimationObjectTypeDesign,
virtual public IAnimationObjectTypePositionDesign
{
public:

	CGenericFontWrapper    m_FontWrapper;

    unsigned int	m_nStartTime;
    unsigned int	m_nEndTime;
	std::string 	m_sText;
	double			m_dFontSize;
	CVector         m_vPosition;

    IAnimationObject *CreateInstance(IAnimation *piAnimation,unsigned int dwCurrentTime);
	std::string 	  GetAnimationObjectDescription();
	
	void DesignRender(IGenericRender *pRender,CVector &vPosition,CVector &vAngles,bool bSelected);
	void DesignGetBBox(CVector *pvMins,CVector *pvMaxs);
	double DesignGetRadius();
	CTraceInfo DesignGetTrace(const CVector &vOrigin,const CVector &vAngles,const CVector &p1,const CVector &p2 );

    BEGIN_PROP_MAP(CTextAnimationObjectType)
        PROP_CLASS_CHAIN(CAnimationObjectTypeBase)
        PROP(m_FontWrapper,"Font")
		PROP_FLAGS(m_sText,"Text",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_vPosition,"Position",Origin,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dFontSize,"FontSize",10,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_nStartTime,"StartTime",0,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_nEndTime,"EndTime",0,MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP();

   // ITextAnimationObjectTypeDesign
   void GetConfig(STextAnimationObjectTypeConfig *pConfig);
   void SetConfig(STextAnimationObjectTypeConfig *pConfig);
   
   void SetFont(IGenericFont *piFont);
   void GetFont(IGenericFont**ppiFont);
   
   // IAnimationObjectTypePositionDesign
   
   CVector GetPosition();
   void	SetPosition(CVector vPosition);
   
   CTextAnimationObjectType();
    ~CTextAnimationObjectType();
};


class CTextAnimationObject: public CAnimationObjectBase
{
    CTextAnimationObjectType  *m_pType;
    bool                       m_bVisible;

    void Activate(unsigned int dwCurrentTime);
    void UpdateVisibility(unsigned int dwCurrentTime);
    void Render(IGenericRender *piRender,IGenericCamera *piCamera);

public:

    bool ProcessFrame(IPhysicManager *pPhysicManager,unsigned int dwCurrentTime,double dInterval);
    void CustomRender(IGenericRender *piRender,IGenericCamera *piCamera);

    CTextAnimationObject(CTextAnimationObjectType *pType,IAnimation *piAnimation);
};
