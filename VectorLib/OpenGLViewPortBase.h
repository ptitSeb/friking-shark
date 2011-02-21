// OpenGLViewPortBase.h: interface for the COpenGLViewPortBase class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _OPENGLVIEWPORTBASE_
#define _OPENGLVIEWPORTBASE_

#include "VectorLib.h"
#include <GL/gl.h>
#include <GL/glu.h>

class COpenGLViewPortBase  
{
public:
	HDC			m_hDC;
	HGLRC		m_hRenderContext;
	HWND		m_hWnd;
	bool		m_bInitialized;

	int			m_nPixelFormatIndex;

	void OnCreate(HWND hWnd);
	void OnDestroy();
	void OnPaint();

	static LRESULT CALLBACK HostWindowProc(HWND  hwnd,UINT  uMsg, WPARAM  wParam,LPARAM  lParam);
	static LRESULT CALLBACK ViewPortWindowProc(HWND  hwnd,UINT  uMsg, WPARAM  wParam,LPARAM  lParam);

	virtual LRESULT ProcessMessage(HWND hwnd,UINT  uMsg, WPARAM  wParam,LPARAM  lParam);

	virtual void Render()=0;

	virtual void OnLButtonDown(WORD wKeyState,POINT pos);
	virtual void OnLButtonUp(WORD wKeyState,POINT pos);
	virtual void OnRButtonDown(WORD wKeyState,POINT pos);
	virtual void OnRButtonUp(WORD wKeyState,POINT pos);
	virtual void OnMButtonDown(WORD wKeyState,POINT pos);
	virtual void OnMButtonUp(WORD wKeyState,POINT pos);
	virtual void OnMouseMove(WORD wKeyState,POINT pos);
	virtual void OnMouseWheel(WORD wKeyState,short wWheelDelta,POINT pos);
	virtual void OnSize(WORD cx,WORD cy);
	virtual void OnMove();
	virtual void OnSetCursor();
	virtual void OnCharacter(WORD wCharacter);
	virtual void OnKeyDown(WORD wKeyState);
	virtual void OnKeyUp(WORD wKeyState);
	
	virtual void OnPreFlush();
	virtual void OnAfterSwapBuffers();

public:

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

		 	 COpenGLViewPortBase();
	virtual ~COpenGLViewPortBase();

};

#endif
