#include "./stdafx.h"
#include "./GameRunTimeLib.h"
#include "./PersistentTypes.h"

HRESULT MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<DWORD> *pItem)
{
    char sTemp[1024]={0};
    sprintf(sTemp,"%d",*pItem->GetValueAddress());
	if(piNode){piNode->SetValue(sTemp);}
    return piNode?S_OK:E_FAIL;
}

HRESULT MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<DWORD> *pItem)
{
    pItem->SetDefaultValue();
	if(!piNode || !piNode->GetValue()){return E_FAIL;}
	(*pItem->GetValueAddress())=atoi(piNode->GetValue());
	return S_OK;
}


HRESULT MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<DWORD> *pItem)
{
	return S_OK;
}

HRESULT MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<int> *pItem)
{
	char sTemp[1024]={0};
	sprintf(sTemp,"%d",*pItem->GetValueAddress());
	if(piNode){piNode->SetValue(sTemp);}
	return piNode?S_OK:E_FAIL;
}

HRESULT MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<int> *pItem)
{
	pItem->SetDefaultValue();
	if(!piNode || !piNode->GetValue()){return E_FAIL;}
	(*pItem->GetValueAddress())=atoi(piNode->GetValue());
	return S_OK;
}

HRESULT MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<int> *pItem)
{
	return S_OK;
}

HRESULT MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<bool> *pItem)
{
	char sTemp[1024]={0};
	sprintf(sTemp,"%d",*pItem->GetValueAddress());
	if(piNode){piNode->SetValue(sTemp);}
	return piNode?S_OK:E_FAIL;
}

HRESULT MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<bool> *pItem)
{
	pItem->SetDefaultValue();
	if(!piNode || !piNode->GetValue()){return E_FAIL;}
	(*pItem->GetValueAddress())=atoi(piNode->GetValue())?true:false;
	return S_OK;
}


HRESULT MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<bool> *pItem)
{
	return S_OK;
}

HRESULT MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<float> *pItem)
{
	char sTemp[1024]={0};
	sprintf(sTemp,"%f",*pItem->GetValueAddress());
	if(piNode){piNode->SetValue(sTemp);}
	return piNode?S_OK:E_FAIL;
}

HRESULT MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<float> *pItem)
{
	pItem->SetDefaultValue();
	if(!piNode || !piNode->GetValue()){return E_FAIL;}
	(*pItem->GetValueAddress())=(float)atof(piNode->GetValue());
	return S_OK;
}


HRESULT MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<float> *pItem)
{
	return S_OK;
}

HRESULT MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<double> *pItem)
{
	char sTemp[1024]={0};
	sprintf(sTemp,"%f",*pItem->GetValueAddress());
	if(piNode){piNode->SetValue(sTemp);}
	return piNode?S_OK:E_FAIL;
}

HRESULT MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<double> *pItem)
{
	pItem->SetDefaultValue();
	if(!piNode || !piNode->GetValue()){return E_FAIL;}
	(*pItem->GetValueAddress())=atof(piNode->GetValue());
	return S_OK;
}


HRESULT MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<double> *pItem)
{
	return S_OK;
}


HRESULT MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<std::string> *pItem)
{
	if(piNode){piNode->SetValue(pItem->GetValueAddress()->c_str());}
	return piNode?S_OK:E_FAIL;
}

HRESULT MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<std::string> *pItem)
{
	pItem->SetDefaultValue();
	if(!piNode || !piNode->GetValue()){return E_FAIL;}
	(*pItem->GetValueAddress())=piNode->GetValue();
	return S_OK;}


HRESULT MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<std::string> *pItem)
{
	return S_OK;
}
void MRPersistencyInitialize(CMRPersistentReferenceT<DWORD> *pItem)			{(*pItem->GetValueAddress())=0;}
void MRPersistencyInitialize(CMRPersistentReferenceT<int> *pItem)			{(*pItem->GetValueAddress())=0;}
void MRPersistencyInitialize(CMRPersistentReferenceT<float> *pItem)			{(*pItem->GetValueAddress())=0;}
void MRPersistencyInitialize(CMRPersistentReferenceT<double> *pItem)		{(*pItem->GetValueAddress())=0;}
void MRPersistencyInitialize(CMRPersistentReferenceT<bool> *pItem)			{(*pItem->GetValueAddress())=0;}
void MRPersistencyInitialize(CMRPersistentReferenceT<std::string> *pItem)   {(*pItem->GetValueAddress())="";}


void MRPersistencyFree(CMRPersistentReferenceT<DWORD> *prop){}
void MRPersistencyFree(CMRPersistentReferenceT<int> *prop){}
void MRPersistencyFree(CMRPersistentReferenceT<float> *prop){}
void MRPersistencyFree(CMRPersistentReferenceT<double> *prop){}
void MRPersistencyFree(CMRPersistentReferenceT<bool> *pItem){}
void MRPersistencyFree(CMRPersistentReferenceT<std::string> *pItem){}

