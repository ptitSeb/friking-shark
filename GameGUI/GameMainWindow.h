//  Friking Shark, a Flying Shark arcade remake.
//  Copyright (C) 2011  Javier Martin Garcia (javiermartingarcia@gmail.com)
//	
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//  


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

	void 	ComputeClipRect(SGameRect *pRect);
	void	UpdateRealRect();
	void	GetRealRect(SGameRect *pRect);
	double  GetSizeInLayout();
	void    SetSizeInLayout(double dSize);
	
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

	void    OnCharacter( int nKey,bool *pbProcessed);
	void 	OnKeyDown(int nKey,bool *pbProcessed);
	void 	OnKeyUp(int nKey,bool *pbProcessed);

	void 	OnMouseDown(int nButton,double x,double y);
	void 	OnMouseUp(int nButton,double x,double y);
	void 	OnMouseMove(double x,double y);
	void 	OnMouseDoubleClick(int nButton,double x,double y);
	void 	OnMouseWheelUp(double x,double y);
	void 	OnMouseWheelDown(double x,double y);
	 
	void 	OnDrawMouseCursor(SGamePos position,IGenericRender *piRender,bool *pbDrawed);
	void 	OnDrawBackground(IGenericRender *piRender);
	void 	OnDraw(IGenericRender *piRender);

	void 	OnSetFocus();
	void 	OnKillFocus(IGameWindow *piFocusedWindow);
	void 	OnWantFocus(bool *pbWant);

	void 	OnReleaseMouseCapture();

	CGameMainWindow(IGameGUIManager *piGUIManager);
	~CGameMainWindow(void);
};
