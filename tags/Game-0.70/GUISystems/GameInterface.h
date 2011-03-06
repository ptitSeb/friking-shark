#pragma once

#include "GameRender.h"

class CGameInterface: virtual public CGameDialogBase, virtual public IGameInterfaceWindow,virtual public IPlayAreaElementEnumerationCallback,virtual public IEntityEvents
{
	ISystemManager			*m_piSystemManager;
	ISystem					*m_piGameSystem;

	CGenericCameraWrapper	 m_PlayCamera;
	CGenericCameraWrapper	 m_InspectionCamera;
	CGameRenderWrapper		 m_RenderWrapper;
	CGameControllerWrapper   m_GameControllerWrapper;
	CPlayAreaManagerWrapper  m_PlayAreaManagerWrapper;
	CEntityManagerWrapper	 m_EntityManagerWrapper;
	CFrameManagerWrapper	 m_FrameManagerWrapper;
	IPlayer					*m_piPlayer;
	IEntity					*m_piPlayerEntity;

	bool m_bPlayerKilledOnPreviousFrame;

	IGameGUILabel *m_piSTFrameRate;
	IGameGUILabel *m_piSTGameTime;
	IGameGUILabel *m_piSTObjectCount;
	IGameGUILabel *m_piSTEntityCount;
	IGameWindow	  *m_piGamePlayWindow;

	bool		m_bCompleted;
	bool		m_bFrozen;
	bool		m_bResumeAfterFreeze;

	unsigned int		m_dwNextAcceptedControlKeyTime;
	unsigned int		m_dwNextAcceptedPauseKeyTime;
	unsigned int		m_dwMovementType;

	double  m_dMovementInspectionSpeed;

	bool m_bGameStarted;
	CVector m_vLastCheckPointPosition;
	bool m_bShowPerformanceIndicators;
	bool m_bPauseOnNextFrame;

	SGamePos m_InspectionMovementStartPoint;


	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY("GamePlayWindow",m_piGamePlayWindow);
		CHILD_MAP_ENTRY_FLAGS("FrameRateLabel",m_piSTFrameRate,CMEF_OPTIONAL);
		CHILD_MAP_ENTRY_FLAGS("GameTimeLabel",m_piSTGameTime,CMEF_OPTIONAL);
		CHILD_MAP_ENTRY_FLAGS("EntityCountLabel",m_piSTEntityCount,CMEF_OPTIONAL);
		CHILD_MAP_ENTRY_FLAGS("ObjectCountLabel",m_piSTObjectCount,CMEF_OPTIONAL);
	END_CHILD_MAP()

	void StartGame();
	void StopGame();
	void ResetGame(bool bGoToLastCheckPoint);

	bool LoadScenario(std::string sFileName);
	void CloseScenario();

	void UpdatePlayCameraPosition();
	void ProcessInput();
	void ProcessKey(unsigned short nKey);

	void MoveInspection(unsigned short nKey);

	// IPlayAreaElementEnumerationCallback

	void ProcessEnumeratedPlayAreaElement(IPlayAreaElement *piElement,bool *pbStopEnumerating);

	// IGameInterfaceWindow

	void	Freeze(bool bFreeze);
	bool	IsFrozen();

	// IEntityEvents

	void OnKilled(IEntity *piEntity);

	void SetMovementType(unsigned long nType);

public:

	bool InitWindow(IGameWindow *piParent,bool bPopup);
	void DestroyWindow();
	void OnDraw(IGenericRender *piRender);

	void OnMouseMove(double x,double y);

	CGameInterface(void);
	~CGameInterface(void);
};
