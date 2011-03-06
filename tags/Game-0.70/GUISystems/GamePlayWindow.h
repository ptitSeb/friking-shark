#pragma once

class CGamePlayWindow: virtual public CGameWindowBase
{
public:

	CGameRenderWrapper m_Render;
	CPlayAreaManagerWrapper m_PlayAreaManager;

	bool InitWindow(IGameWindow *piParent,bool bPopup);
	void DestroyWindow();

	void OnDraw(IGenericRender *piRender);

	CGamePlayWindow(void);
	~CGamePlayWindow(void);
};
