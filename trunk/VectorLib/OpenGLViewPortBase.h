// OpenGLViewPortBase.h: interface for the COpenGLViewPortBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENGLVIEWPORTBASE_H__B5341926_A706_4F5C_93E7_42FD74EFD74F__INCLUDED_)
#define AFX_OPENGLVIEWPORTBASE_H__B5341926_A706_4F5C_93E7_42FD74EFD74F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VectorLib.h"
#include <gl\gl.h>
#include <gl\glu.h>

// Interaction Flags
#define GLV_ENABLE_ROTATE					0x0001
#define GLV_ENABLE_SCALE					0x0002
#define GLV_ENABLE_MOVE						0x0004
											
#define GLV_MODE_SUBJETIVE					0x0010
#define GLV_MODE_DESIGN						0x0020
											
// Rendering flags							
#define GLV_ENABLE_PERSPECTIVE				0x0100
#define GLV_DRAW_AXISES						0x0200
#define GLV_DRAW_FOCUS_RECT					0x0400
#define GLV_RENDER_SOLID					0x1000
#define GLV_RENDER_WIREFRAME				0x2000
#define GLV_RENDER_DISABLE_CLEAR			0x4000
#define GLV_DISABLE_SMOOTH_MOVEMENT_TIMER	0x8000

class COpenGLViewPortBase  
{
public:
	HDC			m_hDC;
	HGLRC		m_hRenderContext;
	HWND		m_hWnd;
	HWND		m_hHostWnd;
	WNDPROC		m_pOldHostWindowProc;
	DWORD		m_dwOldHostWindowData;
	bool		m_bInitialized;

	double		m_dCameraViewWidth;
	double		m_dCameraViewHeight;
	double		m_dCameraNearPlane;
	double		m_dCameraFarPlane;
	double		m_dCameraProyectionPlane;
	double		m_dCameraFocus;
	CVector		m_vCameraPos;

	double		m_dCameraPitch;
	double		m_dCameraYaw;
	double		m_dCameraRoll;

	CVector		m_vCameraForward;
	CVector		m_vCameraRight;
	CVector		m_vCameraUp;

	//Stats

	LARGE_INTEGER m_ldBeforeFlushTime;
	LARGE_INTEGER m_ldAfterFlushTime;

	// Movement Control 
	POINT		m_MovementStartPoint;


	int			m_nPixelFormatIndex;
	DWORD		m_dwFlags;
	double		m_dFarClippingPlane;
	double		m_dViewAngle;

	void OnCreate(HWND hWnd);
	void OnDestroy();
	void OnPaint();

	static LRESULT CALLBACK HostWindowProc(HWND  hwnd,UINT  uMsg, WPARAM  wParam,LPARAM  lParam);
	static LRESULT CALLBACK ViewPortWindowProc(HWND  hwnd,UINT  uMsg, WPARAM  wParam,LPARAM  lParam);

	virtual LRESULT ProcessMessage(HWND hwnd,UINT  uMsg, WPARAM  wParam,LPARAM  lParam);

	virtual void Render()=0;

	void RenderAxis(CVector vAxis,double dLength,COLORREF cColor);

	virtual void OnLButtonDown(WORD wKeyState,POINT pos);
	virtual void OnLButtonUp(WORD wKeyState,POINT pos);
	virtual void OnRButtonDown(WORD wKeyState,POINT pos);
	virtual void OnRButtonUp(WORD wKeyState,POINT pos);
	virtual void OnMButtonDown(WORD wKeyState,POINT pos);
	virtual void OnMButtonUp(WORD wKeyState,POINT pos);
	virtual void OnMouseMove(WORD wKeyState,POINT pos);
	virtual void OnMouseWheel(WORD wKeyState,short wWheelDelta,POINT pos);
	virtual void OnSize(WORD cx,WORD cy);
	virtual void OnSetCursor();

	virtual void ProcessKey(int nKey);
	virtual void ProcessKeyboardMovement();

	virtual void OnPreFlush();
	virtual void OnAfterSwapBuffers();

public:

	virtual void MoveCamera(CVector vCurrentPos,CVector vNewPos);

	HGLRC	ActivateRenderContext();
	void	DeactivateRenderContext();
	HGLRC	CreateRenderContext();
	void	DestroyRenderContext();

	virtual void	ApplyMatrix(DWORD dwType);
	void			GetGLMatrix(DWORD dwType,CMatrix *pMatrix);
	void			GetGLMatrix(DWORD dwType,double *pMatrix);
			
	virtual void	Init(HWND hHostWnd,DWORD dwShowCommand=SW_SHOW,DWORD dwStyle=0xFFFFFFFF,DWORD dwStyleEx=WS_EX_DLGMODALFRAME);
	virtual	void	Destroy();
			
	CVector ProjectPoint(CVector vVector);
	void	ResetCamera();

		 	 COpenGLViewPortBase(DWORD dwFlags=GLV_ENABLE_SCALE|GLV_ENABLE_MOVE|GLV_DRAW_AXISES);
	virtual ~COpenGLViewPortBase();

};

#endif // !defined(AFX_OPENGLVIEWPORTBASE_H__B5341926_A706_4F5C_93E7_42FD74EFD74F__INCLUDED_)
