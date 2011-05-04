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
#include "./PersistentTypes.h"

bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<unsigned int> *pItem)
{
    char sTemp[1024]={0};
    sprintf(sTemp,"%d",*pItem->GetValueAddress());
	if(piNode){piNode->SetValue(sTemp);}
    return piNode?true:false;
}

bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<unsigned int> *pItem)
{
    pItem->SetDefaultValue();
	if(!piNode || !piNode->GetValue()){return false;}
	(*pItem->GetValueAddress())=atoi(piNode->GetValue());
	return true;
}


bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<unsigned int> *pItem)
{
	return true;
}

bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<int> *pItem)
{
	char sTemp[1024]={0};
	sprintf(sTemp,"%d",*pItem->GetValueAddress());
	if(piNode){piNode->SetValue(sTemp);}
	return piNode?true:false;
}

bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<int> *pItem)
{
	pItem->SetDefaultValue();
	if(!piNode || !piNode->GetValue()){return false;}
	(*pItem->GetValueAddress())=atoi(piNode->GetValue());
	return true;
}

bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<int> *pItem)
{
	return true;
}

bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<bool> *pItem)
{
	char sTemp[1024]={0};
	sprintf(sTemp,"%d",*pItem->GetValueAddress());
	if(piNode){piNode->SetValue(sTemp);}
	return piNode?true:false;
}

bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<bool> *pItem)
{
	pItem->SetDefaultValue();
	if(!piNode || !piNode->GetValue()){return false;}
	(*pItem->GetValueAddress())=atoi(piNode->GetValue())?true:false;
	return true;
}


bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<bool> *pItem)
{
	return true;
}

bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<float> *pItem)
{
	char sTemp[1024]={0};
	sprintf(sTemp,"%f",*pItem->GetValueAddress());
	if(piNode){piNode->SetValue(sTemp);}
	return piNode?true:false;
}

bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<float> *pItem)
{
	pItem->SetDefaultValue();
	if(!piNode || !piNode->GetValue()){return false;}
	(*pItem->GetValueAddress())=(float)atof(piNode->GetValue());
	return true;
}


bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<float> *pItem)
{
	return true;
}

bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<double> *pItem)
{
	char sTemp[1024]={0};
	sprintf(sTemp,"%f",*pItem->GetValueAddress());
	if(piNode){piNode->SetValue(sTemp);}
	return piNode?true:false;
}

bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<double> *pItem)
{
	pItem->SetDefaultValue();
	if(!piNode || !piNode->GetValue()){return false;}
	(*pItem->GetValueAddress())=atof(piNode->GetValue());
	return true;
}


bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<double> *pItem)
{
	return true;
}


bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<std::string> *pItem)
{
	if(piNode){piNode->SetValue(pItem->GetValueAddress()->c_str());}
	return piNode?true:false;
}

bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<std::string> *pItem)
{
	pItem->SetDefaultValue();
	if(!piNode || !piNode->GetValue()){return false;}
	(*pItem->GetValueAddress())=piNode->GetValue();
	return true;}


bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<std::string> *pItem)
{
	return true;
}
void MRPersistencyInitialize(CMRPersistentReferenceT<unsigned int> *pItem)			{(*pItem->GetValueAddress())=0;}
void MRPersistencyInitialize(CMRPersistentReferenceT<int> *pItem)			{(*pItem->GetValueAddress())=0;}
void MRPersistencyInitialize(CMRPersistentReferenceT<float> *pItem)			{(*pItem->GetValueAddress())=0;}
void MRPersistencyInitialize(CMRPersistentReferenceT<double> *pItem)		{(*pItem->GetValueAddress())=0;}
void MRPersistencyInitialize(CMRPersistentReferenceT<bool> *pItem)			{(*pItem->GetValueAddress())=0;}
void MRPersistencyInitialize(CMRPersistentReferenceT<std::string> *pItem)   {(*pItem->GetValueAddress())="";}


void MRPersistencyFree(CMRPersistentReferenceT<unsigned int> *prop){}
void MRPersistencyFree(CMRPersistentReferenceT<int> *prop){}
void MRPersistencyFree(CMRPersistentReferenceT<float> *prop){}
void MRPersistencyFree(CMRPersistentReferenceT<double> *prop){}
void MRPersistencyFree(CMRPersistentReferenceT<bool> *pItem){}
void MRPersistencyFree(CMRPersistentReferenceT<std::string> *pItem){}

