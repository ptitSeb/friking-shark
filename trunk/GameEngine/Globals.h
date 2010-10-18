// Globals.h: interface for the Globals class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLOBALS_H__283B9BD5_3BC0_4975_AD39_56072033E8A9__INCLUDED_)
#define AFX_GLOBALS_H__283B9BD5_3BC0_4975_AD39_56072033E8A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#endif // !defined(AFX_GLOBALS_H__283B9BD5_3BC0_4975_AD39_56072033E8A9__INCLUDED_)

#include "Log.h"

class CPlayAreaManager;
class CGUIManager;
class CGameScreen;
class CEntity;
class CDetailLevelModel;

class CGlobals
{
public:
	static bool Init();
	static void Destroy();
};

void LOG(char *format,...);
void LOG_ERROR_AND_EXIT(char *pCaption,char *format, ...);






