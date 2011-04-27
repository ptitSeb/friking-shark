#pragma once

DECLARE_CUSTOM_WRAPPER1(CViewportWrapper,IGenericViewport,m_piViewport)
DECLARE_CUSTOM_WRAPPER1(CGameDialogWrapper,IGameDialog,m_piDialog)
DECLARE_CUSTOM_WRAPPER1(CGameInterfaceWrapper,IGameInterfaceWindow,m_piInterfaceWindow)

enum eInterfaceStage
{
	eInterfaceStage_None,
	eInterfaceStage_MainMenu,
	eInterfaceStage_Playing
};

class CMainWindow: virtual public CGameWindowBase,virtual public IGameInterfaceWindowEvents
{
	eInterfaceStage m_eStage;

	CConfigFile m_GUIConfigFile;

	IGameGUILabel        *m_piSTBackground;
	IGameInterfaceWindow *m_piGameInterface;

	CGameWindowWrapper m_BackgroundWindow;
	CGameDialogWrapper m_GameOverDialog;
	CGameDialogWrapper m_CongratulationsDialog;
	CGameDialogWrapper m_MainMenuDialog;
	CGameDialogWrapper m_GameMenuDialog;
	CGameDialogWrapper m_ConfirmationDialog;

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
	 
	void	OnScenarioFinished(eScenarioFinishedReason eReason);

	CMainWindow(void);
	~CMainWindow(void);
};