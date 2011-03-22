#pragma once

struct SSystemLoaderHelperModule
{
    std::string sPath;
};

BEGIN_STRUCT_PROPS(SSystemLoaderHelperModule)
    PROP(sPath,"Path")
END_STRUCT_PROPS()

struct CSystemLoaderHelperModuleList
{
    std::deque<SSystemLoaderHelperModule> m_dModules;

    BEGIN_PROP_MAP(CSystemLoaderHelperModuleList)
        PROP(m_dModules,"Modules")
    END_PROP_MAP()
};


class CDelayedUnserializeObjectWrapper: public CSystemObjectWrapper
{
protected:
	ISystemPersistencyNode	*m_piNode;
	
	bool UnserializeObject(ISystemPersistencyNode *piNode);
	
public:
	
	bool UnserializeNow();
	
	CDelayedUnserializeObjectWrapper();
	~CDelayedUnserializeObjectWrapper();
	
};

bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CDelayedUnserializeObjectWrapper> *pItem);
bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CDelayedUnserializeObjectWrapper> *pItem);
bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CDelayedUnserializeObjectWrapper> *pItem);
void MRPersistencyInitialize(CMRPersistentReferenceT<CDelayedUnserializeObjectWrapper> *pItem);
void MRPersistencyFree(CMRPersistentReferenceT<CDelayedUnserializeObjectWrapper> *pItem);

class CSystemLoaderHelper
{
    CSystemLoaderHelperModuleList    m_Modules;
	std::deque<CDelayedUnserializeObjectWrapper> m_dObjects;
	
	
public:

    ISystem *LoadSystem(ISystemPersistencyNode *piNode,std::string sSystemName);
	ISystem *LoadSystem(std::string sFile,std::string sSystemName);
	
    BEGIN_PROP_MAP(CSystemLoaderHelper)
        PROP(m_dObjects,"Objects")
    END_PROP_MAP()
    
    CSystemLoaderHelper(void);
    virtual ~CSystemLoaderHelper(void);
};

class CSystemSaverHelper
{
	std::deque<SSystemLoaderHelperModule> m_dModules;
	std::deque<CSystemObjectWrapper> m_dObjects;
	
public:
	
	bool SaveSystemNamedObjects(std::string sFile,std::string sSystemName);
	bool SaveSystemNamedObjects(ISystemPersistencyNode *piNode,std::string sSystemName);
	
	BEGIN_PROP_MAP(CSystemSaverHelper)
		PROP_FLAGS(m_dModules,"Modules",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_dObjects,"Objects",MRPF_WRITE)
	END_PROP_MAP()
	
	CSystemSaverHelper(void);
	~CSystemSaverHelper(void);
};
