// GAME.h
//
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_GAMELIB_H__B628EDC0_7542_4E57_9003_82D1A3DFD6FC__INCLUDED_)
#define AFX_GAMELIB_H__B628EDC0_7542_4E57_9003_82D1A3DFD6FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef WIN32
  #pragma warning ( disable : 4786 )
#endif

#include <windows.h>
#include <math.h>
#include <list>
#include <vector>
#include <map>
#include <deque>
#include <string>

#define YAW		0
#define PITCH	1
#define ROLL	2

#include "VectorLib.h"
#include "GameRunTimeLib.h"

bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CVector> *pItem);
bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CVector> *pItem);
bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CVector> *pItem);
void MRPersistencyInitialize(CMRPersistentReferenceT<CVector> *pItem);
void MRPersistencyFree(CMRPersistentReferenceT<CVector> *pItem);

bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CRGBColor> *pItem);
bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CRGBColor> *pItem);
bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CRGBColor> *pItem);
void MRPersistencyInitialize(CMRPersistentReferenceT<CRGBColor> *pItem);
void MRPersistencyFree(CMRPersistentReferenceT<CRGBColor> *pItem);

bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<C3DSVector> *pItem);
bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<C3DSVector> *pItem);
bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<C3DSVector> *pItem);
void MRPersistencyInitialize(CMRPersistentReferenceT<C3DSVector> *pItem);
void MRPersistencyFree(CMRPersistentReferenceT<C3DSVector> *pItem);

using namespace std;
#ifndef GAME_LIB_PROYECT
    #include "ResourceStore.h"
    #include "GameSpecs.h"
#endif
#endif // !defined(AFX_VECTORLIB_H__B628EDC0_7542_4E57_9003_82D1A3DFD6FC__INCLUDED_)
