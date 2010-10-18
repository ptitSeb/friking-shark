// Globals.cpp: implementation of the Globals class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GameEngine.h"
#include "GUIManager.h"
#include "Globals.h"

class CFrameManager : virtual public CSystemObjectBase, virtual public IFrameManager
{
public:
  virtual DWORD  GetCurrentRealTime(){return CGlobals::g_dwCurrentRealTime;}
  virtual DWORD  GetCurrentTime(){return CGlobals::g_dwCurrentTime;}
  virtual double GetTimeFraction(){return CGlobals::g_dTimeFraction;}
  CFrameManager::CFrameManager(){m_sName="FrameManager";}
};

IPlayAreaManager	*CGlobals::g_piPlayAreaManager=NULL;
IEntityManager		*CGlobals::g_pEntityManager=NULL;
CGUIManager			*CGlobals::g_pGUIManager=NULL;
IRender				*CGlobals::g_pRender=NULL;
IResourceStore		*CGlobals::g_pResourceStore=NULL;
IEntity				*CGlobals::g_piPlayerEntity=NULL;
IPlayer       *CGlobals::g_piPlayer=NULL;
CGameScreen			*CGlobals::g_pScreen=NULL;
DWORD				 CGlobals::g_dwCurrentTime=0;
DWORD				 CGlobals::g_dwLastTime=0;
DWORD				 CGlobals::g_dwCurrentRealTime=0;
DWORD				 CGlobals::g_dwLastRealTime=0;
double				 CGlobals::g_dTimeFraction=0;
double				 CGlobals::g_dRealTimeFraction=0;
CLog				*CGlobals::g_pLog=NULL;
ICamera			    *CGlobals::g_pPlayCamera=NULL;
ICamera			    *CGlobals::g_pInspectionCamera=NULL;
ISystem             *CGlobals::g_piParticleSystemLibrary=NULL;
ISystem             *CGlobals::g_piAnimationLibrary=NULL;
ISystemManager      *CGlobals::g_piSystemManager=NULL;
ISystem             *CGlobals::g_piMainSystem=NULL;
CFrameManager                  *g_pFrameManager=NULL;

CIniFile		 CGlobals::g_IniFile;

bool CGlobals::Init()
{
  g_IniFile.Open("config.ini");

  g_pLog				      =new CLog;
  g_pFrameManager     =new CFrameManager;
  CRender *pRender=new CRender;
  g_pRender           =pRender;
  CResourceStore *pResourceStore = new CResourceStore();
  g_pResourceStore	  =pResourceStore;
  g_pGUIManager		    =new CGUIManager();
  g_pPlayCamera		    =new CGameCamera;
  g_pInspectionCamera	=new CGameCamera;
  g_pScreen           =new CGameScreen;
  g_piSystemManager   =GetSystemManager();
  g_piMainSystem      =g_piSystemManager->CreateSystem("MainSystem");
  g_piMainSystem->RegisterObject(pResourceStore);
  g_piMainSystem->RegisterObject(pRender);
  g_piMainSystem->RegisterObject(g_pFrameManager);
  g_piMainSystem->RegisterObject(g_pGUIManager);
  g_piMainSystem->RegisterObject(g_pScreen);

  return true;
}

void CGlobals::Destroy()
{
  if(g_piMainSystem){CGlobals::g_piMainSystem->Destroy();REL(CGlobals::g_piMainSystem);}
  REL(g_pGUIManager);
  REL(g_pRender);
  if(g_pResourceStore){delete g_pResourceStore;g_pResourceStore=NULL;}
  if(g_pInspectionCamera){delete g_pInspectionCamera;g_pInspectionCamera=NULL;}
  if(g_pPlayCamera){delete g_pPlayCamera;g_pPlayCamera=NULL;}
  REL(g_pFrameManager);
  REL(CGlobals::g_piSystemManager);
  REL(g_pScreen);
  if(g_pLog){delete g_pLog;g_pLog=NULL;}
  g_IniFile.Close();
}

void LOG(char *format, ...)
{
  va_list vargs;
  va_start (vargs,format);
  char pTempBuffer[2400];
  int res=_vsnprintf(pTempBuffer,1023,format,vargs);
  if(res==-1){pTempBuffer[1022]=0;res=1022;}
  va_end (vargs);

  pTempBuffer[res]=0;
  CGlobals::g_pLog->Log(pTempBuffer);
}

void LOG_ERROR_AND_EXIT(char *pCaption,char *format, ...)
{
  va_list vargs;
  va_start (vargs,format);
  char pTempBuffer[2400];
  int res=_vsnprintf(pTempBuffer,1023,format,vargs);
  if(res==-1){pTempBuffer[1022]=0;res=1022;}
  va_end (vargs);

  pTempBuffer[res]=0;
  CGlobals::g_pLog->LogErrorAndExit(pCaption,pTempBuffer);
}
