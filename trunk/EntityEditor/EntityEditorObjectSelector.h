#pragma once

class CEntityEditorObjectSelector: public CGameDialogBase, virtual public IEntityEditorObjectSelector,virtual public IGameGUIButtonEvents
{
	std::string m_sTitle;
	
	double m_dButtonSizeX;
	double m_dButtonSizeY;

	IGameGUILabel *m_piSTTitle;
	IGameGUILabel *m_piSTObjectList;
	
	std::vector<IDesignObject *>    *m_pvObjects;
	std::vector<CButtonWrapper>      m_vButtons;
	std::vector<CObjectLabelWrapper> m_vLabels;
	unsigned long m_nSelectedObject;
	

	void OnInitDialog();
	void OnEndDialog();
public:

	BEGIN_CHILD_MAP()
	CHILD_MAP_ENTRY("Title",m_piSTTitle);
	CHILD_MAP_ENTRY("ObjectSelectorList",m_piSTObjectList);
	END_CHILD_MAP()

	bool SelectObject(std::string sTitle,IGameWindow *piParent,std::vector<IDesignObject *> *vObjects,unsigned long *pnSelectedObject,double dButtonSizeX,double dButtonSizeY);

	void OnButtonClicked(IGameGUIButton *piControl);
	void OnDraw(IGenericRender *piRender);

	CEntityEditorObjectSelector(void);
	~CEntityEditorObjectSelector(void);
};
