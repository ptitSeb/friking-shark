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

#define MAX_LIVES_TO_DISPLAY 6
#define MAX_BOMBS_TO_DISPLAY 6

enum EGameInterfaceState
{
	eGameInterfaceState_Idle,
	eGameInterfaceState_StartCourtain,
	eGameInterfaceState_Playing,
	eGameInterfaceState_CountintWait,
	eGameInterfaceState_CountingBombs,
	eGameInterfaceState_CountingPoints,
	eGameInterfaceState_EndWait,
	eGameInterfaceState_EndCourtain
};

class CGameInterface: virtual public CGameWindowBase, virtual public IGameInterfaceWindow,virtual public IPlayAreaElementEnumerationCallback,virtual public IEntityEvents
{
	ISystemManager			*m_piSystemManager;
	ISystem					*m_piGameSystem;
	
	EGameInterfaceState      m_eState;
	EGameMode 				 m_eGameMode;
	unsigned int             m_nEndBombs;
	unsigned int             m_nEndPoints;
	unsigned int             m_nLastCountTime;
	
	CGameControllerWrapper   m_GameControllerWrapper;
	CPlayAreaManagerWrapper  m_PlayAreaManagerWrapper;
	CEntityManagerWrapper	 m_EntityManagerWrapper;
	CFrameManagerWrapper	 m_FrameManagerWrapper;
	CWorldManagerWrapper	 m_WorldManagerWrapper;
	IPlayer					*m_piPlayer;
	IEntity					*m_piPlayerEntity;

	bool         m_bCourtainOpen;
	bool         m_bCourtainClosed;
	bool         m_bCourtainOpening;
	bool         m_bCourtainClosing;
	unsigned int m_nCourtainStartTime;
	
	bool m_bPlayerKilledOnPreviousFrame;

	IGameGUILabel *m_piSTFrameRate;
	IGameGUILabel *m_piSTGameTime;
	IGameGUILabel *m_piSTObjectCount;
	IGameGUILabel *m_piSTEntityCount;

	bool		m_bCompleted;
	bool		m_bFrozen;
	bool		m_bResumeAfterFreeze;

	unsigned int		m_dwNextAcceptedControlKeyTime;
	unsigned int		m_dwNextAcceptedPauseKeyTime;
	unsigned int		m_dwMovementType;
	
	unsigned int m_nHighScore;
	unsigned int m_nScore;
	
	bool m_bGameStarted;
	CVector m_vLastCheckPointPosition;
	bool m_bShowPerformanceIndicators;

	SGamePos m_InspectionMovementStartPoint;

	IGameWindow		*m_piSTCentralPanel;
	IGameGUILabel   *m_piSTPoints;
	IGameGUILabel   *m_piSTEndPoints;
	IGameGUILabel   *m_piSTHighScore;
	IGameWindow		*m_piSTLives[MAX_LIVES_TO_DISPLAY];
	IGameWindow		*m_piSTBombs[MAX_BOMBS_TO_DISPLAY];
	IGameWindow		*m_piSTEndBombs[MAX_BOMBS_TO_DISPLAY];
	
	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY_FLAGS("FrameRateLabel",m_piSTFrameRate,CMEF_OPTIONAL);
		CHILD_MAP_ENTRY_FLAGS("GameTimeLabel",m_piSTGameTime,CMEF_OPTIONAL);
		CHILD_MAP_ENTRY_FLAGS("EntityCountLabel",m_piSTEntityCount,CMEF_OPTIONAL);
		CHILD_MAP_ENTRY_FLAGS("ObjectCountLabel",m_piSTObjectCount,CMEF_OPTIONAL);
		CHILD_MAP_ENTRY("CentralPanel",m_piSTCentralPanel);
		CHILD_MAP_ENTRY("LevelEndPoints",m_piSTEndPoints);
		CHILD_MAP_ENTRY("LevelEndBomb0",m_piSTEndBombs[0]);
		CHILD_MAP_ENTRY("LevelEndBomb1",m_piSTEndBombs[1]);
		CHILD_MAP_ENTRY("LevelEndBomb2",m_piSTEndBombs[2]);
		CHILD_MAP_ENTRY("LevelEndBomb3",m_piSTEndBombs[3]);
		CHILD_MAP_ENTRY("LevelEndBomb4",m_piSTEndBombs[4]);
		CHILD_MAP_ENTRY("LevelEndBomb5",m_piSTEndBombs[5]);
		CHILD_MAP_ENTRY("PlayerPoints",m_piSTPoints);
		CHILD_MAP_ENTRY("HighScore",m_piSTHighScore);
		CHILD_MAP_ENTRY("PlayerLive0",m_piSTLives[0]);
		CHILD_MAP_ENTRY("PlayerLive1",m_piSTLives[1]);
		CHILD_MAP_ENTRY("PlayerLive2",m_piSTLives[2]);
		CHILD_MAP_ENTRY("PlayerLive3",m_piSTLives[3]);
		CHILD_MAP_ENTRY("PlayerLive4",m_piSTLives[4]);
		CHILD_MAP_ENTRY("PlayerLive5",m_piSTLives[5]);
		CHILD_MAP_ENTRY("PlayerBomb0",m_piSTBombs[0]);
		CHILD_MAP_ENTRY("PlayerBomb1",m_piSTBombs[1]);
		CHILD_MAP_ENTRY("PlayerBomb2",m_piSTBombs[2]);
		CHILD_MAP_ENTRY("PlayerBomb3",m_piSTBombs[3]);
		CHILD_MAP_ENTRY("PlayerBomb4",m_piSTBombs[4]);
		CHILD_MAP_ENTRY("PlayerBomb5",m_piSTBombs[5]);
	END_CHILD_MAP()
	
	void RenderCourtain(IGenericRender *piRender,unsigned int nCurrentTime);

	void StartGame(EGameMode eMode,unsigned int nPoints, unsigned int nLivesLeft,unsigned int nWeaponLevel);
	void StopGame();
	void ResetGame(bool bGoToLastCheckPoint);

	bool LoadScenario(std::string sFileName);
	void CloseScenario();

	void UpdateGUI(unsigned int dwCurrentTime);
	void UpdatePlayCameraPosition();
	void ProcessInput();
	void ProcessKey(unsigned short nKey);

	// IPlayAreaElementEnumerationCallback

	void ProcessEnumeratedPlayAreaElement(IPlayAreaElement *piElement,bool *pbStopEnumerating);

	// IGameInterfaceWindow

	void Freeze(bool bFreeze);
	bool IsFrozen();
	
	void SetHighScore(unsigned int nScore);
	unsigned int GetScore();
	
	// IEntityEvents

	void OnRemoved(IEntity *piEntity);
	void OnKilled(IEntity *piEntity);

	static void RenderEntity(IEntity *piEntity,void *pParam1,void *pParam2);

	void OpenCourtain(unsigned int nCurrentTime);
	void CloseCourtain(unsigned int nCurrentTime);
	
public:

	bool InitWindow(IGameWindow *piParent,bool bPopup);
	void DestroyWindow();
	void OnDraw(IGenericRender *piRender);
	
	CGameInterface(void);
	~CGameInterface(void);
};
