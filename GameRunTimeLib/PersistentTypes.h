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

template<typename T1,typename T2> static void MRPersistencyAsign(T1 *pVar1,T2 *pVar2){(*pVar1)=(*pVar2);}

/////////////////////////////////////////////////////
// Funciones para guardar tipos fundamentales

bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<unsigned int> *prop);
bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<int> *prop);
bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<float> *prop);
bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<double> *prop);
bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<bool> *pItem);
bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<std::string> *pItem);

bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<bool> *pItem);
bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<int> *pItem);
bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<unsigned int> *prop);
bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<float> *prop);
bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<double> *prop);
bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<std::string> *pItem);

bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<unsigned int> *prop);
bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<int> *prop);
bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<float> *prop);
bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<double> *prop);
bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<bool> *pItem);
bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<std::string> *pItem);

void MRPersistencyInitialize(CMRPersistentReferenceT<unsigned int> *prop);
void MRPersistencyInitialize(CMRPersistentReferenceT<int> *prop);
void MRPersistencyInitialize(CMRPersistentReferenceT<float> *prop);
void MRPersistencyInitialize(CMRPersistentReferenceT<double> *prop);
void MRPersistencyInitialize(CMRPersistentReferenceT<bool> *pItem);
void MRPersistencyInitialize(CMRPersistentReferenceT<std::string> *pItem);

void MRPersistencyFree(CMRPersistentReferenceT<unsigned int> *prop);
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
	unsigned int itemCount=piNode->GetNodeCount();
    for(unsigned int x=0;x<itemCount;x++)
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
			RTTRACE("GameRunTimeLib::MRLoadFromContainer-> Failed To Load container item %s",piChildNode->GetDebugInfoPath().c_str());
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
	// To keep the order of the elements we have to write the items as Item087 when needed
	// here we compute the number of the padding zeros needed to guarantee the ordering and make 
	// the text files as readable as possible
	unsigned int nDigits=0;
	unsigned int nElems=pItem->GetValueAddress()->size();
	while(nElems){nDigits++;nElems/=10;}

	char sFormatString[200]="Item%d";
	if(nDigits){sprintf(sFormatString,"Item%%0%dd",nDigits);}

    bool bOk=true,bFinalOk=true;
    if(bFinalOk)
    {
        typename T1::iterator i;
		int x=0;
        for(x=0,i=pItem->GetValueAddress()->begin();i!=pItem->GetValueAddress()->end();i++,x++)
        {
			char sItemName[200];
			sprintf(sItemName,sFormatString,x);
			ISystemPersistencyNode *piChildNode=piNode->AddNode(sItemName);
            CMRPersistentSimpleReferenceT<CONTAINED_TYPE> *pRef=MRCreateReference(&(*i),sItemName);
            bOk=MRPersistencySave(piChildNode,pRef);
            if(!bOk)
			{
				RTTRACE("GameRunTimeLib::MRLoadFromContainer-> Failed To Save container item %s",piChildNode->GetDebugInfoPath().c_str());
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

template<typename T1,typename KEY_TYPE,typename CONTENT_TYPE>
bool MRLoadFromContainer(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<T1>*pItem)
{
	if(piNode==NULL){return false;}
	pItem->GetValueAddress()->clear();
	
	bool bOk=true,bFinalOk=true;
	unsigned int itemCount=piNode->GetNodeCount();
	for(unsigned int x=0;x<itemCount;x++)
	{
		KEY_TYPE key;
		CONTENT_TYPE content;
		ISystemPersistencyNode *piChildNode=piNode->GetNode(x);
		ISystemPersistencyNode *piChildKeyNode=piChildNode?piChildNode->GetNode("Key"):NULL;
		ISystemPersistencyNode *piChildContentNode=piChildNode?piChildNode->GetNode("Content"):NULL;
		bOk=(piChildKeyNode!=NULL && piChildContentNode!=NULL);
		if(!bOk)
		{
			RTTRACE("GameRunTimeLib::MRLoadFromContainer-> Failed To Load associative container item %s",piChildNode->GetDebugInfoPath().c_str());
			bFinalOk=bOk;
		}
		if(bOk)
		{
			CMRPersistentSimpleReferenceT<KEY_TYPE>     *pKeyRef=MRCreateReference(&key,piChildKeyNode->GetName());
			CMRPersistentSimpleReferenceT<CONTENT_TYPE> *pContentRef=MRCreateReference(&content,piChildContentNode->GetName());
			MRPersistencyInitialize(pKeyRef);
			MRPersistencyInitialize(pContentRef);
			bOk=MRPersistencyLoad(piChildKeyNode,pKeyRef);
			if(!bOk){RTTRACE("GameRunTimeLib::MRLoadFromContainer-> Failed To Load key %s",piChildKeyNode->GetDebugInfoPath().c_str());}
			
			if(bOk)
			{
				bOk=MRPersistencyLoad(piChildContentNode,pContentRef);
				if(!bOk){RTTRACE("GameRunTimeLib::MRLoadFromContainer-> Failed To Load content %s",piChildContentNode->GetDebugInfoPath().c_str());}
			}
			if(bOk){pItem->GetValueAddress()->insert(std::pair<KEY_TYPE,CONTENT_TYPE>(*pKeyRef->GetValueAddress(),*pContentRef->GetValueAddress()));}
			delete pKeyRef;
			delete pContentRef;
			pKeyRef=NULL;
			pContentRef=NULL;
		}
	}
	
	return bFinalOk;
}

template<typename T1,typename KEY_TYPE,typename CONTENT_TYPE>
bool MRSaveToContainer(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<T1> *pItem)
{
	if(piNode==NULL){return false;}
	piNode->Clear();
	
	// To keep the order of the elements we have to write the items as Item087 when needed
	// here we compute the number of the padding zeros needed to guarantee the ordering and make 
	// the text files as readable as possible
	// Not really necesary here. It is included to simplify debugging as it is easier to find the config node.

	unsigned int nDigits=0;
	unsigned int nElems=pItem->GetValueAddress()->size();
	while(nElems){nDigits++;nElems/=10;}

	char sFormatString[200]="Item%d";
	if(nDigits){sprintf(sFormatString,"Item%%0%dd",nDigits);}

	bool bOk=true,bFinalOk=true;
	if(bFinalOk)
	{
		typename T1::iterator i;
		int x=0;
		for(x=0,i=pItem->GetValueAddress()->begin();i!=pItem->GetValueAddress()->end();i++,x++)
		{
			char sItemName[200];
			sprintf(sItemName,sFormatString,x);
			ISystemPersistencyNode *piChildNode=piNode->AddNode(sItemName);
			ISystemPersistencyNode *piChildKeyNode=piChildNode?piChildNode->AddNode("Key"):NULL;
			ISystemPersistencyNode *piChildContentNode=piChildNode?piChildNode->AddNode("Content"):NULL;
			bOk=(piChildKeyNode!=NULL && piChildContentNode!=NULL);
			if(bOk)
			{
				CMRPersistentSimpleReferenceT<KEY_TYPE> *pKeyRef=MRCreateReference(const_cast<KEY_TYPE*>(&(i->first)),piChildKeyNode->GetName());
				CMRPersistentSimpleReferenceT<CONTENT_TYPE> *pContentRef=MRCreateReference(&(i->second),piChildContentNode->GetName());
				bOk=MRPersistencySave(piChildKeyNode,pKeyRef);
				if(!bOk)
				{
					RTTRACE("GameRunTimeLib::MRLoadFromContainer-> Failed To Save container key %s",piChildKeyNode->GetDebugInfoPath().c_str());
					bFinalOk=bOk;
				}
				if(bOk)
				{
					bOk=MRPersistencySave(piChildContentNode,pContentRef);
					if(!bOk)
					{
						RTTRACE("GameRunTimeLib::MRLoadFromContainer-> Failed To Save container content %s",piChildContentNode->GetDebugInfoPath().c_str());
						bFinalOk=bOk;
					}
				}
				delete pKeyRef;
				delete pContentRef;
				pKeyRef=NULL;
				pContentRef=NULL;
			}
		}
	}
	return bFinalOk;
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
