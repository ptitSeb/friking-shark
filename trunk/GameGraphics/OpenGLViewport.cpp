#include "./stdafx.h"
#include "OpenGLGraphics.h"
#include "OpenGLViewport.h"

#ifdef WIN32
  #define VIEWPORT_CLASSNAME "OpenGLViewport"
  #define WM_GL_VIEWPORT_END_LOOP WM_USER+0x001
#else
  #define DETECT_DRAG_SIZE 3
  #define X_WINDOWS_EVENT_MASK ExposureMask|ButtonPressMask|ButtonReleaseMask|PointerMotionMask|KeyPressMask|KeyReleaseMask|StructureNotifyMask
#endif


#ifdef WIN32

int TranslateKeyFromWindows(int nWindowsKey)
{
	if(nWindowsKey>=32 && nWindowsKey<127){return nWindowsKey;}
	switch(nWindowsKey)
	{
	case VK_UP:return GK_UP;
	case VK_DOWN:return GK_DOWN;
	case VK_LEFT:return GK_LEFT;
	case VK_RIGHT:return GK_RIGHT;
	case VK_NUMPAD0:return GK_NUMPAD0;
	case VK_NUMPAD1:return GK_NUMPAD1;
	case VK_NUMPAD2:return GK_NUMPAD2;
	case VK_NUMPAD3:return GK_NUMPAD3;
	case VK_NUMPAD4:return GK_NUMPAD4;
	case VK_NUMPAD5:return GK_NUMPAD5;
	case VK_NUMPAD6:return GK_NUMPAD6;
	case VK_NUMPAD7:return GK_NUMPAD7;
	case VK_NUMPAD8:return GK_NUMPAD8;
	case VK_NUMPAD9:return GK_NUMPAD9;
	case VK_LCONTROL:return GK_LCONTROL;
	case VK_HOME:return GK_HOME;
	case VK_END:return GK_END;
	case VK_BACK:return GK_BACK;
	case VK_DELETE:return GK_DELETE;
	case VK_ESCAPE:return GK_ESCAPE;
	case VK_RETURN:return GK_RETURN;
	case GK_LMENU:return GK_LMENU;
	case VK_LSHIFT:return GK_LSHIFT;
	case VK_INSERT:return GK_INSERT;
	case VK_PAGEUP:return GK_PAGEUP;
	case VK_PAGEDOWN:return GK_PAGEDOWN;
	case VK_F1:return GK_F1;
	case VK_F2:return GK_F2;
	case VK_F3:return GK_F3;
	case VK_F4:return GK_F4;
	case VK_F5:return GK_F5;
	case VK_F6:return GK_F6;
	case VK_F7:return GK_F7;
	case VK_F8:return GK_F8;
	case VK_F9:return GK_F9;
	case VK_F10:return GK_F10;
	case VK_F11:return GK_F11;
	case VK_F12:return GK_F12;
	case VK_PAUSE:return GK_PAUSE;
	};
	return 0;
}

int TranslateKeyToWindows(int nGameKey)
{
	if(nGameKey>='a' && nGameKey<='z'){return nGameKey-('a'-'A');}
	if(nGameKey>=32 && nGameKey<127){return nGameKey;}
	switch(nGameKey)
	{
	case GK_UP:return VK_UP;
	case GK_DOWN:return VK_DOWN;
	case GK_LEFT:return VK_LEFT;
	case GK_RIGHT:return VK_RIGHT;
	case GK_NUMPAD0:return VK_NUMPAD0;
	case GK_NUMPAD1:return VK_NUMPAD1;
	case GK_NUMPAD2:return VK_NUMPAD2;
	case GK_NUMPAD3:return VK_NUMPAD3;
	case GK_NUMPAD4:return VK_NUMPAD4;
	case GK_NUMPAD5:return VK_NUMPAD5;
	case GK_NUMPAD6:return VK_NUMPAD6;
	case GK_NUMPAD7:return VK_NUMPAD7;
	case GK_NUMPAD8:return VK_NUMPAD8;
	case GK_NUMPAD9:return VK_NUMPAD9;
	case GK_LCONTROL:return VK_LCONTROL;
	case GK_MENU:return VK_MENU;
	case GK_HOME:return VK_HOME;
	case GK_END:return VK_END;
	case GK_BACK:return VK_BACK;
	case GK_DELETE:return VK_DELETE;
	case GK_ESCAPE:return VK_ESCAPE;
	case GK_RETURN:return VK_RETURN;
	case GK_LMENU:return VK_LMENU;
	case GK_LSHIFT:return VK_LSHIFT;
	case GK_INSERT:return VK_INSERT;
	case GK_F1:return VK_F1;
	case GK_F2:return VK_F2;
	case GK_F3:return VK_F3;
	case GK_F4:return VK_F4;
	case GK_F5:return VK_F5;
	case GK_PAUSE:return VK_PAUSE;
	case GK_F10:return VK_F10;
	case GK_PAGEUP:return VK_PAGEUP;
	case GK_PAGEDOWN:return VK_PAGEDOWN;
	};
	return 0;
}
#else

int TranslateKeyFromX11(int nX11Key)
{
	if(nX11Key>=32 && nX11Key<127){return nX11Key;}
	switch(nX11Key)
	{
	case XK_Up:return GK_UP;
	case XK_Down:return GK_DOWN;
	case XK_Left:return GK_LEFT;
	case XK_Right:return GK_RIGHT;
	case XK_KP_0:return GK_NUMPAD0;
	case XK_KP_1:return GK_NUMPAD1;
	case XK_KP_2:return GK_NUMPAD2;
	case XK_KP_3:return GK_NUMPAD3;
	case XK_KP_4:return GK_NUMPAD4;
	case XK_KP_5:return GK_NUMPAD5;
	case XK_KP_6:return GK_NUMPAD6;
	case XK_KP_7:return GK_NUMPAD7;
	case XK_KP_8:return GK_NUMPAD8;
	case XK_KP_9:return GK_NUMPAD9;
	case XK_Control_L:return GK_LCONTROL;
	case XK_Home:return GK_HOME;
	case XK_End:return GK_END;
	case XK_BackSpace:return GK_BACK;
	case XK_Delete:return GK_DELETE;
	case XK_Escape:return GK_ESCAPE;
	case XK_Return:return GK_RETURN;
	case XK_Alt_L:return GK_LMENU;
	case XK_Shift_L:return GK_LSHIFT;
	case XK_Insert:return GK_INSERT;
	case XK_F1:return GK_F1;
	case XK_F2:return GK_F2;
	case XK_F3:return GK_F3;
	case XK_F4:return GK_F4;
	case XK_F5:return GK_F5;
	case XK_F6:return GK_F6;
	case XK_F7:return GK_F7;
	case XK_F8:return GK_F8;
	case XK_F9:return GK_F9;
	case XK_F10:return GK_F10;
	case XK_F11:return GK_F11;
	case XK_F12:return GK_F12;
	case XK_Pause:return GK_PAUSE;
	case XK_Page_Up:return GK_PAGEUP;
	case XK_Page_Down:return GK_PAGEDOWN;
	};
	return 0;
}

int TranslateKeyToX11(int nGameKey)
{
	if(nGameKey>='A' && nGameKey<='Z'){return nGameKey+('a'-'A');}
	if(nGameKey>=32 && nGameKey<127){return nGameKey;}
	switch(nGameKey)
	{
	case GK_UP:return XK_Up;
	case GK_DOWN:return XK_Down;
	case GK_LEFT:return XK_Left;
	case GK_RIGHT:return XK_Right;
	case GK_NUMPAD0:return XK_KP_0;
	case GK_NUMPAD1:return XK_KP_1;
	case GK_NUMPAD2:return XK_KP_2;
	case GK_NUMPAD3:return XK_KP_3;
	case GK_NUMPAD4:return XK_KP_4;
	case GK_NUMPAD5:return XK_KP_5;
	case GK_NUMPAD6:return XK_KP_6;
	case GK_NUMPAD7:return XK_KP_7;
	case GK_NUMPAD8:return XK_KP_8;
	case GK_NUMPAD9:return XK_KP_9;
	case GK_LCONTROL:return XK_Control_L;
	case GK_MENU:return XK_Menu;
	case GK_HOME:return XK_Home;
	case GK_END:return XK_End;
	case GK_BACK:return XK_BackSpace;
	case GK_DELETE:return XK_Delete;
	case GK_ESCAPE:return XK_Escape;
	case GK_RETURN:return XK_Return;
	case GK_LMENU:return XK_Alt_L;
	case GK_LSHIFT:return XK_Shift_L;
	case GK_INSERT:return XK_Insert;
	case GK_F1:return XK_F1;
	case GK_F2:return XK_F2;
	case GK_F3:return XK_F3;
	case GK_F4:return XK_F4;
	case GK_F5:return XK_F5;
	case GK_PAUSE:return XK_Pause;
	case GK_F10:return XK_F10;
	case GK_PAGEUP:return XK_Page_Up;
	case GK_PAGEDOWN:return XK_Page_Down;
	};
	return 0;
}
#endif

COpenGLViewport::COpenGLViewport(void)
{
	m_piCallBack=NULL;
	m_bVerticalSync=false;

#ifdef WIN32
	m_hDC=NULL;
	m_hWnd=NULL;
	m_hRenderContext=NULL;
	m_nPixelFormatIndex=0;
#else
	m_bShowSystemMouseCursor=true;
	m_pXDisplay=NULL;
	m_pGLXContext=NULL;
	m_pXColorMap=NULL;
	m_pXHollowCursor=NULL;
	m_XWindow=None;
	m_XLastX=-1;
	m_XLastY=-1;
	m_XLastWidth=-1;
	m_XLastHeight=-1;
	m_bXExit=false;
	
	m_nLoopDepth=0;
	m_bDetectedDrag=false;
	m_nDetectDragX=0;
	m_nDetectDragY=0;
	m_nDetectDragButton=0;
#endif
}

COpenGLViewport::~COpenGLViewport(void)
{
}
#ifdef WIN32
void COpenGLViewport::OnCreate(HWND hWnd)
{
	SetWindowLong(hWnd,GWL_USERDATA,(unsigned int)this);
	m_hDC=GetDC(hWnd);
	if(m_hDC)
	{
		SetMapMode(m_hDC,MM_TEXT);

		PIXELFORMATDESCRIPTOR pixelFormat={0};
		pixelFormat.nSize		= sizeof(PIXELFORMATDESCRIPTOR);
		pixelFormat.nVersion	= 1;
		pixelFormat.dwFlags		= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL| PFD_DOUBLEBUFFER;
		pixelFormat.iPixelType	= PFD_TYPE_RGBA;
		pixelFormat.cColorBits	= GetDeviceCaps(m_hDC,BITSPIXEL);
		pixelFormat.cDepthBits	= 32;

		// se coge el formato de pixel mas adecuado con la descripcion anterior
		if(m_nPixelFormatIndex==0)
		{
			m_nPixelFormatIndex= ChoosePixelFormat(m_hDC, &pixelFormat);
			DescribePixelFormat(m_hDC,m_nPixelFormatIndex,sizeof(PIXELFORMATDESCRIPTOR), &pixelFormat);
		}
		// Se pone el formato de pixel para este dc
		if(SetPixelFormat(m_hDC, m_nPixelFormatIndex, &pixelFormat))
		{
			m_hRenderContext=wglCreateContext(m_hDC);
			if(m_hRenderContext){wglMakeCurrent(m_hDC,m_hRenderContext);}
		}
	}
}

void COpenGLViewport::OnDestroy()
{
	if(m_hRenderContext)
	{
		wglMakeCurrent(m_hDC,NULL);
		wglDeleteContext(m_hRenderContext);
		m_hRenderContext=NULL;
	}

	if(m_hDC){ReleaseDC(m_hWnd,m_hDC);m_hDC=NULL;}
	SetWindowLong(m_hWnd,GWL_USERDATA,(unsigned int)NULL);
}

LRESULT COpenGLViewport::ProcessMessage(HWND hWnd,UINT  uMsg, WPARAM  wParam,LPARAM  lParam)
{
	int pointX=(short)LOWORD(lParam);
	int pointY=(short)HIWORD(lParam);

	switch(uMsg)
	{
	case WM_ACTIVATE:
		return 0;
	case WM_MOUSEACTIVATE:
		SetFocus(hWnd);
		return MA_ACTIVATE;
	case WM_CREATE:
		OnCreate(hWnd);
		break;
	case WM_SETCURSOR:
		if(LOWORD(lParam)==HTCLIENT)
		{
			SetCursor(m_bShowSystemMouseCursor?LoadCursor(NULL,IDC_ARROW):NULL);
			return 0L;
		}
		break;
	case WM_DESTROY:
		OnDestroy();
		m_hWnd=NULL;
		break;
	case WM_PAINT:
		if(m_hRenderContext!=wglGetCurrentContext())
		{
			wglMakeCurrent(m_hDC,m_hRenderContext);
		}
		Render();
		return 0;
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		OnKeyDown(TranslateKeyFromWindows((unsigned short)wParam));
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		OnKeyUp(TranslateKeyFromWindows((unsigned short)wParam));
		break;
	case WM_MOUSEWHEEL:
		if(GET_WHEEL_DELTA_WPARAM(wParam)>0)
		{
		  OnMouseWheelUp(pointX,pointY);
		}
		else
		{
		  OnMouseWheelDown(pointX,pointY);
		}
		break;		
	case WM_LBUTTONDOWN:
		OnLButtonDown(pointX,pointY);
		break;
	case WM_LBUTTONDBLCLK:
		OnLButtonDoubleClick(pointX,pointY);
		break;
	case WM_RBUTTONDOWN:
		OnRButtonDown(pointX,pointY);
		break;
	case WM_RBUTTONDBLCLK:
		OnRButtonDoubleClick(pointX,pointY);
		break;
	case WM_LBUTTONUP:
		OnLButtonUp(pointX,pointY);
		break;
	case WM_RBUTTONUP:
		OnRButtonUp(pointX,pointY);
		break;
	case WM_MOUSEMOVE:
		OnMouseMove(pointX,pointY);
		break;
	case WM_SIZE:
		OnSize(LOWORD(lParam),HIWORD(lParam));
		break;
	case WM_CHAR:
		OnCharacter((unsigned short)wParam);
		break;
	case WM_MOVE:
		OnMove();
		break;
	}
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

LRESULT COpenGLViewport::WindowProc(HWND  hWnd,UINT  uMsg, WPARAM  wParam,LPARAM  lParam)
{
	CREATESTRUCT *pCreateInfo=(CREATESTRUCT *)lParam;
	COpenGLViewport *pData=NULL;
	pData=((uMsg==WM_CREATE)?(COpenGLViewport *)pCreateInfo->lpCreateParams:(COpenGLViewport *)GetWindowLong(hWnd,GWL_USERDATA));
	return pData?pData->ProcessMessage(hWnd,uMsg,wParam,lParam):DefWindowProc(hWnd,uMsg,wParam,lParam);
}
#endif

bool COpenGLViewport::Create(unsigned x, unsigned y, unsigned w, unsigned h, bool bMaximized)
{
	bool bOk=false;
#ifdef WIN32
	WNDCLASSEX		wcex={0};
	wcex.cbSize=sizeof(wcex);
	if(!GetClassInfoEx(NULL,VIEWPORT_CLASSNAME,&wcex))
	{
		WNDCLASS wc={0};
		wc.style			= CS_HREDRAW | CS_VREDRAW|CS_DBLCLKS;
		wc.lpfnWndProc      = (WNDPROC) COpenGLViewport::WindowProc;
		wc.lpszClassName    = VIEWPORT_CLASSNAME;
		wc.hInstance		= GetModuleHandle(NULL);
		RegisterClass(&wc);
	}

	unsigned int dwStyle=WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME|WS_OVERLAPPED;
	if(dwStyle==0xFFFFFFFF){dwStyle=WS_OVERLAPPED;}
	m_hWnd = CreateWindowEx(WS_EX_DLGMODALFRAME,VIEWPORT_CLASSNAME,"ViewPort",dwStyle,x,y,w,h,NULL,NULL,NULL,(void *)this);
	if(m_hWnd)
	{
		unsigned int dwStyle=GetWindowLong(m_hWnd,GWL_STYLE);
		SetWindowLong(m_hWnd,GWL_STYLE,dwStyle);
		ShowWindow(m_hWnd,bMaximized?SW_MAXIMIZE:SW_SHOW);
		EnableWindow(m_hWnd,TRUE);
		UpdateWindow(m_hWnd);
	}
	bOk=(m_hRenderContext!=NULL);
	if(!bOk){RTTRACE("COpenGLViewport::Create -> Failed to get OpenGL render context");}
#else
	XVisualInfo *pVisualInfo=NULL;
	
	m_pXDisplay=XOpenDisplay(NULL);
	if(m_pXDisplay)
	{
	  int nScreen = DefaultScreen( m_pXDisplay );
	  int pVisualAttribs[] = {GLX_RGBA,GLX_RED_SIZE,8,GLX_GREEN_SIZE,8,GLX_BLUE_SIZE,8,GLX_ALPHA_SIZE,8,GLX_DEPTH_SIZE,8,GLX_DOUBLEBUFFER,None};
	  pVisualInfo = glXChooseVisual( m_pXDisplay, nScreen,pVisualAttribs);
	}	  
	if(pVisualInfo){m_pGLXContext = glXCreateContext(m_pXDisplay, pVisualInfo,NULL, GL_TRUE);}
	if(m_pGLXContext){m_pXColorMap = XCreateColormap(m_pXDisplay, RootWindow(m_pXDisplay, pVisualInfo->screen),pVisualInfo->visual, AllocNone);}
	if(m_pXColorMap)
	{
		XSetWindowAttributes windowAttribs;
		windowAttribs.colormap = m_pXColorMap;
		windowAttribs.border_pixel = 0;
		windowAttribs.event_mask = X_WINDOWS_EVENT_MASK;
  
		m_XWindow = XCreateWindow(m_pXDisplay,RootWindow(m_pXDisplay,pVisualInfo->screen),
								x,y,w,h, 0, 
								pVisualInfo->depth,InputOutput,pVisualInfo->visual,
								CWBorderPixel|CWColormap|CWEventMask,&windowAttribs);
	}
	if(m_XWindow!=None)
	{
	  XSetStandardProperties(m_pXDisplay,m_XWindow,"ViewPort","ViewPort",None,NULL,0,NULL);
	  XMapWindow(m_pXDisplay,m_XWindow);
	  glXMakeCurrent(m_pXDisplay,m_XWindow,m_pGLXContext);
	  
	  bOk=true;
	}
	
	if(bOk)
	{
	  // Se crea un cursor transparente, ya que no hay forma de ocultarlo
	  Pixmap emptyBitmap;
	  XColor black;
	  static char pContent[] = { 0,0,0,0,0,0,0,0 };
	  black.red = black.green = black.blue = 0;

	  emptyBitmap = XCreateBitmapFromData(m_pXDisplay, m_XWindow, pContent, 8, 8);
	  m_pXHollowCursor= XCreatePixmapCursor(m_pXDisplay, emptyBitmap,emptyBitmap,&black, &black, 0, 0);
	  XFreePixmap(m_pXDisplay,emptyBitmap );
	}

	if(!bOk){RTTRACE("COpenGLViewport::Create -> Failed to get OpenGL render context");}
	
	if(pVisualInfo){XFree(pVisualInfo);pVisualInfo=NULL;}
	
#endif
	return bOk;
}

void COpenGLViewport::Destroy()
{
#ifdef WIN32
	if(m_hWnd){DestroyWindow(m_hWnd);m_hWnd=NULL;}
#else
	if(m_pXHollowCursor && !m_bShowSystemMouseCursor)
	{
	  XUndefineCursor(m_pXDisplay,m_XWindow);
	  XFreeCursor(m_pXDisplay,m_pXHollowCursor);
	  m_pXHollowCursor=NULL;
	}

	if(m_XWindow!=None)
	{
	  glXMakeCurrent(m_pXDisplay,None,NULL);	  
	  XUnmapWindow(m_pXDisplay,m_XWindow);
	  XDestroyWindow(m_pXDisplay,m_XWindow);
	  m_XWindow=None;
	}
	if(m_pXColorMap)
	{
	  XFreeColormap(m_pXDisplay,m_pXColorMap);
	  m_pXColorMap=NULL;
	}
	if(m_pGLXContext)
	{
	  glXDestroyContext(m_pXDisplay,m_pGLXContext);
	  m_pGLXContext=NULL;
	}
	if(m_pXDisplay)
	{
	  XCloseDisplay(m_pXDisplay);
	  m_pXDisplay=NULL;
	}
#endif
	
	CSystemObjectBase::Destroy();
}

bool COpenGLViewport::IsMaximized()
{
#ifdef WIN32
	WINDOWPLACEMENT placement={0};
	placement.length=sizeof(placement);
	GetWindowPlacement(m_hWnd,&placement);
	return (placement.showCmd==SW_MAXIMIZE);
#else
	return false;
#endif
}

void COpenGLViewport::SetMaximized(bool bMaximized)
{
#ifdef WIN32
	unsigned int dwStyle=GetWindowLong(m_hWnd,GWL_STYLE);
	if(bMaximized)
	{
		dwStyle&=~(WS_CAPTION|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME|WS_VISIBLE|WS_OVERLAPPED);
		dwStyle|=WS_POPUP;
	}
	else
	{
		dwStyle|=WS_CAPTION|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME|WS_VISIBLE|WS_OVERLAPPED;
		dwStyle&=~WS_POPUP;
	}
	SetWindowLong(m_hWnd,GWL_STYLE,dwStyle);
	ShowWindow(m_hWnd,bMaximized?SW_MAXIMIZE:SW_SHOWNORMAL);
#else
#endif
}

void COpenGLViewport::SetPos(unsigned dwX,unsigned dwY)
{
#ifdef WIN32
	SetWindowPos(m_hWnd,NULL,dwX,dwY,0,0,SWP_NOSIZE|SWP_NOZORDER);
#else
	if(m_XWindow!=None){XMoveWindow(m_pXDisplay,m_XWindow,dwX,dwY);}
#endif
}

void COpenGLViewport::GetPos(unsigned *pdwX,unsigned *pdwY)
{
#ifdef WIN32
	RECT R={0};
	GetWindowRect(m_hWnd,&R);
	*pdwX=R.left;
	*pdwY=R.top;
#else
	if(m_XWindow!=None)
	{
		XWindowAttributes attribs;
		XGetWindowAttributes(m_pXDisplay, m_XWindow, &attribs);
		*pdwX=attribs.x;
		*pdwY=attribs.y;
	}
	else
	{
		*pdwX=0;
		*pdwY=0;
	}
#endif
}

void COpenGLViewport::GetSize(unsigned *pdwWidth,unsigned *pdwHeight)
{
#ifdef WIN32
	RECT R={0};
	GetWindowRect(m_hWnd,&R);
	*pdwWidth=R.right-R.left;
	*pdwHeight=R.bottom-R.top;
#else
	if(m_XWindow!=None)
	{
		XWindowAttributes attribs;
		XGetWindowAttributes(m_pXDisplay, m_XWindow, &attribs);
		*pdwWidth=attribs.width;
		*pdwHeight=attribs.height;
	}
	else
	{
		*pdwWidth=0;
		*pdwHeight=0;
	}
#endif
}

void COpenGLViewport::GetClientSize(unsigned *pdwWidth,unsigned *pdwHeight)
{
#ifdef WIN32
	RECT R={0};
	GetClientRect(m_hWnd,&R);
	*pdwWidth=R.right-R.left;
	*pdwHeight=R.bottom-R.top;
#else
	return GetSize(pdwWidth,pdwHeight);
#endif
}

void COpenGLViewport::SetSize(unsigned dwWidth,unsigned dwHeight)
{
#ifdef WIN32
	SetWindowPos(m_hWnd,NULL,0,0,dwWidth,dwHeight,SWP_NOMOVE|SWP_NOZORDER);
#else
	if(m_XWindow!=None){XResizeWindow(m_pXDisplay,m_XWindow,dwWidth,dwHeight);}
#endif
}

void COpenGLViewport::GetRect(unsigned *pdwX,unsigned *pdwY,unsigned *pdwWidth,unsigned *pdwHeight)
{
#ifdef WIN32
	RECT R={0};
	GetWindowRect(m_hWnd,&R);
	*pdwX=R.left;
	*pdwY=R.top;
	*pdwWidth=R.right-R.left;
	*pdwHeight=R.bottom-R.top;
#else
	if(m_XWindow!=None)
	{
		XWindowAttributes attribs;
		XGetWindowAttributes(m_pXDisplay, m_XWindow, &attribs);
		*pdwX=attribs.x;
		*pdwY=attribs.y;
		*pdwWidth=attribs.width;
		*pdwHeight=attribs.height;
	}
#endif
}

void COpenGLViewport::SetRect(unsigned dwX,unsigned dwY,unsigned dwWidth,unsigned dwHeight)
{
#ifdef WIN32
	SetWindowPos(m_hWnd,NULL,dwX,dwY,dwWidth,dwHeight,SWP_NOZORDER);
#else
	if(m_XWindow!=None)
	{
		XMoveWindow(m_pXDisplay,m_XWindow,dwX,dwY);
		XResizeWindow(m_pXDisplay,m_XWindow,dwWidth,dwHeight);
	}
#endif
}

void COpenGLViewport::SetCallBack(IGenericViewportCallBack *pCallBack)
{
	m_piCallBack=pCallBack;
}

void COpenGLViewport::SetCurrentRenderTarget(bool bSetAsCurrent){}
void COpenGLViewport::Render()
{
	glFrontFace(GL_CCW);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glCullFace(GL_BACK);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
	glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	glPolygonMode(GL_FRONT,GL_FILL);

	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	if(m_piCallBack){m_piCallBack->OnRender();}
	
	glFlush();
	glFinish();
	
#ifdef WIN32
	SwapBuffers(m_hDC);
#else
	glXSwapBuffers(m_pXDisplay,m_XWindow);
#endif
}

void COpenGLViewport::OnLButtonDoubleClick(int pointX,int pointY){RTTRACE("LDBLCLK");if(m_piCallBack){m_piCallBack->OnLButtonDoubleClick(pointX,pointY);}}
void COpenGLViewport::OnLButtonDown(int pointX,int pointY){if(m_piCallBack){m_piCallBack->OnLButtonDown(pointX,pointY);}}
void COpenGLViewport::OnLButtonUp(int pointX,int pointY){if(m_piCallBack){m_piCallBack->OnLButtonUp(pointX,pointY);}}
void COpenGLViewport::OnRButtonDoubleClick(int pointX,int pointY){RTTRACE("RDBLCLK");if(m_piCallBack){m_piCallBack->OnRButtonDoubleClick(pointX,pointY);}}
void COpenGLViewport::OnRButtonDown(int pointX,int pointY){if(m_piCallBack){m_piCallBack->OnRButtonDown(pointX,pointY);}}
void COpenGLViewport::OnRButtonUp(int pointX,int pointY){if(m_piCallBack){m_piCallBack->OnRButtonUp(pointX,pointY);}}
void COpenGLViewport::OnMouseMove(int pointX,int pointY){if(m_piCallBack){m_piCallBack->OnMouseMove(pointX,pointY);}}
void COpenGLViewport::OnMouseWheelUp(int pointX,int pointY){if(m_piCallBack){m_piCallBack->OnMouseWheelUp(pointX,pointY);}}
void COpenGLViewport::OnMouseWheelDown(int pointX,int pointY){if(m_piCallBack){m_piCallBack->OnMouseWheelDown(pointX,pointY);}}
void COpenGLViewport::OnCharacter(unsigned short wChar){if(m_piCallBack){m_piCallBack->OnCharacter(wChar);}}
void COpenGLViewport::OnKeyDown(unsigned short wKey){if(m_piCallBack){m_piCallBack->OnKeyDown(wKey);}}
void COpenGLViewport::OnKeyUp(unsigned short wKey){if(m_piCallBack){m_piCallBack->OnKeyUp(wKey);}}
void COpenGLViewport::OnSize(unsigned short cx,unsigned short cy){if(m_piCallBack){m_piCallBack->OnSize(cx,cy);}}
void COpenGLViewport::OnMove(){if(m_piCallBack){m_piCallBack->OnMove();}}

void COpenGLViewport::SetVSync(bool bVSync)
{
	m_bVerticalSync=bVSync;
	#pragma message("COpenGLViewport::Create -> me he cargado el SetVSync de OpenGLExtensions (deberia ir a traves de Glee)")
	//COpenGLExtensions::SetVSync(m_bVerticalSync);
}

bool COpenGLViewport::GetVSync()
{
	return m_bVerticalSync;
}

void COpenGLViewport::SetCaption(std::string sCaption)
{
	m_sCaption=sCaption;
#ifdef WIN32
	if(m_hWnd){SetWindowText(m_hWnd,m_sCaption.c_str());}
#else
	if(m_XWindow!=None)
	{
	  XSetStandardProperties(m_pXDisplay,m_XWindow,m_sCaption.c_str(),m_sCaption.c_str(),None,NULL,0,NULL);
	}
#endif
}

std::string COpenGLViewport::GetCaption()
{
	return m_sCaption;
}

void COpenGLViewport::EnterLoop()
{
#ifdef WIN32
	while(m_hWnd)
	{
		MSG msg;
		while(PeekMessage(&msg,NULL,0,0,FALSE))
		{
			if(msg.hwnd==m_hWnd && msg.message==WM_GL_VIEWPORT_END_LOOP)
			{
				PeekMessage(&msg,NULL,0,0,TRUE);
				InvalidateRect(msg.hwnd,NULL,FALSE);
				return;
			}
			GetMessage(&msg,NULL,0,0);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if(m_hWnd)
		{
			InvalidateRect(m_hWnd,NULL,FALSE);
		}
	}
#else
	XEvent event;
	int nLoopId=++m_nLoopDepth;
	
	while(m_XWindow!=None && m_nLoopDepth>=nLoopId && !m_bXExit)
	{		
		while(m_XWindow!=None && XCheckWindowEvent(m_pXDisplay,m_XWindow,X_WINDOWS_EVENT_MASK,&event))
		{
		  if (event.type==KeyPress) 
		  {
			KeySym key=XLookupKeysym(&event.xkey,0);
			if(key!=None)
			{
				OnKeyDown(TranslateKeyFromX11(key));
			}
			char cKey=0;
			XLookupString(&event.xkey, &cKey,1, &key, NULL);  
			if(cKey>=32 && cKey<127){OnCharacter(cKey);}
		  }
		  else if (event.type==KeyRelease) 
		  {
			KeySym key=XLookupKeysym(&event.xkey,0);
			if(key!=None)
			{
				OnKeyUp(TranslateKeyFromX11(key));
			}
		  }
		  else if (event.type==ButtonPress) 
		  {
			  if(event.xbutton.button==Button1){OnLButtonDown(event.xbutton.x,event.xbutton.y);}
			  else if(event.xbutton.button==Button3){OnRButtonDown(event.xbutton.x,event.xbutton.y);}
			  else if(event.xbutton.button==Button4){OnMouseWheelUp(event.xbutton.x,event.xbutton.y);}
			  else if(event.xbutton.button==Button5){OnMouseWheelDown(event.xbutton.x,event.xbutton.y);}
		  }
		  else if (event.type==ButtonRelease) 
		  {
			  if(event.xbutton.button==Button1){OnLButtonUp(event.xbutton.x,event.xbutton.y);}
			  else if(event.xbutton.button==Button3){OnRButtonUp(event.xbutton.x,event.xbutton.y);}
			  
			  if(m_nDetectDragButton==event.xbutton.button)
			  {
				m_nLoopDepth--;
				return;
			  }			  
		  }
		  else if (event.type==MotionNotify) 
		  {
			  OnMouseMove(event.xmotion.x,event.xmotion.y);
			  
			  if(m_nDetectDragButton!=0)
			  {
				int nXDist=event.xmotion.x-m_nDetectDragX;
				int nYDist=event.xmotion.y-m_nDetectDragY;
				
				if(nXDist>DETECT_DRAG_SIZE || nXDist<(0-DETECT_DRAG_SIZE) ||
					nYDist>DETECT_DRAG_SIZE || nYDist<(0-DETECT_DRAG_SIZE))
				{
				  m_bDetectedDrag=true;
				  m_nLoopDepth--;
				  return;
				}
			  }
		  }
		  else if (event.type==ConfigureNotify) 
		  {
			if(m_XLastX!=event.xconfigure.x || 
			   m_XLastY!=event.xconfigure.y)
			{
			  OnMove();
			}
			if(m_XLastWidth!=event.xconfigure.width || 
			   m_XLastHeight!=event.xconfigure.height)
			{
			  OnSize(event.xconfigure.width,event.xconfigure.height);
			}
		  }
		  else if (event.type==DestroyNotify) 
		  {
			printf("Exit notified\n");
			m_bXExit=true;
		  }
		}
		glXMakeCurrent(m_pXDisplay,m_XWindow,m_pGLXContext);
		Render();
	}

#endif
}
void COpenGLViewport::ExitLoop()
{
#ifdef WIN32
	PostMessage(m_hWnd,WM_GL_VIEWPORT_END_LOOP,0,0);
#else
	if(m_nLoopDepth>0){m_nLoopDepth--;}
#endif
}

void  COpenGLViewport::GetCursorPos(int *pX,int *pY)
{
#ifdef WIN32
	POINT P={0};
	::GetCursorPos(&P);
	ScreenToClient(m_hWnd,&P);
	*pX=P.x;
	*pY=P.y;
#else
	if(m_XWindow!=None)
	{
		Window root,child;
		int rootx, rooty;
		unsigned int keys;
		XQueryPointer(m_pXDisplay,m_XWindow,&root, &child,&rootx,&rooty,pX,pY,&keys);
	}
	else
	{
		*pX=0;
		*pY=0;
	}
#endif
}
void  COpenGLViewport::SetCursorPos(int x,int y)
{
#ifdef WIN32
	POINT P;
	P.x=x;
	P.y=y;
	ClientToScreen(m_hWnd,&P);
	SetCursorPos(P.x,P.y);
#else
	if(m_XWindow!=None){XWarpPointer(m_pXDisplay,None,m_XWindow,0,0,0,0,x,y);}
#endif
}

bool  COpenGLViewport::HasMouseCapture(){return false;}
void  COpenGLViewport::SetMouseCapture(){}
void  COpenGLViewport::ReleaseMouseCapture(){}

bool  COpenGLViewport::IsKeyDown(unsigned int nKey)
{
#ifdef WIN32
	if(m_hWnd!=GetActiveWindow())
	{
		return false;
	}
	USHORT nKeyState=GetKeyState(TranslateKeyToWindows(nKey));
	return (nKeyState&0x8000)!=0;
#else
	if(m_pXDisplay!=None)
	{
		char keys[32];
		XQueryKeymap(m_pXDisplay,keys);
		KeyCode nKeyCode=XKeysymToKeycode(m_pXDisplay,TranslateKeyToX11(nKey));
		if(nKeyCode<sizeof(keys)*8)
		{  
		  if(keys[nKeyCode>>3]&(1<<(nKeyCode&0x7)))
		  {
			return true;
		  }
		}
	}
	return false;
#endif
}

bool  COpenGLViewport::IsActiveWindow(){return true;}

bool  COpenGLViewport::IsMouseVisible()
{
	return m_bShowSystemMouseCursor;
}
void  COpenGLViewport::ShowMouseCursor(bool bShow)
{
    // en windows el cursor se pone WM_SETCURSOR
#ifndef WIN32
	if(m_XWindow!=None && m_bShowSystemMouseCursor!=bShow)
	{
	  if(bShow)
	  {
		  XUndefineCursor(m_pXDisplay,m_XWindow);
	  }
	  else
	  {
		  XDefineCursor(m_pXDisplay,m_XWindow,m_pXHollowCursor);
	  }
	}
#endif
	m_bShowSystemMouseCursor=bShow;
	
}

bool COpenGLViewport::DetectDrag(double dx,double dy)
{
#ifdef WIN32
	unsigned int w=0,h=0;
	GetSize(&w,&h);
	GUITHREADINFO threadInfo={0};
	threadInfo.cbSize=sizeof(threadInfo);
	GetGUIThreadInfo(GetCurrentThreadId(),&threadInfo);
	unsigned int dwFlags=MB_OK;
	POINT pt;
	pt.x=(LONG)dx;
	pt.y=(LONG)((h-1)-dy);
	return (DragDetect(threadInfo.hwndActive,pt)?true:false);
#else
	if(m_nDetectDragButton){return false;}
	
	m_nDetectDragButton=Button1;
	m_nDetectDragX=dx;
	m_nDetectDragY=dy;
	EnterLoop();
	bool bDetected=m_bDetectedDrag;
	m_bDetectedDrag=false;
	m_nDetectDragX=0;
	m_nDetectDragY=0;
	m_nDetectDragButton=0;
	return bDetected;
#endif  
}

void COpenGLViewport::GetCurrentVideoMode(SVideoMode *pMode)
{
#ifdef WIN32
	DEVMODE mode={0};
	mode.dmSize=sizeof(DEVMODE);
	EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&mode);
	pMode->bpp=mode.dmBitsPerPel;
	pMode->w=mode.dmPelsWidth;
	pMode->h=mode.dmPelsHeight;
	pMode->rate=60;
#else
	Display* pDisplay = XOpenDisplay( NULL );
	if (!pDisplay ) {return;}
	Screen *pScreen = DefaultScreenOfDisplay( pDisplay );
	if ( !pScreen ) {return;}

	pMode->w = pScreen->width;
	pMode->h = pScreen->height;
	pMode->bpp = pScreen->root_depth;
	pMode->rate=60;
	XCloseDisplay( pDisplay );
#endif
}

bool COpenGLViewport::SetFullScreen(unsigned int w,unsigned int h,unsigned int bpp,unsigned int rate)
{
#ifdef WIN32
	SetMaximized(true);
	return true;
#else
	/*if((m_nSDLWindowFlags&SDL_FULLSCREEN)==0)
	{
	  SDL_WM_ToggleFullScreen(m_pSDLWindow); 
	  m_nSDLWindowFlags|=(SDL_FULLSCREEN);
	}*/
	return true;
#endif
/*	RTTRACE("COpenGLViewport::SetCurrentVideoMode -> Setting full screen video mode to %dx%d : %d",w,h,bpp);
	m_nSDLWindowFlags|=SDL_FULLSCREEN;
	m_pSDLWindow=SDL_WM_ToggleFullScreen(); 
	if(m_pSDLWindow){OnSize(m_pSDLWindow->w,m_pSDLWindow->h);}
	return m_pSDLWindow!=NULL;*/
	//if(m_pSDLWindow){OnSize(m_pSDLWindow->w,m_pSDLWindow->h);}
}

bool COpenGLViewport::SetWindowed(unsigned int x,unsigned int y,unsigned int w,unsigned int h)
{
#ifdef WIN32
	SetMaximized(false);
	return true;
#else
	/*if((m_nSDLWindowFlags&SDL_FULLSCREEN)!=0)
	{
	  SDL_WM_ToggleFullScreen(m_pSDLWindow); 
	  m_nSDLWindowFlags&=~(SDL_FULLSCREEN);
	}*/
	return true;
#endif
	/*RTTRACE("COpenGLViewport::SetCurrentVideoMode -> Setting windowed %dx%d - %dx%d",x,y,w,h);
	m_nSDLWindowFlags&=~(SDL_FULLSCREEN);
	m_pSDLWindow=SDL_SetVideoMode(w, h, 0,m_nSDLWindowFlags); 
	if(m_pSDLWindow){OnSize(m_pSDLWindow->w,m_pSDLWindow->h);}
	return m_pSDLWindow!=NULL;*/
/*		DEVMODE mode={0};
		mode.dmSize=sizeof(DEVMODE);
		mode.dmBitsPerPel=(unsigned int)m_sScreenProperties.dFullScreenRefreshBitsPerPixel;
		mode.dmPelsWidth=(unsigned int)m_sScreenProperties.sFullScreenResolution.w;
		mode.dmPelsHeight=(unsigned int)m_sScreenProperties.sFullScreenResolution.h;
		mode.dmDisplayFrequency=(unsigned int)m_sScreenProperties.dFullScreenRefreshRate;
		mode.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFREQUENCY;
		ChangeDisplaySettings(&mode,CDS_FULLSCREEN);*/
}












  







