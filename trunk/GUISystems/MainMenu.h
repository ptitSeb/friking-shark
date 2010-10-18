#pragma once

class CMainMenu: virtual public CGameDialogBase,public IGameGUIButtonEvents
{
	IGameGUIButton *m_piBTNewGame;
	IGameGUIButton *m_piBTExit;

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY_EX("NewGame",m_piBTNewGame,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Exit",m_piBTExit,IGameGUIButtonEvents);
	END_CHILD_MAP()

public:
	void OnInitDialog();

	// IGameButtonEvents

	void OnButtonClicked(IGameGUIButton *piControl);

	 CMainMenu(void);
	~CMainMenu(void);
};
