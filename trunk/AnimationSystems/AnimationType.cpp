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


#include "./stdafx.h"
#include "AnimationSystems.h"
#include "AnimationType.h"

CAnimationType::CAnimationType(void)
{
    m_bLoop=false;
}
CAnimationType::~CAnimationType(void){}

IAnimation *CAnimationType::CreateInstance(IEntity *piEntity,unsigned int dwCurrentTime)
{
    unsigned x=0;

    CAnimation *pAnimation=new CAnimation(this,piEntity,dwCurrentTime);
    for(x=0;x<m_vObjects.size();x++)
    {
        IAnimationObject *piObject=m_vObjects[x].m_piObjectType->CreateInstance(pAnimation,dwCurrentTime);
        if(piObject){pAnimation->AddObject(piObject);}
    }

    return pAnimation;
}

bool CAnimationType::PrepareResources()
{
	bool bOk=true;
	for(unsigned x=0;x<m_vObjects.size();x++)
	{
		bOk=bOk && m_vObjects[x].m_piObjectType->PrepareResources();
	}
	return bOk;
}

void CAnimationType::DesignRender( IGenericRender *piRender,CVector &vPosition,CVector &vAngles,bool bSelected)
{
	for(unsigned x=0;x<m_vObjects.size();x++)
	{
		m_vObjects[x].m_piObjectType->DesignRender(piRender,vPosition,vAngles,bSelected);
	}
}

void CAnimationType::DesignGetBBox( CVector *pvMins,CVector *pvMaxs )
{
	CVector vFakeMins(1000,1000,1000),vFakeMaxs(-1000,-1000,-1000);
	CVector vMins(1000,1000,1000),vMaxs(-1000,-1000,-1000);
	for(unsigned x=0;x<m_vObjects.size();x++)
	{
		CVector vTempMins,vTempMaxs;
		m_vObjects[x].m_piObjectType->DesignGetBBox(&vTempMins,&vTempMaxs);
		for(unsigned int c=0;c<3;c++)
		{
			if(vTempMins.c[c]<vMins.c[c]){vMins.c[c]=vTempMins.c[c];}
			if(vTempMaxs.c[c]>vMaxs.c[c]){vMaxs.c[c]=vTempMaxs.c[c];}
		}
	}
	if(vFakeMins!=vMins && vFakeMaxs!=vMaxs)
	{
		if(pvMins){*pvMins=vMins;}
		if(pvMaxs){*pvMaxs=vMaxs;}
	}
}

double CAnimationType::DesignGetRadius()
{
	CVector vMins,vMaxs;
	DesignGetBBox(&vMins,&vMaxs);
	return GetBBoxRadius(vMins,vMaxs);
}

CTraceInfo CAnimationType::DesignGetTrace( const CVector &vPosition,const CVector &vAngles,const CVector &p1,const CVector &p2 )
{
	CTraceInfo info;
	info.m_dTraceFraction=1.0;
	info.m_vTracePos=p2;
	for(unsigned x=0;x<m_vObjects.size();x++)
	{
		CTraceInfo temp=m_vObjects[x].m_piObjectType->DesignGetTrace(vPosition,vAngles,p1,p2);    
		if(temp.m_dTraceFraction<info.m_dTraceFraction)
		{
			info=temp;
		}
	}
	return info;
}

// IAnimationTypeDesign

void CAnimationType::GetAnimationTypeConfig(SAnimationTypeConfig *pConfig)
{
	pConfig->bLoop=m_bLoop;
}

void CAnimationType::SetAnimationTypeConfig(SAnimationTypeConfig *pConfig)
{
	m_bLoop=pConfig->bLoop;
}

// Weapon management

unsigned int CAnimationType::AddObject(std::string sObjectType)
{
	CAnimationObjectTypeWrapper wrapper;
	int nIndex=OBJECT_INVALID;
	bool bOk=wrapper.Create(m_piSystem,sObjectType,"");
	if(bOk)
	{
		nIndex=m_vObjects.size();
		m_vObjects.push_back(wrapper);
	}
	return nIndex;
}

bool CAnimationType::RemoveObject(unsigned int nObject)
{
	if(nObject>=m_vObjects.size()){return false;}
	unsigned int x=0;
	vector<CAnimationObjectTypeWrapper>::iterator i;
	for(x=0,i=m_vObjects.begin();x<m_vObjects.size();x++,i++)
	{
		if(x==nObject){m_vObjects.erase(i);return true;}
	}
	return false;
}

bool CAnimationType::GetObject(unsigned int nObject,IAnimationObjectType **ppiObject)
{
	*ppiObject=NULL;
	if(nObject>=m_vObjects.size()){return false;}
	*ppiObject=ADD(m_vObjects[nObject].m_piObjectType);
	return true;
}

unsigned int CAnimationType::GetObjectCount(){return m_vObjects.size();}


CAnimation::CAnimation(CAnimationType *pType,IEntity *piEntity,unsigned int dwCurrentTimeBase)
{
    m_bActive=false;
    m_bFinished=true;
    m_pType=pType;
    m_piEntity=piEntity;
    m_dwCurrentTimeBase=dwCurrentTimeBase;
}

CAnimation::~CAnimation()
{
    unsigned x;
    for(x=0;x<m_vObjects.size();x++){delete m_vObjects[x];}
}

void CAnimation::AddObject(IAnimationObject *piAnimationObject){m_vObjects.push_back(piAnimationObject);}

IAnimationObject *CAnimation::GetObject(string sName)
{
    for(unsigned x=0;x<m_vObjects.size();x++)
    {
        if(m_vObjects[x]->GetName()==sName){return m_vObjects[x];}
    }
    return NULL;
}

IEntity *CAnimation::GetEntity(){return m_piEntity;}

void CAnimation::Activate(unsigned int dwCurrentTime)
{
    m_bActive=true;
    m_bFinished=false;
    m_dwCurrentTimeBase=dwCurrentTime;
    for(unsigned x=0;x<m_vObjects.size();x++)
    {
        m_vObjects[x]->Activate(dwCurrentTime);
    }
 }

bool CAnimation::IsActive(){return m_bActive;}

void CAnimation::Deactivate()
{
    for(unsigned x=0;x<m_vObjects.size();x++)
    {
        m_vObjects[x]->Deactivate();
    }
    m_bActive=false;
}

bool CAnimation::HasFinished()
{
    return m_bFinished;
}

unsigned int CAnimation::GetCurrentTimeBase()
{
    return m_dwCurrentTimeBase;
}

bool CAnimation::ProcessFrame(IPhysicManager *piPhysicManager,unsigned int dwCurrentTime,double dInterval)
{
    bool bAllObjectsFinished=true;
    for(unsigned x=0;x<m_vObjects.size();x++)
    {
        if(m_vObjects[x]->ProcessFrame(piPhysicManager,dwCurrentTime,dInterval))
        {
            bAllObjectsFinished=false;
        }
    }
    m_bFinished=/*!m_bActive && */bAllObjectsFinished;
    return !m_bFinished;
}

void CAnimation::CustomRender(IGenericRender *piRender,IGenericCamera *piCamera)
{
  for(unsigned x=0;x<m_vObjects.size();x++)
  {
    m_vObjects[x]->CustomRender(piRender,piCamera);    
  }
}

CTraceInfo CAnimation::GetTrace( const CVector &vOrigin,const CVector &vAngles,const CVector &p1,const CVector &p2 )
{
	CTraceInfo info;
	info.m_dTraceFraction=1.0;
	info.m_vTracePos=p2;
	for(unsigned x=0;x<m_vObjects.size();x++)
	{
		CTraceInfo temp=m_vObjects[x]->GetTrace(vOrigin,vAngles,p1,p2);    
		if(temp.m_dTraceFraction<info.m_dTraceFraction)
		{
			info=temp;
		}
	}
	return info;
}
