#pragma once

#define MRPF_READ					0x0001
#define MRPF_WRITE					0x0002
#define MRPF_OPTIONAL				0x0004
#define MRPF_NORMAL					MRPF_READ|MRPF_WRITE

class IMRPersistencyItem
{
public:

    virtual void Initialize()=0;
    virtual void Free()=0;
    virtual void SetDefaultValue()=0;

    virtual HRESULT Load(ISystemPersistencyNode *)=0;
    virtual HRESULT Save(ISystemPersistencyNode *)=0;
    virtual HRESULT Remove(ISystemPersistencyNode *)=0;

    virtual char *GetName()=0;
};


template<typename T1> 
class CMRPersistentReferenceT:public IMRPersistencyItem
{
protected:

    DWORD		m_dwFlags;

    T1			*m_pValue;
    char		m_sName[200];

public:

    char *GetName(){return m_sName;}
    T1	 *GetValueAddress(){return m_pValue;}
    void SetDefaultValue(){}

    CMRPersistentReferenceT(T1 *pValue,const char *pName,DWORD flags)
    {
        m_dwFlags=flags;
        m_pValue=pValue;
        strcpy(m_sName,pName);
    }
    virtual ~CMRPersistentReferenceT(){}
};


template<typename T1> 
class CMRPersistentSimpleReferenceT:public CMRPersistentReferenceT<T1>
{
public:

    void	Initialize(){MRPersistencyInitialize(this);}
    void	Free(){MRPersistencyFree(this);}
    void	SetDefaultValue(){}
    HRESULT Load(ISystemPersistencyNode *piNode){HRESULT hr=S_OK;if(m_dwFlags&MRPF_READ){hr=MRPersistencyLoad(piNode,this);}if(m_dwFlags&MRPF_OPTIONAL){return S_OK;}return hr;}
    HRESULT Save(ISystemPersistencyNode *piNode){HRESULT hr=S_OK;if(m_dwFlags&MRPF_WRITE){hr=MRPersistencySave(piNode,this);}if(m_dwFlags&MRPF_OPTIONAL){return S_OK;}return hr;}
    HRESULT Remove(ISystemPersistencyNode *piNode){HRESULT hr=S_OK;if(m_dwFlags&MRPF_WRITE){hr=MRPersistencyRemove(piNode,this);}if(m_dwFlags&MRPF_OPTIONAL){return S_OK;}return hr;}

    CMRPersistentSimpleReferenceT(T1 *pValue,const char *pName,DWORD flags):CMRPersistentReferenceT<T1>(pValue,pName,flags){}
    virtual ~CMRPersistentSimpleReferenceT(){}
};


template<typename T1> 
class CMRPersistentValueReferenceT:public CMRPersistentReferenceT<T1>
{
protected:
    T1 m_DefValue;

public:
    CMRPersistentValueReferenceT<T1> *SetDefaultValueAndReturnThis(T1 def)
    {
        MRPersistencyAsign(&m_DefValue,&def);
        return this;
    }

    CMRPersistentValueReferenceT(T1 *pValue,const char *pName,DWORD flags)
        :CMRPersistentReferenceT<T1>(pValue,pName,flags)
    {
    }
    void	Initialize(){MRPersistencyInitialize(this);}
    void	Free(){MRPersistencyFree(this);}
    void	SetDefaultValue(){MRPersistencyAsign(m_pValue,&m_DefValue);}

    HRESULT Load(ISystemPersistencyNode *piNode){HRESULT hr=S_OK;if(m_dwFlags&MRPF_READ){hr=MRPersistencyLoad(piNode,this);}if(m_dwFlags&MRPF_OPTIONAL){return S_OK;}return hr;}
    HRESULT Save(ISystemPersistencyNode *piNode){HRESULT hr=S_OK;if(m_dwFlags&MRPF_WRITE){hr=MRPersistencySave(piNode,this);}if(m_dwFlags&MRPF_OPTIONAL){return S_OK;}return hr;}
    HRESULT Remove(ISystemPersistencyNode *piNode){HRESULT hr=S_OK;if(m_dwFlags&MRPF_WRITE){hr=MRPersistencyRemove(piNode,this);}if(m_dwFlags&MRPF_OPTIONAL){return S_OK;}return hr;}

    ~CMRPersistentValueReferenceT(){}
};

template<typename T1> CMRPersistentValueReferenceT<T1> *MRCreateReferenceWithDefaultValue(T1 *pVar,const char *name,DWORD flags=MRPF_NORMAL){return new CMRPersistentValueReferenceT<T1>(pVar,name,flags);}
template<typename T1> CMRPersistentSimpleReferenceT<T1> *MRCreateReference(T1 *pVar,const char *name,DWORD flags=MRPF_NORMAL){return new CMRPersistentSimpleReferenceT<T1>(pVar,name,flags);}

#define DECLARE_SERIALIZABLE(className)\
    static HRESULT MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<className>*pItem)\
    {\
        if(piNode==NULL){return E_FAIL;}else {return pItem->GetValueAddress()->PersistencyLoad(piNode);}\
    }\
    static HRESULT MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<className>*pItem)\
    {\
         if(piNode==NULL){return E_FAIL;}else {return pItem->GetValueAddress()->PersistencySave(piNode);}\
    }\
    static HRESULT MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<className>*pItem){return S_OK;}\
    static void MRPersistencyInitialize(CMRPersistentReferenceT<className>*pItem){pItem->GetValueAddress()->PersistencyInitialize();}\
    static void MRPersistencyFree(CMRPersistentReferenceT<className>*pItem){pItem->GetValueAddress()->PersistencyFree();}

#define DECLARE_SERIALIZABLE_ENUMERATION(enumeration)\
	static HRESULT MRPersistencyLoad(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<enumeration>*pItem){return MRPersistencyLoad(piParent,(CMRPersistentReferenceT<int>*)pItem);}\
	static HRESULT MRPersistencySave(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<enumeration>*pItem){return MRPersistencySave(piParent,(CMRPersistentReferenceT<int>*)pItem);}\
	static HRESULT MRPersistencyRemove(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<enumeration>*pItem){piParent->DeleteNode(pItem->GetName());return S_OK;}\
	static void MRPersistencyInitialize(CMRPersistentReferenceT<enumeration>*pItem){return MRPersistencyInitialize((CMRPersistentReferenceT<int>*)pItem);}\
	static void MRPersistencyFree(CMRPersistentReferenceT<enumeration>*pItem){}

#define BEGIN_PROP_MAP(className)	\
    IMRPersistencyItem **MRGetPropertyMap(char *pMapName,char *persistNamePrefix)\
    {												\
        bool bInSpecifiedSubMap=(pMapName==NULL);\
        className *pInstance=this;						\
        std::list<IMRPersistencyItem *> items;				\
        char szPrefix[1024]={0};						\
        char szvarName[1024]={0};						\
        if(persistNamePrefix!=NULL){sprintf(szPrefix,"%s",persistNamePrefix);}

#define PROP(var,str) 							if(bInSpecifiedSubMap){wsprintf(szvarName,"%s%s",szPrefix,str);items.push_back(MRCreateReference(&pInstance->var,szvarName));}
#define PROP_FLAGS(var,str,flags) 				if(bInSpecifiedSubMap){wsprintf(szvarName,"%s%s",szPrefix,str);items.push_back(MRCreateReference(&pInstance->var,szvarName,flags));}
#define PROP_VALUE(var,str,value) 				if(bInSpecifiedSubMap){wsprintf(szvarName,"%s%s",szPrefix,str);items.push_back(MRCreateReferenceWithDefaultValue(&pInstance->var,szvarName)->SetDefaultValueAndReturnThis(value));}
#define PROP_VALUE_FLAGS(var,str,value,flags) 	if(bInSpecifiedSubMap){wsprintf(szvarName,"%s%s",szPrefix,str);items.push_back(MRCreateReferenceWithDefaultValue(&pInstance->var,szvarName,flags)->SetDefaultValueAndReturnThis(value));}
#define PROP_POINTER(var,str) 					        if(bInSpecifiedSubMap && var){wsprintf(szvarName,"%s%s",szPrefix,str);items.push_back(MRCreateReference(pInstance->var,szvarName));}
#define PROP_POINTER_FLAGS(var,str,flags) 				if(bInSpecifiedSubMap && var){wsprintf(szvarName,"%s%s",szPrefix,str);items.push_back(MRCreateReference(pInstance->var,szvarName,flags));}
#define PROP_POINTER_VALUE(var,str,value) 				if(bInSpecifiedSubMap && var){wsprintf(szvarName,"%s%s",szPrefix,str);items.push_back(MRCreateReferenceWithDefaultValue(pInstance->var,szvarName)->SetDefaultValueAndReturnThis(value));}
#define PROP_POINTER_VALUE_FLAGS(var,str,value,flags) 	if(bInSpecifiedSubMap && var){wsprintf(szvarName,"%s%s",szPrefix,str);items.push_back(MRCreateReferenceWithDefaultValue(pInstance->var,szvarName,flags)->SetDefaultValueAndReturnThis(value));}

#define BEGIN_PROP_SUBMAP(str) 	if(pMapName && strcmp(pMapName,str)==0){bInSpecifiedSubMap=true;
#define END_PROP_SUBMAP(str) 	bInSpecifiedSubMap=false;}

#define PROP_CLASS_CHAIN(otherClass)\
if(bInSpecifiedSubMap){\
    IMRPersistencyItem **ppOtherClassItems=otherClass::MRGetPropertyMap(NULL,szPrefix);\
    int x=0;\
    while(ppOtherClassItems[x]!=NULL){items.push_back(ppOtherClassItems[x]);x++;}\
    delete [] ppOtherClassItems;\
}
#define PROP_CLASS_CHAIN_SUBMAP(otherClass,subMap)\
if(bInSpecifiedSubMap){\
    IMRPersistencyItem **ppOtherClassItems=otherClass::MRGetPropertyMap(subMap,szPrefix);\
    int x=0;\
    while(ppOtherClassItems[x]!=NULL){items.push_back(ppOtherClassItems[x]);x++;}\
    delete [] ppOtherClassItems;\
}

#define PROP_CLASS_CHAIN_PREFIX(otherClass,chainPrefix)\
if(bInSpecifiedSubMap){\
    char szChainPrefix[STRING_SIZE]={0};\
    sprintf(szChainPrefix,szPrefix);\
    strcat(szChainPrefix,chainPrefix);\
    IMRPersistencyItem **ppOtherClassItems=otherClass::MRGetPropertyMap(pMapName,szChainPrefix);\
    int x=0;\
    while(ppOtherClassItems[x]!=NULL){items.push_back(ppOtherClassItems[x]);x++;}\
    delete [] ppOtherClassItems;\
}

void	PersistencyItemListDefaultValue(IMRPersistencyItem **ppiList,char *pPrefixName=NULL);
HRESULT PersistencyItemListSave(IMRPersistencyItem **ppiList,ISystemPersistencyNode *piNode,char *pPrefixName=NULL);
HRESULT PersistencyItemListLoad(IMRPersistencyItem **ppiList,ISystemPersistencyNode *piNode,char *pPrefixName=NULL);
HRESULT PersistencyItemListRemove(IMRPersistencyItem **ppiList,ISystemPersistencyNode *piNode,char *pPrefixName=NULL);
void	PersistencyItemListInitialize(IMRPersistencyItem **ppiList,char *pPrefixName=NULL);
void	PersistencyItemListFree(IMRPersistencyItem **ppiList,char *pPrefixName=NULL);
void	FreePersistencyItemList(IMRPersistencyItem ***ppiList);

#define END_PROP_MAP()\
    int x;\
    std::list<IMRPersistencyItem *>::iterator i;	\
    IMRPersistencyItem **pList=new IMRPersistencyItem *[(items.size()+1)];\
    for(x=0,i=items.begin();i!=items.end();i++,x++){pList[x]=(*i);}\
    pList[items.size()]=NULL;\
    return pList;\
};     \
    virtual void PersistencyDefaultValue(char *pMapName=NULL,char *pPrefixName=NULL)\
{\
    IMRPersistencyItem **ppiList=MRGetPropertyMap(pMapName,pPrefixName);\
    PersistencyItemListDefaultValue(ppiList, pPrefixName);\
    FreePersistencyItemList(&ppiList);\
}\
    virtual HRESULT PersistencySave(ISystemPersistencyNode *piNode,char *pMapName=NULL,char *pPrefixName=NULL)\
{\
    if(piNode==NULL){return E_FAIL;}\
    IMRPersistencyItem **ppiList=MRGetPropertyMap(pMapName,pPrefixName);\
    HRESULT finalhr=PersistencyItemListSave(ppiList,piNode,pPrefixName);\
    FreePersistencyItemList(&ppiList);\
    return finalhr;\
} \
    virtual HRESULT PersistencyLoad(ISystemPersistencyNode *piNode,char *pMapName=NULL,char *pPrefixName=NULL)\
{\
    if(piNode==NULL){return E_FAIL;}\
    IMRPersistencyItem **ppiList=MRGetPropertyMap(pMapName,pPrefixName);\
    HRESULT finalhr=PersistencyItemListLoad(ppiList,piNode,pPrefixName);\
    FreePersistencyItemList(&ppiList);\
    return finalhr;\
}\
    virtual HRESULT PersistencyRemove(ISystemPersistencyNode *piNode,char *pMapName=NULL,char *pPrefixName=NULL)\
{\
    if(piNode==NULL){return E_FAIL;}\
    IMRPersistencyItem **ppiList=MRGetPropertyMap(pMapName,pPrefixName);\
    HRESULT finalhr=PersistencyItemListRemove(ppiList,piNode,pPrefixName);\
    FreePersistencyItemList(&ppiList);\
    return finalhr;\
}\
    virtual void PersistencyInitialize(char *pMapName=NULL,char *pPrefixName=NULL)\
{\
    IMRPersistencyItem **ppiList=MRGetPropertyMap(pMapName,pPrefixName);\
    PersistencyItemListInitialize(ppiList,pPrefixName);\
    FreePersistencyItemList(&ppiList);\
}\
    virtual void PersistencyFree(char *pMapName=NULL,char *pPrefixName=NULL)\
{\
    IMRPersistencyItem **ppiList=MRGetPropertyMap(pMapName,pPrefixName);\
    PersistencyItemListFree(ppiList,pPrefixName);\
    FreePersistencyItemList(&ppiList);\
}

#define BEGIN_STRUCT_PROPS(className)			\
    static IMRPersistencyItem **MRGetPropertyMap(className *pInstance);\
    static HRESULT MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentSimpleReferenceT<className> *prop)\
    {\
        if(piNode==NULL){return E_FAIL;}\
        HRESULT hr=S_OK,finalhr=S_OK;\
        IMRPersistencyItem **ppiList=MRGetPropertyMap(prop->GetValueAddress());\
        int x=0;\
        while(ppiList[x]!=NULL){hr=ppiList[x]->Save(piNode->AddNode(ppiList[x]->GetName()));if(FAILED(hr)){finalhr=hr;};delete ppiList[x];ppiList[x]=NULL;x++;}\
        delete [] ppiList;ppiList=NULL;\
        return finalhr;\
    }\
    static HRESULT MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentSimpleReferenceT<className> *prop)\
    {\
        if(piNode==NULL){return E_FAIL;}\
        HRESULT hr=S_OK,finalhr=S_OK;\
        IMRPersistencyItem **ppiList=MRGetPropertyMap(prop->GetValueAddress());\
        int x=0;\
        while(ppiList[x]!=NULL){hr=ppiList[x]->Load(piNode->GetNode(ppiList[x]->GetName()));if(FAILED(hr)){finalhr=hr;};delete ppiList[x];ppiList[x]=NULL;x++;}\
        delete [] ppiList;ppiList=NULL;\
        return finalhr;\
    }\
    static HRESULT MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentSimpleReferenceT<className> *prop)\
    {\
        if(piNode==NULL){return E_FAIL;}\
        piNode->DeleteNode(prop->GetName());\
        return S_OK;\
    }\
    static void MRPersistencyInitialize(CMRPersistentSimpleReferenceT<className> *prop)\
    {\
        HRESULT hr=S_OK,finalhr=S_OK;\
        IMRPersistencyItem **ppiList=MRGetPropertyMap(prop->GetValueAddress());\
        int x=0;\
        while(ppiList[x]!=NULL){ppiList[x]->Initialize();if(FAILED(hr)){finalhr=hr;};delete ppiList[x];ppiList[x]=NULL;x++;}\
        delete [] ppiList;ppiList=NULL;\
    }\
    static void MRPersistencyFree(CMRPersistentSimpleReferenceT<className> *prop)\
    {\
        HRESULT hr=S_OK,finalhr=S_OK;\
        IMRPersistencyItem **ppiList=MRGetPropertyMap(prop->GetValueAddress());\
        int x=0;\
        while(ppiList[x]!=NULL){ppiList[x]->Free();if(FAILED(hr)){finalhr=hr;};delete ppiList[x];ppiList[x]=NULL;x++;}\
        delete [] ppiList;ppiList=NULL;\
    }\
    static IMRPersistencyItem **MRGetPropertyMap(className *pInstance)	\
    {													\
        bool bInSpecifiedSubMap=true;\
        std::list<IMRPersistencyItem *> items;			\
        char szPrefix[1024]={0};						\
        char szvarName[1024]={0};						

#define END_STRUCT_PROPS()						\
        int x;\
        std::list<IMRPersistencyItem *>::iterator i;	\
        IMRPersistencyItem **pList=new IMRPersistencyItem *[(items.size()+1)];\
        for(x=0,i=items.begin();i!=items.end();i++,x++){pList[x]=(*i);}\
        pList[items.size()]=NULL;\
        return pList;\
    };



class CSystemSerializableBase:virtual public CSystemUnknownBase, virtual public ISystemSerializable
{
public:

    // Virtual functions to be used from ISystemSerializable implementation, this functions are overloaded
    // using BEGIN_PROP_MAP/END_PROP_MAP blocks

    virtual void    PersistencyDefaultValue(char *pMapName=NULL,char *pPrefixName=NULL);
    virtual HRESULT PersistencySave(ISystemPersistencyNode *piNode,char *pMapName=NULL,char *pPrefixName=NULL);
    virtual HRESULT PersistencyLoad(ISystemPersistencyNode *piNode,char *pMapName=NULL,char *pPrefixName=NULL);
    virtual HRESULT PersistencyRemove(ISystemPersistencyNode *piNode,char *pMapName=NULL,char *pPrefixName=NULL);
    virtual void    PersistencyInitialize(char *pMapName=NULL,char *pPrefixName=NULL);
    virtual void    PersistencyFree(char *pMapName=NULL,char *pPrefixName=NULL);

    // ISystemSerializable
    virtual bool    Serialize(ISystemPersistencyNode *piNode);
    virtual bool    Unserialize(ISystemPersistencyNode *piNode);
};
