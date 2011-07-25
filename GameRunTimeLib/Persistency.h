//  Friking Shark, a Flying Shark arcade remake.
//  Copyright (C) 2011  Javier Martin Garcia (javiermartingarcia@gmail.com)
//	
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//  


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

    virtual bool Load(ISystemPersistencyNode *)=0;
    virtual bool Save(ISystemPersistencyNode *)=0;
    virtual bool Remove(ISystemPersistencyNode *)=0;

    virtual char *GetName()=0;
};


template<typename T1> 
class CMRPersistentReferenceT:public IMRPersistencyItem
{
protected:

    unsigned int		m_dwFlags;

    T1			*m_pValue;
    char		m_sName[200];

public:

    char *GetName(){return m_sName;}
    T1	 *GetValueAddress(){return m_pValue;}
    void SetDefaultValue(){}

    CMRPersistentReferenceT(T1 *pValue,const char *pName,unsigned int flags)
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
    bool Load(ISystemPersistencyNode *piNode){bool bOk=true;if(this->m_dwFlags&MRPF_READ){bOk=MRPersistencyLoad(piNode,this);}if(this->m_dwFlags&MRPF_OPTIONAL){return true;}return bOk;}
    bool Save(ISystemPersistencyNode *piNode){bool bOk=true;if(this->m_dwFlags&MRPF_WRITE){bOk=MRPersistencySave(piNode,this);}if(this->m_dwFlags&MRPF_OPTIONAL){return true;}return bOk;}
    bool Remove(ISystemPersistencyNode *piNode){bool bOk=true;if(this->m_dwFlags&MRPF_WRITE){bOk=MRPersistencyRemove(piNode,this);}if(this->m_dwFlags&MRPF_OPTIONAL){return true;}return bOk;}

    CMRPersistentSimpleReferenceT(T1 *pValue,const char *pName,unsigned int flags):CMRPersistentReferenceT<T1>(pValue,pName,flags){}
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

    CMRPersistentValueReferenceT(T1 *pValue,const char *pName,unsigned int flags)
        :CMRPersistentReferenceT<T1>(pValue,pName,flags)
    {
    }
    void Initialize(){MRPersistencyInitialize(this);}
    void Free(){MRPersistencyFree(this);}
    void SetDefaultValue(){MRPersistencyAsign(this->m_pValue,&m_DefValue);}

    bool Load(ISystemPersistencyNode *piNode){bool bOk=true;if(this->m_dwFlags&MRPF_READ){bOk=MRPersistencyLoad(piNode,this);}if(this->m_dwFlags&MRPF_OPTIONAL){return true;}return bOk;}
    bool Save(ISystemPersistencyNode *piNode){bool bOk=true;if(this->m_dwFlags&MRPF_WRITE){bOk=MRPersistencySave(piNode,this);}if(this->m_dwFlags&MRPF_OPTIONAL){return true;}return bOk;}
    bool Remove(ISystemPersistencyNode *piNode){bool bOk=true;if(this->m_dwFlags&MRPF_WRITE){bOk=MRPersistencyRemove(piNode,this);}if(this->m_dwFlags&MRPF_OPTIONAL){return true;}return bOk;}

    ~CMRPersistentValueReferenceT(){}
};

template<typename T1> CMRPersistentValueReferenceT<T1> *MRCreateReferenceWithDefaultValue(T1 *pVar,const char *name,unsigned int flags=MRPF_NORMAL){return new CMRPersistentValueReferenceT<T1>(pVar,name,flags);}
template<typename T1> CMRPersistentSimpleReferenceT<T1> *MRCreateReference(T1 *pVar,const char *name,unsigned int flags=MRPF_NORMAL){return new CMRPersistentSimpleReferenceT<T1>(pVar,name,flags);}

#define DECLARE_SERIALIZABLE(className)\
    __UNUSED_ATTRIB__ static bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<className>*pItem)\
    {\
        if(piNode==NULL){return false;}else {return pItem->GetValueAddress()->PersistencyLoad(piNode);}\
    }\
    __UNUSED_ATTRIB__ static bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<className>*pItem)\
    {\
         if(piNode==NULL){return false;}else {return pItem->GetValueAddress()->PersistencySave(piNode);}\
    }\
    __UNUSED_ATTRIB__ static bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<className>*pItem){return true;}\
    __UNUSED_ATTRIB__ static void MRPersistencyInitialize(CMRPersistentReferenceT<className>*pItem){pItem->GetValueAddress()->PersistencyInitialize();}\
    __UNUSED_ATTRIB__ static void MRPersistencyFree(CMRPersistentReferenceT<className>*pItem){pItem->GetValueAddress()->PersistencyFree();}

#define DECLARE_SERIALIZABLE_ENUMERATION(enumeration)\
	__UNUSED_ATTRIB__ static bool MRPersistencyLoad(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<enumeration>*pItem){return MRPersistencyLoad(piParent,(CMRPersistentReferenceT<int>*)pItem);}\
	__UNUSED_ATTRIB__ static bool MRPersistencySave(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<enumeration>*pItem){return MRPersistencySave(piParent,(CMRPersistentReferenceT<int>*)pItem);}\
	__UNUSED_ATTRIB__ static bool MRPersistencyRemove(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<enumeration>*pItem){return MRPersistencyRemove(piParent,(CMRPersistentReferenceT<int>*)pItem);}\
	__UNUSED_ATTRIB__ static void MRPersistencyInitialize(CMRPersistentReferenceT<enumeration>*pItem){return MRPersistencyInitialize((CMRPersistentReferenceT<int>*)pItem);}\
	__UNUSED_ATTRIB__ static void MRPersistencyFree(CMRPersistentReferenceT<enumeration>*pItem){}

#define BEGIN_PROP_MAP(className)	\
    IMRPersistencyItem **MRGetPropertyMap(const char *pMapName,const char *persistNamePrefix)\
    {												\
        bool bInSpecifiedSubMap=(pMapName==NULL);\
        className *pInstance=this;						\
        std::list<IMRPersistencyItem *> items;				\
        char szPrefix[1024]={0};						\
        char szvarName[1024]={0};						\
        if(persistNamePrefix!=NULL){sprintf(szPrefix,"%s",persistNamePrefix);}

#define PROP(var,str) 							if(bInSpecifiedSubMap){sprintf(szvarName,"%s%s",szPrefix,str);items.push_back(MRCreateReference(&pInstance->var,szvarName));}
#define PROP_FLAGS(var,str,flags) 				if(bInSpecifiedSubMap){sprintf(szvarName,"%s%s",szPrefix,str);items.push_back(MRCreateReference(&pInstance->var,szvarName,flags));}
#define PROP_VALUE(var,str,value) 				if(bInSpecifiedSubMap){sprintf(szvarName,"%s%s",szPrefix,str);items.push_back(MRCreateReferenceWithDefaultValue(&pInstance->var,szvarName)->SetDefaultValueAndReturnThis(value));}
#define PROP_VALUE_FLAGS(var,str,value,flags) 	if(bInSpecifiedSubMap){sprintf(szvarName,"%s%s",szPrefix,str);items.push_back(MRCreateReferenceWithDefaultValue(&pInstance->var,szvarName,flags)->SetDefaultValueAndReturnThis(value));}
#define PROP_POINTER(var,str) 					        if(bInSpecifiedSubMap && var){sprintf(szvarName,"%s%s",szPrefix,str);items.push_back(MRCreateReference(pInstance->var,szvarName));}
#define PROP_POINTER_FLAGS(var,str,flags) 				if(bInSpecifiedSubMap && var){sprintf(szvarName,"%s%s",szPrefix,str);items.push_back(MRCreateReference(pInstance->var,szvarName,flags));}
#define PROP_POINTER_VALUE(var,str,value) 				if(bInSpecifiedSubMap && var){sprintf(szvarName,"%s%s",szPrefix,str);items.push_back(MRCreateReferenceWithDefaultValue(pInstance->var,szvarName)->SetDefaultValueAndReturnThis(value));}
#define PROP_POINTER_VALUE_FLAGS(var,str,value,flags) 	if(bInSpecifiedSubMap && var){sprintf(szvarName,"%s%s",szPrefix,str);items.push_back(MRCreateReferenceWithDefaultValue(pInstance->var,szvarName,flags)->SetDefaultValueAndReturnThis(value));}

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

void	PersistencyItemListDefaultValue(IMRPersistencyItem **ppiList,const char *pPrefixName=NULL);
bool PersistencyItemListSave(IMRPersistencyItem **ppiList,ISystemPersistencyNode *piNode,const char *pPrefixName=NULL);
bool PersistencyItemListLoad(IMRPersistencyItem **ppiList,ISystemPersistencyNode *piNode,const char *pPrefixName=NULL);
bool PersistencyItemListRemove(IMRPersistencyItem **ppiList,ISystemPersistencyNode *piNode,const char *pPrefixName=NULL);
void	PersistencyItemListInitialize(IMRPersistencyItem **ppiList,const char *pPrefixName=NULL);
void	PersistencyItemListFree(IMRPersistencyItem **ppiList,const char *pPrefixName=NULL);
void	FreePersistencyItemList(IMRPersistencyItem ***ppiList);

#define END_PROP_MAP()\
    int x;\
    std::list<IMRPersistencyItem *>::iterator i;	\
    IMRPersistencyItem **pList=new IMRPersistencyItem *[(items.size()+1)];\
    for(x=0,i=items.begin();i!=items.end();i++,x++){pList[x]=(*i);}\
    pList[items.size()]=NULL;\
    return pList;\
};     \
    virtual void PersistencyDefaultValue(const char *pMapName=NULL,const char *pPrefixName=NULL)\
{\
    IMRPersistencyItem **ppiList=MRGetPropertyMap(pMapName,pPrefixName);\
    PersistencyItemListDefaultValue(ppiList, pPrefixName);\
    FreePersistencyItemList(&ppiList);\
}\
    virtual bool PersistencySave(ISystemPersistencyNode *piNode,const char *pMapName=NULL,const char *pPrefixName=NULL)\
{\
    if(piNode==NULL){return false;}\
    IMRPersistencyItem **ppiList=MRGetPropertyMap(pMapName,pPrefixName);\
    bool bFinalOk=PersistencyItemListSave(ppiList,piNode,pPrefixName);\
    FreePersistencyItemList(&ppiList);\
    return bFinalOk;\
} \
    virtual bool PersistencyLoad(ISystemPersistencyNode *piNode,const char *pMapName=NULL,const char *pPrefixName=NULL)\
{\
    if(piNode==NULL){return false;}\
    IMRPersistencyItem **ppiList=MRGetPropertyMap(pMapName,pPrefixName);\
    bool bFinalOk=PersistencyItemListLoad(ppiList,piNode,pPrefixName);\
    FreePersistencyItemList(&ppiList);\
    return bFinalOk;\
}\
    virtual bool PersistencyRemove(ISystemPersistencyNode *piNode,const char *pMapName=NULL,const char *pPrefixName=NULL)\
{\
    if(piNode==NULL){return false;}\
    IMRPersistencyItem **ppiList=MRGetPropertyMap(pMapName,pPrefixName);\
    bool bFinalOk=PersistencyItemListRemove(ppiList,piNode,pPrefixName);\
    FreePersistencyItemList(&ppiList);\
    return bFinalOk;\
}\
    virtual void PersistencyInitialize(const char *pMapName=NULL,const char *pPrefixName=NULL)\
{\
    IMRPersistencyItem **ppiList=MRGetPropertyMap(pMapName,pPrefixName);\
    PersistencyItemListInitialize(ppiList,pPrefixName);\
    FreePersistencyItemList(&ppiList);\
}\
    virtual void PersistencyFree(const char *pMapName=NULL,const char *pPrefixName=NULL)\
{\
    IMRPersistencyItem **ppiList=MRGetPropertyMap(pMapName,pPrefixName);\
    PersistencyItemListFree(ppiList,pPrefixName);\
    FreePersistencyItemList(&ppiList);\
}

#define BEGIN_STRUCT_PROPS(className)			\
    static IMRPersistencyItem **MRGetPropertyMap(className *pInstance);\
    __UNUSED_ATTRIB__ static bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentSimpleReferenceT<className> *prop)\
    {\
        if(piNode==NULL){return false;}\
        IMRPersistencyItem **ppiList=MRGetPropertyMap(prop->GetValueAddress());\
		bool bFinalOk=PersistencyItemListSave(ppiList,piNode,NULL);\
		FreePersistencyItemList(&ppiList);\
        return bFinalOk;\
    }\
    __UNUSED_ATTRIB__ static bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentSimpleReferenceT<className> *prop)\
    {\
        if(piNode==NULL){return false;}\
		IMRPersistencyItem **ppiList=MRGetPropertyMap(prop->GetValueAddress());\
		bool bFinalOk=PersistencyItemListLoad(ppiList,piNode,NULL);\
		FreePersistencyItemList(&ppiList);\
        return bFinalOk;\
    }\
    __UNUSED_ATTRIB__ static bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentSimpleReferenceT<className> *prop)\
    {\
        if(piNode==NULL){return false;}\
		IMRPersistencyItem **ppiList=MRGetPropertyMap(prop->GetValueAddress());\
		PersistencyItemListRemove(ppiList,piNode,NULL);\
		FreePersistencyItemList(&ppiList);\
		return true;\
    }\
    __UNUSED_ATTRIB__ static void MRPersistencyInitialize(CMRPersistentSimpleReferenceT<className> *prop)\
    {\
		IMRPersistencyItem **ppiList=MRGetPropertyMap(prop->GetValueAddress());\
		PersistencyItemListInitialize(ppiList,NULL);\
		FreePersistencyItemList(&ppiList);\
    }\
    __UNUSED_ATTRIB__ static void MRPersistencyFree(CMRPersistentSimpleReferenceT<className> *prop)\
    {\
		IMRPersistencyItem **ppiList=MRGetPropertyMap(prop->GetValueAddress());\
		PersistencyItemListFree(ppiList,NULL);\
		FreePersistencyItemList(&ppiList);\
    }\
    __UNUSED_ATTRIB__ static IMRPersistencyItem **MRGetPropertyMap(className *pInstance)	\
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
    }



class CSystemSerializableBase:virtual public CSystemUnknownBase, virtual public ISystemSerializable
{
public:

    // Virtual functions to be used from ISystemSerializable implementation, this functions are overloaded
    // using BEGIN_PROP_MAP/END_PROP_MAP blocks

    virtual void PersistencyDefaultValue(const char *pMapName=NULL,const char *pPrefixName=NULL);
    virtual bool PersistencySave(ISystemPersistencyNode *piNode,const char *pMapName=NULL,const char *pPrefixName=NULL);
    virtual bool PersistencyLoad(ISystemPersistencyNode *piNode,const char *pMapName=NULL,const char *pPrefixName=NULL);
    virtual bool PersistencyRemove(ISystemPersistencyNode *piNode,const char *pMapName=NULL,const char *pPrefixName=NULL);
    virtual void PersistencyInitialize(const char *pMapName=NULL,const char *pPrefixName=NULL);
    virtual void PersistencyFree(const char *pMapName=NULL,const char *pPrefixName=NULL);

    // ISystemSerializable
    virtual bool Serialize(ISystemPersistencyNode *piNode);
    virtual bool Unserialize(ISystemPersistencyNode *piNode);
};
