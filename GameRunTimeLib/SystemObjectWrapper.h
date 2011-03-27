#pragma once

class CSystemObjectWrapper
{
protected:
    bool        m_bObjectMustBeDestroyed;
    bool        m_bAttached;

    virtual bool GetInterfaces(ISystemUnknown *piUnknown);
    virtual void ReleaseInterfaces();

	virtual bool UnserializeObject(ISystemPersistencyNode *piNode);
	
public:

    ISystemObject           *m_piObject;
    ISystemSerializable     *m_piSerializable;

    bool Load(ISystemPersistencyNode *pNode,std::string sName);
    bool Save(ISystemPersistencyNode *pNode,std::string sName);
    bool Remove(ISystemPersistencyNode *pNode,std::string sName);

    bool Create(std::string sSystem,std::string sClass,std::string sName);
    bool Create(ISystem *piSystem,std::string sClass,std::string sName);
    void Destroy();
    bool Attach(ISystemUnknown*);
    bool Attach(ISystem *piSystem,std::string sName);
    bool Attach(std::string sSystemName,std::string sName);
    void Detach(bool bDestroyIfMust=false);

    CSystemObjectWrapper &operator=(const CSystemObjectWrapper &);

            CSystemObjectWrapper(const CSystemObjectWrapper &);
            CSystemObjectWrapper();
    virtual ~CSystemObjectWrapper();
};


bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CSystemObjectWrapper> *pItem);
bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CSystemObjectWrapper> *pItem);
bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CSystemObjectWrapper> *pItem);
void MRPersistencyInitialize(CMRPersistentReferenceT<CSystemObjectWrapper> *pItem);
void MRPersistencyFree(CMRPersistentReferenceT<CSystemObjectWrapper> *pItem);


#define DECLARE_CUSTOM_WRAPPER1(WRAPPER_NAME,INTERFACE,VARNAME)\
class WRAPPER_NAME:public CSystemObjectWrapper\
{\
protected:\
    virtual bool GetInterfaces(ISystemUnknown *piUnknown)\
    {\
        bool bOk=CSystemObjectWrapper::GetInterfaces(piUnknown);\
        if(bOk)\
        {\
            if((VARNAME=QI(INTERFACE,m_piObject))==NULL){ReleaseInterfaces();bOk=false;}\
        }\
        return bOk;\
    }\
    virtual void ReleaseInterfaces(){CSystemObjectWrapper::ReleaseInterfaces();REL(VARNAME);}\
public:\
    WRAPPER_NAME &operator=(const WRAPPER_NAME &otherWrapper){Attach(static_cast<ISystemUnknown*>(otherWrapper.m_piObject));m_bAttached=otherWrapper.m_bAttached;return *this;}\
    WRAPPER_NAME(const WRAPPER_NAME &otherWrapper){VARNAME=NULL;if(otherWrapper.m_piObject){Attach(otherWrapper.m_piObject);m_bAttached=otherWrapper.m_bAttached;}}\
    WRAPPER_NAME(){VARNAME=NULL;}\
    ~WRAPPER_NAME(){REL(VARNAME);}\
    INTERFACE *VARNAME;\
};\
__UNUSED_ATTRIB__ static bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<WRAPPER_NAME> *pItem){return pItem->GetValueAddress()->Save(piNode,pItem->GetName())?true:false;}\
__UNUSED_ATTRIB__ static bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<WRAPPER_NAME> *pItem){return pItem->GetValueAddress()->Load(piNode,pItem->GetName())?true:false;}\
__UNUSED_ATTRIB__ static bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<WRAPPER_NAME> *pItem){if(piNode){piNode->DeleteNode(pItem->GetName());}return true;}\
__UNUSED_ATTRIB__ static void    MRPersistencyInitialize(CMRPersistentReferenceT<WRAPPER_NAME> *pItem){}\
__UNUSED_ATTRIB__ static void    MRPersistencyFree(CMRPersistentReferenceT<WRAPPER_NAME> *pItem){pItem->GetValueAddress()->Detach();}

#define DECLARE_CUSTOM_WRAPPER2(WRAPPER_NAME,INTERFACE1,VARNAME1,INTERFACE2,VARNAME2)\
class WRAPPER_NAME:public CSystemObjectWrapper\
{\
protected:\
	virtual bool GetInterfaces(ISystemUnknown *piUnknown)\
{\
	bool bOk=CSystemObjectWrapper::GetInterfaces(piUnknown);\
	if(bOk)\
	{\
		if((VARNAME1=QI(INTERFACE1,m_piObject))==NULL){ReleaseInterfaces();bOk=false;}\
	}\
	if(bOk)\
	{\
		if((VARNAME2=QI(INTERFACE2,m_piObject))==NULL){ReleaseInterfaces();bOk=false;}\
	}\
	return bOk;\
}\
	virtual void ReleaseInterfaces(){CSystemObjectWrapper::ReleaseInterfaces();REL(VARNAME1);REL(VARNAME2);}\
public:\
	WRAPPER_NAME &operator=(const WRAPPER_NAME &otherWrapper){Attach(static_cast<ISystemUnknown*>(otherWrapper.m_piObject));m_bAttached=otherWrapper.m_bAttached;return *this;}\
	WRAPPER_NAME(const WRAPPER_NAME &otherWrapper){VARNAME1=NULL;VARNAME2=NULL;if(otherWrapper.m_piObject){Attach(otherWrapper.m_piObject);m_bAttached=otherWrapper.m_bAttached;}}\
	WRAPPER_NAME(){VARNAME1=NULL;VARNAME2=NULL;}\
	~WRAPPER_NAME(){REL(VARNAME1);REL(VARNAME2);}\
	INTERFACE1 *VARNAME1;\
	INTERFACE2 *VARNAME2;\
};\
	__UNUSED_ATTRIB__ static bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<WRAPPER_NAME> *pItem){return pItem->GetValueAddress()->Save(piNode,pItem->GetName())?true:false;}\
	__UNUSED_ATTRIB__ static bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<WRAPPER_NAME> *pItem){return pItem->GetValueAddress()->Load(piNode,pItem->GetName())?true:false;}\
	__UNUSED_ATTRIB__ static bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<WRAPPER_NAME> *pItem){if(piNode){piNode->DeleteNode(pItem->GetName());}return true;}\
	__UNUSED_ATTRIB__ static void    MRPersistencyInitialize(CMRPersistentReferenceT<WRAPPER_NAME> *pItem){}\
	__UNUSED_ATTRIB__ static void    MRPersistencyFree(CMRPersistentReferenceT<WRAPPER_NAME> *pItem){pItem->GetValueAddress()->Detach();}

