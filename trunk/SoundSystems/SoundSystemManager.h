#pragma once

#include "GameGraphics.h"

DECLARE_CUSTOM_WRAPPER1(CGameViewportWrapper,IGenericViewport,m_piGenericViewport);
DECLARE_CUSTOM_WRAPPER1(CGameControllerWrapper,IGameController,m_piGameController);
DECLARE_CUSTOM_WRAPPER1(CEntityManagerWrapper,IEntityManager,m_piEntityManager);

struct ISoundManager;

struct ISoundManager : virtual public ISystemUnknown
{
  virtual CSoundManager *GetSoundManager()=0;
  virtual bool Is3DSoundEnabled()=0;

  virtual int  GetMasterPan()=0;
  virtual void SetMasterPan(int dPan)=0;
  virtual int  GetMasterVolume()=0;
  virtual void SetMasterVolume(int dVolume)=0;

  virtual ~ISoundManager(){}
};

struct ISoundManagerEvents : virtual public ISystemUnknown
{
  virtual void OnMasterPanChanged(int dMasterPan)=0;
  virtual void OnMasterVolumeChanged(int dMasterVolume)=0;

  virtual ~ISoundManagerEvents(){}
};

// No se puede llamar CSoundManager porque colisiona con el helper de DirectSound
class CSoundSystemManager:virtual public CSystemObjectBase, virtual public ISoundManager, virtual public IGameManager
{
  CGameViewportWrapper    m_ViewportWrapper;
  CGameControllerWrapper  m_GameControllerWrapper;
  CEntityManagerWrapper   m_EntityManagerWrapper;
  IDirectSound3DListener *m_piListener;

  CSoundManager m_dxSoundManager;

  int m_dMasterPan; //-100,100
  int m_dMasterVolume;// 0,100

  bool m_bEnable3DSound;
  IEntity *m_piPlayerEntity;

public:
  // IGameManager

	void CreateScenario();
	void LoadScenario(ISystemPersistencyNode *piNode);
	void SaveScenario(ISystemPersistencyNode *piNode);
	void CloseScenario();

	void Start();
	void Stop();
	void ProcessFrame(DWORD dwCurrentTime,double dTimeFraction);

	CSoundManager *GetSoundManager();
	bool           Is3DSoundEnabled();

	int  GetMasterPan();
	void SetMasterPan(int dPan);
	int  GetMasterVolume();
	void SetMasterVolume(int dVolume);

	BEGIN_PROP_MAP(CSoundSystemManager)
		PROP_VALUE_FLAGS(m_dMasterPan,"PosicionEstereoMastra",0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dMasterVolume,"VolumenMaestro",100,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_bEnable3DSound,"Sonidos3D",false,MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP();

	bool Init(std::string sClass,std::string sName,ISystem *piSystem);
	void Destroy();

	CSoundSystemManager();
	virtual ~CSoundSystemManager();
};
