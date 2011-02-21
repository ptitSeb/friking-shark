#pragma once
#include "GameGraphics.h"
#include <SDL/SDL.h>

class COpenGLViewport: virtual public CSystemObjectBase,virtual public IGenericViewport,virtual public IOpenGLViewport
{
	bool         m_bSDLInitialized;
	int          m_nSDLWindowFlags;
	SDL_Surface* m_pSDLWindow;
	int          m_nLoopDepth;
	
	bool m_bVerticalSync;
	std::string m_sCaption;
	IGenericViewportCallBack *m_piCallBack;
	
	int m_nDetectDragX;
	int m_nDetectDragY;
	int m_nDetectDragButton;
	bool m_bDetectedDrag;


	// Virtuales de COpenGLViewPortBase

	void Render();

	void OnCharacter(WORD wCharacter);
	void OnKeyDown(WORD wKeyState);
	void OnKeyUp(WORD wKeyState);
	
	void OnSize(WORD cx,WORD cy);
	void OnMove();
	
	void OnLButtonDown(WORD wKeyState,POINT pos);
	void OnLButtonUp(WORD wKeyState,POINT pos);
	void OnRButtonDown(WORD wKeyState,POINT pos);
	void OnRButtonUp(WORD wKeyState,POINT pos);
	void OnMouseMove(WORD wKeyState,POINT pos);

	LRESULT ProcessMessage(HWND hWnd,UINT  uMsg, WPARAM  wParam,LPARAM  lParam);
public:

	//IGenericViewport

	bool Create(RECT *pRect,bool bMaximized);
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
	
	void EnterLoop();
	void ExitLoop();
	
	
	void GetCursorPos(int *pX,int *pY);
	void SetCursorPos(int x,int y);

	bool HasMouseCapture();
	void SetMouseCapture();
	void ReleaseMouseCapture();

	bool IsKeyDown(unsigned int nKey);

	bool IsActiveWindow();

	bool IsMouseVisible();
	void ShowMouseCursor(bool bShow);	
	
	bool DetectDrag(double dx,double dy);

	void				SetCaption(std::string sCaption);
	std::string 		GetCaption();

	void				SetIcon(HICON hIcon);
	HICON				GetIcon();

	void GetCurrentVideoMode(SVideoMode *pMode);

	bool SetWindowed(unsigned int x,unsigned int y,unsigned int w,unsigned int h);
	bool SetFullScreen(unsigned int w,unsigned int h,unsigned int bpp,unsigned int rate);
	
	// IOpenGLViewport

	void SetCurrentRenderTarget(bool bSetAsCurrent);

	 COpenGLViewport(void);
	~COpenGLViewport(void);
};
