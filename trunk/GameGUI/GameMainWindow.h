#pragma once

class CGameMainWindow :	virtual public CSystemObjectBase, virtual public IGameWindow
{
	std::vector<IGameWindow*> m_vChildren;

	IGameGUIManager *m_piGUIManager;

public:

	bool	InitWindow(IGameWindow *piParent,bool bPopup);
	void	DestroyWindow();

	void		SetWindowName(std::string sName);
	std::string GetWindowName();

	IGameGUIManager *GetGUIManager();

	bool	IsPopup();

	void	Show(bool bShow);
	void	Activate(bool bActivate);
	bool	IsVisible();
	bool	IsActive();

	void	GetRect(SGameRect *pRect);
	void	SetRect(SGameRect *pRect);

	void	SetFont(IGenericFont *piFont,double dSize);
	void	GetFont(IGenericFont **ppiFont,double *pdSize);

	void	UpdateRealRect();
	void	GetRealRect(SGameRect *pRect);

	void	SetReferenceSystem(eGameGUIReferenceSystem eRefSystem);
	eGameGUIReferenceSystem GetReferenceSystem();

	IGameWindow *GetParent();
	void	GetChildren(std::vector<IGameWindow *> *pvChildren);

	void				AddChild(IGameWindow *);
	void				RemoveChild(IGameWindow *);
	void				EnumerateChildren(IGameWindowEnumerationCallback *piCallback);
	IGameWindow	*FindChild(std::string sName);

	void	BringToFront();
	void	BringChildToFront(IGameWindow *);

	IGenericTexture *GetBackgroundTexture();
	void SetBackgroundTexture(IGenericTexture *piTexture);

	IGenericModel *GetBackgroundModel();
	void SetBackgroundModel(IGenericModel *piModel);

	void	GetBackgroundColor(CVector *pvColor,double *pdAlpha);
	void	SetBackgroundColor(CVector vColor,double dAlpha);

	// Events received from the manager

	void 	OnKeyDown(int nKey,bool *pbProcessed);
	void 	OnKeyUp(int nKey,bool *pbProcessed);

	void 	OnMouseDown(int nButton,double x,double y);
	void 	OnMouseUp(int nButton,double x,double y);
	void 	OnMouseMove(double x,double y);

	void 	OnDrawMouseCursor(SGamePos position,IGenericRender *piRender,bool *pbDrawed);
	void 	OnDrawBackground(IGenericRender *piRender);
	void 	OnDraw(IGenericRender *piRender);

	void 	OnSetFocus();
	void 	OnKillFocus(IGameWindow *piFocusedWindow);
	void 	OnReleaseMouseCapture();

	CGameMainWindow(IGameGUIManager *piGUIManager);
	~CGameMainWindow(void);
};
