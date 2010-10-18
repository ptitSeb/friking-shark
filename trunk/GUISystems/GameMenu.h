#pragma once

class CGameMenu: virtual public CGameDialogBase,public IGameGUIButtonEvents
{
	IGameGUIButton *m_piBTContinue;
	IGameGUIButton *m_piBTEndGame;

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY_EX("Continue",m_piBTContinue,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EndGame",m_piBTEndGame,IGameGUIButtonEvents);
	END_CHILD_MAP()

public:
	void OnInitDialog();

	// IGameButtonEvents

	void OnButtonClicked(IGameGUIButton *piControl);

	CGameMenu(void);
	~CGameMenu(void);
};
