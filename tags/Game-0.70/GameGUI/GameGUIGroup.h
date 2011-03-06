#pragma once

class CGameGUIGroup: virtual public CGameWindowBase
{
public:
	bool InitWindow(IGameWindow *piParent,bool bPopup);

	CGameGUIGroup(void);
	~CGameGUIGroup(void);
};
