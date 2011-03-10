#pragma once

class CGameGUIInputDialog: public CGameDialogBase, virtual public IGameGUIInputDialog, virtual public IGameGUIButtonEvents
{
	IGameGUIButton *m_piBTAccept;
	IGameGUIButton *m_piBTCancel;
	IGameGUILabel  *m_piSTTitle;
	IGameGUIEdit   *m_piEDText;
	
	std::string m_sText;
	std::string m_sTitle;
protected:

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY("Title",m_piSTTitle);
		CHILD_MAP_ENTRY("Text",m_piEDText);
		CHILD_MAP_ENTRY_EX("Accept",m_piBTAccept,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Cancel",m_piBTCancel,IGameGUIButtonEvents);
	END_CHILD_MAP()
	
	void OnButtonClicked(IGameGUIButton *piControl);
	void OnKeyDown(int nKey,bool *pbProcessed);
	void OnInitDialog();
	void OnEndDialog();

public:
	// IGameGUIButton

	bool ShowInput(IGameWindow *piParent,std::string *psText,std::string sTitle);

	CGameGUIInputDialog(void);
	~CGameGUIInputDialog(void);
};
