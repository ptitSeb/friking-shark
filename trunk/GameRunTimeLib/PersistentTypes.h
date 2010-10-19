#pragma once

template<typename T1,typename T2> static void MRPersistencyAsign(T1 *pVar1,T2 *pVar2){(*pVar1)=(*pVar2);}

/////////////////////////////////////////////////////
// Funciones para guardar tipos fundamentales

HRESULT MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<DWORD> *prop);
HRESULT MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<int> *prop);
HRESULT MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<float> *prop);
HRESULT MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<double> *prop);
HRESULT MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<bool> *pItem);
HRESULT MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<std::string> *pItem);

HRESULT MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<bool> *pItem);
HRESULT MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<int> *pItem);
HRESULT MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<DWORD> *prop);
HRESULT MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<float> *prop);
HRESULT MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<double> *prop);
HRESULT MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<std::string> *pItem);

HRESULT MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<DWORD> *prop);
HRESULT MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<int> *prop);
HRESULT MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<float> *prop);
HRESULT MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<double> *prop);
HRESULT MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<bool> *pItem);
HRESULT MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<std::string> *pItem);

void MRPersistencyInitialize(CMRPersistentReferenceT<DWORD> *prop);
void MRPersistencyInitialize(CMRPersistentReferenceT<int> *prop);
void MRPersistencyInitialize(CMRPersistentReferenceT<float> *prop);
void MRPersistencyInitialize(CMRPersistentReferenceT<double> *prop);
void MRPersistencyInitialize(CMRPersistentReferenceT<bool> *pItem);
void MRPersistencyInitialize(CMRPersistentReferenceT<std::string> *pItem);

void MRPersistencyFree(CMRPersistentReferenceT<DWORD> *prop);
void MRPersistencyFree(CMRPersistentReferenceT<int> *prop);
void MRPersistencyFree(CMRPersistentReferenceT<float> *prop);
void MRPersistencyFree(CMRPersistentReferenceT<double> *prop);
void MRPersistencyFree(CMRPersistentReferenceT<bool> *pItem);
void MRPersistencyFree(CMRPersistentReferenceT<std::string> *pItem);

/////////////////////////////////////////////////////
// Funciones para guardar contenedores no asociativos

template<typename T1,typename CONTAINED_TYPE>
HRESULT MRLoadFromContainer(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<T1>*pItem)
{
    if(piNode==NULL){return E_FAIL;}
    pItem->GetValueAddress()->clear();

    HRESULT hr=S_OK,finalhr=S_OK;
	DWORD itemCount=piNode->GetNodeCount();
    for(DWORD x=0;x<itemCount;x++)
    {
        CONTAINED_TYPE var;
		ISystemPersistencyNode *piChildNode=piNode->GetNode(x);
		CMRPersistentSimpleReferenceT<CONTAINED_TYPE> *pRef=MRCreateReference(&var,piChildNode->GetName());
        MRPersistencyInitialize(pRef);
        hr=MRPersistencyLoad(piChildNode,pRef);
        if(SUCCEEDED(hr)){pItem->GetValueAddress()->insert(pItem->GetValueAddress()->end(),*pRef->GetValueAddress());}
        delete pRef;
        pRef=NULL;
        if(FAILED(hr))
		{
			RTTRACE("GameRunTimeLib::MRLoadFromContainer-> Failed To Load container item %s, result 0x%08x",piChildNode->GetDebugInfoPath().c_str(),hr);
			finalhr=hr;
		}
    }

    return finalhr;
}

template<typename T1,typename CONTAINED_TYPE>
HRESULT MRSaveToContainer(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<T1> *pItem)
{
    if(piNode==NULL){return E_FAIL;}
    piNode->Clear();

    HRESULT hr=S_OK,finalhr=S_OK;
    if(SUCCEEDED(finalhr))
    {
        T1::iterator i;
		int x=0;
        for(x=0,i=pItem->GetValueAddress()->begin();i!=pItem->GetValueAddress()->end();i++,x++)
        {
			char sItemName[200];
			sprintf(sItemName,"Item%d",x);
			ISystemPersistencyNode *piChildNode=piNode->AddNode(sItemName);
            CMRPersistentSimpleReferenceT<CONTAINED_TYPE> *pRef=MRCreateReference(&(*i),sItemName);
            hr=MRPersistencySave(piChildNode,pRef);
            if(FAILED(hr))
			{
				RTTRACE("GameRunTimeLib::MRLoadFromContainer-> Failed To Save container item %s, result 0x%08x",piChildNode->GetDebugInfoPath().c_str(),hr);
				finalhr=hr;
			}
            delete pRef;
            pRef=NULL;
        }
    }
    return finalhr;
}

/////////////////////////////////////////////////////
// Funciones para guardar contenedores asociativos

template<typename T1,typename KEY_TYPE,typename CONTAINED_TYPE>
HRESULT MRLoadFromContainer(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<T1>*pItem)
{
 /*   ISystemPersistencyNode *piNode=piParent->GetNode(pItem->GetName());
    if(piNode==NULL){return E_FAIL;}
    pItem->GetValueAddress()->clear();

    DWORD itemCount=piNode->GetItemCount();

    HRESULT hr=S_OK,finalhr=S_OK;
    for(DWORD x=0;x<itemCount;x++)
    {
        char keyName[512];
        char valueName[512];
        sprintf(keyName,"ItemKey%d",x);
        sprintf(valueName,"ItemValue%d",x);

        KEY_TYPE        key;
        CONTAINED_TYPE  value;
        CMRPersistentSimpleReferenceT<KEY_TYPE>         *pKeyRef=MRCreateReference(&key,keyName);
        CMRPersistentSimpleReferenceT<CONTAINED_TYPE>   *pValueRef=MRCreateReference(&value,valueName);
        MRPersistencyInitialize(pKeyRef);
        MRPersistencyInitialize(pValueRef);
        hr=MRPersistencyLoad(piNode,pKeyRef);
        if(SUCCEEDED(hr)){hr=MRPersistencyLoad(piNode,pValueRef);}
        if(SUCCEEDED(hr)){pItem->GetValueAddress()->insert(T1::value_type(key,value));}
        delete pKeyRef;
        delete pValueRef;
        pKeyRef=NULL;
        pValueRef=NULL;
        if(FAILED(hr)){finalhr=hr;}
    }
    return finalhr;*/
	return E_FAIL;
}

template<typename T1,typename KEY_TYPE,typename CONTAINED_TYPE>
HRESULT MRSaveToContainer(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<T1> *pItem)
{
	return E_FAIL;
	/*
    ISystemPersistencyNode *piNode=piParent->AddNode(pItem->GetName());
    if(piNode==NULL){return E_FAIL;}
    piNode->Clear();

    char countValue[512];
    sprintf(countValue,"%d",pItem->GetValueAddress()->size());
    SSystemProperty countProp;
    countProp.name="ItemCount";
    countProp.value=countValue;
    piNode->AddProperty(countProp);

    DWORD itemCount=0;
    HRESULT hr=S_OK,finalhr=S_OK;
    if(SUCCEEDED(finalhr))
    {
        T1::iterator i;
        for(i=pItem->GetValueAddress()->begin();i!=pItem->GetValueAddress()->end();i++)
        {
            char keyName[512];
            char valueName[512];
            sprintf(keyName,"ItemKey%d",itemCount);
            sprintf(valueName,"ItemValue%d",itemCount);
            CMRPersistentSimpleReferenceT<KEY_TYPE>         *pKeyRef=MRCreateReference(const_cast<KEY_TYPE *>(&(i->first)),keyName);
            CMRPersistentSimpleReferenceT<CONTAINED_TYPE>   *pValueRef=MRCreateReference(&(i->second),valueName);
            hr=MRPersistencySave(piNode,pKeyRef);
            if(SUCCEEDED(hr)){hr=MRPersistencySave(piNode,pValueRef);}
            if(SUCCEEDED(hr)){itemCount++;}
            if(FAILED(hr)){finalhr=hr;}
            delete pKeyRef;
            delete pValueRef;
            pKeyRef=NULL;
            pValueRef=NULL;
        }
    }
    return finalhr;*/
}

/////////////////////////////////////////////////////
// Funciones para guardar contenedores, usan las funciones anteriores
// para guardar todos los contenedores de la misma manera.

template<typename T1>void    MRPersistencyInitialize(CMRPersistentReferenceT<std::list<T1> >*pItem){}
template<typename T1>void    MRPersistencyFree(CMRPersistentReferenceT<std::list<T1> >*pItem){}
template<typename T1>HRESULT MRPersistencyRemove(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::list<T1> >*pItem){piParent->DeleteNode(pItem->GetName());return S_OK;}
template<typename T1>HRESULT MRPersistencyLoad(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::list<T1> > *pItem){return MRLoadFromContainer<std::list<T1> , T1 >(piParent,pItem);}
template<typename T1>HRESULT MRPersistencySave(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::list<T1> > *pItem){return MRSaveToContainer<std::list<T1> , T1 >(piParent,pItem);}

template<typename T1>void    MRPersistencyInitialize(CMRPersistentReferenceT<std::deque<T1> >*pItem){}
template<typename T1>void    MRPersistencyFree(CMRPersistentReferenceT<std::deque<T1> >*pItem){}
template<typename T1>HRESULT MRPersistencyRemove(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::deque<T1> >*pItem){piParent->DeleteNode(pItem->GetName());return S_OK;}
template<typename T1>HRESULT MRPersistencyLoad(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::deque<T1> > *pItem){return MRLoadFromContainer<std::deque<T1> , T1 >(piParent,pItem);}
template<typename T1>HRESULT MRPersistencySave(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::deque<T1> > *pItem){return MRSaveToContainer<std::deque<T1> , T1 >(piParent,pItem);}

template<typename T1>void    MRPersistencyInitialize(CMRPersistentReferenceT<std::vector<T1> >*pItem){}
template<typename T1>void    MRPersistencyFree(CMRPersistentReferenceT<std::vector<T1> >*pItem){}
template<typename T1>HRESULT MRPersistencyRemove(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::vector<T1> >*pItem){piParent->DeleteNode(pItem->GetName());return S_OK;}
template<typename T1>HRESULT MRPersistencyLoad(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::vector<T1> > *pItem){return MRLoadFromContainer<std::vector<T1> , T1 >(piParent,pItem);}
template<typename T1>HRESULT MRPersistencySave(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::vector<T1> > *pItem){return MRSaveToContainer<std::vector<T1> , T1 >(piParent,pItem);}

template<typename T1>void    MRPersistencyInitialize(CMRPersistentReferenceT<std::set<T1> >*pItem){}
template<typename T1>void    MRPersistencyFree(CMRPersistentReferenceT<std::set<T1> >*pItem){}
template<typename T1>HRESULT MRPersistencyRemove(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::set<T1> >*pItem){piParent->DeleteNode(pItem->GetName());return S_OK;}
template<typename T1>HRESULT MRPersistencyLoad(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::set<T1> > *pItem){return MRLoadFromContainer<std::set<T1> , T1 >(piParent,pItem);}
template<typename T1>HRESULT MRPersistencySave(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::set<T1> > *pItem){return MRSaveToContainer<std::set<T1> , T1 >(piParent,pItem);}

template<typename T1,typename T2>void    MRPersistencyInitialize(CMRPersistentReferenceT<std::map<T1,T2> >*pItem){}
template<typename T1,typename T2>void    MRPersistencyFree(CMRPersistentReferenceT<std::map<T1,T2> >*pItem){}
template<typename T1,typename T2>HRESULT MRPersistencyRemove(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::map<T1,T2> >*pItem){piParent->DeleteNode(pItem->GetName());return S_OK;}
template<typename T1,typename T2>HRESULT MRPersistencyLoad(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::map<T1,T2> > *pItem){return MRLoadFromContainer<std::map<T1,T2> , T1, T2 >(piParent,pItem);}
template<typename T1,typename T2>HRESULT MRPersistencySave(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::map<T1,T2> > *pItem){return MRSaveToContainer<std::map<T1,T2> , T1 , T2>(piParent,pItem);}

template<typename T1,typename T2>void    MRPersistencyInitialize(CMRPersistentReferenceT<std::multimap<T1,T2> >*pItem){}
template<typename T1,typename T2>void    MRPersistencyFree(CMRPersistentReferenceT<std::multimap<T1,T2> >*pItem){}
template<typename T1,typename T2>HRESULT MRPersistencyRemove(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::multimap<T1,T2> >*pItem){piParent->DeleteNode(pItem->GetName());return S_OK;}
template<typename T1,typename T2>HRESULT MRPersistencyLoad(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::multimap<T1,T2> > *pItem){return MRLoadFromContainer<std::multimap<T1,T2> , T1, T2 >(piParent,pItem);}
template<typename T1,typename T2>HRESULT MRPersistencySave(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::multimap<T1,T2> > *pItem){return MRSaveToContainer<std::multimap<T1,T2> , T1 , T2>(piParent,pItem);}
