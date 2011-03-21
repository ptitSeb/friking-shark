#pragma once

class CEntityEditorEntityPropertyPanel: public CGameWindowBase, virtual public IEntityEditorPropertyPanel, 
	virtual public IGameGUIButtonEvents
{
	CEntityTypeWrapper m_Object;

	void UpdateData();

	IGameGUIButton *m_piBTDamageType;
	IGameGUIButton *m_piBTBoundsType;
	IGameGUIButton *m_piBTMovementType;
	IGameGUIButton *m_piBTCollisionType;
	IGameGUIButton *m_piBTAlignment;
	IGameGUILabel  *m_piSTHealth;
	IGameGUIButton *m_piBTHealthIncrease;
	IGameGUIButton *m_piBTHealthDecrease;
	IGameGUILabel  *m_piSTVelocity;
	IGameGUIButton *m_piBTVelocityIncrease;
	IGameGUIButton *m_piBTVelocityDecrease;	
protected:

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY_EX("EntityDamageType",m_piBTDamageType,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityBoundsType",m_piBTBoundsType,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityMovementType",m_piBTMovementType,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityCollisionType",m_piBTCollisionType,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityAlignment",m_piBTAlignment,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY("EntityHealth",m_piSTHealth);
		CHILD_MAP_ENTRY_EX("EntityDecreaseHealth",m_piBTHealthDecrease,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityIncreaseHealth",m_piBTHealthIncrease,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY("EntityVelocity",m_piSTVelocity);
		CHILD_MAP_ENTRY_EX("EntityDecreaseVelocity",m_piBTVelocityDecrease,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityIncreaseVelocity",m_piBTVelocityIncrease,IGameGUIButtonEvents);
	END_CHILD_MAP()
	
	void OnButtonClicked(IGameGUIButton *piControl);

public:
	// IEntityEditorPropertyPanel

	bool SetObject(ISystemObject *piObject);
	
	CEntityEditorEntityPropertyPanel(void);
	~CEntityEditorEntityPropertyPanel(void);
};
