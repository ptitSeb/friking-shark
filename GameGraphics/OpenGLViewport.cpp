#include "./stdafx.h"
#include "OpenGLGraphics.h"
#include "OpenGLViewport.h"
#ifdef WIN32
#define VIEWPORT_CLASSNAME "OpenGLViewport"
#define WM_GL_VIEWPORT_END_LOOP WM_USER+0x001
#else
#include <SDL/SDL.h>
#endif

#define DETECT_DRAG_SIZE 3

COpenGLViewport::COpenGLViewport(void)
{
	m_piCallBack=NULL;
	m_bVerticalSync=false;

#ifdef WIN32
	m_hDC=NULL;
	m_hWnd=NULL;
	m_hRenderContext=NULL;
	m_nPixelFormatIndex=0;
	m_bShowSystemMouseCursor=true;
#else

	m_pSDLWindow=NULL;
	m_bSDLInitialized=false;
	m_nLoopDepth=0;
	m_nSDLWindowFlags=0;

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
	SetWindowLong(hWnd,GWL_USERDATA,(DWORD)this);
	m_hDC=GetDC(hWnd);
	if(m_hDC)
	{
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
	SetWindowLong(m_hWnd,GWL_USERDATA,(DWORD)NULL);
}

LRESULT COpenGLViewport::ProcessMessage(HWND hWnd,UINT  uMsg, WPARAM  wParam,LPARAM  lParam)
{
	POINT point;
	point.x=(short)LOWORD(lParam);
	point.y=(short)HIWORD(lParam);

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
		OnKeyDown((WORD)wParam);
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		OnKeyUp((WORD)wParam);
		break;
	case WM_LBUTTONDOWN:
		OnLButtonDown(wParam,point);
		break;
	case WM_RBUTTONDOWN:
		OnRButtonDown(wParam,point);
		break;
	case WM_LBUTTONUP:
		OnLButtonUp(wParam,point);
		break;
	case WM_RBUTTONUP:
		OnRButtonUp(wParam,point);
		break;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam,point);
		break;
	case WM_SIZE:
		OnSize(LOWORD(lParam),HIWORD(lParam));
		break;
	case WM_CHAR:
		OnCharacter((WORD)wParam);
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

bool COpenGLViewport::Create(RECT *pRect,bool bMaximized)
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

	DWORD dwStyle=WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME|WS_OVERLAPPED;
	if(dwStyle==0xFFFFFFFF){dwStyle=WS_OVERLAPPED;}
	m_hWnd = CreateWindowEx(WS_EX_DLGMODALFRAME,VIEWPORT_CLASSNAME,"ViewPort",dwStyle,pRect->left,pRect->top,pRect->right-pRect->left,pRect->bottom-pRect->top,NULL,NULL,NULL,(void *)this);
	if(m_hWnd)
	{
		DWORD dwStyle=GetWindowLong(m_hWnd,GWL_STYLE);
		SetWindowLong(m_hWnd,GWL_STYLE,dwStyle);
		ShowWindow(m_hWnd,bMaximized?SW_MAXIMIZE:SW_SHOW);
		EnableWindow(m_hWnd,TRUE);
		UpdateWindow(m_hWnd);
	}
	bOk=(m_hRenderContext!=NULL);

#else
	bOk=(SDL_Init(SDL_INIT_VIDEO)==0);
	if(!bOk)
	{
		RTTRACE("COpenGLViewport::Create -> Failed to initialize SDL: %s",SDL_GetError());
		return false;
	}
	
	if(bOk)
	{
		m_nSDLWindowFlags=SDL_HWSURFACE|SDL_OPENGL|SDL_RESIZABLE;
//		if(bMaximized){m_nSDLWindowFlags|=SDL_FULLSCREEN;}
		SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL,m_bVerticalSync);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1 );
		m_pSDLWindow=SDL_SetVideoMode(pRect->right-pRect->left,pRect->bottom-pRect->top, 32, m_nSDLWindowFlags);
		bOk=(m_pSDLWindow!=NULL);
		if(!bOk)
		{
			RTTRACE("COpenGLViewport::Create -> Failed to create window: %s",SDL_GetError());
		}
	}
	
	SetCurrentRenderTarget(true);

	if(bOk)
	{
		SDL_EnableKeyRepeat(0,0);
		m_bSDLInitialized=true;
	}
	else
	{
		SDL_Quit();
	}
#endif
	return bOk;
}

void COpenGLViewport::Destroy()
{
#ifdef WIN32
	if(m_hWnd){DestroyWindow(m_hWnd);m_hWnd=NULL;}
#else
	if(!m_bSDLInitialized){SDL_Quit();m_bSDLInitialized=false;}
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
	return (m_pSDLWindow && m_pSDLWindow->flags&SDL_FULLSCREEN);
#endif
}

void COpenGLViewport::SetMaximized(bool bMaximized)
{
#ifdef WIN32
	DWORD dwStyle=GetWindowLong(m_hWnd,GWL_STYLE);
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
	if(m_pSDLWindow)
	{
//		m_pSDLWindow->flags|=SDL_FULLSCREEN;
	}
#endif
}

void COpenGLViewport::SetPos(unsigned dwX,unsigned dwY)
{
#ifdef WIN32
	SetWindowPos(m_hWnd,NULL,dwX,dwY,0,0,SWP_NOSIZE|SWP_NOZORDER);
#else
	#pragma message("COpenGLViewport::Create -> Falta saber como cambiar la posicion de la ventana")
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
	*pdwX=0;
	*pdwY=0;
	#pragma message("COpenGLViewport::Create -> Falta saber la posicion de la ventana")
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
	if(m_pSDLWindow)
	{
		*pdwWidth=m_pSDLWindow->w;
		*pdwHeight=m_pSDLWindow->h;
	}
	else
	{
		*pdwWidth=*pdwHeight=0;
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
	if(m_pSDLWindow){m_pSDLWindow=SDL_SetVideoMode(m_pSDLWindow->w, m_pSDLWindow->h, 32,m_nSDLWindowFlags);}
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
	if(m_pSDLWindow)
	{
		*pdwX=0;
		*pdwY=0;
		*pdwWidth=m_pSDLWindow->w;
		*pdwHeight=m_pSDLWindow->h;
	}
	else
	{
		*pdwX=*pdwY=*pdwWidth=*pdwHeight=0;
	}
#endif
}

void COpenGLViewport::SetRect(unsigned dwX,unsigned dwY,unsigned dwWidth,unsigned dwHeight)
{
#ifdef WIN32
	SetWindowPos(m_hWnd,NULL,dwX,dwY,dwWidth,dwHeight,SWP_NOZORDER);
#else
	if(m_pSDLWindow){m_pSDLWindow=SDL_SetVideoMode(dwWidth, dwHeight, 32,m_nSDLWindowFlags);}
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
	SDL_GL_SwapBuffers();
#endif
}

void COpenGLViewport::OnLButtonDown(WORD wKeyState,POINT mousepos){if(m_piCallBack){m_piCallBack->OnLButtonDown(wKeyState,mousepos.x,mousepos.y);}}
void COpenGLViewport::OnLButtonUp(WORD wKeyState,POINT mousepos){if(m_piCallBack){m_piCallBack->OnLButtonUp(wKeyState,mousepos.x,mousepos.y);}}
void COpenGLViewport::OnRButtonDown(WORD wKeyState,POINT mousepos){if(m_piCallBack){m_piCallBack->OnRButtonDown(wKeyState,mousepos.x,mousepos.y);}}
void COpenGLViewport::OnRButtonUp(WORD wKeyState,POINT mousepos){if(m_piCallBack){m_piCallBack->OnRButtonUp(wKeyState,mousepos.x,mousepos.y);}}
void COpenGLViewport::OnMouseMove(WORD wKeyState,POINT mousepos){if(m_piCallBack){m_piCallBack->OnMouseMove(mousepos.x,mousepos.y);}}
void COpenGLViewport::OnCharacter(WORD wChar){if(m_piCallBack){m_piCallBack->OnCharacter(wChar);}}
void COpenGLViewport::OnKeyDown(WORD wKey){if(m_piCallBack){m_piCallBack->OnKeyDown(wKey);}}
void COpenGLViewport::OnKeyUp(WORD wKey){if(m_piCallBack){m_piCallBack->OnKeyUp(wKey);}}
void COpenGLViewport::OnSize(WORD cx,WORD cy){if(m_piCallBack){m_piCallBack->OnSize(cx,cy);}}
void COpenGLViewport::OnMove(){if(m_piCallBack){m_piCallBack->OnMove();}}

void COpenGLViewport::SetVSync(bool bVSync)
{
	m_bVerticalSync=bVSync;
	#pragma message("COpenGLViewport::Create -> me he cargado el SetVSync de OpenGLExtensions (deberia ir a traves de SDL")
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
	SDL_WM_SetCaption(m_sCaption.c_str(), NULL);
#endif
}

std::string COpenGLViewport::GetCaption()
{
	return m_sCaption;
}

void	COpenGLViewport::SetIcon(HICON hIcon)
{
/*
	if(COpenGLViewPortBase::m_hWnd)
	{
		::SendMessage(COpenGLViewPortBase::m_hWnd,WM_SETICON,ICON_BIG,(LPARAM)hIcon);
		::SendMessage(COpenGLViewPortBase::m_hWnd,WM_SETICON,ICON_SMALL,(LPARAM)hIcon);
	}
*/
}

HICON	COpenGLViewport::GetIcon()
{
/*
	if(COpenGLViewPortBase::m_hWnd)
	{
		return (HICON)::SendMessage(COpenGLViewPortBase::m_hWnd,WM_GETICON,ICON_BIG,0);
	}
	else
	{
		return NULL;
	}
*/
	return NULL;
}
#ifdef WIN32
#else
int TranslateKeyFromSDL(int nSDLKey)
{
	if(nSDLKey>=32 && nSDLKey<128){return nSDLKey;}
	switch(nSDLKey)
	{
	  case SDLK_UP:return VK_UP;
	  case SDLK_DOWN:return VK_DOWN;
	  case SDLK_LEFT:return VK_LEFT;
	  case SDLK_RIGHT:return VK_RIGHT;
	  case SDLK_KP0:return VK_NUMPAD0;
	  case SDLK_KP1:return VK_NUMPAD1;
	  case SDLK_KP2:return VK_NUMPAD2;
	  case SDLK_KP3:return VK_NUMPAD3;
	  case SDLK_KP4:return VK_NUMPAD4;
	  case SDLK_KP5:return VK_NUMPAD5;
	  case SDLK_KP6:return VK_NUMPAD6;
	  case SDLK_KP7:return VK_NUMPAD7;
	  case SDLK_KP8:return VK_NUMPAD8;
	  case SDLK_KP9:return VK_NUMPAD9;
	  case SDLK_LCTRL:return VK_LCONTROL;
	  case SDLK_MENU:return VK_MENU;
	  case SDLK_HOME:return VK_HOME;
	  case SDLK_END:return VK_END;
	  case SDLK_BACKSPACE:return VK_BACK;
	  case SDLK_DELETE:return VK_DELETE;
	  case SDLK_ESCAPE:return VK_ESCAPE;
	  case SDLK_RETURN:return VK_RETURN;
	  case SDLK_LALT:return VK_LMENU;
	  case SDLK_LSHIFT:return VK_LSHIFT;
	  case SDLK_INSERT:return VK_INSERT;
	  case SDLK_F1:return VK_F1;
	  case SDLK_F2:return VK_F2;
	  case SDLK_F3:return VK_F3;
	  case SDLK_F4:return VK_F4;
	  case SDLK_F5:return VK_F5;
	  case SDLK_F6:return VK_F6;
	  case SDLK_F7:return VK_F7;
	  case SDLK_F8:return VK_F8;
	  case SDLK_F9:return VK_F9;
	  case SDLK_F10:return VK_F10;
	  case SDLK_F11:return VK_F11;
	  case SDLK_F12:return VK_F12;
	  case SDLK_PAUSE:return VK_PAUSE;
	};
	return 0;
}

int TranslateKeyToSDL(int nGameKey)
{
	if(nGameKey>='A' && nGameKey<='Z'){return nGameKey+('a'-'A');}
	if(nGameKey>=32 && nGameKey<128){return nGameKey;}
	switch(nGameKey)
	{
	  case VK_UP:return SDLK_UP;
	  case VK_DOWN:return SDLK_DOWN;
	  case VK_LEFT:return SDLK_LEFT;
	  case VK_RIGHT:return SDLK_RIGHT;
	  case VK_NUMPAD0:return SDLK_KP0;
	  case VK_NUMPAD1:return SDLK_KP1;
	  case VK_NUMPAD2:return SDLK_KP2;
	  case VK_NUMPAD3:return SDLK_KP3;
	  case VK_NUMPAD4:return SDLK_KP4;
	  case VK_NUMPAD5:return SDLK_KP5;
	  case VK_NUMPAD6:return SDLK_KP6;
	  case VK_NUMPAD7:return SDLK_KP7;
	  case VK_NUMPAD8:return SDLK_KP8;
	  case VK_NUMPAD9:return SDLK_KP9;
	  case VK_LCONTROL:return SDLK_LCTRL;
	  case VK_MENU:return SDLK_MENU;
	  case VK_HOME:return SDLK_HOME;
	  case VK_END:return SDLK_END;
	  case VK_BACK:return SDLK_BACKSPACE;
	  case VK_DELETE:return SDLK_DELETE;
	  case VK_ESCAPE:return SDLK_ESCAPE;
	  case VK_RETURN:return SDLK_RETURN;
	  case VK_LMENU:return SDLK_LALT;
	  case VK_LSHIFT:return SDLK_LSHIFT;
	  case VK_INSERT:return SDLK_INSERT;
	  case VK_F1:return SDLK_F1;
	  case VK_F2:return SDLK_F2;
	  case VK_F3:return SDLK_F3;
	  case VK_F4:return SDLK_F4;
	  case VK_F5:return SDLK_F5;
	  case VK_PAUSE:return SDLK_PAUSE;
	  case VK_F10:return SDLK_F10;
	};
	return 0;
}
#endif
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
	POINT point;
	int nLoopId=++m_nLoopDepth;
	while(m_nLoopDepth>=nLoopId)
	{ 
		SDL_Event event; 
		while(SDL_PollEvent(&event))
		{ 
			int nKeyCode=0;
			switch(event.type)
			{ 
				case SDL_QUIT:
					ExitLoop();
					break;
				case SDL_VIDEORESIZE:
					m_pSDLWindow=SDL_SetVideoMode(event.resize.w, event.resize.h, 32,m_nSDLWindowFlags); 
					OnSize(event.resize.w, event.resize.h);
					break; 
				case SDL_KEYDOWN:
					nKeyCode=TranslateKeyFromSDL(event.key.keysym.sym);
					OnKeyDown(nKeyCode);
					break;
				case SDL_KEYUP:
					nKeyCode=TranslateKeyFromSDL(event.key.keysym.sym);
					OnKeyUp(nKeyCode);
					break;
				case SDL_MOUSEBUTTONDOWN:
					point.x=event.button.x;
					point.y=event.button.y;
					if(event.button.button==SDL_BUTTON_LEFT){OnLButtonDown(0,point);}
					else if(event.button.button==SDL_BUTTON_RIGHT){OnRButtonDown(0,point);}
					break;			
				case SDL_MOUSEBUTTONUP:
					point.x=event.button.x;
					point.y=event.button.y;
					if(event.button.button==SDL_BUTTON_LEFT){OnLButtonUp(0,point);}
					else if(event.button.button==SDL_BUTTON_RIGHT){OnRButtonUp(0,point);}
					
					if(m_nDetectDragButton==event.button.button)
					{
					  m_nLoopDepth--;
					  return;
					}
					break;
				case SDL_MOUSEMOTION:
					point.x=event.motion.x;
					point.y=event.motion.y;
					OnMouseMove(0,point);
					
					if(m_nDetectDragButton!=0)
					{
					  int nXDist=point.x-m_nDetectDragX;
					  int nYDist=point.y-m_nDetectDragY;
					  
					  if(nXDist>DETECT_DRAG_SIZE || nXDist<(0-DETECT_DRAG_SIZE) ||
						 nYDist>DETECT_DRAG_SIZE || nYDist<(0-DETECT_DRAG_SIZE))
					  {
						m_bDetectedDrag=true;
						m_nLoopDepth--;
						return;
					  }
					}
					break;
			}
		}
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
	SDL_GetMouseState(pX,pY);
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
	SDL_WarpMouse(x,y);
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
	USHORT nKeyState=GetKeyState(nKey);
	return (nKeyState&0x8000)!=0;
#else
	int nSDLKey=TranslateKeyToSDL(nKey);
	int nKeys=0;
	Uint8 *pKeys=SDL_GetKeyState(&nKeys);
	if(nSDLKey>=nKeys){return false;}
	return pKeys[nSDLKey];
#endif
}

bool  COpenGLViewport::IsActiveWindow(){return true;}

bool  COpenGLViewport::IsMouseVisible()
{
#ifdef WIN32
	return m_bShowSystemMouseCursor;
#else
	return SDL_ShowCursor(-1)!=0;
#endif
}
void  COpenGLViewport::ShowMouseCursor(bool bShow)
{
#ifdef WIN32
	m_bShowSystemMouseCursor=bShow;
#else
	SDL_ShowCursor(bShow);
#endif
}

bool COpenGLViewport::DetectDrag(double dx,double dy)
{
#ifdef WIN32
	unsigned int w=0,h=0;
	GetSize(&w,&h);
	GUITHREADINFO threadInfo={0};
	threadInfo.cbSize=sizeof(threadInfo);
	GetGUIThreadInfo(GetCurrentThreadId(),&threadInfo);
	DWORD dwFlags=MB_OK;
	POINT pt;
	pt.x=(LONG)dx;
	pt.y=(LONG)((h-1)-dy);
	return (DragDetect(threadInfo.hwndActive,pt)?true:false);
#else
	if(m_nDetectDragButton){return false;}
	
	m_nDetectDragButton=SDL_BUTTON_LEFT;
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
	if((m_nSDLWindowFlags&SDL_FULLSCREEN)==0)
	{
	  SDL_WM_ToggleFullScreen(m_pSDLWindow); 
	  m_nSDLWindowFlags|=(SDL_FULLSCREEN);
	}
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
	if((m_nSDLWindowFlags&SDL_FULLSCREEN)!=0)
	{
	  SDL_WM_ToggleFullScreen(m_pSDLWindow); 
	  m_nSDLWindowFlags&=~(SDL_FULLSCREEN);
	}
	return true;
#endif
	/*RTTRACE("COpenGLViewport::SetCurrentVideoMode -> Setting windowed %dx%d - %dx%d",x,y,w,h);
	m_nSDLWindowFlags&=~(SDL_FULLSCREEN);
	m_pSDLWindow=SDL_SetVideoMode(w, h, 0,m_nSDLWindowFlags); 
	if(m_pSDLWindow){OnSize(m_pSDLWindow->w,m_pSDLWindow->h);}
	return m_pSDLWindow!=NULL;*/
/*		DEVMODE mode={0};
		mode.dmSize=sizeof(DEVMODE);
		mode.dmBitsPerPel=(DWORD)m_sScreenProperties.dFullScreenRefreshBitsPerPixel;
		mode.dmPelsWidth=(DWORD)m_sScreenProperties.sFullScreenResolution.w;
		mode.dmPelsHeight=(DWORD)m_sScreenProperties.sFullScreenResolution.h;
		mode.dmDisplayFrequency=(DWORD)m_sScreenProperties.dFullScreenRefreshRate;
		mode.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFREQUENCY;
		ChangeDisplaySettings(&mode,CDS_FULLSCREEN);*/
}












  







