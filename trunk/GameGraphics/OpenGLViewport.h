#pragma once
#include "GameGraphics.h"
#ifndef WIN32
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#endif

class COpenGLViewport: virtual public CSystemObjectBase,virtual public IGenericViewport,virtual public IOpenGLViewport
{
	#ifdef WIN32
		HDC			m_hDC;
		HGLRC		m_hRenderContext;
		HWND		m_hWnd;
		int			m_nPixelFormatIndex;

		void OnCreate(HWND hWnd);
		void OnDestroy();
		LRESULT ProcessMessage(HWND hWnd,UINT  uMsg, WPARAM  wParam,LPARAM  lParam);
		static LRESULT WindowProc(HWND  hWnd,UINT  uMsg, WPARAM  wParam,LPARAM  lParam);

	#else
		
		Display 	*m_pXDisplay;
		Colormap 	 m_pXColorMap;
		GLXContext   m_pGLXContext;
		Cursor 		 m_pXHollowCursor;
		Window   	 m_XWindow;
		int 		 m_XLastX;
		int 		 m_XLastY;
		int 		 m_XLastWidth;
		int 		 m_XLastHeight;
		bool		 m_bXExit;

		int          m_nLoopDepth;
		int 		 m_nDetectDragX;
		int 		 m_nDetectDragY;
		unsigned int m_nDetectDragButton;
		bool 		 m_bDetectedDrag;
	#endif
	
	bool  	m_bShowSystemMouseCursor;
	bool  	m_bVerticalSync;
	
	std::string m_sCaption;
	IGenericViewportCallBack *m_piCallBack;
	

	// Virtuales de COpenGLViewPortBase

	void Render();

	void OnCharacter(unsigned short wCharacter);
	void OnKeyDown(unsigned short wKeyState);
	void OnKeyUp(unsigned short wKeyState);
	
	void OnSize(unsigned short cx,unsigned short cy);
	void OnMove();
	
	void OnLButtonDown(int pointX,int pointY);
	void OnLButtonDoubleClick(int pointX,int pointY);
	void OnLButtonUp(int pointX,int pointY);
	void OnRButtonDoubleClick(int pointX,int pointY);
	void OnRButtonDown(int pointX,int pointY);
	void OnRButtonUp(int pointX,int pointY);
	void OnMouseMove(int pointX,int pointY);
	void OnMouseWheelUp(int pointX,int pointY);
	void OnMouseWheelDown(int pointX,int pointY);
	
public:

	//IGenericViewport

	bool Create(unsigned x, unsigned y, unsigned w,unsigned h,bool bMaximized);
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

	void GetCurrentVideoMode(SVideoMode *pMode);

	bool SetWindowed(unsigned int x,unsigned int y,unsigned int w,unsigned int h);
	bool SetFullScreen(unsigned int w,unsigned int h,unsigned int bpp,unsigned int rate);
	
	// IOpenGLViewport

	void SetCurrentRenderTarget(bool bSetAsCurrent);

	 COpenGLViewport(void);
	~COpenGLViewport(void);
};
