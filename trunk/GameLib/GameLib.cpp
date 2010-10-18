// GameLib.cpp: implementation of the VectorLib class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning ( disable : 4786 )

#include "stdafx.h"
#include "GameLib.h"
#include "float.h"
#include <crtdbg.h>


HRESULT MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CVector> *pItem)
{
	char sTemp[1024]={0};
	CVector *pVector=pItem->GetValueAddress();
	sprintf(sTemp,"%f,%f,%f",pVector->c[0],pVector->c[1],pVector->c[2]);
	if(piNode){piNode->SetValue(sTemp);}
	return piNode?S_OK:E_FAIL;
}

HRESULT MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CVector> *pItem)
{
	pItem->SetDefaultValue();
	if(!piNode || !piNode->GetValue()){return E_FAIL;}

	char sTemp[1024]={0};
	strcpy(sTemp,piNode->GetValue());
	CVector vPos;
	char *pToken=strtok(sTemp,", ");
	if(pToken){vPos.c[0]=atof(pToken);pToken=strtok(NULL,", ");}
	if(pToken){vPos.c[1]=atof(pToken);pToken=strtok(NULL,", ");}
	if(pToken){vPos.c[2]=atof(pToken);}
	(*pItem->GetValueAddress())=vPos;
	return S_OK;
}

HRESULT MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CVector> *pItem)
{
	return S_OK;
}

void    MRPersistencyInitialize(CMRPersistentReferenceT<CVector> *pItem){(*pItem->GetValueAddress())=Origin;}
void    MRPersistencyFree(CMRPersistentReferenceT<CVector> *pItem){}

HRESULT MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CRGBColor> *pItem)
{
	char sTemp[1024]={0};
	CRGBColor *pVector=pItem->GetValueAddress();
	sprintf(sTemp,"%f,%f,%f",pVector->c[0]*255.0,pVector->c[1]*255.0,pVector->c[2]*255.0);
	if(piNode){piNode->SetValue(sTemp);}
	return piNode?S_OK:E_FAIL;
}

HRESULT MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CRGBColor> *pItem)
{
	pItem->SetDefaultValue();
	if(!piNode || !piNode->GetValue()){return E_FAIL;}

	char sTemp[1024]={0};
	strcpy(sTemp,piNode->GetValue());
	CRGBColor vPos;
	char *pToken=strtok(sTemp,", ");
	if(pToken){vPos.c[0]=atof(pToken)/255.0;pToken=strtok(NULL,", ");}
	if(pToken){vPos.c[1]=atof(pToken)/255.0;pToken=strtok(NULL,", ");}
	if(pToken){vPos.c[2]=atof(pToken)/255.0;}
	(*pItem->GetValueAddress())=vPos;
	return S_OK;
}

HRESULT MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CRGBColor> *pItem)
{
	return S_OK;
}


void    MRPersistencyInitialize(CMRPersistentReferenceT<CRGBColor> *pItem){(*pItem->GetValueAddress())=Origin;}
void    MRPersistencyFree(CMRPersistentReferenceT<CRGBColor> *pItem){}

HRESULT MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<C3DSVector> *pItem)
{
	char sTemp[1024]={0};
	CVector tempVector=FromOpenGLTo3DS(*pItem->GetValueAddress());
	sprintf(sTemp,"%f,%f,%f",tempVector.c[0],tempVector.c[1],tempVector.c[2]);
	if(piNode){piNode->SetValue(sTemp);}
	return piNode?S_OK:E_FAIL;
}

HRESULT MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<C3DSVector> *pItem)
{
	pItem->SetDefaultValue();
	if(!piNode || !piNode->GetValue()){return E_FAIL;}

	char sTemp[1024]={0};
	strcpy(sTemp,piNode->GetValue());
	CVector vPos;
	char *pToken=strtok(sTemp,", ");
	if(pToken){vPos.c[0]=atof(pToken);pToken=strtok(NULL,", ");}
	if(pToken){vPos.c[1]=atof(pToken);pToken=strtok(NULL,", ");}
	if(pToken){vPos.c[2]=atof(pToken);}
	(*pItem->GetValueAddress())=From3DSToOpenGL(vPos);
	return S_OK;
}

HRESULT MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<C3DSVector> *pItem)
{
	return S_OK;
}

void    MRPersistencyInitialize(CMRPersistentReferenceT<C3DSVector> *pItem){(*pItem->GetValueAddress())=Origin;}
void    MRPersistencyFree(CMRPersistentReferenceT<C3DSVector> *pItem){}
