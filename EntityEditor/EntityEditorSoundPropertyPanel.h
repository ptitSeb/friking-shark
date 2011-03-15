#pragma once

class CEntityEditorSoundPropertyPanel: public CGameWindowBase, virtual public IEntityEditorPropertyPanel, 
	virtual public IGameGUIButtonEvents
{
	CAnimationSoundTypeWrapper m_Object;

	void UpdateData();

	IGameGUIButton *m_piBTLoop;

	IGameGUILabel *m_piSTStartTime;
	IGameGUIButton *m_piBTDecreaseStartTime;
	IGameGUIButton *m_piBTIncreaseStartTime;

	IGameGUILabel *m_piSTEndTime;
	IGameGUIButton *m_piBTDecreaseEndTime;
	IGameGUIButton *m_piBTIncreaseEndTime;

	IGameGUILabel *m_piSTVolume;
	IGameGUIButton *m_piBTDecreaseVolume;
	IGameGUIButton *m_piBTIncreaseVolume;

	IGameGUIButton *m_piBTRemove;

protected:

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY_EX("SoundLoop",m_piBTLoop,IGameGUIButtonEvents);
		
		CHILD_MAP_ENTRY("SoundStartTime",m_piSTStartTime);
		CHILD_MAP_ENTRY_EX("SoundDecreaseStartTime",m_piBTDecreaseStartTime,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("SoundIncreaseStartTime",m_piBTIncreaseStartTime,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY("SoundEndTime",m_piSTEndTime);
		CHILD_MAP_ENTRY_EX("SoundDecreaseEndTime",m_piBTDecreaseEndTime,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("SoundIncreaseEndTime",m_piBTIncreaseEndTime,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY("SoundVolume",m_piSTVolume);
		CHILD_MAP_ENTRY_EX("SoundDecreaseVolume",m_piBTDecreaseVolume,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("SoundIncreaseVolume",m_piBTIncreaseVolume,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY_EX("SoundRemove",m_piBTRemove,IGameGUIButtonEvents);
	END_CHILD_MAP()
	
	void OnButtonClicked(IGameGUIButton *piControl);

public:
	// IEntityEditorPropertyPanel

	bool SetObject(ISystemObject *piObject);
	
	CEntityEditorSoundPropertyPanel(void);
	~CEntityEditorSoundPropertyPanel(void);
};
