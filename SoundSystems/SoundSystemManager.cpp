#include "./stdafx.h"
#include "GameRunTimeLib.h"
#include "SoundSystems.h"
#include "SoundSystemManager.h"

CSoundSystemManager::CSoundSystemManager()
{
  m_dMasterVolume=100;
  m_bEnable3DSound=false;
  m_piPlayerEntity=NULL;
  m_pContext = NULL;
  m_pDevice = NULL;
}

CSoundSystemManager::~CSoundSystemManager()
{

}


bool CSoundSystemManager::Init(std::string sClass,std::string sName,ISystem *piSystem)
{
  bool bOk=CSystemObjectBase::Init(sClass,sName,piSystem);;
  if(bOk){bOk=m_GameControllerWrapper.Attach("GameSystem","GameController");}
  if(bOk){m_GameControllerWrapper.m_piGameController->RegisterManager(500,this);}
  
  if(bOk) {m_pDevice=alcOpenDevice(NULL);bOk=(m_pDevice!=NULL);}
  if(bOk) {m_pContext=alcCreateContext(m_pDevice,NULL);bOk=(m_pContext!=NULL);}
  if(bOk) {alcMakeContextCurrent(m_pContext);}
  if(bOk) {alutInitWithoutContext(NULL,NULL);}
  if(!bOk)
  {
	  RTTRACE("CSoundSystemManager::Init -> Failed to create sound context %d",alGetError());
  }
  return bOk;
}


void CSoundSystemManager::Destroy()
{
  if(m_pContext){alutExit();}
  if(m_pContext){alcMakeContextCurrent(NULL);alcDestroyContext(m_pContext);m_pContext=NULL;}
  if(m_pDevice){alcCloseDevice(m_pDevice);m_pDevice=NULL;}
  if(m_GameControllerWrapper.m_piGameController){m_GameControllerWrapper.m_piGameController->UnregisterManager(this);}
  m_GameControllerWrapper.Detach();
  CSystemObjectBase::Destroy();
}

void CSoundSystemManager::CreateScenario(){}
void CSoundSystemManager::LoadScenario(ISystemPersistencyNode *piNode){}
void CSoundSystemManager::SaveScenario(ISystemPersistencyNode *piNode){}
void CSoundSystemManager::CloseScenario(){}

void CSoundSystemManager::Start()
{
  m_EntityManagerWrapper.Attach("GameSystem","EntityManager");
}

void CSoundSystemManager::Stop()
{
  m_EntityManagerWrapper.Detach();
  m_piPlayerEntity=NULL;
}
void CSoundSystemManager::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
  alListenerf(AL_GAIN ,(m_dMasterVolume/100.0));
  if(!Is3DSoundEnabled()){return;}

  if(m_EntityManagerWrapper.m_piEntityManager && m_piPlayerEntity==NULL)
  {
    m_piPlayerEntity=m_EntityManagerWrapper.m_piEntityManager->FindEntity("Player");
  }

  if(m_piPlayerEntity )
  {
      SPhysicInfo *pPhysicInfo=m_piPlayerEntity->GetPhysicInfo();
	  alListener3f(AL_POSITION,(float)pPhysicInfo->vPosition.c[0],(float)pPhysicInfo->vPosition.c[1],(float)pPhysicInfo->vPosition.c[2]);
	  alListener3f(AL_VELOCITY,(float)pPhysicInfo->vVelocity.c[0],(float)pPhysicInfo->vVelocity.c[1],(float)pPhysicInfo->vVelocity.c[2]);
	  alListener3f(AL_ORIENTATION,(float)pPhysicInfo->vForward.c[0],(float)pPhysicInfo->vForward.c[1],(float)pPhysicInfo->vForward.c[2]);
 }
}

int  CSoundSystemManager::GetMasterVolume(){return m_dMasterVolume;}
void CSoundSystemManager::SetMasterVolume(int dVolume){m_dMasterVolume=dVolume;NOTIFY_EVENT(ISoundManagerEvents,OnMasterPanChanged(m_dMasterVolume));}
bool CSoundSystemManager::Is3DSoundEnabled(){return m_bEnable3DSound;}

