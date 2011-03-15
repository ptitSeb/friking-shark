#pragma once

class CEntityEditorObjectLabel: virtual public CGameWindowBase, virtual public IEntityEditorObjectLabel
{
	IDesignObject *m_piDesignObject;
	CFrameManagerWrapper m_FrameManager;
	CVector			m_vVisualizationAngles;
public:

	// IEntityEditorObjectLabel
	void SetObject(IDesignObject *piObject);

	bool InitWindow(IGameWindow *piParent,bool bPopup);
	void DestroyWindow();

	void OnDrawBackground(IGenericRender *piRender);
	
	CEntityEditorObjectLabel(void);
	~CEntityEditorObjectLabel(void);
};
