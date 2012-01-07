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

class CControlsDialog: virtual public CGameDialogBase,virtual public IGameGUISliderEvents,virtual public IGameGUIButtonEvents, virtual public IControlsDialog
{
	bool m_bShowKeyboardControls;	

	CKeyCaptureDialogWrapper m_KeyCaptureDialog;
	
	SKeyMapping m_UpKeyMapping;
	SKeyMapping m_DownKeyMapping;
	SKeyMapping m_LeftKeyMapping;
	SKeyMapping m_RightKeyMapping;
	SKeyMapping m_FireKeyMapping;
	SKeyMapping m_BombKeyMapping;

	double m_dJoystickDeadZone;

	SJoystickButtonMapping m_FireJoyMapping;
	SJoystickButtonMapping m_BombJoyMapping;
	SJoystickButtonMapping m_BackJoyMapping;

	IGameWindow    *m_piSTKeyboardControls;
	IGameWindow    *m_piSTJoystickControls;
	IGameGUIButton *m_piBTControls;

	IGameGUIButton *m_piBTKeyUp;
	IGameGUIButton *m_piBTKeyDown;
	IGameGUIButton *m_piBTKeyLeft;
	IGameGUIButton *m_piBTKeyRight;
	IGameGUIButton *m_piBTKeyBomb;
	IGameGUIButton *m_piBTKeyFire;
	IGameGUIButton *m_piBTJoyBomb;
	IGameGUIButton *m_piBTJoyFire;
	IGameGUIButton *m_piBTJoyBack;
	IGameGUIButton *m_piBTOk;
	IGameGUISlider *m_piSLJoyDeadZone;

		
	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY("KeyboardControls",m_piSTKeyboardControls);	
		CHILD_MAP_ENTRY("JoystickControls",m_piSTJoystickControls);	
		CHILD_MAP_ENTRY_EX("Controls",m_piBTControls,IGameGUIButtonEvents);	
		CHILD_MAP_ENTRY_EX("KeyUp",m_piBTKeyUp,IGameGUIButtonEvents);	
		CHILD_MAP_ENTRY_EX("KeyDown",m_piBTKeyDown,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("KeyLeft",m_piBTKeyLeft,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("KeyRight",m_piBTKeyRight,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("KeyBomb",m_piBTKeyBomb,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("KeyFire",m_piBTKeyFire,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("JoyDeadZone",m_piSLJoyDeadZone,IGameGUISliderEvents);
		CHILD_MAP_ENTRY_EX("JoyBomb",m_piBTJoyBomb,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("JoyFire",m_piBTJoyFire,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("JoyBack",m_piBTJoyBack,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Ok",m_piBTOk,IGameGUIButtonEvents);
	END_CHILD_MAP()
	
	BEGIN_PROP_MAP(CControlsDialog)
		PROP_CLASS_CHAIN(CGameDialogBase);
		PROP(m_KeyCaptureDialog,"KeyCaptureDialog")
	END_PROP_MAP()
	
	void UpdateRow(IGameGUIButton *piKey1Button, SKeyMapping *pKeyMapping);
	void UpdateRow(IGameGUIButton *piJoyButton,SJoystickButtonMapping *pJoyMapping);
	void UpdateGUI();
	void UpdateGUIControls();
	
	void ProcessKeyChange(SKeyMapping *pKeyMapping);
	void ProcessJoystickButtonChange(SJoystickButtonMapping *pJoyMapping);
	
public:
	void OnInitDialog();
	void OnKeyDown(int nKey,bool *pbProcessed);
	
	bool SelectControls(IGameWindow *piParent,IPlayerProfile *piProfile);
	
	// IGameButtonEvents

	void OnButtonClicked(IGameGUIButton *piControl);

	// IGameSliderEvents

	void OnSliderValueChanged(IGameGUISlider *piControl,double dValue);

	CControlsDialog(void);
	~CControlsDialog(void);
};
