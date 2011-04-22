#pragma once

class CEntityEditorEntityPropertyPanel: public CGameWindowBase, virtual public IEntityEditorPropertyPanel, 
	virtual public IGameGUIButtonEvents
{
	CAnimationEntityTypeWrapper m_Object;

	void UpdateData();
	
	IGameGUILabel *m_piSTTime;
	IGameGUIButton *m_piBTDecreaseTime;
	IGameGUIButton *m_piBTIncreaseTime;

	IGameGUIButton *m_piBTRemove;

protected:

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY("EntityTime",m_piSTTime);
		CHILD_MAP_ENTRY_EX("EntityDecreaseTime",m_piBTDecreaseTime,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityIncreaseTime",m_piBTIncreaseTime,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityAnimationObjectRemove",m_piBTRemove,IGameGUIButtonEvents);
	END_CHILD_MAP()
	
	void OnButtonClicked(IGameGUIButton *piControl);

public:
	// IEntityEditorPropertyPanel

	bool SetObject(ISystemObject *piObject);
	
	CEntityEditorEntityPropertyPanel(void);
	~CEntityEditorEntityPropertyPanel(void);
};
