#include "./stdafx.h"
#include "./GameRunTimeLib.h"
#include "./Persistency.h"

void PersistencyItemListDefaultValue(IMRPersistencyItem **ppiList,const char *pPrefixName)
{
    int x=0;
    if(ppiList)
    {
        while(ppiList[x]!=NULL)
        {
            ppiList[x]->SetDefaultValue();
            x++;
        }
    }
}
HRESULT PersistencyItemListSave(IMRPersistencyItem **ppiList,ISystemPersistencyNode *piNode,const char *pPrefixName)
{
    HRESULT hr=S_OK,finalhr=S_OK;
    if(pPrefixName==NULL){piNode->Clear();}
    int x=0;
    if(ppiList)
    {
        while(ppiList[x]!=NULL)
        {
			ISystemPersistencyNode *piChildNode=piNode->AddNode(ppiList[x]->GetName());
            hr=ppiList[x]->Remove(piChildNode);
            hr=ppiList[x]->Save(piChildNode);
            if(FAILED(hr))
            {
				RTTRACE("GameRunTimeLib::PersistencyLoad-> Failed To Save item %s\\%s, result 0x%08x",piNode->GetDebugInfoPath().c_str(),ppiList[x]->GetName(),hr);
                finalhr=hr;
            }
            x++;
        }
    }
    return finalhr;
} 
HRESULT PersistencyItemListLoad(IMRPersistencyItem **ppiList,ISystemPersistencyNode *piNode,const char *pPrefixName)
{
    HRESULT hr=S_OK,finalhr=S_OK;
    int x=0;
    if(ppiList)
    {
        while(ppiList[x]!=NULL)
        {
			ISystemPersistencyNode *piChildNode=piNode->GetNode(ppiList[x]->GetName());
            hr=ppiList[x]->Load(piChildNode);
            if(FAILED(hr))
            {
                RTTRACE("GameRunTimeLib::PersistencyLoad-> Failed To Load item %s\\%s, result 0x%08x",piNode->GetDebugInfoPath().c_str(),ppiList[x]->GetName(),hr);
                finalhr=hr;
            }
            x++;
        }
    }
    return finalhr;
}
HRESULT PersistencyItemListRemove(IMRPersistencyItem **ppiList,ISystemPersistencyNode *piNode,const char *pPrefixName)
{
    HRESULT hr=S_OK,finalhr=S_OK;
    int x=0;
    if(ppiList)
    {
        while(ppiList[x]!=NULL)
        {
			ISystemPersistencyNode *piChildNode=piNode->GetNode(ppiList[x]->GetName());
            hr=ppiList[x]->Remove(piChildNode);
            if(FAILED(hr))
            {
                finalhr=hr;
            }
            x++;
        }
    }
    return finalhr;
}
void PersistencyItemListInitialize(IMRPersistencyItem **ppiList,const char *pPrefixName)
{
    int x=0;
    if(ppiList)
    {
        while(ppiList[x]!=NULL)
        {
            ppiList[x]->Initialize();
            x++;
        }
    }
}
void PersistencyItemListFree(IMRPersistencyItem **ppiList,const char *pPrefixName)
{
    int x=0;
    if(ppiList)
    {
        while(ppiList[x]!=NULL)
        {
            ppiList[x]->Free();
            x++;
        }
    }
}

void FreePersistencyItemList(IMRPersistencyItem ***ppiList)
{
    int x=0;
    if(ppiList)
    {
        while((*ppiList)[x]!=NULL){delete ((*ppiList)[x]);x++;}
        delete [] ((*ppiList));
    }
}

// Virtual functions to be used from ISystemSerializable implementation, this functions are overloaded
// using BEGIN_PROP_MAP/END_PROP_MAP blocks

void    CSystemSerializableBase::PersistencyDefaultValue(const char *pMapName,const char *pPrefixName){}
HRESULT CSystemSerializableBase::PersistencySave(ISystemPersistencyNode *piNode,const char *pMapName,const char *pPrefixName){return S_OK;}
HRESULT CSystemSerializableBase::PersistencyLoad(ISystemPersistencyNode *piNode,const char *pMapName,const char *pPrefixName){return S_OK;}
HRESULT CSystemSerializableBase::PersistencyRemove(ISystemPersistencyNode *piNode,const char *pMapName,const char *pPrefixName){return S_OK;}
void    CSystemSerializableBase::PersistencyInitialize(const char *pMapName,const char *pPrefixName){}
void    CSystemSerializableBase::PersistencyFree(const char *pMapName,const char *pPrefixName){}

// ISystemSerializable

bool    CSystemSerializableBase::Serialize(ISystemPersistencyNode *piNode){return SUCCEEDED(PersistencySave(piNode,NULL));}
bool    CSystemSerializableBase::Unserialize(ISystemPersistencyNode *piNode){return SUCCEEDED(PersistencyLoad(piNode,NULL));}
