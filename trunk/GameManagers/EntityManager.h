#pragma once

class CEntityManager: virtual public CSystemObjectBase, virtual public IEntityManager, virtual public IGameManager
{
    vector<IEntity *>	 m_lNewEntities;
    list<IEntity *>	     m_lEntities;

    void RemoveEntity(IEntity *pEntity); 

public:

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

    // IEntityManager
    void AddEntity(IEntity *pEntity); 
    void RemoveAllEntities();
	unsigned long GetEntityCount();

	void RenderEntities(IGenericRender *piRender,IGenericCamera *piCamera);

    void PerformUnaryOperation(EntityUnaryOperation pFunc,void *pParam1,void *pParam2);
    void PerformBinaryOperation(EntityBinaryOperation pFunc,void *pParam1,void *pParam2);

    IEntity *FindEntity(string sName);

    CEntityManager();
    ~CEntityManager();
};