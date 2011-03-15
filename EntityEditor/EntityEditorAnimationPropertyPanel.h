#pragma once

class CEntityEditorAnimationPropertyPanel: public CGameWindowBase, virtual public IEntityEditorPropertyPanel, 
	virtual public IGameGUIButtonEvents
{
	CAnimationDesignTypeWrapper m_Object;

	void UpdateData();

	IGameGUIButton *m_piBTLoop;
	IGameGUIButton *m_piBTRemove;

protected:

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY_EX("AnimationLoop",m_piBTLoop,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("AnimationRemove",m_piBTRemove,IGameGUIButtonEvents);
	END_CHILD_MAP()
	
	void OnButtonClicked(IGameGUIButton *piControl);

public:
	// IEntityEditorPropertyPanel

	bool SetObject(ISystemObject *piObject);
	
	CEntityEditorAnimationPropertyPanel(void);
	~CEntityEditorAnimationPropertyPanel(void);
};
