#pragma once

class CScenarioEditorObjectLabel: virtual public CGameWindowBase, virtual public IScenarioEditorObjectLabel
{
	IDesignObject *m_piDesignObject;
	CFrameManagerWrapper m_FrameManager;
	CVector			m_vVisualizationAngles;
public:

	// IScenarioEditorObjectLabel
	void SetObject(IDesignObject *piObject);

	bool InitWindow(IGameWindow *piParent,bool bPopup);
	void DestroyWindow();

	void OnDrawBackground(IGenericRender *piRender);
	
	CScenarioEditorObjectLabel(void);
	~CScenarioEditorObjectLabel(void);
};
