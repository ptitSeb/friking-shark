#pragma once

template<typename T1,typename T2> static void MRPersistencyAsign(T1 *pVar1,T2 *pVar2){(*pVar1)=(*pVar2);}

/////////////////////////////////////////////////////
// Funciones para guardar tipos fundamentales

bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<DWORD> *prop);
bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<int> *prop);
bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<float> *prop);
bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<double> *prop);
bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<bool> *pItem);
bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<std::string> *pItem);

bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<bool> *pItem);
bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<int> *pItem);
bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<DWORD> *prop);
bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<float> *prop);
bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<double> *prop);
bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<std::string> *pItem);

bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<DWORD> *prop);
bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<int> *prop);
bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<float> *prop);
bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<double> *prop);
bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<bool> *pItem);
bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<std::string> *pItem);

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
bool MRLoadFromContainer(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<T1>*pItem)
{
    if(piNode==NULL){return false;}
    pItem->GetValueAddress()->clear();

    bool bOk=true,bFinalOk=true;
	DWORD itemCount=piNode->GetNodeCount();
    for(DWORD x=0;x<itemCount;x++)
    {
        CONTAINED_TYPE var;
		ISystemPersistencyNode *piChildNode=piNode->GetNode(x);
		CMRPersistentSimpleReferenceT<CONTAINED_TYPE> *pRef=MRCreateReference(&var,piChildNode->GetName());
        MRPersistencyInitialize(pRef);
        bOk=MRPersistencyLoad(piChildNode,pRef);
        if(bOk){pItem->GetValueAddress()->insert(pItem->GetValueAddress()->end(),*pRef->GetValueAddress());}
        delete pRef;
        pRef=NULL;
        if(!bOk)
		{
			RTTRACE("GameRunTimeLib::MRLoadFromContainer-> Failed To Load container item %s, result 0x%08x",piChildNode->GetDebugInfoPath().c_str(),bOk);
			bFinalOk=bOk;
		}
    }

    return bFinalOk;
}

template<typename T1,typename CONTAINED_TYPE>
bool MRSaveToContainer(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<T1> *pItem)
{
    if(piNode==NULL){return false;}
    piNode->Clear();

    bool bOk=true,bFinalOk=true;
    if(bFinalOk)
    {
        typename T1::iterator i;
		int x=0;
        for(x=0,i=pItem->GetValueAddress()->begin();i!=pItem->GetValueAddress()->end();i++,x++)
        {
			char sItemName[200];
			sprintf(sItemName,"Item%d",x);
			ISystemPersistencyNode *piChildNode=piNode->AddNode(sItemName);
            CMRPersistentSimpleReferenceT<CONTAINED_TYPE> *pRef=MRCreateReference(&(*i),sItemName);
            bOk=MRPersistencySave(piChildNode,pRef);
            if(!bOk)
			{
				RTTRACE("GameRunTimeLib::MRLoadFromContainer-> Failed To Save container item %s, result 0x%08x",piChildNode->GetDebugInfoPath().c_str(),bOk);
				bFinalOk=bOk;
			}
            delete pRef;
            pRef=NULL;
        }
    }
    return bFinalOk;
}

/////////////////////////////////////////////////////
// Funciones para guardar contenedores asociativos

template<typename T1,typename KEY_TYPE,typename CONTAINED_TYPE>
bool MRLoadFromContainer(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<T1>*pItem)
{
 /*   ISystemPersistencyNode *piNode=piParent->GetNode(pItem->GetName());
    if(piNode==NULL){return false;}
    pItem->GetValueAddress()->clear();

    DWORD itemCount=piNode->GetItemCount();

    bool bOk=true,bFinalOk=true;
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
        bOk=MRPersistencyLoad(piNode,pKeyRef);
        if(bOk){bOk=MRPersistencyLoad(piNode,pValueRef);}
        if(bOk){pItem->GetValueAddress()->insert(T1::value_type(key,value));}
        delete pKeyRef;
        delete pValueRef;
        pKeyRef=NULL;
        pValueRef=NULL;
        if(!bOk){bFinalOk=bOk;}
    }
    return bFinalOk;*/
	return false;
}

template<typename T1,typename KEY_TYPE,typename CONTAINED_TYPE>
bool MRSaveToContainer(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<T1> *pItem)
{
	return false;
	/*
    ISystemPersistencyNode *piNode=piParent->AddNode(pItem->GetName());
    if(piNode==NULL){return false;}
    piNode->Clear();

    char countValue[512];
    sprintf(countValue,"%d",pItem->GetValueAddress()->size());
    SSystemProperty countProp;
    countProp.name="ItemCount";
    countProp.value=countValue;
    piNode->AddProperty(countProp);

    DWORD itemCount=0;
    bool bOk=true,bFinalOk=true;
    if(bFinalOk)
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
            bOk=MRPersistencySave(piNode,pKeyRef);
            if(bOk){bOk=MRPersistencySave(piNode,pValueRef);}
            if(bOk){itemCount++;}
            if(bOk){bFinalOk=bOk;}
            delete pKeyRef;
            delete pValueRef;
            pKeyRef=NULL;
            pValueRef=NULL;
        }
    }
    return bFinalOk;*/
}

/////////////////////////////////////////////////////
// Funciones para guardar contenedores, usan las funciones anteriores
// para guardar todos los contenedores de la misma manera.

template<typename T1>void MRPersistencyInitialize(CMRPersistentReferenceT<std::list<T1> >*pItem){}
template<typename T1>void MRPersistencyFree(CMRPersistentReferenceT<std::list<T1> >*pItem){}
template<typename T1>bool MRPersistencyRemove(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::list<T1> >*pItem){piParent->DeleteNode(pItem->GetName());return true;}
template<typename T1>bool MRPersistencyLoad(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::list<T1> > *pItem){return MRLoadFromContainer<std::list<T1> , T1 >(piParent,pItem);}
template<typename T1>bool MRPersistencySave(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::list<T1> > *pItem){return MRSaveToContainer<std::list<T1> , T1 >(piParent,pItem);}

template<typename T1>void MRPersistencyInitialize(CMRPersistentReferenceT<std::deque<T1> >*pItem){}
template<typename T1>void MRPersistencyFree(CMRPersistentReferenceT<std::deque<T1> >*pItem){}
template<typename T1>bool MRPersistencyRemove(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::deque<T1> >*pItem){piParent->DeleteNode(pItem->GetName());return true;}
template<typename T1>bool MRPersistencyLoad(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::deque<T1> > *pItem){return MRLoadFromContainer<std::deque<T1> , T1 >(piParent,pItem);}
template<typename T1>bool MRPersistencySave(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::deque<T1> > *pItem){return MRSaveToContainer<std::deque<T1> , T1 >(piParent,pItem);}

template<typename T1>void MRPersistencyInitialize(CMRPersistentReferenceT<std::vector<T1> >*pItem){}
template<typename T1>void MRPersistencyFree(CMRPersistentReferenceT<std::vector<T1> >*pItem){}
template<typename T1>bool MRPersistencyRemove(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::vector<T1> >*pItem){piParent->DeleteNode(pItem->GetName());return true;}
template<typename T1>bool MRPersistencyLoad(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::vector<T1> > *pItem){return MRLoadFromContainer<std::vector<T1> , T1 >(piParent,pItem);}
template<typename T1>bool MRPersistencySave(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::vector<T1> > *pItem){return MRSaveToContainer<std::vector<T1> , T1 >(piParent,pItem);}

template<typename T1>void MRPersistencyInitialize(CMRPersistentReferenceT<std::set<T1> >*pItem){}
template<typename T1>void MRPersistencyFree(CMRPersistentReferenceT<std::set<T1> >*pItem){}
template<typename T1>bool MRPersistencyRemove(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::set<T1> >*pItem){piParent->DeleteNode(pItem->GetName());return true;}
template<typename T1>bool MRPersistencyLoad(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::set<T1> > *pItem){return MRLoadFromContainer<std::set<T1> , T1 >(piParent,pItem);}
template<typename T1>bool MRPersistencySave(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::set<T1> > *pItem){return MRSaveToContainer<std::set<T1> , T1 >(piParent,pItem);}

template<typename T1,typename T2>void MRPersistencyInitialize(CMRPersistentReferenceT<std::map<T1,T2> >*pItem){}
template<typename T1,typename T2>void MRPersistencyFree(CMRPersistentReferenceT<std::map<T1,T2> >*pItem){}
template<typename T1,typename T2>bool MRPersistencyRemove(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::map<T1,T2> >*pItem){piParent->DeleteNode(pItem->GetName());return true;}
template<typename T1,typename T2>bool MRPersistencyLoad(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::map<T1,T2> > *pItem){return MRLoadFromContainer<std::map<T1,T2> , T1, T2 >(piParent,pItem);}
template<typename T1,typename T2>bool MRPersistencySave(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::map<T1,T2> > *pItem){return MRSaveToContainer<std::map<T1,T2> , T1 , T2>(piParent,pItem);}

template<typename T1,typename T2>void MRPersistencyInitialize(CMRPersistentReferenceT<std::multimap<T1,T2> >*pItem){}
template<typename T1,typename T2>void MRPersistencyFree(CMRPersistentReferenceT<std::multimap<T1,T2> >*pItem){}
template<typename T1,typename T2>bool MRPersistencyRemove(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::multimap<T1,T2> >*pItem){piParent->DeleteNode(pItem->GetName());return true;}
template<typename T1,typename T2>bool MRPersistencyLoad(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::multimap<T1,T2> > *pItem){return MRLoadFromContainer<std::multimap<T1,T2> , T1, T2 >(piParent,pItem);}
template<typename T1,typename T2>bool MRPersistencySave(ISystemPersistencyNode *piParent,CMRPersistentReferenceT<std::multimap<T1,T2> > *pItem){return MRSaveToContainer<std::multimap<T1,T2> , T1 , T2>(piParent,pItem);}
