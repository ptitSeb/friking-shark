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


#pragma once
#include "GameGraphics.h"
#ifdef LINUX
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

struct SJoystickEvent 
{
	u_int32_t time;     /* event timestamp in milliseconds */
	int16_t value;    /* value */
	u_int8_t type;      /* event type */
	u_int8_t number;    /* axis/button number */
};

#define JS_EVENT_BUTTON         0x01    /* button pressed/released */
#define JS_EVENT_AXIS           0x02    /* joystick moved */
#define JS_EVENT_INIT           0x80    /* initial state of device */

#elif defined ANDROID
#include <android/configuration.h>
#include <android/looper.h>
#include <android/native_activity.h>
#include <EGL/egl.h>
#include "../GameEngine/android_native_app_glue.h"

struct SAndroidInputEvent
{
	int nType;
	int nAction;
	int nGameKey;
	float x;
	float y;
	
	SAndroidInputEvent()
	{
		nType=0;
		nAction=0;
		nGameKey=0;
		x=0;
		y=0;
	}
};
#elif defined WIN32
	#include <mmsystem.h>
#endif


class COpenGLViewport: virtual public CSystemObjectBase,virtual public IGenericViewport
{
	
	unsigned int m_nJoystickButtons;
	int m_nJoystickDevice;
	int m_nJoystickXAxis;
	int m_nJoystickYAxis;
	double m_dJoystickDeadZone;
	
	#ifdef WIN32
		HDC			m_hDC;
		HGLRC		m_hRenderContext;
		HWND		m_hWnd;
		int			m_nPixelFormatIndex;
		int			m_nLastMouseMoveX;
		int			m_nLastMouseMoveY;
		JOYCAPS		m_JoystickCaps;
		void UpdateJoystick();
		
		void OnCreate(HWND hWnd);
		void OnDestroy();
		LRESULT ProcessMessage(HWND hWnd,UINT  uMsg, WPARAM  wParam,LPARAM  lParam);
		static LRESULT WindowProc(HWND  hWnd,UINT  uMsg, WPARAM  wParam,LPARAM  lParam);
		static BOOL CALLBACK PrimaryMonitorEnumerationCallBack(HMONITOR hMonitor,HDC hdcMonitor,LPRECT lprcMonitor,LPARAM dwData);
	#elif defined LINUX

		void UpdateJoystick(bool bGenerateEvents);
		
		static int CustomXIOErrorHandler(Display*);
		
		Display 	*m_pXDisplay;
		XVisualInfo *m_pXVisualInfo;
		Colormap 	 m_pXColorMap;
		GLXContext   m_pGLXContext;
		Cursor 		 m_pXHollowCursor;
		Window   	 m_XWindow;
		int 		 m_XLastX;
		int 		 m_XLastY;
		int 		 m_XLastWidth;
		int 		 m_XLastHeight;
		bool		 m_bXExit;
		unsigned int m_nDblClkDetectLastButton;
		int 		 m_nDblClkDetectLastX;
		int 		 m_nDblClkDetectLastY;
		int 		 m_nDblClkDetectLastTime;
		int 		 m_nDblClkDetectMilliseconds;
		int 		 m_nDblClkDetectDistance;


		int          m_nLoopDepth;
		int 		 m_nDetectDragX;
		int 		 m_nDetectDragY;
		unsigned int m_nDetectDragButton;
		bool 		 m_bDetectedDrag;
		
		bool GetFirstXineramaScreen(int *pX,int *pY,int *pW,int *pH);
		void SetupXWindowParameters();
		bool WaitForXEvent(int nEventType);
		void ProcessXEvent(XEvent &event,bool *pbBreakLoop);
		
	#elif defined ANDROID
		android_app *m_pAndroidApp;
		int          m_nLoopDepth;
		static int32_t OnAndroidInput(struct android_app *pApplication, AInputEvent *pEvent);
		static void    OnAndroidCommand(struct android_app *pApplication, int32_t nCommand); 
		
		EGLDisplay m_AndroidDisplay;
		EGLSurface m_AndroidSurface;
		EGLContext m_AndroidRenderContext;
		int32_t    m_AndroidWidth;
		int32_t    m_AndroidHeight;
		SGamePos   m_AndroidMousePos;
		
		std::set<int> m_sAndroidPressedKeys;
		std::deque<SAndroidInputEvent> m_vAndroidInputEvents;
		
		void AndroidCreateRenderContext();
	#endif
		
	SVideoMode  	m_OriginalVideoMode;

	std::map<unsigned int,std::string> m_mKeyNames;

	void InitializeKeyNames();
	
	bool Init(std::string sClass,std::string sName,ISystem *piSystem);

	void SetupBasicRenderOptions();
public:

	bool  	m_bShowSystemMouseCursor;
	bool  	m_bVerticalSync;
	
	std::string m_sCaption;
	IGenericViewportCallBack *m_piCallBack;
	
	// Virtuales de COpenGLViewPortBase

	void Render();

	void OnCharacter(unsigned short wCharacter);
	void OnKeyDown(unsigned short wKeyState);
	void OnKeyUp(unsigned short wKeyState);
	
	void OnSize(unsigned short cx,unsigned short cy);
	void OnMove(unsigned x,unsigned y);
	
	void OnLButtonDown(int pointX,int pointY);
	void OnLButtonDoubleClick(int pointX,int pointY);
	void OnLButtonUp(int pointX,int pointY);
	void OnRButtonDoubleClick(int pointX,int pointY);
	void OnRButtonDown(int pointX,int pointY);
	void OnRButtonUp(int pointX,int pointY);
	void OnMouseMove(int pointX,int pointY);
	void OnMouseWheelUp(int pointX,int pointY);
	void OnMouseWheelDown(int pointX,int pointY);
	
	bool SetVideoMode(SVideoMode *pMode);
	
public:

	//IGenericViewport

	bool CreateFullScreen(unsigned int w,unsigned int h,unsigned int bpp,unsigned int rate);
	bool CreateWindowed(unsigned x, unsigned y, unsigned w, unsigned h);
	void Destroy();

	void GetSize(unsigned *pdwWidth,unsigned *pdwHeight);
	
	void SetCallBack(IGenericViewportCallBack *pCallBack);

	void SetVSync(bool bVSync);
	bool GetVSync();
	
	void EnterLoop();
	void ExitLoop();
	
	
	void GetCursorPos(int *pX,int *pY);
	void SetCursorPos(int x,int y);

	bool HasMouseCapture();
	void SetMouseCapture();
	void ReleaseMouseCapture();

	bool IsKeyDown(unsigned int nKey);
	bool GetKeyName(int nKey,std::string *psKey);
	
	bool IsActiveWindow();

	bool IsMouseVisible();
	void ShowMouseCursor(bool bShow);	
	
	bool DetectDrag(double dx,double dy);

	void				SetCaption(std::string sCaption);
	std::string 		GetCaption();

	void GetCurrentVideoMode(SVideoMode *pMode);

	bool SetWindowed(unsigned int x,unsigned int y,unsigned int w,unsigned int h);
	bool SetFullScreen(unsigned int w,unsigned int h,unsigned int bpp,unsigned int rate);
	
	void 	SetJoystickDeadZone(double dDeadZone);
	double  GetJoystickDeadZone();
	
	 COpenGLViewport(void);
	~COpenGLViewport(void);
};
