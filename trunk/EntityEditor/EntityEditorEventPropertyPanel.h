#pragma once

class CEntityEditorEventPropertyPanel: public CGameWindowBase, virtual public IEntityEditorPropertyPanel, 
	virtual public IGameGUIButtonEvents, 
	virtual public IGameGUIEditEvents
{
	CAnimationEventTypeWrapper m_Object;

	void UpdateData();
	
	IGameGUIEdit *m_piEDName;
	IGameGUIEdit *m_piEDParams;
	
	IGameGUILabel *m_piSTTime;
	IGameGUIButton *m_piBTDecreaseTime;
	IGameGUIButton *m_piBTIncreaseTime;

	IGameGUIButton *m_piBTRemove;

protected:

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY("EventTime",m_piSTTime);
		CHILD_MAP_ENTRY_EX("EventDecreaseTime",m_piBTDecreaseTime,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EventIncreaseTime",m_piBTIncreaseTime,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EventName",m_piEDName,IGameGUIEditEvents);
		CHILD_MAP_ENTRY_EX("EventParams",m_piEDParams,IGameGUIEditEvents);
		CHILD_MAP_ENTRY_EX("EventRemove",m_piBTRemove,IGameGUIButtonEvents);
	END_CHILD_MAP()
	
	void OnButtonClicked(IGameGUIButton *piControl);
	void OnTextChanged(IGameGUIEdit *piControl,std::string sNewText);

public:
	// IEntityEditorPropertyPanel

	bool SetObject(ISystemObject *piObject);
	
	CEntityEditorEventPropertyPanel(void);
	~CEntityEditorEventPropertyPanel(void);
};
