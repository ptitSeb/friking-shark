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
DECLARE_CUSTOM_WRAPPER2(CLevelOptionsDialogWrapper,IGameDialog,m_piDialog,ILevelOptions,m_piLevelOptions)
DECLARE_CUSTOM_WRAPPER2(CAudioOptionsDialogWrapper,IGameDialog,m_piDialog,IAudioOptions,m_piAudioOptions)
DECLARE_CUSTOM_WRAPPER2(CVideoOptionsDialogWrapper,IGameDialog,m_piDialog,IVideoOptions,m_piVideoOptions)
DECLARE_CUSTOM_WRAPPER1(CHighScoresTableWrapper,IHighScoresTable,m_piHighScoresTable)
DECLARE_CUSTOM_WRAPPER2(CHighScoresDialogWrapper,IGameDialog,m_piDialog,IHighScoresDialog,m_piHighScoresDialog)
DECLARE_CUSTOM_WRAPPER2(CControlsDialogWrapper,IGameDialog,m_piDialog,IControlsDialog,m_piControlsDialog)
DECLARE_CUSTOM_WRAPPER2(CLoadDialogWrapper,IGameDialog,m_piDialog,ILoadDialog,m_piLoadDialog)
DECLARE_CUSTOM_WRAPPER2(CSaveDialogWrapper,IGameDialog,m_piDialog,ISaveDialog,m_piSaveDialog)
DECLARE_CUSTOM_WRAPPER2(CGameMenuWrapper,IGameDialog,m_piDialog,IGameMenu,m_piGameMenu)
DECLARE_CUSTOM_WRAPPER2(CMainMenuWrapper,IGameDialog,m_piDialog,IMainMenu,m_piMainMenu)
DECLARE_CUSTOM_WRAPPER2(COptionsMenuWrapper,IGameDialog,m_piDialog,IOptionsMenu,m_piOptionsMenu)

enum eInterfaceStage
{
	eInterfaceStage_None,
	eInterfaceStage_Initializing,
	eInterfaceStage_MainMenu,
	eInterfaceStage_GameMenu,
	eInterfaceStage_OptionsMenu,
	eInterfaceStage_Playing,
	eInterfaceStage_WaitingForContinuePause,
	eInterfaceStage_LaunchNextLevel,
	eInterfaceStage_HighScores,
	eInterfaceStage_WaitingForDemoEndCourtain,
	eInterfaceStage_WaitingForDemoEndCourtainLoading,
	eInterfaceStage_WaitingForManualGameEndCourtain,
	eInterfaceStage_WaitingForExitCourtain,
	eInterfaceStage_Exit
};

BEGIN_STRUCT_PROPS(SGameState)
	PROP_VALUE(nPoints,"Points",0)
	PROP_VALUE(nWeapon,"Weapon",0)
	PROP_VALUE(nLevel,"Level",-1)
	PROP_VALUE(nCheckpoint,"Checkpoint",-1)
	PROP_VALUE(nBombs,"Bombs",0)
	PROP_VALUE(nLivesLeft,"Lives",0)
	PROP_VALUE(eMode,"Mode",eGameMode_Normal)
	PROP_VALUE(eDifficulty,"Difficulty",eGameDifficulty_Easy)
END_STRUCT_PROPS()

struct SPlayerRenderOptions: public CSystemSerializableBase
{
public:

	bool				  m_bEnableShadows;
	EShadowQuality		  m_eShadowQuality;
	std::string			  m_sRenderPath;
	SGameScreenProperties m_sScreenProperties;

	SPlayerRenderOptions(){m_bEnableShadows=true;m_eShadowQuality=eShadowQuality_High;}
};

BEGIN_STRUCT_PROPS(SPlayerRenderOptions)
	PROP_VALUE_FLAGS(m_bEnableShadows,"EnableShadows",true,MRPF_NORMAL|MRPF_OPTIONAL);
	PROP_VALUE_FLAGS(m_eShadowQuality,"ShadowQuality",eShadowQuality_High,MRPF_NORMAL|MRPF_OPTIONAL);
	PROP_VALUE_FLAGS(m_sRenderPath,"RenderPath","",MRPF_NORMAL|MRPF_OPTIONAL);
	PROP_FLAGS(m_sScreenProperties,"Screen",MRPF_NORMAL|MRPF_OPTIONAL);
END_STRUCT_PROPS()

class SPlayerData: public CSystemSerializableBase
{
public:
	bool					m_bHasCurrentGame;
	SGameState			  	m_CurrentGame;
	std::vector<SGameState> m_vSavedGames;
	CPlayerProfileWrapper 	m_PlayerProfile;
	
	EGameMode 			  m_eLastMode;
	EGameDifficulty       m_eLastDifficulty;
	unsigned int 		  m_nLastLevel;
	unsigned int 		  m_nAvailableLevels;
	
	unsigned int 		  m_nMasterVolume;
	unsigned int 		  m_nMusicVolume;
	unsigned int 		  m_nSoundFXVolume;

	SPlayerRenderOptions  m_RenderOptions;
	

	BEGIN_PROP_MAP(SPlayerData)
		PROP_VALUE_FLAGS(m_eLastMode,"LastMode",eGameMode_Normal,MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_VALUE_FLAGS(m_eLastDifficulty,"LastDifficulty",eGameDifficulty_Easy,MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_VALUE_FLAGS(m_nLastLevel,"LastLevel",0,MRPF_NORMAL|MRPF_OPTIONAL);

		PROP_VALUE_FLAGS(m_nMasterVolume,"MasterVolume",100,MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_VALUE_FLAGS(m_nMusicVolume,"MusicVolume",100,MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_VALUE_FLAGS(m_nSoundFXVolume,"SoundFXVolume",100,MRPF_NORMAL|MRPF_OPTIONAL);

		PROP_FLAGS(m_CurrentGame,"CurrentGame",MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_FLAGS(m_vSavedGames,"SavedGames",MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_FLAGS(m_PlayerProfile,"Profile",MRPF_NORMAL|MRPF_OPTIONAL);

		PROP_FLAGS(m_RenderOptions,"Video",MRPF_NORMAL|MRPF_OPTIONAL);
	END_PROP_MAP()
	
	SPlayerData(){PersistencyDefaultValue();}
};

class CMainWindow: virtual public CGameWindowBase,virtual public IGameInterfaceWindowEvents
{
	eInterfaceStage m_eStage;

	CConfigFile m_GUIConfigFile;
	CConfigFile m_HighScoresConfigFile;
	CConfigFile m_PlayerProfileConfigFile;

	IGameGUILabel        *m_piSTBackground;
	IGameInterfaceWindow *m_piGameInterface;

	CSoundManagerWrapper m_SoundManager;
	
	CGameWindowWrapper m_BackgroundWindow;
	CGameDialogWrapper m_GameOverDialog;
	CMainMenuWrapper m_MainMenuDialog;
	CGameMenuWrapper m_GameMenuDialog;
	COptionsMenuWrapper   m_OptionsMenuDialog;
	CGameDialogWrapper m_CreditsDialog;
	CLevelOptionsDialogWrapper m_LevelOptionsDialog;
	CAudioOptionsDialogWrapper m_AudioOptionsDialog;
	CVideoOptionsDialogWrapper m_VideoOptionsDialog;
	CHighScoresDialogWrapper m_HighScoresDialog;
	CHighScoresTableWrapper  m_HighScoresTable;
	CControlsDialogWrapper   m_ControlsDialog;
	CLoadDialogWrapper   	 m_LoadDialog;
	CSaveDialogWrapper   	 m_SaveDialog;
	
	unsigned int m_nContinuePauseStartTime;
	unsigned int m_nAvailableLevels;
	
	SPlayerData m_PlayerData;
	
	void Destroy();
	void DestroyWindow();
	void SavePlayerData();
	
	bool Unserialize(ISystemPersistencyNode *piNode);
	
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
	void OnDrawBackground(IGenericRender *piRender);
	void OnDrawMouseCursor(SGamePos position,IGenericRender *piRender,bool *pbDrawed);
	
	void	OnScenarioFinished(eScenarioFinishedReason eReason);
	void	OnGameOverCourtainClosed();
	void    OnManualStopCourtainClosed();
	void	OnPaused(bool bPaused);
	void	OnCheckpoint();
	void	OnPlayerKilled();
	
	CMainWindow(void);
	~CMainWindow(void);
};