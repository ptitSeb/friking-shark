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

#include <GameManagers.h>

DECLARE_CUSTOM_WRAPPER1(CKeyCaptureDialogWrapper,IKeyCaptureDialog,m_piKeyCaptureDialog)


class CControlsDialog: virtual public CGameDialogBase,virtual public IGameGUIButtonEvents, virtual public IControlsDialog
{
	CKeyCaptureDialogWrapper m_KeyCaptureDialog;
	
	SKeyMapping m_UpKeyMapping;
	SKeyMapping m_DownKeyMapping;
	SKeyMapping m_LeftKeyMapping;
	SKeyMapping m_RightKeyMapping;
	SKeyMapping m_FireKeyMapping;
	SKeyMapping m_BombKeyMapping;
	
	IGameGUIButton *m_piBTUpA;
	IGameGUIButton *m_piBTDownA;
	IGameGUIButton *m_piBTLeftA;
	IGameGUIButton *m_piBTRightA;
	IGameGUIButton *m_piBTBombA;
	IGameGUIButton *m_piBTFireA;
	IGameGUIButton *m_piBTOk;
	IGameGUIButton *m_piBTCancel;
	
	
	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY_EX("UpA",m_piBTUpA,IGameGUIButtonEvents);	
		CHILD_MAP_ENTRY_EX("DownA",m_piBTDownA,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("LeftA",m_piBTLeftA,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("RightA",m_piBTRightA,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("BombA",m_piBTBombA,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("FireA",m_piBTFireA,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Ok",m_piBTOk,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Cancel",m_piBTCancel,IGameGUIButtonEvents);
	END_CHILD_MAP()
	
	BEGIN_PROP_MAP(CControlsDialog)
		PROP_CLASS_CHAIN(CGameDialogBase);
		PROP(m_KeyCaptureDialog,"KeyCaptureDialog")
	END_PROP_MAP()
	
	void UpdateRow(IGameGUIButton *piKey1Button, SKeyMapping *pKeyMapping);
	void UpdateGUI();
	
	void ProcessKeyChange(SKeyMapping *pKeyMapping, unsigned int nIndex);
	
public:
	void OnInitDialog();
	void OnKeyDown(int nKey,bool *pbProcessed);
	
	bool SelectControls(IGameWindow *piParent,IPlayerProfile *piProfile);
	
	// IGameButtonEvents

	void OnButtonClicked(IGameGUIButton *piControl);

	CControlsDialog(void);
	~CControlsDialog(void);
};
