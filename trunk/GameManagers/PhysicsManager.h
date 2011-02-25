#pragma once


class CPhysicManager:virtual public CSystemObjectBase, virtual public IPhysicManager, virtual public IGameManager
{
    vector<SPhysicForce> m_vGlobalForces;
    void ApplyForce(SPhysicInfo *pInfo,SPhysicForce *pForce,double dInterval);

public:

    CEntityManagerWrapper   m_EntityManagerWrapper;
    CGameControllerWrapper  m_GameControllerWrapper;

    bool Init(std::string sClass,std::string sName,ISystem *piSystem);
    void Destroy();

    // IGameManager
	void CreateScenario();
	void LoadScenario(ISystemPersistencyNode *piNode);
    void SaveScenario(ISystemPersistencyNode *piNode);
    void CloseScenario();

    void Start();
    void Stop();
    void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);

    // IPhysicManager
    CVector ProcessPhysicInfo(SPhysicInfo *pInfo,double dInterval);

    CPhysicManager();
    virtual ~CPhysicManager();
};
