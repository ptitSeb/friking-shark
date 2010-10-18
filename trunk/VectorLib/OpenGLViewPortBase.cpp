// OpenGLViewPortBase.cpp: implementation of the COpenGLViewPortBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <crtdbg.h>
#include "OpenGLViewPortBase.h"
#include "BMPFiles.h"

#define VIEWPORT_CLASSNAME		"MY423"
#define SMOOTH_MOVEMENT_TIMER	1
#define FOCUS_FRAME_SIZE		3

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COpenGLViewPortBase::COpenGLViewPortBase(DWORD dwFlags)
{
	memset(&m_ldBeforeFlushTime,0,sizeof(m_ldBeforeFlushTime));
	memset(&m_ldAfterFlushTime,0,sizeof(m_ldAfterFlushTime));
	m_hDC=NULL;
	m_hRenderContext=NULL;
	m_hWnd=NULL;
	m_hHostWnd=NULL;
	m_pOldHostWindowProc=NULL;
	m_dwOldHostWindowData=NULL;
	m_bInitialized=false;
	m_dFarClippingPlane=4000.0;
	m_dViewAngle=60;

	m_nPixelFormatIndex=0;
	m_dwFlags=dwFlags;

	ResetCamera();
}

void COpenGLViewPortBase::ResetCamera()
{
	m_dCameraViewWidth=300;
	m_dCameraViewHeight=300;
	m_dCameraNearPlane=0;
	m_dCameraFarPlane=4000;
	m_dCameraProyectionPlane=1;
	m_dCameraFocus=800;
  	m_dCameraPitch=0;
	m_dCameraYaw=0;
	m_dCameraRoll=0;
	m_vCameraPos=Origin;
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
		m_hHostWnd=hHostWnd;
		m_pOldHostWindowProc	=(WNDPROC)SetWindowLong(hHostWnd,GWL_WNDPROC,(DWORD)HostWindowProc);
		m_dwOldHostWindowData	=SetWindowLong(hHostWnd,GWL_USERDATA,(DWORD)this);

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
		SetWindowLong(m_hHostWnd,GWL_WNDPROC,(DWORD)m_pOldHostWindowProc);
		SetWindowLong(m_hHostWnd,GWL_USERDATA,(DWORD)m_dwOldHostWindowData);
		m_pOldHostWindowProc=NULL;
		m_dwOldHostWindowData=0;
		m_bInitialized=false;
	}

}

LRESULT COpenGLViewPortBase::HostWindowProc(HWND hWnd,UINT  uMsg, WPARAM  wParam,LPARAM  lParam)
{
	COpenGLViewPortBase *pData=(COpenGLViewPortBase *)GetWindowLong(hWnd,GWL_USERDATA);
	WNDPROC wndProcToCall=pData->m_pOldHostWindowProc;

	switch(uMsg)
	{
	case WM_DESTROY:
		pData->Destroy();
		break;
	case WM_ERASEBKGND:
		return 1L;
	case WM_PAINT:
		ValidateRect(pData->m_hHostWnd,NULL);
		InvalidateRect(pData->m_hWnd,NULL,TRUE);
		return 0L;
	case WM_SIZE:
		SetWindowPos(hWnd,NULL,0,0,LOWORD(lParam),HIWORD(lParam),SWP_NOMOVE|SWP_NOZORDER);
		break;
	}
	return CallWindowProc(wndProcToCall,hWnd,uMsg,wParam,lParam);
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
		ProcessKey(wParam);
		break;
	case WM_LBUTTONDOWN:
		if((m_dwFlags&(GLV_ENABLE_ROTATE|GLV_ENABLE_SCALE|GLV_ENABLE_MOVE))==0){break;}
		ProcessKeyboardMovement();
		OnLButtonDown(wParam,point);
		break;
	case WM_RBUTTONDOWN:
		if((m_dwFlags&(GLV_ENABLE_ROTATE|GLV_ENABLE_SCALE|GLV_ENABLE_MOVE))==0){break;}
		ProcessKeyboardMovement();
		OnRButtonDown(wParam,point);
		break;
	case WM_MBUTTONDOWN:
		if((m_dwFlags&(GLV_ENABLE_ROTATE|GLV_ENABLE_SCALE|GLV_ENABLE_MOVE))==0){break;}
		ProcessKeyboardMovement();
		OnMButtonDown(wParam,point);
		break;
	case WM_LBUTTONUP:
		if((m_dwFlags&(GLV_ENABLE_ROTATE|GLV_ENABLE_SCALE|GLV_ENABLE_MOVE))==0){break;}
		ProcessKeyboardMovement();
		OnLButtonUp(wParam,point);
		break;
	case WM_RBUTTONUP:
		if((m_dwFlags&(GLV_ENABLE_ROTATE|GLV_ENABLE_SCALE|GLV_ENABLE_MOVE))==0){break;}
		ProcessKeyboardMovement();
		OnRButtonUp(wParam,point);
		break;
	case WM_MBUTTONUP:
		if((m_dwFlags&(GLV_ENABLE_ROTATE|GLV_ENABLE_SCALE|GLV_ENABLE_MOVE))==0){break;}
		ProcessKeyboardMovement();
		OnMButtonUp(wParam,point);
		break;
	case WM_MOUSEWHEEL:
		ProcessKeyboardMovement();
		OnMouseWheel(LOWORD(wParam),(short)HIWORD(wParam),point);
		break;

	case WM_MOUSEMOVE:
		if((m_dwFlags&(GLV_ENABLE_ROTATE|GLV_ENABLE_SCALE|GLV_ENABLE_MOVE))==0){break;}
		ProcessKeyboardMovement();
		OnMouseMove(wParam,point);
		break;
	case WM_SIZE:
		OnSize(LOWORD(lParam),HIWORD(lParam));
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

void COpenGLViewPortBase::ProcessKeyboardMovement()
{
	if(m_dwFlags&GLV_MODE_SUBJETIVE && m_dwFlags&GLV_ENABLE_MOVE)
	{
		bool bInvalidate=false;
		if(GetKeyState(VK_UP)&0x8000 || GetKeyState(VK_NUMPAD8)&0x8000 || GetKeyState('W')&0x8000){ProcessKey('W');bInvalidate=true;}
		if(GetKeyState(VK_DOWN)&0x8000 || GetKeyState(VK_NUMPAD2)&0x8000 || GetKeyState('S')&0x8000){ProcessKey('S');bInvalidate=true;}
		if(GetKeyState(VK_LEFT)&0x8000 || GetKeyState(VK_NUMPAD4)&0x8000 || GetKeyState('A')&0x8000){ProcessKey('A');bInvalidate=true;}
		if(GetKeyState(VK_RIGHT)&0x8000 || GetKeyState(VK_NUMPAD6)&0x8000 || GetKeyState('D')&0x8000){ProcessKey('D');bInvalidate=true;}
		if(GetKeyState(VK_NUMPAD9)&0x8000 || GetKeyState('R')&0x8000){ProcessKey('R');bInvalidate=true;}
		if(GetKeyState(VK_NUMPAD3)&0x8000 || GetKeyState('F')&0x8000){ProcessKey('F');bInvalidate=true;}
		if(bInvalidate){InvalidateRect(m_hWnd,NULL,TRUE);}
	}
}

void COpenGLViewPortBase::MoveCamera(CVector vCurrentPos,CVector vNewPos)
{
	m_vCameraPos=vNewPos;
}

void COpenGLViewPortBase::ProcessKey(int nKey)
{
	if(m_dwFlags&GLV_MODE_SUBJETIVE && m_dwFlags&GLV_ENABLE_MOVE)
	{
		double dSpeed=(GetKeyState(VK_LSHIFT)&0x8000)?15.0:7.0;
		if(nKey=='W')
		{
			MoveCamera(m_vCameraPos,m_vCameraPos+m_vCameraForward*dSpeed);
			return;
		}
		if(nKey=='S')
		{
			MoveCamera(m_vCameraPos,m_vCameraPos-m_vCameraForward*dSpeed);
			InvalidateRect(m_hWnd,NULL,TRUE);
			return;
		}
		if(nKey=='A')
		{
			MoveCamera(m_vCameraPos,m_vCameraPos-m_vCameraRight*dSpeed);
			InvalidateRect(m_hWnd,NULL,TRUE);
			return;
		}
		if(nKey=='D')
		{
			MoveCamera(m_vCameraPos,m_vCameraPos+m_vCameraRight*dSpeed);
			InvalidateRect(m_hWnd,NULL,TRUE);
			return;
		}
		if(nKey=='R')
		{
			MoveCamera(m_vCameraPos,m_vCameraPos+m_vCameraUp*dSpeed);
			InvalidateRect(m_hWnd,NULL,TRUE);
			return;
		}
		if(nKey=='F')
		{
			MoveCamera(m_vCameraPos,m_vCameraPos-m_vCameraUp*dSpeed);
			InvalidateRect(m_hWnd,NULL,TRUE);
			return;
		}
	}
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

unsigned long __stdcall WorkerThread(void * pParam);


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

		if(m_dwFlags&GLV_RENDER_SOLID)
		{
			glPolygonMode(GL_FRONT,GL_FILL);

			float color[3]={0,0,0};
			glEnable(GL_FOG);
			glFogf(GL_FOG_MODE,GL_LINEAR);
			glFogf(GL_FOG_START,200);
			glFogf(GL_FOG_END,2000);
			glFogfv (GL_FOG_COLOR,color);
		}
		if(m_dwFlags&GLV_RENDER_WIREFRAME)
		{
			glPolygonMode(GL_FRONT,GL_LINE);
		}

		ApplyMatrix(GL_PROJECTION);
		ApplyMatrix(GL_MODELVIEW);

		if(m_dwFlags&GLV_DRAW_FOCUS_RECT && GetFocus()==m_hWnd)
		{
			// Se pinta el marco y se 
			glClearColor(0,0,1,1);
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
			glEnable(GL_SCISSOR_TEST);
			glScissor(FOCUS_FRAME_SIZE,FOCUS_FRAME_SIZE,clientRect.right-FOCUS_FRAME_SIZE*2,clientRect.bottom-FOCUS_FRAME_SIZE*2);
		}
		if((m_dwFlags&GLV_RENDER_DISABLE_CLEAR)==0)
		{
			glClearColor(0,0,0,1);
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		}
		else
		{
			glClear(GL_DEPTH_BUFFER_BIT);
		}

		if(m_dwFlags&GLV_DRAW_AXISES)
		{
			RenderAxis(AxisPosX,30,RGB(0  ,0  ,255));
			RenderAxis(AxisPosY,30,RGB(255,255,0));
			RenderAxis(AxisPosZ,30,RGB(255,0  ,0));
		}

		Render();

		OnPreFlush();
		QueryPerformanceCounter(&m_ldBeforeFlushTime);
		glFlush();
		glFinish();
		SwapBuffers(m_hDC);
		QueryPerformanceCounter(&m_ldAfterFlushTime);
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
	if(GetCapture()==m_hWnd)
	{
		SIZE sMove={pos.x-m_MovementStartPoint.x,-1*(pos.y-m_MovementStartPoint.y)};

		if(m_dwFlags&GLV_MODE_SUBJETIVE)
		{
			RECT R;
			GetClientRect(m_hWnd,&R);
			double dAspectRatio=((double)R.bottom-R.top)/((double)R.right-R.left);
			double w=m_dCameraViewWidth/(double)R.right;
			double h=m_dCameraViewHeight/(double)R.bottom;

			if(wKeyState&MK_LBUTTON && wKeyState&MK_RBUTTON)
			{
				double dAmmount2=-sMove.cy/2.0;
				m_dCameraRoll+=dAmmount2;
			}
			else
			{
				double dAmmount1=sMove.cx/2.0;
				double dAmmount2=-sMove.cy/2.0;
				m_dCameraYaw+=dAmmount1;
				m_dCameraPitch+=dAmmount2;
			}

			m_MovementStartPoint=pos;

			// GIRO DE LOS EJES DE LA CAMARA. 
			CMatrix m,temp;

			// Se rota solo en pitch y yaw
			// para rotar correctamente hay que rotar en yaw y despues rotar en pitch CON EL EJE
			// DE PITCH GIRADO CON EL NUEVO YAW.
			// idem para el eje de ROLL, hay que rotarlo en yaw y pitch antes de usarlo

			m_vCameraUp=AxisPosY;
			m_vCameraRight=AxisPosX;
			m_vCameraForward=AxisNegZ;

			m.R(m_vCameraUp,m_dCameraYaw/360.0*2.0*PI,Origin);
			m_vCameraRight*=m;
			m_vCameraForward*=m;
			m.R(m_vCameraRight,m_dCameraPitch/360.0*2.0*PI,Origin);
			m_vCameraForward*=m;
			m_vCameraUp*=m;
			m.R(m_vCameraForward,m_dCameraRoll/360.0*2.0*PI,Origin);
			m_vCameraRight*=m;
			m_vCameraUp*=m;

			InvalidateRect(m_hWnd,NULL,TRUE);
		}
		else if(m_dwFlags&GLV_MODE_DESIGN)
		{
			RECT R;
			GetClientRect(m_hWnd,&R);
			double dAspectRatio=((double)R.bottom-R.top)/((double)R.right-R.left);
			double w=m_dCameraViewWidth/(double)R.right;
			double h=m_dCameraViewHeight/(double)R.bottom;

			if((wKeyState&(MK_LBUTTON|MK_RBUTTON))==(MK_LBUTTON|MK_RBUTTON))
			{
				m_dCameraViewWidth+=(double)(pos.y-m_MovementStartPoint.y)*h;
				m_dCameraViewHeight+=(double)(pos.y-m_MovementStartPoint.y)*h;
				m_MovementStartPoint=pos;
				InvalidateRect(m_hWnd,NULL,TRUE);
			}
			else if((wKeyState&(MK_LBUTTON))==(MK_LBUTTON))
			{
				double dAmmount1=sMove.cx/2.0;
				double dAmmount2=-sMove.cy/2.0;
				m_dCameraYaw+=dAmmount1;
				m_dCameraPitch+=dAmmount2;
				m_MovementStartPoint=pos;

				CVector vForward,vRight,vUp;
				VectorsFromAngles(m_dCameraYaw,m_dCameraPitch,m_dCameraRoll,m_vCameraForward,m_vCameraRight,m_vCameraUp);
				InvalidateRect(m_hWnd,NULL,TRUE);
			}
			else if((wKeyState&(MK_RBUTTON))==(MK_RBUTTON))
			{
			//	poner tambien los vectores de la camara para hacer el desplazamiento paralelo a la vista

				// Vectores a partir de los angulos

				m_vCameraPos+=m_vCameraRight*(0.0-(double)sMove.cx)*w;
				m_vCameraPos+=m_vCameraUp*((double)sMove.cy)*h*dAspectRatio;
				//m_vCameraPos.c[2]+=cos(m_dCameraYaw)*dScreenMovX+sin(m_dCameraPitch)*dScreenMovX;
				m_MovementStartPoint=pos;
				InvalidateRect(m_hWnd,NULL,TRUE);
			}
		}
	}
}

void COpenGLViewPortBase::RenderAxis(CVector vAxis,double dLength,COLORREF cColor)
{
	CVector vEndPoint=vAxis*dLength;
	double dLineSize=0.0;
	double dPointSize=0.0;
	
	glGetDoublev(GL_POINT_SIZE,&dPointSize);
	glPointSize(6.0);
	
	glColor3ub(GetRValue(cColor),GetGValue(cColor),GetBValue(cColor));
	
	glLineWidth(1);
	
	// dibujado de la linea del eje
	
	glBegin(GL_LINES);
	glVertex3d(0,0,0);
	glVertex3d(vEndPoint.c[0],vEndPoint.c[1],vEndPoint.c[2]);
	glEnd();
	
	
	glBegin(GL_POINTS);
	glVertex3d(vEndPoint.c[0],vEndPoint.c[1],vEndPoint.c[2]);
	glEnd();
	
	glPointSize(dPointSize);
}

void COpenGLViewPortBase::ApplyMatrix(DWORD dwType)
{
	glMatrixMode(dwType);

	if(dwType==GL_MODELVIEW)
	{
		glLoadIdentity();
		glRotated((0-m_dCameraRoll)	,AxisPosZ.c[0],AxisPosZ.c[1],AxisPosZ.c[2]);
		glRotated((0-m_dCameraPitch),AxisPosX.c[0],AxisPosX.c[1],AxisPosX.c[2]);
		glRotated((0-(m_dCameraYaw-90))	,AxisPosY.c[0],AxisPosY.c[1],AxisPosY.c[2]);

		//if(m_dwFlags&GLV_ENABLE_MOVE)
		{
			glTranslated(-m_vCameraPos.c[0],-m_vCameraPos.c[1],-m_vCameraPos.c[2]);
		}
	}
	if(dwType==GL_PROJECTION)
	{
		glLoadIdentity();

		RECT windowRect={0},clientRect={0};
		GetClientRect(m_hWnd,&clientRect);
		GetWindowRect(m_hWnd,&windowRect);
		POINT p={windowRect.left,windowRect.top};
		ScreenToClient(GetParent(m_hWnd),&p);
		//clientRect.right*=0.80;
		double dAspectRatio=((double)clientRect.right)/((double)clientRect.bottom);
		SIZE windowSize={windowRect.right-windowRect.left,windowRect.bottom-windowRect.top};
		SIZE clientSize={clientRect.right-clientRect.left,clientRect.bottom-clientRect.top};
		if(m_dwFlags&GLV_DRAW_FOCUS_RECT && GetFocus()==m_hWnd)
		{
			glViewport(FOCUS_FRAME_SIZE,FOCUS_FRAME_SIZE,clientRect.right-FOCUS_FRAME_SIZE*2,clientRect.bottom-FOCUS_FRAME_SIZE*2);
		}
		else
		{
			glViewport(0,0,clientRect.right,clientRect.bottom);
		}
		if(m_dwFlags&GLV_ENABLE_PERSPECTIVE)
		{
			gluPerspective(m_dViewAngle,dAspectRatio,1,m_dFarClippingPlane);
		}
		else
		{
			glOrtho(-m_dCameraViewWidth/2.0,m_dCameraViewWidth/2.0,(-m_dCameraViewHeight/2.0)/dAspectRatio,(m_dCameraViewHeight/2.0)/dAspectRatio,m_dCameraNearPlane,m_dCameraFarPlane);
		}
	}
}

void COpenGLViewPortBase::GetGLMatrix(DWORD dwType,double *dMatrix)
{
	HGLRC hRenderContext=wglGetCurrentContext();
	_ASSERTE(hRenderContext!=NULL);

	glMatrixMode(dwType);
	glPushMatrix();
	ApplyMatrix(dwType);

	DWORD dwMatrix=0;
	if(dwType==GL_MODELVIEW){dwMatrix=GL_MODELVIEW_MATRIX;}
	if(dwType==GL_PROJECTION){dwMatrix=GL_PROJECTION_MATRIX;}
	glGetDoublev(dwMatrix,(double*)dMatrix);
	glPopMatrix();
}

void COpenGLViewPortBase::GetGLMatrix(DWORD dwType,CMatrix *pMatrix)
{
	double dMatrix[4][4]={{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};
	GetGLMatrix(dwType,(double*)dMatrix);
	pMatrix->e[0][0]=dMatrix[0][0];
	pMatrix->e[1][0]=dMatrix[0][1];
	pMatrix->e[2][0]=dMatrix[0][2];
	pMatrix->e[3][0]=dMatrix[0][3];
	pMatrix->e[0][1]=dMatrix[1][0];
	pMatrix->e[1][1]=dMatrix[1][1];
	pMatrix->e[2][1]=dMatrix[1][2];
	pMatrix->e[3][1]=dMatrix[1][3];
	pMatrix->e[0][2]=dMatrix[2][0];
	pMatrix->e[1][2]=dMatrix[2][1];
	pMatrix->e[2][2]=dMatrix[2][2];
	pMatrix->e[3][2]=dMatrix[2][3];
	pMatrix->e[0][3]=dMatrix[3][0];
	pMatrix->e[1][3]=dMatrix[3][1];
	pMatrix->e[2][3]=dMatrix[3][2];
	pMatrix->e[3][3]=dMatrix[3][3];
}

CVector COpenGLViewPortBase::ProjectPoint(CVector vVector)
{
	CVector vTemp(vVector);

	HGLRC hRenderContext=ActivateRenderContext();
	if(hRenderContext)
	{
		CMatrix mModel,mProjection;

		GetGLMatrix(GL_MODELVIEW,&mModel);
		GetGLMatrix(GL_PROJECTION,&mProjection);

		vTemp*=mModel;
		vTemp*=mProjection;

		RECT clientRect={0};
		GetClientRect(m_hWnd,&clientRect);
		double dWidth=((double)(clientRect.right-clientRect.left));
		double dHeigth=((double)(clientRect.bottom-clientRect.top));

		if(m_dwFlags&GLV_ENABLE_PERSPECTIVE)
		{
			vTemp.c[0]=vTemp.c[0]/vTemp.c[2];
			vTemp.c[1]=vTemp.c[1]/vTemp.c[2];
		}

		vTemp.c[0]=dWidth/2+vTemp.c[0]*dWidth/2;
		vTemp.c[1]=dHeigth-(dHeigth/2+vTemp.c[1]*dHeigth/2);

		DeactivateRenderContext();
	}
	return vTemp;
}

void COpenGLViewPortBase::OnSize(WORD cx,WORD cy)
{
}
