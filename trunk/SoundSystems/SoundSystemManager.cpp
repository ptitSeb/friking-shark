#include "StdAfx.h"
#include "GameRuntimeLib.h"
#include ".\SoundSystems.h"
#include ".\SoundSystemManager.h"

CSoundSystemManager::CSoundSystemManager()
{
  m_dMasterPan=0;
  m_dMasterVolume=100;
  m_bEnable3DSound=false;
  m_piPlayerEntity=NULL;
  m_piListener=NULL;
}

CSoundSystemManager::~CSoundSystemManager()
{

}

bool CSoundSystemManager::Init(std::string sClass,std::string sName,ISystem *piSystem)
{
  bool bOk=CSystemObjectBase::Init(sClass,sName,piSystem);;
  if(bOk){bOk=m_ViewportWrapper.Attach("GameGUI","Viewport");}
  if(bOk){bOk=m_GameControllerWrapper.Attach("GameSystem","GameController");}
  if(bOk){m_GameControllerWrapper.m_piGameController->RegisterManager(500,this);}
  if(bOk)
  {
    m_dxSoundManager.Initialize(m_ViewportWrapper.m_piGenericViewport->GetWindowHandle(),DSSCL_NORMAL);
    HRESULT hr=m_dxSoundManager.Get3DListenerInterface(&m_piListener);
  }
  return bOk;
}


void CSoundSystemManager::Destroy()
{
  if(m_GameControllerWrapper.m_piGameController){m_GameControllerWrapper.m_piGameController->UnregisterManager(this);}
  m_ViewportWrapper.Detach();
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
void CSoundSystemManager::ProcessFrame(DWORD dwCurrentTime,double dTimeFraction)
{
  if(!Is3DSoundEnabled()){return;}

  if(m_EntityManagerWrapper.m_piEntityManager && m_piPlayerEntity==NULL)
  {
    m_piPlayerEntity=m_EntityManagerWrapper.m_piEntityManager->FindEntity("Player");
  }

  if(m_piPlayerEntity )
  {
    if(m_piListener)
    {
      SPhysicInfo *pPhysicInfo=m_piPlayerEntity->GetPhysicInfo();
      DS3DLISTENER listener={0};
      listener.dwSize=sizeof(listener);
      m_piListener->GetAllParameters(&listener);

      CVector vForward(1,0,0),vUp(0,1,0);
      //VectorsFromAngles(pPhysicInfo->vAngles,&vForward,NULL,&vUp);
      HRESULT hr=S_OK;
      vUp=Origin-vUp;
      hr=m_piListener->SetOrientation((D3DVALUE)vForward.c[0],(D3DVALUE)vForward.c[1],(D3DVALUE)vForward.c[2],(D3DVALUE)vUp.c[0],(D3DVALUE)vUp.c[1],(D3DVALUE)vUp.c[2],DS3D_DEFERRED);
      hr=m_piListener->SetPosition((D3DVALUE)pPhysicInfo->vPosition.c[0],(D3DVALUE)pPhysicInfo->vPosition.c[1],0,DS3D_DEFERRED);
      hr=m_piListener->CommitDeferredSettings();
    }
  }
}

int  CSoundSystemManager::GetMasterPan(){return m_dMasterPan;}
void CSoundSystemManager::SetMasterPan(int dPan){m_dMasterPan=dPan;NOTIFY_EVENT(ISoundManagerEvents,OnMasterPanChanged(m_dMasterPan));}
int  CSoundSystemManager::GetMasterVolume(){return m_dMasterVolume;}
void CSoundSystemManager::SetMasterVolume(int dVolume){m_dMasterVolume=dVolume;NOTIFY_EVENT(ISoundManagerEvents,OnMasterPanChanged(m_dMasterVolume));}
CSoundManager *CSoundSystemManager::GetSoundManager(){return &m_dxSoundManager;}
bool CSoundSystemManager::Is3DSoundEnabled(){return m_bEnable3DSound;}

