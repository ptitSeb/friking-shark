#pragma once

class IScenarioEditorColorSelector: virtual public ISystemUnknown
{
public:

	virtual bool SelectColor(IGameWindow *piParent,IGenericTexture *piTexture,CVector *pOutColor)=0;

};

class CScenarioEditorColorSelector: public CGameDialogBase, virtual public IScenarioEditorColorSelector
{
	IGameGUILabel *m_piSTColorMap;
	IGenericTexture *m_piColorMapTexture;
	CVector m_vColor;

	void OnInitDialog();
	void OnMouseDown(int nButton,double x,double y);
public:

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY("ColorMap",m_piSTColorMap);
	END_CHILD_MAP()

	bool SelectColor(IGameWindow *piParent,IGenericTexture *piTexture,CVector *pOutColor);

	CScenarioEditorColorSelector(void);
	~CScenarioEditorColorSelector(void);
};
