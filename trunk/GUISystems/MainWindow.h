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

DECLARE_CUSTOM_WRAPPER1(CViewportWrapper,IGenericViewport,m_piViewport)
DECLARE_CUSTOM_WRAPPER1(CGameDialogWrapper,IGameDialog,m_piDialog)
DECLARE_CUSTOM_WRAPPER1(CGameInterfaceWrapper,IGameInterfaceWindow,m_piInterfaceWindow)
DECLARE_CUSTOM_WRAPPER2(CLevelOptionsDialogWrapper,IGameDialog,m_piDialog,ILevelOptions,m_piLevelOptions)
DECLARE_CUSTOM_WRAPPER1(CHighScoresTableWrapper,IHighScoresTable,m_piHighScoresTable)
DECLARE_CUSTOM_WRAPPER2(CHighScoresDialogWrapper,IGameDialog,m_piDialog,IHighScoresDialog,m_piHighScoresDialog)
DECLARE_CUSTOM_WRAPPER2(CControlsDialogWrapper,IGameDialog,m_piDialog,IControlsDialog,m_piControlsDialog)

enum eInterfaceStage
{
	eInterfaceStage_None,
	eInterfaceStage_MainMenu,
	eInterfaceStage_Playing,
	eInterfaceStage_LaunchNextLevel,
	eInterfaceStage_HighScores
};

class CMainWindow: virtual public CGameWindowBase,virtual public IGameInterfaceWindowEvents
{
	eInterfaceStage m_eStage;

	CConfigFile m_GUIConfigFile;
	CConfigFile m_HighScoresConfigFile;
	CConfigFile m_PlayerProfileConfigFile;

	EGameMode 			  m_eGameMode;
	IGameGUILabel        *m_piSTBackground;
	IGameInterfaceWindow *m_piGameInterface;

	CGameWindowWrapper m_BackgroundWindow;
	CGameDialogWrapper m_GameOverDialog;
	CGameDialogWrapper m_MainMenuDialog;
	CGameDialogWrapper m_GameMenuDialog;
	CGameDialogWrapper m_ConfirmationDialog;
	CGameDialogWrapper m_CreditsDialog;
	CPlayerProfileWrapper m_PlayerProfile;
	CLevelOptionsDialogWrapper m_LevelOptionsDialog;
	CHighScoresDialogWrapper m_HighScoresDialog;
	CHighScoresTableWrapper  m_HighScoresTable;
	CControlsDialogWrapper   m_ControlsDialog;
	
	unsigned int m_nCurrentLevel;
	unsigned int m_nPoints;
	unsigned int m_nLivesLeft;
	unsigned int m_nWeaponLevel;
	
	void Destroy();
	
public:
	// Sobrecarga para cambiar el valor por defecto del sistema de referencia.

	BEGIN_PROP_MAP(CMainWindow)
		PROP_CLASS_CHAIN(CGameWindowBase);
		PROP_VALUE_FLAGS(m_eReferenceSystem,"ReferenceSystem",eGameGUIReferenceSystem_Relative,MRPF_NORMAL|MRPF_OPTIONAL)
		END_PROP_MAP()

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY("Background",m_piSTBackground);
		CHILD_MAP_ENTRY_EX("GameInterface",m_piGameInterface,IGameInterfaceWindowEvents);
	END_CHILD_MAP()

	bool InitWindow(IGameWindow *piParent,bool bPopup);
	void OnKeyDown(int nKey,bool *pbProcessed);
	void OnWantFocus(bool *pbWant);
	void OnDraw(IGenericRender *piRender);
	
	void	OnScenarioFinished(eScenarioFinishedReason eReason,unsigned int nPoints, unsigned int nLivesLeft,unsigned int nWeaponLevel);
	void	OnGameOverCourtainClosed();
	
	CMainWindow(void);
	~CMainWindow(void);
};