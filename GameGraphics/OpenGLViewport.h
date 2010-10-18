#pragma once
#include "gamegraphics.h"

class COpenGLViewport: virtual public CSystemObjectBase,public COpenGLViewPortBase,virtual public IGenericViewport,virtual public IOpenGLViewport
{
	bool m_bVerticalSync;

	std::string m_sCaption;

	IGenericViewportCallBack *m_piCallBack;

	// Virtuales de COpenGLViewPortBase

	void Render();

	void OnLButtonDown(WORD wKeyState,POINT pos);
	void OnLButtonUp(WORD wKeyState,POINT pos);
	void OnRButtonDown(WORD wKeyState,POINT pos);
	void OnRButtonUp(WORD wKeyState,POINT pos);
	void OnMouseMove(WORD wKeyState,POINT pos);

	LRESULT ProcessMessage(HWND hWnd,UINT  uMsg, WPARAM  wParam,LPARAM  lParam);
public:

	//IGenericViewport

	HWND Create(HWND hParent,RECT *pRect,bool bMaximized);
	HWND GetWindowHandle();
	HWND GetParentWindowHandle();

	void Destroy();

	bool IsMaximized();
	void SetMaximized(bool bMaximized);

	void SetPos(unsigned dwX,unsigned dwY);
	void GetPos(unsigned *pdwX,unsigned *pdwY);

	void GetSize(unsigned *pdwWidth,unsigned *pdwHeight);
	void SetSize(unsigned dwWidth,unsigned dwHeight);

	void GetClientSize(unsigned *pdwWidth,unsigned *pdwHeight);

	void GetRect(unsigned *pdwX,unsigned *pdwY,unsigned *pdwWidth,unsigned *pdwHeight);
	void SetRect(unsigned dwX,unsigned dwY,unsigned dwWidth,unsigned dwHeight);

	void SetCallBack(IGenericViewportCallBack *pCallBack);

	void SetVSync(bool bVSync);
	bool GetVSync();

	void				SetCaption(std::string sCaption);
	std::string GetCaption();

	void				SetIcon(HICON hIcon);
	HICON				GetIcon();

	// IOpenGLViewport

	HGLRC GetRenderContext();
	HDC	 GetDeviceContext();

	void SetCurrentRenderTarget(bool bSetAsCurrent);

	 COpenGLViewport(void);
	~COpenGLViewport(void);
};
