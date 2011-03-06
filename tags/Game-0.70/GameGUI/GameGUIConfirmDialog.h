#pragma once

class CGameGUIConfirmDialog: public CGameDialogBase, virtual public IGameGUIConfirmDialog, virtual public IGameGUIButtonEvents
{
	IGameGUIButton *m_piBTYes;
	IGameGUIButton *m_piBTNo;
	IGameGUILabel  *m_piSTTitle;
	IGameGUILabel  *m_piSTText;
	
	std::string m_sText;
	std::string m_sTitle;
	EMessageDialogType m_eType;
	
	CVector m_vWarningBkColor;
	CVector m_vErrorBkColor;
	CVector m_vInfoBkColor;
	CVector m_vQuestionBKColor;
	
protected:

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY("Title",m_piSTTitle);
		CHILD_MAP_ENTRY("Text",m_piSTText);
		CHILD_MAP_ENTRY_EX("Yes",m_piBTYes,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("No",m_piBTNo,IGameGUIButtonEvents);
	END_CHILD_MAP()
	
	BEGIN_PROP_MAP(CGameGUIConfirmDialog)
		PROP_CLASS_CHAIN(CGameDialogBase)
		PROP_VALUE_FLAGS(m_vWarningBkColor,"WarningBkColor",CVector(0.4,0.4,0),MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_vErrorBkColor,"ErrorBkColor",CVector(0.4,0,0),MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_vInfoBkColor,"InfoBkColor",CVector(0.4,0.4,0.4),MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_vQuestionBKColor,"QuestionBKColor",CVector(0,0,0.4),MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP()
	
	void OnButtonClicked(IGameGUIButton *piControl);
	void OnKeyDown(int nKey,bool *pbProcessed);
	void OnInitDialog();
	void OnEndDialog();

public:
	// IGameGUIButton

	bool Confirm(IGameWindow *piParent,std::string sText,std::string sTitle,EMessageDialogType eType);


	CGameGUIConfirmDialog(void);
	~CGameGUIConfirmDialog(void);
};
