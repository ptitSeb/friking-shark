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

class CAnimationObjectTypeBase: virtual public CSystemObjectBase,virtual public IAnimationObjectType
{
public:

    string m_sObjectName;

	bool PrepareResources();
	void DesignRender(IGenericRender *pRender,CVector &vPosition,CVector &vAngles,bool bSelected);
	void DesignGetAABBox(CVector &vPosition,CVector &vAngles,CVector *pvMins,CVector *pvMaxs);
	double DesignGetRadius();
	CTraceInfo DesignGetTrace(const CVector &vOrigin,const CVector &vAngles,const CVector &p1,const CVector &p2 );

    BEGIN_PROP_MAP(CAnimationObjectTypeBase);
        PROP_VALUE_FLAGS(m_sObjectName,"Nombre","",MRPF_NORMAL|MRPF_OPTIONAL)
    END_PROP_MAP();

    CAnimationObjectTypeBase();
};

class CAnimationObjectBase: public IAnimationObject
{
protected:

    bool                        m_bActive;
    IAnimation                  *m_piAnimation;
    CAnimationObjectTypeBase    *m_pTypeBase;

public:

    string      GetName();

    void        Activate(unsigned int dwCurrentTime);
    void        Deactivate();
    bool        IsActive();
    IAnimation *GetAnimation();

	CTraceInfo GetTrace(const CVector &vOrigin,const CVector &vAngles,const CVector &p1,const CVector &p2 );

    bool ProcessFrame(IPhysicManager *pPhysicManager,unsigned int dwCurrentTime,double dInterval);
    void CustomRender(IGenericRender *piRender,IGenericCamera *piCamera);

    CAnimationObjectBase(CAnimationObjectTypeBase *pType,IAnimation *piAnimation);
};
