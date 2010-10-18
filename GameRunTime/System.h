#pragma once

#include <map>

class CSystem:public CSystemObjectBase,public ISystem
{
    ISystemManager  *m_piSystemManager;
    ISystem         *m_piSystem;

    std::map<std::string,ISystemObject*>     m_mObjects;
    std::map<std::string,ISystemClass*>      m_mClasses;
    std::map<std::string,ISystemModule*>     m_mModules;

public:

    // ISystemObject

    virtual bool Init(std::string sClass,std::string sName,ISystem *piSystem);
    virtual void Destroy();

    // ISystem

    virtual bool RegisterClassFactory(ISystemClass *piClass);
    virtual void UnregisterClassFactory(ISystemClass *piClass);

    virtual bool LoadModule(std::string sPath,ISystemModule **ppiModule);
    virtual bool GetModule(std::string sPath,ISystemModule **ppiModule);
    virtual bool RegisterModule(ISystemModule *piModule);
    virtual void UnregisterModule(ISystemModule *piModule);

    virtual bool RegisterObject(ISystemObject *piObject);
    virtual void UnregisterObject(ISystemObject *piObject);

    virtual bool GetObject(std::string sName,ISystemObject **piObject);
	virtual void GetObjects(std::vector<ISystemObject *> *pvObjects);

    virtual bool CreateObject(std::string sClass,ISystemObject **piObject);
    virtual bool CreateObject(std::string sClass,std::string sName,ISystemObject **piObject);

	virtual void DestroyAllObjects();

     CSystem(ISystemManager *piSystemManager);
    ~CSystem(void);
};
