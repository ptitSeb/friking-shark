#pragma once

class CGameGUIColorDialog: public CGameDialogBase, virtual public IGameGUIColorDialog, virtual public IGameGUIButtonEvents
{
	IGameGUIButton *m_piBTCancel;
	IGameGUIButton *m_piBTSelect;
	IGameGUILabel  *m_piSTTitle;
	IGameGUILabel  *m_piSTRGB;
	IGameGUILabel  *m_piSTHSV;
	IGameGUILabel  *m_piSTSample;
	IGameGUILabel  *m_piSTHSelector;
	IGameGUILabel  *m_piSTSVSelector;
	
	bool            m_bDraggingH;
	bool            m_bDraggingSV;
	
	CVector     m_vColor;
	std::string m_sTitle;
protected:

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY("Title",m_piSTTitle);
		CHILD_MAP_ENTRY("RGB",m_piSTRGB);
		CHILD_MAP_ENTRY("HSV",m_piSTHSV);
		CHILD_MAP_ENTRY("Sample",m_piSTSample);
		CHILD_MAP_ENTRY("HSelector",m_piSTHSelector);
		CHILD_MAP_ENTRY("SVSelector",m_piSTSVSelector);
		CHILD_MAP_ENTRY_EX("Cancel",m_piBTCancel,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Select",m_piBTSelect,IGameGUIButtonEvents);
	END_CHILD_MAP()
	
	void OnButtonClicked(IGameGUIButton *piControl);
	void OnMouseDown(int nButton,double x,double y);
	void OnMouseUp(int nButton,double x,double y);
	void OnMouseMove(double x,double y);
	void OnKeyDown(int nKey,bool *pbProcessed);
	void OnDraw(IGenericRender *piRender);
	
	void OnInitDialog();
	void OnEndDialog();

public:
	// IGameGUIButton

	bool SelectColor(IGameWindow *piParent,std::string sTitle,CVector *pvColor);

	CGameGUIColorDialog(void);
	~CGameGUIColorDialog(void);
};
