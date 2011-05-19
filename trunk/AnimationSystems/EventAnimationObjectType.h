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

class CEventAnimationObjectType: public CAnimationObjectTypeBase,virtual public IEventAnimationObjectTypeDesign
{
public:

    string      m_sEvent;
    string      m_sParams;
    unsigned int       m_nTime;
    
    IAnimationObject *CreateInstance(IAnimation *piAnimation,unsigned int dwCurrentTime);
	std::string 	  GetAnimationObjectDescription();
	
    BEGIN_PROP_MAP(CEventAnimationObjectType)
        PROP_CLASS_CHAIN(CAnimationObjectTypeBase)
		PROP(m_nTime,"Time")
        PROP(m_sEvent,"Event")
        PROP_VALUE_FLAGS(m_sParams,"Parameters","",MRPF_NORMAL|MRPF_OPTIONAL)
    END_PROP_MAP();
	

	
	// IEventAnimationObjectTypeDesign
	void GetConfig(SEventAnimationObjectTypeConfig *pConfig);
	void SetConfig(SEventAnimationObjectTypeConfig *pConfig);
	
	CEventAnimationObjectType();
    ~CEventAnimationObjectType();
};


class CEventAnimationObject: public CAnimationObjectBase
{
    bool                        m_bFired;
    CEventAnimationObjectType  *m_pType;
public:

    bool ProcessFrame(IPhysicManager *pPhysicManager,unsigned int dwCurrentTime,double dInterval);
	
    CEventAnimationObject(CEventAnimationObjectType *pType,IAnimation *piAnimation);
};
