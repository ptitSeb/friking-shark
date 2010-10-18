// GameScreen.h: interface for the CGameScreen class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMESCREEN_H__89CAE6A1_8EE3_4ABC_89B0_351457FD0FC1__INCLUDED_)
#define AFX_GAMESCREEN_H__89CAE6A1_8EE3_4ABC_89B0_351457FD0FC1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGameCamera:public ICamera
{
	CVector m_vPosition;
	CVector m_vAngles;
	double  m_dViewAngle;
	double  m_dFarClippingPlane;
	double  m_dNearClippingPlane;
	double  m_dAspectRatio;

	CVector m_vUp;
	CVector m_vRight;
	CVector m_vForward;

public:

	CVector &GetPosition();
	CVector &GetAngles();

	void	SetPosition(CVector &vPosition);
	void	SetAngles(CVector &vAngles);

	void	SetClippingPlanes(double dNear,double dFar);
	void	GetClippingPlanes(double &dNear,double &dFar);

	void	SetViewAngle(double dAngle);
	double	GetViewAngle();

	CVector	&GetUpVector();
	CVector	&GetRightVector();
	CVector	&GetForwardVector();

	void	 SetAspectRatio(double dRatio);
	double GetAspectRatio();

	CGameCamera();
};

class CGameScreen : virtual public CSystemObjectBase, public COpenGLViewPortBase,virtual public IGameScreen
{
	ICamera *m_piCamera;

	void	Render();

	void ReflectCurrentCameraSettings();

	LRESULT ProcessMessage(HWND hwnd,UINT  uMsg, WPARAM  wParam,LPARAM  lParam);

public:

	void	Init();
	void	Destroy();
	void	OnPreFlush();
	void	OnAfterSwapBuffers();

	void	Redraw();
	void	FlushMessages();

	HDC		GetCurrentDC(){return m_hDC;}
	HGLRC	GetCurrentRC(){return m_hRenderContext;}
	RECT	GetCurrentRect(){RECT r={0};GetWindowRect(m_hWnd,&r);return r;}
	SIZE	GetCurrentSize(){RECT r={0};GetWindowRect(m_hWnd,&r);SIZE s={r.right-r.left,r.bottom-r.top};return s;}
	HWND	GetWindow(){return m_hWnd;}
	double  GetAspectRatio(){RECT r={0};GetWindowRect(m_hWnd,&r);return double(r.bottom-r.top)/double(r.right-r.left);}

	DWORD	GetFlags(){return m_dwFlags;}
	void	SetFlags(DWORD dwFlags){m_dwFlags=dwFlags;}

	void	GetFlushTimes(LARGE_INTEGER *pBeforeFlushTime,LARGE_INTEGER *pAfterFlushTime){*pBeforeFlushTime=m_ldBeforeFlushTime;*pAfterFlushTime=m_ldAfterFlushTime;}

	ICamera *GetCamera();
	void	SetCamera(ICamera *piCamera);

	CGameScreen();
	virtual ~CGameScreen();

};

#endif // !defined(AFX_GAMESCREEN_H__89CAE6A1_8EE3_4ABC_89B0_351457FD0FC1__INCLUDED_)
