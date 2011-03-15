#pragma once

class CEntityEditorModelPropertyPanel: public CGameWindowBase, virtual public IEntityEditorPropertyPanel, 
	virtual public IGameGUIButtonEvents
{
	CAnimationModelTypeWrapper m_Object;

	void UpdateData();

	IGameGUIButton *m_piBTLoop;
	IGameGUIButton *m_piBTCastShadow;

	IGameGUILabel *m_piSTStartTime;
	IGameGUIButton *m_piBTDecreaseStartTime;
	IGameGUIButton *m_piBTIncreaseStartTime;

	IGameGUILabel *m_piSTEndTime;
	IGameGUIButton *m_piBTDecreaseEndTime;
	IGameGUIButton *m_piBTIncreaseEndTime;

	IGameGUILabel *m_piSTFps;
	IGameGUIButton *m_piBTDecreaseFps;
	IGameGUIButton *m_piBTIncreaseFps;

	IGameGUIButton *m_piBTRemove;

protected:

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY_EX("ModelLoop",m_piBTLoop,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("ModelShadow",m_piBTCastShadow,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY("ModelStartTime",m_piSTStartTime);
		CHILD_MAP_ENTRY_EX("ModelDecreaseStartTime",m_piBTDecreaseStartTime,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("ModelIncreaseStartTime",m_piBTIncreaseStartTime,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY("ModelEndTime",m_piSTEndTime);
		CHILD_MAP_ENTRY_EX("ModelDecreaseEndTime",m_piBTDecreaseEndTime,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("ModelIncreaseEndTime",m_piBTIncreaseEndTime,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY("ModelFps",m_piSTFps);
		CHILD_MAP_ENTRY_EX("ModelDecreaseFps",m_piBTDecreaseFps,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("ModelIncreaseFps",m_piBTIncreaseFps,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY_EX("ModelRemove",m_piBTRemove,IGameGUIButtonEvents);
	END_CHILD_MAP()
	
	void OnButtonClicked(IGameGUIButton *piControl);

public:
	// IEntityEditorPropertyPanel

	bool SetObject(ISystemObject *piObject);
	
	CEntityEditorModelPropertyPanel(void);
	~CEntityEditorModelPropertyPanel(void);
};
