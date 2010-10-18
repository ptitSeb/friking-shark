#pragma once

class CSystemModule:virtual public CSystemUnknownBase,
                    virtual public ISystemModule
{
    std::string m_sPath;
    HMODULE     m_hModule;
    ISystem     *m_piSystem;
    bool        m_bRegistered;

    typedef bool (*tSystemModuleRegister)(ISystem *);
    typedef bool (*tSystemModuleUnregister)(ISystem *);

    tSystemModuleRegister    m_pSystemModuleRegister;
    tSystemModuleUnregister  m_pSystemModuleUnregister;
public:

    virtual bool Init(std::string sPath,ISystem *piSystem);
    virtual void Destroy();

    virtual std::string GetName();

    CSystemModule(void);
    ~CSystemModule(void);
};
