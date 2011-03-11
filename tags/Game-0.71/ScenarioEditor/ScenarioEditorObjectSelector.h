#pragma once

#define DEFAULT_OBJECT_SELECTOR_BUTTON_SIZE		128.0

class IScenarioEditorObjectSelector: virtual public ISystemUnknown
{
public:

	virtual bool SelectObject(IGameWindow *piParent,std::vector<IDesignObject *> *vObjects,unsigned long *pnSelectedObject,double dButtonSizeX=DEFAULT_OBJECT_SELECTOR_BUTTON_SIZE,double dButtonSizeY=DEFAULT_OBJECT_SELECTOR_BUTTON_SIZE)=0;

};

class CScenarioEditorObjectSelector: public CGameDialogBase, virtual public IScenarioEditorObjectSelector,virtual public IGameGUIButtonEvents
{
	double m_dButtonSizeX;
	double m_dButtonSizeY;

	IGameGUILabel *m_piSTObjectList;
	std::vector<IDesignObject *> *m_pvObjects;
	std::vector<CButtonWrapper> m_vButtons;
	std::vector<CObjectLabelWrapper> m_vLabels;
	unsigned long m_nSelectedObject;
	

	void OnInitDialog();
	void OnEndDialog();
public:

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY("ObjectSelectorList",m_piSTObjectList);
	END_CHILD_MAP()

	bool SelectObject(IGameWindow *piParent,std::vector<IDesignObject *> *vObjects,unsigned long *pnSelectedObject,double dButtonSizeX,double dButtonSizeY);

	void OnButtonClicked(IGameGUIButton *piControl);
	void OnDraw(IGenericRender *piRender);

	CScenarioEditorObjectSelector(void);
	~CScenarioEditorObjectSelector(void);
};
