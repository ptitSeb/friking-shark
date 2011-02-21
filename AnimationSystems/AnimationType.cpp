#include "./stdafx.h"
#include "AnimationSystems.h"
#include "AnimationType.h"

CAnimationType::CAnimationType(void)
{
    m_bCyclic=false;
}
CAnimationType::~CAnimationType(void){}

IAnimation *CAnimationType::CreateInstance(IEntity *piEntity,DWORD dwCurrentTime)
{
    unsigned x=0;

    CAnimation *pAnimation=new CAnimation(this,piEntity,dwCurrentTime);
    for(x=0;x<m_dObjects.size();x++)
    {
        IAnimationObject *piObject=m_dObjects[x].m_piObjectType->CreateInstance(pAnimation,dwCurrentTime);
        if(piObject){pAnimation->AddObject(piObject);}
    }

    return pAnimation;
}

void CAnimationType::DesignRender( IGenericRender *piRender,CVector &vPosition,CVector &vAngles,bool bSelected)
{
	for(unsigned x=0;x<m_dObjects.size();x++)
	{
		m_dObjects[x].m_piObjectType->DesignRender(piRender,vPosition,vAngles,bSelected);
	}
}

void CAnimationType::DesignGetBBox( CVector *pvMins,CVector *pvMaxs )
{
	CVector vFakeMins(1000,1000,1000),vFakeMaxs(-1000,-1000,-1000);
	CVector vMins(1000,1000,1000),vMaxs(-1000,-1000,-1000);
	for(unsigned x=0;x<m_dObjects.size();x++)
	{
		CVector vTempMins,vTempMaxs;
		m_dObjects[x].m_piObjectType->DesignGetBBox(&vTempMins,&vTempMaxs);
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
	for(unsigned x=0;x<m_dObjects.size();x++)
	{
		CTraceInfo temp=m_dObjects[x].m_piObjectType->DesignGetTrace(vPosition,vAngles,p1,p2);    
		if(temp.m_dTraceFraction<info.m_dTraceFraction)
		{
			info=temp;
		}
	}
	return info;
}

CAnimation::CAnimation(CAnimationType *pType,IEntity *piEntity,DWORD dwCurrentTimeBase)
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
    for(x=0;x<m_dObjects.size();x++){delete m_dObjects[x];}
}

void CAnimation::AddObject(IAnimationObject *piAnimationObject){m_dObjects.push_back(piAnimationObject);}

IAnimationObject *CAnimation::GetObject(string sName)
{
    for(unsigned x=0;x<m_dObjects.size();x++)
    {
        if(m_dObjects[x]->GetName()==sName){return m_dObjects[x];}
    }
    return NULL;
}

IEntity *CAnimation::GetEntity(){return m_piEntity;}

void CAnimation::Activate(DWORD dwCurrentTime)
{
    m_bActive=true;
    m_bFinished=false;
    m_dwCurrentTimeBase=dwCurrentTime;
    for(unsigned x=0;x<m_dObjects.size();x++)
    {
        m_dObjects[x]->Activate(dwCurrentTime);
    }
 }

bool CAnimation::IsActive(){return m_bActive;}

void CAnimation::Deactivate()
{
    for(unsigned x=0;x<m_dObjects.size();x++)
    {
        m_dObjects[x]->Deactivate();
    }
    m_bActive=false;
}

bool CAnimation::HasFinished()
{
    return m_bFinished;
}

DWORD CAnimation::GetCurrentTimeBase()
{
    return m_dwCurrentTimeBase;
}

bool CAnimation::ProcessFrame(IPhysicManager *piPhysicManager,DWORD dwCurrentTime,double dInterval)
{
    bool bAllObjectsFinished=true;
    for(unsigned x=0;x<m_dObjects.size();x++)
    {
        if(m_dObjects[x]->ProcessFrame(piPhysicManager,dwCurrentTime,dInterval))
        {
            bAllObjectsFinished=false;
        }
    }
    m_bFinished=!m_bActive && bAllObjectsFinished;
    return !m_bFinished;
}

void CAnimation::CustomRender(IGenericRender *piRender,IGenericCamera *piCamera)
{
  for(unsigned x=0;x<m_dObjects.size();x++)
  {
    m_dObjects[x]->CustomRender(piRender,piCamera);    
  }
}

CTraceInfo CAnimation::GetTrace( const CVector &vOrigin,const CVector &vAngles,const CVector &p1,const CVector &p2 )
{
	CTraceInfo info;
	info.m_dTraceFraction=1.0;
	info.m_vTracePos=p2;
	for(unsigned x=0;x<m_dObjects.size();x++)
	{
		CTraceInfo temp=m_dObjects[x]->GetTrace(vOrigin,vAngles,p1,p2);    
		if(temp.m_dTraceFraction<info.m_dTraceFraction)
		{
			info=temp;
		}
	}
	return info;
}
