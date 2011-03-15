#pragma once

class CEntityEditorParticleSystemPropertyPanel: public CGameWindowBase, virtual public IEntityEditorPropertyPanel, 
	virtual public IGameGUIButtonEvents
{
	CAnimationParticleSystemTypeWrapper m_Object;

	void UpdateData();

	IGameGUIButton *m_piBTTrackEntity;

	IGameGUILabel *m_piSTStartTime;
	IGameGUIButton *m_piBTDecreaseStartTime;
	IGameGUIButton *m_piBTIncreaseStartTime;

	IGameGUILabel *m_piSTEndTime;
	IGameGUIButton *m_piBTDecreaseEndTime;
	IGameGUIButton *m_piBTIncreaseEndTime;

	IGameGUIButton *m_piBTRemove;

protected:

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY_EX("ParticleSystemTrackEntity",m_piBTTrackEntity,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY("ParticleSystemStartTime",m_piSTStartTime);
		CHILD_MAP_ENTRY_EX("ParticleSystemDecreaseStartTime",m_piBTDecreaseStartTime,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("ParticleSystemIncreaseStartTime",m_piBTIncreaseStartTime,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY("ParticleSystemEndTime",m_piSTEndTime);
		CHILD_MAP_ENTRY_EX("ParticleSystemDecreaseEndTime",m_piBTDecreaseEndTime,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("ParticleSystemIncreaseEndTime",m_piBTIncreaseEndTime,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY_EX("ParticleSystemRemove",m_piBTRemove,IGameGUIButtonEvents);
	END_CHILD_MAP()
	
	void OnButtonClicked(IGameGUIButton *piControl);

public:
	// IEntityEditorPropertyPanel

	bool SetObject(ISystemObject *piObject);
	
	CEntityEditorParticleSystemPropertyPanel(void);
	~CEntityEditorParticleSystemPropertyPanel(void);
};
