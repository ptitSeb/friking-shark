#pragma once

class CGameController:virtual public CSystemObjectBase, virtual public IGameController
{
    map<DWORD,IGameManager *> m_mManagers;

	ISystem *m_piResourcesSystem;
	ISystem *m_piParticleSystemTypesSystem;
	ISystem *m_piAnimationTypesSystem;
	ISystem *m_piEntityTypesSystem;
	ISystem *m_piFormationTypesSystem;
	ISystem *m_piWeaponTypesSystem;

public:

    bool Init(std::string sClass,std::string sName,ISystem *piSystem);
    void Destroy();

    // IGameController

	bool SetupGame();
	void EndGame();

	void CreateScenario();
	bool LoadScenario(std::string sScenario);
	bool SaveScenario(std::string sScenario);
	void CloseScenario();

    void Start();
    void Stop();
    void ProcessFrame(DWORD dwCurrentTime,double dTimeFraction);

    virtual bool RegisterManager(DWORD dwHeight,IGameManager *piManager);
    virtual void UnregisterManager(IGameManager *piManager);

    CGameController();
    virtual ~CGameController();
};
