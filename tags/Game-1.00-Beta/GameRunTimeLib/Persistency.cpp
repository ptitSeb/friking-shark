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
bool PersistencyItemListSave(IMRPersistencyItem **ppiList,ISystemPersistencyNode *piNode,const char *pPrefixName)
{
    bool bOk=true,bFinalOk=true;
    if(pPrefixName==NULL){piNode->Clear();}
    int x=0;
    if(ppiList)
    {
        while(ppiList[x]!=NULL)
        {
			ISystemPersistencyNode *piChildNode=piNode->AddNode(ppiList[x]->GetName());
            bOk=ppiList[x]->Remove(piChildNode);
            bOk=ppiList[x]->Save(piChildNode);
            if(!bOk)
            {
				RTTRACE("GameRunTimeLib::PersistencyLoad-> Failed To Save item %s\\%s",piNode->GetDebugInfoPath().c_str(),ppiList[x]->GetName());
                bFinalOk=bOk;
            }
            x++;
        }
    }
    return bFinalOk;
} 
bool PersistencyItemListLoad(IMRPersistencyItem **ppiList,ISystemPersistencyNode *piNode,const char *pPrefixName)
{
    bool bOk=true,bFinalOk=true;
    int x=0;
    if(ppiList)
    {
        while(ppiList[x]!=NULL)
        {
			ISystemPersistencyNode *piChildNode=piNode->GetNode(ppiList[x]->GetName());
            bOk=ppiList[x]->Load(piChildNode);
            if(!bOk)
            {
                RTTRACE("GameRunTimeLib::PersistencyLoad-> Failed To Load item %s\\%s",piNode->GetDebugInfoPath().c_str(),ppiList[x]->GetName());
                bFinalOk=bOk;
            }
            x++;
        }
    }
    return bFinalOk;
}
bool PersistencyItemListRemove(IMRPersistencyItem **ppiList,ISystemPersistencyNode *piNode,const char *pPrefixName)
{
    bool bOk=true,bFinalOk=true;
    int x=0;
    if(ppiList)
    {
        while(ppiList[x]!=NULL)
        {
			ISystemPersistencyNode *piChildNode=piNode->GetNode(ppiList[x]->GetName());
            bOk=ppiList[x]->Remove(piChildNode);
            if(!bOk)
            {
                bFinalOk=bOk;
            }
            x++;
        }
    }
    return bFinalOk;
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

void CSystemSerializableBase::PersistencyDefaultValue(const char *pMapName,const char *pPrefixName){}
bool CSystemSerializableBase::PersistencySave(ISystemPersistencyNode *piNode,const char *pMapName,const char *pPrefixName){return true;}
bool CSystemSerializableBase::PersistencyLoad(ISystemPersistencyNode *piNode,const char *pMapName,const char *pPrefixName){return true;}
bool CSystemSerializableBase::PersistencyRemove(ISystemPersistencyNode *piNode,const char *pMapName,const char *pPrefixName){return true;}
void CSystemSerializableBase::PersistencyInitialize(const char *pMapName,const char *pPrefixName){}
void CSystemSerializableBase::PersistencyFree(const char *pMapName,const char *pPrefixName){}

// ISystemSerializable

bool CSystemSerializableBase::Serialize(ISystemPersistencyNode *piNode){return PersistencySave(piNode,NULL);}
bool CSystemSerializableBase::Unserialize(ISystemPersistencyNode *piNode){return PersistencyLoad(piNode,NULL);}
