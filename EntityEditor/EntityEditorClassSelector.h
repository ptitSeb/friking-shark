#pragma once

class CEntityEditorClassSelector: public CGameDialogBase, virtual public IEntityEditorClassSelector, 
	virtual public IGameGUIButtonEvents,
	virtual public IGameGUIListEvents
{
	IGameGUIButton *m_piBTCancel;
	IGameGUIButton *m_piBTSelect;
	IGameGUILabel  *m_piSTTitle;
	IGameGUIList   *m_piLSClasses;

	std::string m_sTitle;
	std::string m_sSelectedClass;
	std::string m_sTargetSystem;
	
protected:

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY("Title",m_piSTTitle);
		CHILD_MAP_ENTRY_EX("Classes",m_piLSClasses,IGameGUIListEvents);
		CHILD_MAP_ENTRY_EX("Cancel",m_piBTCancel,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Select",m_piBTSelect,IGameGUIButtonEvents);
	END_CHILD_MAP()
	
	void OnButtonClicked(IGameGUIButton *piControl);
	void OnSelectionChanged(IGameGUIList *piControl,int nElement,std::string sElement);
	void OnSelectionDoubleCliked(IGameGUIList *piControl,int nElement,std::string sElement);

	void OnKeyDown(int nKey,bool *pbProcessed);
	
	void OnInitDialog();
	void OnEndDialog();

	void ProcessSelect();

public:
	// IGameGUIFileDialog

	bool SelectClass(IGameWindow *piParent,std::string sTitle,std::string sSystem,std::string *psClass);
	
	CEntityEditorClassSelector(void);
	~CEntityEditorClassSelector(void);
};
