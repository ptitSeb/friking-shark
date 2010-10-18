#include "StdAfx.h"
#include "OpenGLGraphics.h"
#include "OpenGLViewport.h"

COpenGLViewport::COpenGLViewport(void)
{
	m_piCallBack=NULL;
	m_bVerticalSync=false;
}

COpenGLViewport::~COpenGLViewport(void)
{
}

HWND COpenGLViewport::Create(HWND hParent,RECT *pRect,bool bMaximized)
{
	m_dwFlags&=~GLV_DRAW_AXISES;

	COpenGLViewPortBase::Init(hParent,bMaximized?SW_MAXIMIZE:SW_HIDE,bMaximized?WS_POPUP:WS_CAPTION|WS_THICKFRAME|WS_OVERLAPPED);

	if(COpenGLViewPortBase::m_hWnd)
	{
		SetWindowText(COpenGLViewPortBase::m_hWnd,m_sCaption.c_str());
		WINDOWPLACEMENT placement={0};
		placement.rcNormalPosition=*pRect;
		placement.length=sizeof(placement);
		placement.showCmd=bMaximized?SW_MAXIMIZE:SW_SHOWNORMAL;
		SetWindowPlacement(COpenGLViewPortBase::m_hWnd,&placement);
	}

	SetCurrentRenderTarget(true);

	COpenGLExtensions::Init();
	return COpenGLViewPortBase::m_hWnd;
}

void COpenGLViewport::Destroy()
{
	COpenGLViewPortBase::Destroy();
	CSystemObjectBase::Destroy();
}

HWND COpenGLViewport::GetWindowHandle()
{
	return COpenGLViewPortBase::m_hWnd;
}

HWND COpenGLViewport::GetParentWindowHandle()
{
	return GetParent(COpenGLViewPortBase::m_hWnd);
}

bool COpenGLViewport::IsMaximized()
{
	WINDOWPLACEMENT placement={0};
	placement.length=sizeof(placement);
	GetWindowPlacement(COpenGLViewPortBase::m_hWnd,&placement);
	return (placement.showCmd==SW_MAXIMIZE);
}

void COpenGLViewport::SetMaximized(bool bMaximized)
{
	DWORD dwStyle=GetWindowLong(COpenGLViewPortBase::m_hWnd,GWL_STYLE);
	if(bMaximized)
	{
		dwStyle&=~(WS_CAPTION|WS_THICKFRAME|WS_VISIBLE|WS_OVERLAPPED);
		dwStyle|=WS_POPUP;
	}
	else
	{
		dwStyle|=WS_CAPTION|WS_THICKFRAME|WS_VISIBLE|WS_OVERLAPPED;
		dwStyle&=~WS_POPUP;
	}
	SetWindowLong(COpenGLViewPortBase::m_hWnd,GWL_STYLE,dwStyle);
	ShowWindow(COpenGLViewPortBase::m_hWnd,bMaximized?SW_MAXIMIZE:SW_SHOWNORMAL);
}

void COpenGLViewport::SetPos(unsigned dwX,unsigned dwY)
{
	SetWindowPos(m_hWnd,NULL,dwX,dwY,0,0,SWP_NOSIZE|SWP_NOZORDER);
}

void COpenGLViewport::GetPos(unsigned *pdwX,unsigned *pdwY)
{
	RECT R={0};
	GetWindowRect(m_hWnd,&R);
	*pdwX=R.left;
	*pdwY=R.top;
}

void COpenGLViewport::GetSize(unsigned *pdwWidth,unsigned *pdwHeight)
{
	RECT R={0};
	GetWindowRect(m_hWnd,&R);
	*pdwWidth=R.right-R.left;
	*pdwHeight=R.bottom-R.top;
}

void COpenGLViewport::GetClientSize(unsigned *pdwWidth,unsigned *pdwHeight)
{
	RECT R={0};
	GetClientRect(m_hWnd,&R);
	*pdwWidth=R.right-R.left;
	*pdwHeight=R.bottom-R.top;
}

void COpenGLViewport::SetSize(unsigned dwWidth,unsigned dwHeight)
{
	SetWindowPos(m_hWnd,NULL,0,0,dwWidth,dwHeight,SWP_NOMOVE|SWP_NOZORDER);
}

void COpenGLViewport::GetRect(unsigned *pdwX,unsigned *pdwY,unsigned *pdwWidth,unsigned *pdwHeight)
{
	RECT R={0};
	GetWindowRect(m_hWnd,&R);
	*pdwX=R.left;
	*pdwY=R.top;
	*pdwWidth=R.right-R.left;
	*pdwHeight=R.bottom-R.top;
}

void COpenGLViewport::SetRect(unsigned dwX,unsigned dwY,unsigned dwWidth,unsigned dwHeight)
{
	SetWindowPos(m_hWnd,NULL,dwX,dwY,dwWidth,dwHeight,SWP_NOZORDER);
}

void COpenGLViewport::SetCallBack(IGenericViewportCallBack *pCallBack)
{
	m_piCallBack=pCallBack;
}

HGLRC COpenGLViewport::GetRenderContext(){return COpenGLViewPortBase::m_hRenderContext;}
HDC	 COpenGLViewport::GetDeviceContext(){return COpenGLViewPortBase::m_hDC;}

void COpenGLViewport::SetCurrentRenderTarget(bool bSetAsCurrent)
{
	if(bSetAsCurrent)
	{
		COpenGLViewPortBase::ActivateRenderContext();
	}
	else
	{
		COpenGLViewPortBase::DeactivateRenderContext();
	}
}

void COpenGLViewport::Render()
{
	SetCurrentRenderTarget(true);

	if(m_piCallBack)
	{
		m_piCallBack->OnRender();
	}
}

void COpenGLViewport::OnLButtonDown(WORD wKeyState,POINT mousepos)
{
	SetCurrentRenderTarget(true);

	COpenGLViewPortBase::OnLButtonDown(wKeyState,mousepos);
	if(m_piCallBack)
	{
		m_piCallBack->OnLButtonDown(wKeyState,mousepos.x,mousepos.y);
	}
}

void COpenGLViewport::OnLButtonUp(WORD wKeyState,POINT mousepos)
{
	SetCurrentRenderTarget(true);

	COpenGLViewPortBase::OnLButtonUp(wKeyState,mousepos);
	if(m_piCallBack)
	{
		m_piCallBack->OnLButtonUp(wKeyState,mousepos.x,mousepos.y);
	}
}

void COpenGLViewport::OnRButtonDown(WORD wKeyState,POINT mousepos)
{
	SetCurrentRenderTarget(true);

	COpenGLViewPortBase::OnRButtonDown(wKeyState,mousepos);
	if(m_piCallBack)
	{
		m_piCallBack->OnRButtonDown(wKeyState,mousepos.x,mousepos.y);
	}
}

void COpenGLViewport::OnRButtonUp(WORD wKeyState,POINT mousepos)
{
	SetCurrentRenderTarget(true);

	COpenGLViewPortBase::OnRButtonUp(wKeyState,mousepos);
	if(m_piCallBack)
	{
		m_piCallBack->OnRButtonUp(wKeyState,mousepos.x,mousepos.y);
	}
}

void COpenGLViewport::OnMouseMove(WORD wKeyState,POINT mousepos)
{
	SetCurrentRenderTarget(true);

	COpenGLViewPortBase::OnMouseMove(wKeyState,mousepos);
	if(m_piCallBack)
	{
		m_piCallBack->OnMouseMove(mousepos.x,mousepos.y);
	}
}

LRESULT COpenGLViewport::ProcessMessage(HWND hWnd,UINT  uMsg, WPARAM  wParam,LPARAM  lParam)
{
	SetCurrentRenderTarget(true);

	LRESULT dwResult=COpenGLViewPortBase::ProcessMessage(hWnd,uMsg,wParam,lParam);
	if(m_piCallBack)
	{
		dwResult=m_piCallBack->OnProcessMessage(dwResult,hWnd,uMsg,wParam,lParam);
	}
	return dwResult;
}

void COpenGLViewport::SetVSync(bool bVSync)
{
	m_bVerticalSync=bVSync;
	COpenGLExtensions::SetVSync(m_bVerticalSync);
}

bool COpenGLViewport::GetVSync()
{
	return m_bVerticalSync;
}

void				COpenGLViewport::SetCaption(std::string sCaption)
{
	m_sCaption=sCaption;
	if(COpenGLViewPortBase::m_hWnd)
	{
		SetWindowText(COpenGLViewPortBase::m_hWnd,m_sCaption.c_str());
	}
}

std::string COpenGLViewport::GetCaption()
{
	return m_sCaption;
}

void	COpenGLViewport::SetIcon(HICON hIcon)
{
	if(COpenGLViewPortBase::m_hWnd)
	{
		::SendMessage(COpenGLViewPortBase::m_hWnd,WM_SETICON,ICON_BIG,(LPARAM)hIcon);
		::SendMessage(COpenGLViewPortBase::m_hWnd,WM_SETICON,ICON_SMALL,(LPARAM)hIcon);
	}
}

HICON	COpenGLViewport::GetIcon()
{
	if(COpenGLViewPortBase::m_hWnd)
	{
		return (HICON)::SendMessage(COpenGLViewPortBase::m_hWnd,WM_GETICON,ICON_BIG,0);
	}
	else
	{
		return NULL;
	}
}
