#pragma once

class CEntityEditorGeneralPropertyPanel: public CGameWindowBase, virtual public IEntityEditorPropertyPanel, 
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
		CHILD_MAP_ENTRY_EX("GeneralDamageType",m_piBTDamageType,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("GeneralBoundsType",m_piBTBoundsType,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("GeneralMovementType",m_piBTMovementType,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("GeneralCollisionType",m_piBTCollisionType,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("GeneralAlignment",m_piBTAlignment,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY("GeneralHealth",m_piSTHealth);
		CHILD_MAP_ENTRY_EX("GeneralDecreaseHealth",m_piBTHealthDecrease,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("GeneralIncreaseHealth",m_piBTHealthIncrease,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY("GeneralVelocity",m_piSTVelocity);
		CHILD_MAP_ENTRY_EX("GeneralDecreaseVelocity",m_piBTVelocityDecrease,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("GeneralIncreaseVelocity",m_piBTVelocityIncrease,IGameGUIButtonEvents);
	END_CHILD_MAP()
	
	void OnButtonClicked(IGameGUIButton *piControl);

public:
	// IEntityEditorPropertyPanel

	bool SetObject(ISystemObject *piObject);
	
	CEntityEditorGeneralPropertyPanel(void);
	~CEntityEditorGeneralPropertyPanel(void);
};
