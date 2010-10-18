// GameScreen.cpp: implementation of the CGameScreen class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GameEngine.h"
#include "GameScreen.h"
#include "GUIManager.h"
#include "..\VectorLib\3DSFiles.h"

//////////////////////////////////////////////////////////////////////
// CGameScreen
//////////////////////////////////////////////////////////////////////

CGameCamera::CGameCamera()
{
    DEVMODE mode={0};
    mode.dmSize=sizeof(DEVMODE);
    mode.dmBitsPerPel=CGlobals::g_IniFile.GetInt("Resolucion\\Bits",16);
    mode.dmPelsWidth=CGlobals::g_IniFile.GetInt("Resolucion\\Ancho",800);
    mode.dmPelsHeight=CGlobals::g_IniFile.GetInt("Resolucion\\Alto",600);
    mode.dmDisplayFrequency=CGlobals::g_IniFile.GetInt("Resolucion\\Frecuencia",60);
    mode.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFREQUENCY;
    if(CGlobals::g_IniFile.GetInt("Resolucion\\Pantalla Completa",0)){ChangeDisplaySettings(&mode,CDS_FULLSCREEN);}

	m_dAspectRatio=1;
	m_dViewAngle=50;
	m_dNearClippingPlane=1;
	m_dFarClippingPlane=1000000;
	SetAngles(CVector(0,0,-1));
}

CVector &CGameCamera::GetPosition(){return m_vPosition;}
CVector &CGameCamera::GetAngles(){return m_vAngles;}

void	CGameCamera::SetPosition(CVector &vPosition){m_vPosition=vPosition;}
void	CGameCamera::SetAngles(CVector &vAngles){m_vAngles=vAngles;VectorsFromAngles(m_vAngles,&m_vForward,&m_vRight,&m_vUp);}

void	CGameCamera::SetClippingPlanes(double dNear,double dFar){m_dNearClippingPlane=dNear;m_dFarClippingPlane=dFar;}
void	CGameCamera::GetClippingPlanes(double &dNear,double &dFar){dNear=m_dNearClippingPlane;dFar=m_dFarClippingPlane;}

void	CGameCamera::SetViewAngle(double dAngle){m_dViewAngle=dAngle;}
double	CGameCamera::GetViewAngle(){return m_dViewAngle;}

CVector	&CGameCamera::GetUpVector(){return m_vUp;}
CVector	&CGameCamera::GetRightVector(){return m_vRight;}
CVector	&CGameCamera::GetForwardVector(){return  m_vForward;}
void	CGameCamera::SetAspectRatio(double dRatio){m_dAspectRatio=dRatio;}
double CGameCamera::GetAspectRatio(){return m_dAspectRatio;}

//////////////////////////////////////////////////////////////////////
// CGameScreen
//////////////////////////////////////////////////////////////////////

CGameScreen::CGameScreen()
{
    
    m_sName="Screen";



    m_piCamera=NULL;
}
CGameScreen::~CGameScreen(){}

void CGameScreen::Init()
{
    SIZE sz={GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN)};

    SetCamera(CGlobals::g_pPlayCamera);
    DWORD dwScreenFlags=GetFlags();
    dwScreenFlags|=GLV_ENABLE_PERSPECTIVE|GLV_MODE_SUBJETIVE|GLV_RENDER_SOLID;
    dwScreenFlags&=~(GLV_DRAW_AXISES|GLV_DRAW_FOCUS_RECT|GLV_RENDER_WIREFRAME|GLV_ENABLE_ROTATE|GLV_ENABLE_SCALE|GLV_ENABLE_MOVE);
    if(!CGlobals::g_IniFile.GetInt("Render\\Borrado",0)){dwScreenFlags|=GLV_RENDER_DISABLE_CLEAR;}
    if(CGlobals::g_IniFile.GetInt("Render\\Ver Ejes",0)){dwScreenFlags|=GLV_DRAW_AXISES;}
    SetFlags(dwScreenFlags);
    bool bWindowed=CGlobals::g_IniFile.GetInt("Render\\Ventana",0) || IsDebuggerPresent();
    COpenGLViewPortBase::Init(NULL,bWindowed?SW_SHOW:SW_MAXIMIZE,bWindowed?WS_VISIBLE|WS_OVERLAPPED:WS_POPUP,0);
    if(bWindowed){SetWindowPos(GetWindow(),NULL,sz.cx/8,sz.cy/8,sz.cx*6/8,sz.cy*6/8,SWP_NOZORDER);}
	ActivateRenderContext();
}

LRESULT CGameScreen::ProcessMessage(HWND hWnd,UINT  uMsg, WPARAM  wParam,LPARAM  lParam)
{
    LRESULT lResult=0;
    if(uMsg==WM_KEYDOWN && wParam==VK_ESCAPE){DestroyWindow(hWnd);return 0;}
    if(uMsg==WM_DESTROY){PostQuitMessage(0);}
    lResult=COpenGLViewPortBase::ProcessMessage(hWnd,uMsg, wParam,lParam);
    return lResult;
}

void CGameScreen::ReflectCurrentCameraSettings()
{
    m_piCamera->GetClippingPlanes(COpenGLViewPortBase::m_dCameraNearPlane,COpenGLViewPortBase::m_dCameraFarPlane);
    CVector vAngles=m_piCamera->GetAngles();
    COpenGLViewPortBase::m_vCameraPos=m_piCamera->GetPosition();
    COpenGLViewPortBase::m_dCameraYaw=vAngles.c[YAW];
    COpenGLViewPortBase::m_dCameraRoll=vAngles.c[ROLL];
    COpenGLViewPortBase::m_dCameraPitch=vAngles.c[PITCH];
    COpenGLViewPortBase::m_vCameraUp=m_piCamera->GetUpVector();
    COpenGLViewPortBase::m_vCameraRight=m_piCamera->GetRightVector();
    COpenGLViewPortBase::m_vCameraForward=m_piCamera->GetForwardVector();
    COpenGLViewPortBase::m_dViewAngle=m_piCamera->GetViewAngle();
}

void     CGameScreen::Destroy(){COpenGLViewPortBase::Destroy();}
void     CGameScreen::FlushMessages(){MSG msg;while(PeekMessage(&msg,NULL,WM_PAINT,WM_PAINT,FALSE)){GetMessage(&msg,NULL,WM_PAINT,WM_PAINT);TranslateMessage(&msg);DispatchMessage(&msg);}}
void     CGameScreen::Redraw(){InvalidateRect(m_hWnd,NULL,FALSE);}
void     CGameScreen::Render(){ReflectCurrentCameraSettings();CGlobals::g_pGUIManager->Render();}
void     CGameScreen::OnPreFlush(){theApp.OnPreFlush();}
void     CGameScreen::OnAfterSwapBuffers(){theApp.OnAfterSwapBuffers();}
void     CGameScreen::SetCamera(ICamera *piCamera){m_piCamera=piCamera;ReflectCurrentCameraSettings();}
ICamera *CGameScreen::GetCamera(){return m_piCamera;}
