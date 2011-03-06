#pragma once

#include "GameGraphics.h"

DECLARE_CUSTOM_WRAPPER1(CGameControllerWrapper,IGameController,m_piGameController)
DECLARE_CUSTOM_WRAPPER1(CEntityManagerWrapper,IEntityManager,m_piEntityManager)

class CSoundSystemManager:virtual public CSystemObjectBase, virtual public ISoundManager, virtual public IGameManager
{
	CGameControllerWrapper  m_GameControllerWrapper;
	CEntityManagerWrapper   m_EntityManagerWrapper;

	unsigned int m_nMasterVolume;// 0,100
	bool m_bEnable3DSound;
	IEntity *m_piPlayerEntity;

	ALCcontext *m_pContext;
	ALCdevice *m_pDevice;


public:
  // IGameManager

	void CreateScenario();
	void LoadScenario(ISystemPersistencyNode *piNode);
	void SaveScenario(ISystemPersistencyNode *piNode);
	void CloseScenario();

	void Start();
	void Stop();
	void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);

	bool Is3DSoundEnabled();

	unsigned int  GetMasterVolume();
	void SetMasterVolume(unsigned int dVolume);

	BEGIN_PROP_MAP(CSoundSystemManager)
		PROP_VALUE_FLAGS(m_nMasterVolume,"Volume",100,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_bEnable3DSound,"3DSound",false,MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP();

	bool Init(std::string sClass,std::string sName,ISystem *piSystem);
	void Destroy();

	CSoundSystemManager();
	virtual ~CSoundSystemManager();
};
