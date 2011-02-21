// OpenGLViewPortBase.cpp: implementation of the COpenGLViewPortBase class.
//
//////////////////////////////////////////////////////////////////////

#include "./StdAfx.h"
#include <crtdbg.h>
#include "OpenGLViewPortBase.h"
#include "BMPFiles.h"

#define VIEWPORT_CLASSNAME		"MY423"
#define SMOOTH_MOVEMENT_TIMER	1
#define FOCUS_FRAME_SIZE		3

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COpenGLViewPortBase::COpenGLViewPortBase()
{
	m_hDC=NULL;
	m_hRenderContext=NULL;
	m_hWnd=NULL;
	m_bInitialized=false;

	m_nPixelFormatIndex=0;
}

COpenGLViewPortBase::~COpenGLViewPortBase()
{
}

HGLRC COpenGLViewPortBase::CreateRenderContext()
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
	if(!SetPixelFormat(m_hDC, m_nPixelFormatIndex, &pixelFormat))
	{
		DWORD dwError=GetLastError();
		return NULL;
	}
	
	m_hRenderContext=wglCreateContext(m_hDC);
	return m_hRenderContext;
}

HGLRC COpenGLViewPortBase::ActivateRenderContext()
{
	if(m_hRenderContext==NULL)
	{
		CreateRenderContext();
	}
	if(m_hRenderContext!=wglGetCurrentContext())
	{
		wglMakeCurrent(m_hDC,m_hRenderContext);
		DWORD dwError=GetLastError();
	}
	return m_hRenderContext;
}

void COpenGLViewPortBase::DeactivateRenderContext()
{
	if(m_hRenderContext==NULL){return;}
	wglMakeCurrent(m_hDC,NULL);
}

void COpenGLViewPortBase::DestroyRenderContext()
{

	if(m_hRenderContext==NULL){return;}
	wglMakeCurrent(m_hDC,NULL);
	wglDeleteContext(m_hRenderContext);
	m_hRenderContext=NULL;
}

void COpenGLViewPortBase::Init(HWND hHostWnd,DWORD dwShowCommand,DWORD dwStyle,DWORD dwStyleEx)
{
	WNDCLASSEX		wcex={0};
	wcex.cbSize=sizeof(wcex);
	if(!GetClassInfoEx(NULL,VIEWPORT_CLASSNAME,&wcex))
   	{
		WNDCLASS wc={0};
		wc.style			= CS_HREDRAW | CS_VREDRAW|CS_DBLCLKS;
		wc.lpfnWndProc      = (WNDPROC) COpenGLViewPortBase::ViewPortWindowProc;
		wc.lpszClassName    = VIEWPORT_CLASSNAME;
		wc.hInstance		= GetModuleHandle(NULL);
		RegisterClass(&wc);
	}
	
	RECT hostRect={0};
	GetWindowRect(hHostWnd,&hostRect);
	POINT point={hostRect.left,hostRect.top};
	ScreenToClient(GetParent(hHostWnd),&point);
	HWND hParent=hHostWnd?GetParent(hHostWnd):NULL;
	if(dwStyle==0xFFFFFFFF){dwStyle=hParent?WS_CHILD | WS_TABSTOP:WS_OVERLAPPED;}
	m_hWnd = CreateWindowEx(dwStyleEx,VIEWPORT_CLASSNAME,"ViewPort",dwStyle,point.x,point.y,hostRect.right-hostRect.left,hostRect.bottom-hostRect.top,hParent,NULL,NULL,(void *)this);
	if(m_hWnd)
	{
		int id=GetWindowLong(hHostWnd,GWL_ID);
		SetWindowLong(hHostWnd,GWL_ID,0);
		SetWindowLong(m_hWnd,GWL_ID,id);
		DWORD dwStyle=GetWindowLong(m_hWnd,GWL_STYLE);
		SetWindowLong(m_hWnd,GWL_STYLE,dwStyle);
		ShowWindow(hHostWnd,SW_HIDE);
		ShowWindow(m_hWnd,dwShowCommand);
		EnableWindow(m_hWnd,TRUE);
		UpdateWindow(m_hWnd);
		m_bInitialized=true;
	}
}

void COpenGLViewPortBase::Destroy()
{
	if(m_hWnd){DestroyWindow(m_hWnd);}
	if(m_bInitialized)
	{
		m_bInitialized=false;
	}
}

LRESULT COpenGLViewPortBase::ProcessMessage(HWND hWnd,UINT  uMsg, WPARAM  wParam,LPARAM  lParam)
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
		OnSetCursor();
		return 0L;
		break;
	case WM_DESTROY:
		OnDestroy();
		m_hWnd=NULL;
		break;
	case WM_PAINT:
		OnPaint();
		return 0;
		break;
	case WM_SETFOCUS:
		if((m_dwFlags&(GLV_ENABLE_ROTATE|GLV_ENABLE_SCALE|GLV_ENABLE_MOVE))==0){return 0;}
		if((m_dwFlags&(GLV_DISABLE_SMOOTH_MOVEMENT_TIMER))!=0){return 0;}
		SetTimer(hWnd,SMOOTH_MOVEMENT_TIMER,10,NULL);
		InvalidateRect(hWnd,NULL,TRUE);
		return 0;
	case WM_KILLFOCUS:
		if((m_dwFlags&(GLV_ENABLE_ROTATE|GLV_ENABLE_SCALE|GLV_ENABLE_MOVE))==0){return 0;}
		if((m_dwFlags&(GLV_DISABLE_SMOOTH_MOVEMENT_TIMER))!=0){return 0;}
		InvalidateRect(hWnd,NULL,TRUE);
		return 0;
	case WM_GETDLGCODE:
		return DLGC_WANTARROWS|DLGC_WANTCHARS;
	case WM_TIMER:
		if(wParam==SMOOTH_MOVEMENT_TIMER)
		{
			ProcessKeyboardMovement();
			InvalidateRect(hWnd,NULL,TRUE);
			return 0;
		}
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
	case WM_MBUTTONDOWN:
		OnMButtonDown(wParam,point);
		break;
	case WM_LBUTTONUP:
		OnLButtonUp(wParam,point);
		break;
	case WM_RBUTTONUP:
		OnRButtonUp(wParam,point);
		break;
	case WM_MBUTTONUP:
		OnMButtonUp(wParam,point);
		break;
	case WM_MOUSEWHEEL:
		OnMouseWheel(LOWORD(wParam),(short)HIWORD(wParam),point);
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

LRESULT COpenGLViewPortBase::ViewPortWindowProc(HWND  hWnd,UINT  uMsg, WPARAM  wParam,LPARAM  lParam)
{
	CREATESTRUCT *pCreateInfo=(CREATESTRUCT *)lParam;
	COpenGLViewPortBase *pData=NULL;
	if(uMsg==WM_CREATE)
	{
		pData=(COpenGLViewPortBase *)pCreateInfo->lpCreateParams;
	}
	else
	{
		pData=(COpenGLViewPortBase *)GetWindowLong(hWnd,GWL_USERDATA);
	}
	if(!pData){return DefWindowProc(hWnd,uMsg,wParam,lParam);}
	return pData->ProcessMessage(hWnd,uMsg,wParam,lParam);
}

void COpenGLViewPortBase::OnSetCursor()
{
	SetCursor(LoadCursor(NULL,IDC_ARROW));
}

void COpenGLViewPortBase::OnCreate(HWND hWnd)
{
	SetWindowLong(hWnd,GWL_USERDATA,(DWORD)this);
	m_hDC=GetDC(hWnd);
}

void COpenGLViewPortBase::OnDestroy()
{
	DestroyRenderContext();
	if(m_hDC){ReleaseDC(m_hWnd,m_hDC);m_hDC=NULL;}
	SetWindowLong(m_hWnd,GWL_USERDATA,(DWORD)NULL);
}

void COpenGLViewPortBase::OnPaint()
{
	HGLRC hRenderContext=ActivateRenderContext();
	if(hRenderContext)
	{
		RECT clientRect={0};
		GetClientRect(m_hWnd,&clientRect);
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

		Render();

		OnPreFlush();
		glFlush();
		glFinish();
		SwapBuffers(m_hDC);
		OnAfterSwapBuffers();
		DeactivateRenderContext();
	}
	ValidateRect(m_hWnd,NULL);
}

void COpenGLViewPortBase::OnPreFlush()
{
}
void COpenGLViewPortBase::OnAfterSwapBuffers()
{
}

void COpenGLViewPortBase::OnLButtonDown(WORD wKeyState,POINT pos)
{
	SetCapture(m_hWnd);
	m_MovementStartPoint=pos;
}

void COpenGLViewPortBase::OnLButtonUp(WORD wKeyState,POINT pos)
{
	if(GetCapture()==m_hWnd){ReleaseCapture();}
}

void COpenGLViewPortBase::OnRButtonDown(WORD wKeyState,POINT pos)
{
	SetCapture(m_hWnd);
	m_MovementStartPoint=pos;
}

void COpenGLViewPortBase::OnRButtonUp(WORD wKeyState,POINT pos)
{
	if(GetCapture()==m_hWnd){ReleaseCapture();}
}

void COpenGLViewPortBase::OnMButtonDown(WORD wKeyState,POINT pos)
{
	SetCapture(m_hWnd);
	m_MovementStartPoint=pos;
}

void COpenGLViewPortBase::OnMButtonUp(WORD wKeyState,POINT pos)
{
	if(GetCapture()==m_hWnd){ReleaseCapture();}
}

void COpenGLViewPortBase::OnMouseWheel(WORD wKeyState,short wWheelDelta,POINT pos)
{
}

void COpenGLViewPortBase::OnMouseMove(WORD wKeyState,POINT pos)
{
}

