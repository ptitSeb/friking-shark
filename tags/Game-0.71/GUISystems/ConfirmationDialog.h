#pragma once

class CConfirmationDialog: virtual public CGameDialogBase,public IGameGUIButtonEvents
{
	IGameGUIButton *m_piBTYes;
	IGameGUIButton *m_piBTNo;

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY_EX("Yes",m_piBTYes,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("No",m_piBTNo,IGameGUIButtonEvents);
	END_CHILD_MAP()

public:
	// IGameButtonEvents

	void OnButtonClicked(IGameGUIButton *piControl);

	CConfirmationDialog(void);
	~CConfirmationDialog(void);
};
