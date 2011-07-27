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
	void DesignGetAABBox(CVector *pvMins,CVector *pvMaxs);
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
