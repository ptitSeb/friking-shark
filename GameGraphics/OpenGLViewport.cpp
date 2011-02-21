#include "./stdafx.h"
#include "OpenGLGraphics.h"
#include "OpenGLViewport.h"
#include <SDL/SDL.h>

//#define WM_GUI_MANAGER_END_LOOP WM_USER+0x001


#define DETECT_DRAG_SIZE 3

COpenGLViewport::COpenGLViewport(void)
{
	m_piCallBack=NULL;
	m_bVerticalSync=false;
	m_pSDLWindow=NULL;
	m_bSDLInitialized=false;
	m_nLoopDepth=0;
	m_nSDLWindowFlags=0;

	m_bDetectedDrag=false;
	m_nDetectDragX=0;
	m_nDetectDragY=0;
	m_nDetectDragButton=0;
}

COpenGLViewport::~COpenGLViewport(void)
{
}

bool COpenGLViewport::Create(RECT *pRect,bool bMaximized)
{
	bool bOk;
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
	
	return true;
}

void COpenGLViewport::Destroy()
{
	if(!m_bSDLInitialized){SDL_Quit();m_bSDLInitialized=false;}
	
	CSystemObjectBase::Destroy();
}

bool COpenGLViewport::IsMaximized()
{
	return (m_pSDLWindow && m_pSDLWindow->flags&SDL_FULLSCREEN);
}

void COpenGLViewport::SetMaximized(bool bMaximized)
{
	if(m_pSDLWindow)
	{
//		m_pSDLWindow->flags|=SDL_FULLSCREEN;
	}
}

void COpenGLViewport::SetPos(unsigned dwX,unsigned dwY)
{
	#pragma message("COpenGLViewport::Create -> Falta saber como cambiar la posicion de la ventana")
}

void COpenGLViewport::GetPos(unsigned *pdwX,unsigned *pdwY)
{
	*pdwX=0;
	*pdwY=0;
	#pragma message("COpenGLViewport::Create -> Falta saber la posicion de la ventana")
}

void COpenGLViewport::GetSize(unsigned *pdwWidth,unsigned *pdwHeight)
{
	if(m_pSDLWindow)
	{
		*pdwWidth=m_pSDLWindow->w;
		*pdwHeight=m_pSDLWindow->h;
	}
	else
	{
		*pdwWidth=*pdwHeight=0;
	}
}

void COpenGLViewport::GetClientSize(unsigned *pdwWidth,unsigned *pdwHeight)
{
	return GetSize(pdwWidth,pdwHeight);
}

void COpenGLViewport::SetSize(unsigned dwWidth,unsigned dwHeight)
{
	if(m_pSDLWindow){m_pSDLWindow=SDL_SetVideoMode(m_pSDLWindow->w, m_pSDLWindow->h, 32,m_nSDLWindowFlags);}
}

void COpenGLViewport::GetRect(unsigned *pdwX,unsigned *pdwY,unsigned *pdwWidth,unsigned *pdwHeight)
{
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
}

void COpenGLViewport::SetRect(unsigned dwX,unsigned dwY,unsigned dwWidth,unsigned dwHeight)
{
	if(m_pSDLWindow){m_pSDLWindow=SDL_SetVideoMode(dwWidth, dwHeight, 32,m_nSDLWindowFlags);}
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
	
	SDL_GL_SwapBuffers();
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
	SDL_WM_SetCaption(m_sCaption.c_str(), NULL);	
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

void COpenGLViewport::EnterLoop()
{
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
}
/*
	while(1)
	{
		MSG msg;
		while(PeekMessage(&msg,NULL,0,0,FALSE))
		{
			if(msg.hwnd==m_Viewport.m_piViewport->GetWindowHandle() && msg.message==WM_GUI_MANAGER_END_LOOP)
			{
				PeekMessage(&msg,NULL,0,0,TRUE);
				InvalidateRect(msg.hwnd,NULL,FALSE);
				return;
			}
			GetMessage(&msg,NULL,0,0);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		InvalidateRect(m_Viewport.m_piViewport->GetWindowHandle(),NULL,FALSE);
	}*/
void COpenGLViewport::ExitLoop()
{
	if(m_nLoopDepth>0){m_nLoopDepth--;}
	//PostMessage(m_Viewport.m_piViewport->GetWindowHandle(),WM_GUI_MANAGER_END_LOOP,0,0);
}

void  COpenGLViewport::GetCursorPos(int *pX,int *pY){SDL_GetMouseState(pX,pY);}
void  COpenGLViewport::SetCursorPos(int x,int y){SDL_WarpMouse(x,y);}

bool  COpenGLViewport::HasMouseCapture(){return false;}
void  COpenGLViewport::SetMouseCapture(){}
void  COpenGLViewport::ReleaseMouseCapture(){}

bool  COpenGLViewport::IsKeyDown(unsigned int nKey)
{
	int nSDLKey=TranslateKeyToSDL(nKey);
	int nKeys=0;
	Uint8 *pKeys=SDL_GetKeyState(&nKeys);
	if(nSDLKey>=nKeys){return false;}
	return pKeys[nSDLKey];
}

bool  COpenGLViewport::IsActiveWindow(){return true;}

bool  COpenGLViewport::IsMouseVisible(){return SDL_ShowCursor(-1)!=0;}
void  COpenGLViewport::ShowMouseCursor(bool bShow){SDL_ShowCursor(bShow);}

bool COpenGLViewport::DetectDrag(double dx,double dy)
{
/*#ifdef WIN32
	GUITHREADINFO threadInfo={0};
	threadInfo.cbSize=sizeof(threadInfo);
	GetGUIThreadInfo(GetCurrentThreadId(),&threadInfo);
	DWORD dwFlags=MB_OK;
	POINT pt;
	pt.x=(LONG)dx;
	pt.y=(LONG)((m_rRealRect.h-1)-dy);
	return (DragDetect(threadInfo.hwndActive,pt)?true:false);
#else*/
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
//#endif  
}

void COpenGLViewport::GetCurrentVideoMode(SVideoMode *pMode)
{
	Display* pDisplay = XOpenDisplay( NULL );
	if (!pDisplay ) {return;}
	Screen *pScreen = DefaultScreenOfDisplay( pDisplay );
	if ( !pScreen ) {return;}

	pMode->w = pScreen->width;
	pMode->h = pScreen->height;
	pMode->bpp = pScreen->root_depth;
	pMode->rate=60;
	XCloseDisplay( pDisplay );
}

bool COpenGLViewport::SetFullScreen(unsigned int w,unsigned int h,unsigned int bpp,unsigned int rate)
{
	if((m_nSDLWindowFlags&SDL_FULLSCREEN)==0)
	{
	  SDL_WM_ToggleFullScreen(m_pSDLWindow); 
	  m_nSDLWindowFlags|=(SDL_FULLSCREEN);
	}
	return true;
/*	RTTRACE("COpenGLViewport::SetCurrentVideoMode -> Setting full screen video mode to %dx%d : %d",w,h,bpp);
	m_nSDLWindowFlags|=SDL_FULLSCREEN;
	m_pSDLWindow=SDL_WM_ToggleFullScreen(); 
	if(m_pSDLWindow){OnSize(m_pSDLWindow->w,m_pSDLWindow->h);}
	return m_pSDLWindow!=NULL;*/
	//if(m_pSDLWindow){OnSize(m_pSDLWindow->w,m_pSDLWindow->h);}
}

bool COpenGLViewport::SetWindowed(unsigned int x,unsigned int y,unsigned int w,unsigned int h)
{
	if((m_nSDLWindowFlags&SDL_FULLSCREEN)!=0)
	{
	  SDL_WM_ToggleFullScreen(m_pSDLWindow); 
	  m_nSDLWindowFlags&=~(SDL_FULLSCREEN);
	}
	return true;
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












  







