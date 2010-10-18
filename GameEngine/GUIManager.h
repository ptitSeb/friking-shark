// GUIManager.h: interface for the CGUIManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUIMANAGER_H__01B2BD08_9C7A_4E9C_B985_F5EDF0BCAE52__INCLUDED_)
#define AFX_GUIMANAGER_H__01B2BD08_9C7A_4E9C_B985_F5EDF0BCAE52__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GameRender.h"

#define GUI_STATE_LOADING_SCENARIO		1
#define GUI_STATE_PLAYING				2
#define GUI_STATE_SHOW_SHADOW_BUFFER	3

class CGUIManager:virtual public CSystemObjectBase, virtual public IGUIManager
{
	DWORD	  m_dwState;
	string    m_sLoadString;
	CTexture *m_pLoadTexture;
public:
	DWORD GetState(){return m_dwState;}
	void SetState(DWORD dwState);
	void SetLoadString(const char *format, ...);

	void Load(CGESFileType *pFile);
	void Render();

	CGUIManager();
	virtual ~CGUIManager();

};

#endif // !defined(AFX_GUIMANAGER_H__01B2BD08_9C7A_4E9C_B985_F5EDF0BCAE52__INCLUDED_)
