#pragma once

class CFormationEditorObjectLabel: virtual public CGameWindowBase, virtual public IFormationEditorObjectLabel
{
	IDesignObject *m_piDesignObject;
	CFrameManagerWrapper m_FrameManager;
	CVector			m_vVisualizationAngles;
public:

	// IFormationEditorObjectLabel
	void SetObject(IDesignObject *piObject);

	bool InitWindow(IGameWindow *piParent,bool bPopup);
	void DestroyWindow();

	void OnDrawBackground(IGenericRender *piRender);
	
	CFormationEditorObjectLabel(void);
	~CFormationEditorObjectLabel(void);
};
