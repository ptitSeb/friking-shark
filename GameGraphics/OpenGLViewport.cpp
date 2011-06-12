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


#include "./stdafx.h"
#include "OpenGLGraphics.h"
#include "OpenGLViewport.h"

#ifdef WIN32
  #define VIEWPORT_CLASSNAME "OpenGLViewport"
  #define WM_GL_VIEWPORT_END_LOOP WM_USER+0x001
#else
  #include <X11/extensions/Xrandr.h>
  #define DETECT_DRAG_SIZE 3
  #define X_WINDOWS_EVENT_MASK ExposureMask|ButtonPressMask|ButtonReleaseMask|PointerMotionMask|KeyPressMask|KeyReleaseMask|StructureNotifyMask
#endif


#ifdef WIN32

int TranslateKeyFromWindows(int nWindowsKey)
{
	if( nWindowsKey=='\t' || nWindowsKey==' ' || 
		(nWindowsKey>='A' && nWindowsKey<='Z') ||
		(nWindowsKey>='0' && nWindowsKey<='9')){return nWindowsKey;}

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
	case VK_PRIOR:return GK_PAGEUP;
	case VK_NEXT:return GK_PAGEDOWN;
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
	if( nGameKey=='\t' || nGameKey==' ' || 
		(nGameKey>='A' && nGameKey<='Z') ||
		(nGameKey>='0' && nGameKey<='9')){return nGameKey;}

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
	case GK_PAGEUP:return VK_PRIOR;
	case GK_PAGEDOWN:return VK_NEXT;
	};
	return 0;
}
#else

int TranslateKeyFromX11(int nX11Key)
{
	if(nX11Key>='a' && nX11Key<='z'){return nX11Key-('a'-'A');}
	if(nX11Key>=32 && nX11Key<127){return nX11Key;}
	switch(nX11Key)
	{
	case XK_Tab:return '\t';
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
	case XK_KP_Insert:return GK_NUMPAD0;
	case XK_KP_End:return GK_NUMPAD1;
	case XK_KP_Down:return GK_NUMPAD2;
	case XK_KP_Page_Down:return GK_NUMPAD3;
	case XK_KP_Left:return GK_NUMPAD4;
	case XK_KP_Begin:return GK_NUMPAD5;
	case XK_KP_Right:return GK_NUMPAD6;
	case XK_KP_Home:return GK_NUMPAD7;
	case XK_KP_Up:return GK_NUMPAD8;
	case XK_KP_Page_Up:return GK_NUMPAD9;
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
	case '\t':return XK_Tab;
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

int COpenGLViewport::CustomXIOErrorHandler( Display*)
{
  RTTRACE("COpenGLViewport -> X11 ConnectionLost, Exiting");
  return 0;
}

#endif

COpenGLViewport::COpenGLViewport(void)
{
	m_piCallBack=NULL;
	m_bVerticalSync=false;

	GetCurrentVideoMode(&m_OriginalVideoMode);
	
#ifdef WIN32
	m_hDC=NULL;
	m_hWnd=NULL;
	m_hRenderContext=NULL;
	m_nPixelFormatIndex=0;
#else
	m_bShowSystemMouseCursor=true;
	m_pXDisplay=NULL;
	m_pGLXContext=NULL;
	m_pXColorMap=0;
	m_pXHollowCursor=0;
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
	
	m_nDblClkDetectLastButton=0;
	m_nDblClkDetectLastX=0;
	m_nDblClkDetectLastY=0;
	m_nDblClkDetectLastTime=0;
	m_nDblClkDetectMilliseconds=300;
	m_nDblClkDetectDistance=3;
	
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
			POINT p;
			p.x=pointX;
			p.y=pointY;
			ScreenToClient(hWnd,&p);
			OnMouseWheelUp(p.x,p.y);
		}
		else
		{
			POINT p;
			p.x=pointX;
			p.y=pointY;
			ScreenToClient(hWnd,&p);
			OnMouseWheelDown(p.x,p.y);
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
		if((unsigned short)wParam>=32 && (unsigned short)wParam<127){OnCharacter((unsigned short)wParam);}
		break;
	case WM_MOVE:
		OnMove(LOWORD(lParam),HIWORD(lParam));
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

	unsigned int dwStyle=WS_CAPTION|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME|WS_OVERLAPPED;
	if(dwStyle==0xFFFFFFFF){dwStyle=WS_OVERLAPPED;}
	m_hWnd = CreateWindowEx(WS_EX_DLGMODALFRAME,VIEWPORT_CLASSNAME,"Loading...",dwStyle,x,y,w,h,NULL,NULL,NULL,(void *)this);
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
	  XSetIOErrorHandler(CustomXIOErrorHandler);
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
		XSetStandardProperties(m_pXDisplay,m_XWindow,"Loading...","Loading...",None,NULL,0,NULL);
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
	SetVideoMode(&m_OriginalVideoMode);
#else
	if(m_XWindow!=None)
	{
		SetVideoMode(&m_OriginalVideoMode);
	}
	if(m_pXHollowCursor && !m_bShowSystemMouseCursor)
	{
	  XUndefineCursor(m_pXDisplay,m_XWindow);
	  XFreeCursor(m_pXDisplay,m_pXHollowCursor);
	  m_pXHollowCursor=0;
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
	  m_pXColorMap=0;
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

void COpenGLViewport::GetSize(unsigned *pdwWidth,unsigned *pdwHeight)
{
#ifdef WIN32
	RECT R={0};
	GetClientRect(m_hWnd,&R);
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

void COpenGLViewport::SetCallBack(IGenericViewportCallBack *pCallBack)
{
	m_piCallBack=pCallBack;
}

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

void COpenGLViewport::OnLButtonDoubleClick(int pointX,int pointY){if(m_piCallBack){m_piCallBack->OnLButtonDoubleClick(pointX,pointY);}}
void COpenGLViewport::OnLButtonDown(int pointX,int pointY){if(m_piCallBack){m_piCallBack->OnLButtonDown(pointX,pointY);}}
void COpenGLViewport::OnLButtonUp(int pointX,int pointY){if(m_piCallBack){m_piCallBack->OnLButtonUp(pointX,pointY);}}
void COpenGLViewport::OnRButtonDoubleClick(int pointX,int pointY){if(m_piCallBack){m_piCallBack->OnRButtonDoubleClick(pointX,pointY);}}
void COpenGLViewport::OnRButtonDown(int pointX,int pointY){if(m_piCallBack){m_piCallBack->OnRButtonDown(pointX,pointY);}}
void COpenGLViewport::OnRButtonUp(int pointX,int pointY){if(m_piCallBack){m_piCallBack->OnRButtonUp(pointX,pointY);}}
void COpenGLViewport::OnMouseMove(int pointX,int pointY){if(m_piCallBack){m_piCallBack->OnMouseMove(pointX,pointY);}}
void COpenGLViewport::OnMouseWheelUp(int pointX,int pointY){if(m_piCallBack){m_piCallBack->OnMouseWheelUp(pointX,pointY);}}
void COpenGLViewport::OnMouseWheelDown(int pointX,int pointY){if(m_piCallBack){m_piCallBack->OnMouseWheelDown(pointX,pointY);}}
void COpenGLViewport::OnCharacter(unsigned short wChar){if(m_piCallBack){m_piCallBack->OnCharacter(wChar);}}
void COpenGLViewport::OnKeyDown(unsigned short wKey){if(m_piCallBack){m_piCallBack->OnKeyDown(wKey);}}
void COpenGLViewport::OnKeyUp(unsigned short wKey){if(m_piCallBack){m_piCallBack->OnKeyUp(wKey);}}
void COpenGLViewport::OnSize(unsigned short cx,unsigned short cy){if(m_piCallBack){m_piCallBack->OnSize(cx,cy);}}
void COpenGLViewport::OnMove(unsigned x,unsigned y){if(m_piCallBack){m_piCallBack->OnMove(x,y);}}

void COpenGLViewport::SetVSync(bool bVSync)
{
	m_bVerticalSync=bVSync;

#ifdef WIN32
	wglSwapIntervalEXT(m_bVerticalSync);
#else
	glXSwapIntervalSGI(m_bVerticalSync);
#endif
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
			int nCurrentTime=GetTimeStamp();
			
			// Regular click processing
			if(event.xbutton.button==Button1){OnLButtonDown(event.xbutton.x,event.xbutton.y);}
			else if(event.xbutton.button==Button3){OnRButtonDown(event.xbutton.x,event.xbutton.y);}
			else if(event.xbutton.button==Button4){OnMouseWheelUp(event.xbutton.x,event.xbutton.y);}
			else if(event.xbutton.button==Button5){OnMouseWheelDown(event.xbutton.x,event.xbutton.y);}
			// Double click Detection
			if((event.xbutton.button==Button1 || event.xbutton.button==Button3) &&
			   m_nDblClkDetectLastButton==event.xbutton.button && 
			   (nCurrentTime-m_nDblClkDetectLastTime)<=m_nDblClkDetectMilliseconds &&
			   fabs(m_nDblClkDetectLastX-event.xbutton.x)<=m_nDblClkDetectDistance &&
			   fabs(m_nDblClkDetectLastY-event.xbutton.y)<=m_nDblClkDetectDistance)
			{
				m_nDblClkDetectLastButton=0;
				m_nDblClkDetectLastX=0;
				m_nDblClkDetectLastY=0;
				m_nDblClkDetectLastTime=0;

				if(event.xbutton.button==Button1){OnLButtonDoubleClick(event.xbutton.x,event.xbutton.y);}
				else if(event.xbutton.button==Button3){OnRButtonDoubleClick(event.xbutton.x,event.xbutton.y);}
			}
			else
			{
			    m_nDblClkDetectLastButton=event.xbutton.button;
				m_nDblClkDetectLastX=event.xbutton.x;
				m_nDblClkDetectLastY=event.xbutton.y;
				m_nDblClkDetectLastTime=nCurrentTime;
			}
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
			  OnMove(event.xconfigure.x,event.xconfigure.y);
			}
			if(m_XLastWidth!=event.xconfigure.width || 
			   m_XLastHeight!=event.xconfigure.height)
			{
			  OnSize(event.xconfigure.width,event.xconfigure.height);
			}
		  }
		  else if (event.type==DestroyNotify) 
		  {
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

#ifdef WIN32
bool  COpenGLViewport::HasMouseCapture(){return GetCapture()==m_hWnd;}
void  COpenGLViewport::SetMouseCapture(){SetCapture(m_hWnd);}
void  COpenGLViewport::ReleaseMouseCapture(){ReleaseCapture();}
#else
bool  COpenGLViewport::HasMouseCapture(){return false;}
void  COpenGLViewport::SetMouseCapture(){}
void  COpenGLViewport::ReleaseMouseCapture(){}
#endif

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
	Display *pDisplay=XOpenDisplay(NULL);
	if(pDisplay)
	{
	  int nScreen=DefaultScreen(pDisplay);
	  pMode->w = DisplayWidth(pDisplay,nScreen);
	  pMode->h = DisplayHeight(pDisplay,nScreen);
	  pMode->bpp = DefaultDepth(pDisplay,nScreen);
	  pMode->rate=60;
	  
	  XCloseDisplay(pDisplay);
	  pDisplay=NULL;
	}	
	//RTTRACE("GetCurrentVideoMode : %dx%d : %d",pMode->w,pMode->h,pMode->bpp);
#endif
}

bool COpenGLViewport::SetVideoMode(SVideoMode *pMode)
{
#ifdef WIN32
	DEVMODE mode={0};
	mode.dmSize=sizeof(DEVMODE);
	mode.dmBitsPerPel=(DWORD)pMode->bpp;
	mode.dmPelsWidth=(DWORD)pMode->w;
	mode.dmPelsHeight=(DWORD)pMode->h;
	mode.dmDisplayFrequency=(DWORD)pMode->rate;
	mode.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFREQUENCY;
	return ChangeDisplaySettings(&mode,CDS_FULLSCREEN)==DISP_CHANGE_SUCCESSFUL;
#else
	int nSizeIndex=-1;
	int nScreenSizes=0;
	int nScreenRates=0;
	SVideoMode sCurrentMode;
	GetCurrentVideoMode(&sCurrentMode);
	if(sCurrentMode.w==pMode->w && sCurrentMode.h==pMode->h){return true;}

	XRRScreenConfiguration *pScreenConfig=XRRGetScreenInfo (m_pXDisplay,m_XWindow);
	if(pScreenConfig==NULL)
	{
	  RTTRACE("COpenGLViewport::SetVideoMode -> Failed to get screen configuration");
	  return false;
	}
	XRRScreenSize *pScreenSizes=XRRConfigSizes(pScreenConfig,&nScreenSizes);
	for(int x=0;x<nScreenSizes;x++)
	{
		//RTTRACE("COpenGLViewport::SetVideoMode -> Supported size %dx%d",pScreenSizes[x].width,pScreenSizes[x].height);
		if(pScreenSizes[x].width==(int)pMode->w && pScreenSizes[x].height==(int)pMode->h){nSizeIndex=x;}
	}
	if(nSizeIndex==-1)
	{
	  RTTRACE("COpenGLViewport::SetVideoMode -> Current screen does not support %dx%d",pMode->w,pMode->h);
  	  XRRFreeScreenConfigInfo(pScreenConfig);
	  return false;
	}
	short int rate=pMode->rate;
	bool bRateFound=false;
	short int *pScreenRates=XRRConfigRates(pScreenConfig,nSizeIndex,&nScreenRates);
	for(int x=0;x<nScreenRates;x++)
	{
		//RTTRACE("COpenGLViewport::SetVideoMode -> Supported size %dx%d, rate %d",pMode->w,pMode->h,pScreenRates[x]);
		if(pScreenRates[x]==(short int)pMode->rate){bRateFound=true;}
	}
	if(!bRateFound)
	{
	  if(nScreenRates)
	  {
		rate=pScreenRates[nScreenRates-1];
		RTTRACE("COpenGLViewport::SetVideoMode -> Warning! Current screen %dx%d does not support rate %d, applying rate %d",pMode->w,pMode->h,pMode->rate,rate);
	  }
	  else
	  {
		RTTRACE("COpenGLViewport::SetVideoMode -> Error! no rate supported for %dx%d",pMode->w,pMode->h);
		XRRFreeScreenConfigInfo(pScreenConfig);
		return false;
	  }  
	}
	Status result=XRRSetScreenConfigAndRate(m_pXDisplay,pScreenConfig,m_XWindow,nSizeIndex,RR_Rotate_0,rate,CurrentTime);
	if(result==RRSetConfigSuccess)
	{
	  XRRFreeScreenConfigInfo(pScreenConfig);
	  sleep(1);
	  return true;
	}
	else
	{
	  RTTRACE("COpenGLViewport::SetVideoMode -> Error! Video mode change result %d",result);
	  XRRFreeScreenConfigInfo(pScreenConfig);
	  return false;
	}
#endif
}

bool COpenGLViewport::SetFullScreen(unsigned int w,unsigned int h,unsigned int bpp,unsigned int rate)
{
	SVideoMode mode;
	mode.bpp=bpp;
	mode.h=h;
	mode.w=w;
	mode.rate=rate;

#ifdef WIN32
	SetVideoMode(&mode);
	SetMaximized(true);
	return true;
#else
	//RTTRACE("COpenGLViewport::SetFullScreen -> Enter %dx%d",w,h);
		
	SetVideoMode(&mode);
	
	XSetWindowAttributes attributes;
	attributes.override_redirect=true;
	XChangeWindowAttributes(m_pXDisplay,m_XWindow,CWOverrideRedirect,&attributes);

	XWindowChanges changes;
	changes.width=w;
	changes.height=h;
	changes.x=0;
	changes.y=0;
	changes.border_width=0;
	XConfigureWindow(m_pXDisplay,m_XWindow,CWX|CWY|CWWidth|CWHeight|CWBorderWidth,&changes);
	XRaiseWindow(m_pXDisplay,m_XWindow);
	XSetInputFocus(m_pXDisplay,m_XWindow,RevertToPointerRoot,CurrentTime);
	XGrabKeyboard( m_pXDisplay,m_XWindow, True, GrabModeAsync, GrabModeAsync, CurrentTime );
	
	//RTTRACE("COpenGLViewport::SetFullScreen -> %dx%d:%d",w,h,bpp);
	return true;
#endif
}
bool COpenGLViewport::SetWindowed(unsigned int x,unsigned int y,unsigned int w,unsigned int h)
{

#ifdef WIN32
	// Las coordenadas y tamaños que se gestion siempre son del area cliente.
	// por lo que al establecer la pos de la ventana se deben convertir a coordenadas
	// de pantalla.
	SetVideoMode(&m_OriginalVideoMode);

	SetMaximized(false);
	RECT wr={0},cr={0};
	GetWindowRect(m_hWnd,&wr);
	GetClientRect(m_hWnd,&cr);
	SIZE ws,cs;
	ws.cx=wr.right-wr.left;
	ws.cy=wr.bottom-wr.top;
	cs.cx=cr.right-cr.left;
	cs.cy=cr.bottom-cr.top;
	SIZE nonclientsize;
	nonclientsize.cx=ws.cx-cs.cx;
	nonclientsize.cy=ws.cy-cs.cy;

	POINT p={0};
	ClientToScreen(m_hWnd,&p);
	SetWindowPos(m_hWnd,NULL,x-(p.x-wr.left),y-(p.y-wr.top),w+nonclientsize.cx,h+nonclientsize.cy,SWP_NOZORDER);
	return true;
#else

	//RTTRACE("COpenGLViewport::SetWindowed -> Enter %dx%d",w,h);
	
	if(m_pXDisplay==NULL || m_XWindow==None){return false;}
	SetVideoMode(&m_OriginalVideoMode);
	
	XUngrabKeyboard( m_pXDisplay,CurrentTime);
	XSetWindowAttributes attributes;
	attributes.override_redirect=false;
	XChangeWindowAttributes(m_pXDisplay,m_XWindow,CWOverrideRedirect,&attributes);
	
	XWindowChanges changes;
	changes.width=w;
	changes.height=h;
	changes.x=x;
	changes.y=y;
	changes.border_width=0;
	XConfigureWindow(m_pXDisplay,m_XWindow,CWX|CWY|CWWidth|CWHeight|CWBorderWidth,&changes);
		
	//RTTRACE("COpenGLViewport::SetWindowed -> Exit %dx%d",w,h);
	return true;
#endif
}

