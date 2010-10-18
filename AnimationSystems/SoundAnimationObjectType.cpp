#include "StdAfx.h"
#include ".\AnimationSystems.h"
#include ".\AnimationObjectBase.h"
#include ".\SoundSystems.h"
#include ".\SoundAnimationObjecttype.h"

CSoundAnimationObjectType::CSoundAnimationObjectType()
{
  m_piSoundType=NULL;
  m_dwStartTime=0;
  m_dwEndTime=0;
}

CSoundAnimationObjectType::~CSoundAnimationObjectType()
{
    REL(m_piSoundType);
}

IAnimationObject *CSoundAnimationObjectType::CreateInstance(IAnimation *piAnimation,DWORD dwCurrentTime)
{
    if(m_piSoundType==NULL)
    {
        ISystemManager  *piSystemManager=GetSystemManager();
        ISystem         *piSystem=NULL;
        ISystemObject   *piObject=NULL;
        if(piSystemManager){piSystem=piSystemManager->GetSystem("Animations");}
        if(piSystem){piSystem->GetObject(m_sSoundType,&piObject);}
        if(piObject){m_piSoundType=QI(ISoundType,piObject);}
        REL(piSystemManager);
        REL(piSystem);
        REL(piObject);
    }
    CSoundAnimationObject *pSound=new CSoundAnimationObject(this,piAnimation);
    return pSound;
}

CSoundAnimationObject::CSoundAnimationObject(CSoundAnimationObjectType *pType,IAnimation *piAnimation)
:CAnimationObjectBase(pType,piAnimation)
{
    m_piSound=NULL;
    m_pType=pType;
}
CSoundAnimationObject::~CSoundAnimationObject()
{
    if(m_piSound){delete m_piSound;m_piSound=NULL;}
}

void CSoundAnimationObject::Activate(DWORD dwCurrentTime)
{
    if(m_piSound){delete m_piSound;m_piSound=NULL;}
    CAnimationObjectBase::Activate(dwCurrentTime);
    if(m_pType->m_piSoundType)
    {
      m_piSound=m_pType->m_piSoundType->CreateInstance(m_piAnimation->GetEntity(),dwCurrentTime);
      if(m_piSound){m_piSound->Activate(dwCurrentTime);}
    }
}

void CSoundAnimationObject::Deactivate()
{
    if(m_piSound){m_piSound->Deactivate();}
    CAnimationObjectBase::Deactivate();
}

void CSoundAnimationObject::CustomRender(IGenericRender *piRender,IGenericCamera *piCamera)
{
}

bool CSoundAnimationObject::ProcessFrame(IPhysicManager *pPhysicManager,DWORD dwCurrentTime,double dInterval)
{
    if(m_piSound)
    {
      m_piSound->ProcessFrame(pPhysicManager,dwCurrentTime,dInterval);
      if(!m_piSound->HasFinished())
      {
        return true;
      }
      else
      {
        m_piSound->Deactivate();
      }
    }
    return false;
}

