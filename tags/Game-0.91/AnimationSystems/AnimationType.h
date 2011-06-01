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

DECLARE_CUSTOM_WRAPPER1(CAnimationObjectTypeWrapper,IAnimationObjectType,m_piObjectType)

class CAnimationType: virtual public CSystemObjectBase,virtual public IAnimationType,virtual public IAnimationTypeDesign
{
public:

    // Propiedades Persistentes

	vector<CAnimationObjectTypeWrapper> m_vObjects;
	bool                                m_bLoop;

    IAnimation *CreateInstance(IEntity *piEntity,unsigned int dwCurrentTime);

	void DesignRender( IGenericRender *piRender,CVector &vPosition,CVector &vAngles,bool bSelected);
	void DesignGetBBox(CVector *pvMins,CVector *pvMaxs);
	double DesignGetRadius();
	CTraceInfo DesignGetTrace(const CVector &vPosition,const CVector &vAngles,const CVector &p1,const CVector &p2 );

    BEGIN_PROP_MAP(CAnimationType);
		PROP(m_vObjects,"Objects");
        PROP_VALUE_FLAGS(m_bLoop,"Loop",false,MRPF_NORMAL|MRPF_OPTIONAL);
    END_PROP_MAP();

	// IAnimationTypeDesign
	
	void GetAnimationTypeConfig(SAnimationTypeConfig *pConfig);
	void SetAnimationTypeConfig(SAnimationTypeConfig *pConfig);
	
	// Object management
	
	unsigned int	AddObject(std::string sObjectType);
	bool			RemoveObject(unsigned int nObject);
	bool			GetObject(unsigned int nObject,IAnimationObjectType **ppiObject);
	unsigned int	GetObjectCount();
	
	CAnimationType();
    ~CAnimationType();
};

class CAnimation:public IAnimation
{
protected:

    vector<IAnimationObject *>  m_vObjects;
    CAnimationType             *m_pType;
    bool                        m_bActive;
    bool                        m_bFinished;
    IEntity                    *m_piEntity;
    unsigned int                m_dwCurrentTimeBase;

public:

    void Activate(unsigned int dwCurrentTime);
    void Deactivate();
    bool IsActive();

    bool HasFinished();

    unsigned int GetCurrentTimeBase();

    bool ProcessFrame(IPhysicManager *pPhysicManager,unsigned int dwCurrentTime,double dInterval);
    void CustomRender(IGenericRender *piRender,IGenericCamera *piCamera);
	CTraceInfo GetTrace(const CVector &vOrigin,const CVector &vAngles,const CVector &p1,const CVector &p2 );

    void                AddObject(IAnimationObject *pObject);
    IAnimationObject    *GetObject(string sName);
    IEntity             *GetEntity();

    CAnimation(CAnimationType *pType,IEntity *piEntity,unsigned int dwCurrentTimeBase);
    virtual ~CAnimation();
};
