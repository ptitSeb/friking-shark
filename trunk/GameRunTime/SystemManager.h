#pragma once

#include "GameRunTime.h"
#include "GameRunTimeLib.h"
#include <map>

class CSystemManager:   virtual public CSystemUnknownBase,
                        public CPublisherBase,
                        public ISystemManager,
                        public ISystemManagerPathControl
{
    std::map<std::string,ISystem*> m_mSystems;
    std::set<std::string>          m_sLibraryPaths;
	std::map<ISystemObject *,std::string>       m_mDebugObjects;

public:

  // ISystemManager

    ISystem *CreateSystem(std::string sName);
    void    *DestroySystem(ISystem *piSystem);

    ISystem *GetSystem(std::string sName);

    bool    RegisterSystem(ISystem *piSystem);
    void    UnregisterSystem(ISystem *piSystem);
	
	void			DebugRegisterObject(ISystemObject *piObject);
	void			DebugUnregisterObject(ISystemObject *piObject);
	unsigned long	DebugGetRegisteredObjectCount();
	void			DebugGetRegisteredObjects(std::map<ISystemObject *,std::string> *pObjects);

    // ISystemManagerPathControl

    void AddLibraryPath(std::string sPath);
    void RemoveLibraryPath(std::string sPath);
    void ClearLibraryPaths();
    void GetLibraryPaths(std::set<std::string> *pvPaths);

    CSystemManager(void);
    ~CSystemManager(void);

    static CSystemManager *g_pSystemManager;
};

