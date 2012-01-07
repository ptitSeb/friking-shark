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
  #include <mmsystem.h>
  #define VIEWPORT_CLASSNAME "OpenGLViewport"
  #define WM_GL_VIEWPORT_END_LOOP WM_USER+0x001
#elif defined LINUX
	#include <X11/extensions/Xrandr.h>
	#include <X11/extensions/Xinerama.h>
	#define DETECT_DRAG_SIZE 3
	#define X_WINDOWS_EVENT_MASK ExposureMask|ButtonPressMask|ButtonReleaseMask|PointerMotionMask|KeyPressMask|KeyReleaseMask|StructureNotifyMask
#elif defined ANDROID
	#include <EGL/egl.h>	
	extern android_app* g_pAndroidApp;
#endif
	
#ifdef WIN32

int TranslateKeyFromWindows(unsigned int nVirtualKey,unsigned int nlparam)
{
	unsigned char nScanCode=(nlparam>>16)&0xff;
	unsigned char bExtended=(nlparam>>24)&1;
	unsigned char nRShiftScanCode=MapVirtualKey(VK_RSHIFT,0);
	unsigned char nWindowsKey=MapVirtualKey(nScanCode,3);
		
	if( nWindowsKey=='\t' || nWindowsKey==' ' || 
		(nWindowsKey>='A' && nWindowsKey<='Z') ||
		(nWindowsKey>='0' && nWindowsKey<='9')){return nWindowsKey;}

	if(nRShiftScanCode && nRShiftScanCode==nScanCode){return GK_RSHIFT;}
	if(nWindowsKey==VK_LCONTROL && bExtended){return GK_RCONTROL;}
	if(nWindowsKey==VK_LMENU && bExtended){return GK_RMENU;}

	switch(nVirtualKey)
	{
		case VK_LBUTTON  :return GK_LBUTTON;
		case VK_RBUTTON  :return GK_RBUTTON;
		case VK_MBUTTON  :return GK_MBUTTON;
		case VK_BACK     :return GK_BACK;
		case VK_TAB      :return '\t';
		case VK_SPACE    :return ' ';
		case VK_CLEAR    :return GK_CLEAR;
		case VK_RETURN   :return GK_RETURN;
		case VK_SHIFT    :return GK_LSHIFT;
		case VK_PAUSE    :return GK_PAUSE;
		case VK_CAPITAL  :return GK_CAPITAL;
		case VK_ESCAPE   :return GK_ESCAPE;
		case VK_PRIOR    :return GK_PAGEUP;
		case VK_NEXT     :return GK_PAGEDOWN;
		case VK_END      :return GK_END;
		case VK_HOME     :return GK_HOME;
		case VK_LEFT     :return GK_LEFT;
		case VK_UP       :return GK_UP;
		case VK_RIGHT    :return GK_RIGHT;
		case VK_DOWN     :return GK_DOWN;
		case VK_SELECT   :return GK_SELECT;
		case VK_PRINT    :return GK_PRINT;
		case VK_EXECUTE  :return GK_EXECUTE;
		case VK_INSERT   :return GK_INSERT;
		case VK_DELETE   :return GK_DELETE;
		case VK_HELP     :return GK_HELP;
		case VK_LWIN     :return GK_LWIN;
		case VK_RWIN     :return GK_RWIN;
		case VK_NUMPAD0  :return GK_NUMPAD0;
		case VK_NUMPAD1  :return GK_NUMPAD1;
		case VK_NUMPAD2  :return GK_NUMPAD2;
		case VK_NUMPAD3  :return GK_NUMPAD3;
		case VK_NUMPAD4  :return GK_NUMPAD4;
		case VK_NUMPAD5  :return GK_NUMPAD5;
		case VK_NUMPAD6  :return GK_NUMPAD6;
		case VK_NUMPAD7  :return GK_NUMPAD7;
		case VK_NUMPAD8  :return GK_NUMPAD8;
		case VK_NUMPAD9  :return GK_NUMPAD9;
		case VK_MULTIPLY :return GK_MULTIPLY;
		case VK_ADD      :return GK_ADD;
		case VK_SEPARATOR:return GK_SEPARATOR;
		case VK_SUBTRACT :return GK_SUBTRACT;
		case VK_DECIMAL  :return GK_DECIMAL;
		case VK_DIVIDE   :return GK_DIVIDE;
		case VK_F1       :return GK_F1;
		case VK_F2       :return GK_F2;
		case VK_F3       :return GK_F3;
		case VK_F4       :return GK_F4;
		case VK_F5       :return GK_F5;
		case VK_F6       :return GK_F6;
		case VK_F7       :return GK_F7;
		case VK_F8       :return GK_F8;
		case VK_F9       :return GK_F9;
		case VK_F10      :return GK_F10;
		case VK_F11      :return GK_F11;
		case VK_F12      :return GK_F12;
		case VK_F13      :return GK_F13;
		case VK_F14      :return GK_F14;
		case VK_F15      :return GK_F15;
		case VK_F16      :return GK_F16;
		case VK_F17      :return GK_F17;
		case VK_F18      :return GK_F18;
		case VK_F19      :return GK_F19;
		case VK_F20      :return GK_F20;
		case VK_F21      :return GK_F21;
		case VK_F22      :return GK_F22;
		case VK_F23      :return GK_F23;
		case VK_F24      :return GK_F24;
		case VK_NUMLOCK  :return GK_NUMLOCK;
		case VK_SCROLL   :return GK_SCROLL;
		case VK_LSHIFT   :return GK_LSHIFT;
		case VK_RSHIFT   :return GK_RSHIFT;
		case VK_LCONTROL :return GK_LCONTROL;
		case VK_RCONTROL :return GK_RCONTROL;
		case VK_CONTROL  :return GK_LCONTROL;
		case VK_LMENU    :return GK_LMENU;
		case VK_RMENU    :return GK_RMENU;
		case VK_MENU     :return GK_LMENU;
	};

	WORD nCharacter=0;
	static const BYTE keyboardstate[256]={0};
	if(ToAscii(nVirtualKey,nScanCode,keyboardstate,&nCharacter,0)){return nCharacter;}

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
		case GK_RBUTTON  :return VK_RBUTTON;
		case GK_MBUTTON  :return VK_MBUTTON;
		case GK_BACK     :return VK_BACK;
		case GK_CLEAR    :return VK_CLEAR;
		case GK_RETURN   :return VK_RETURN;
		case GK_PAUSE    :return VK_PAUSE;
		case GK_CAPITAL  :return VK_CAPITAL;
		case GK_ESCAPE   :return VK_ESCAPE;
		case GK_PAGEUP   :return VK_PRIOR;
		case GK_PAGEDOWN :return VK_NEXT;
		case GK_END      :return VK_END;
		case GK_HOME     :return VK_HOME;
		case GK_LEFT     :return VK_LEFT;
		case GK_UP       :return VK_UP;
		case GK_RIGHT    :return VK_RIGHT;
		case GK_DOWN     :return VK_DOWN;
		case GK_SELECT   :return VK_SELECT;
		case GK_PRINT    :return VK_PRINT;
		case GK_EXECUTE  :return VK_EXECUTE;
		case GK_INSERT   :return VK_INSERT;
		case GK_DELETE   :return VK_DELETE;
		case GK_HELP     :return VK_HELP;
		case GK_LWIN     :return VK_LWIN;
		case GK_RWIN     :return VK_RWIN;
		case GK_NUMPAD0  :return VK_NUMPAD0;
		case GK_NUMPAD1  :return VK_NUMPAD1;
		case GK_NUMPAD2  :return VK_NUMPAD2;
		case GK_NUMPAD3  :return VK_NUMPAD3;
		case GK_NUMPAD4  :return VK_NUMPAD4;
		case GK_NUMPAD5  :return VK_NUMPAD5;
		case GK_NUMPAD6  :return VK_NUMPAD6;
		case GK_NUMPAD7  :return VK_NUMPAD7;
		case GK_NUMPAD8  :return VK_NUMPAD8;
		case GK_NUMPAD9  :return VK_NUMPAD9;
		case GK_MULTIPLY :return VK_MULTIPLY;
		case GK_ADD      :return VK_ADD;
		case GK_SEPARATOR:return VK_SEPARATOR;
		case GK_SUBTRACT :return VK_SUBTRACT;
		case GK_DECIMAL  :return VK_DECIMAL;
		case GK_DIVIDE   :return VK_DIVIDE;
		case GK_F1       :return VK_F1;
		case GK_F2       :return VK_F2;
		case GK_F3       :return VK_F3;
		case GK_F4       :return VK_F4;
		case GK_F5       :return VK_F5;
		case GK_F6       :return VK_F6;
		case GK_F7       :return VK_F7;
		case GK_F8       :return VK_F8;
		case GK_F9       :return VK_F9;
		case GK_F10      :return VK_F10;
		case GK_F11      :return VK_F11;
		case GK_F12      :return VK_F12;
		case GK_F13      :return VK_F13;
		case GK_F14      :return VK_F14;
		case GK_F15      :return VK_F15;
		case GK_F16      :return VK_F16;
		case GK_F17      :return VK_F17;
		case GK_F18      :return VK_F18;
		case GK_F19      :return VK_F19;
		case GK_F20      :return VK_F20;
		case GK_F21      :return VK_F21;
		case GK_F22      :return VK_F22;
		case GK_F23      :return VK_F23;
		case GK_F24      :return VK_F24;
		case GK_NUMLOCK  :return VK_NUMLOCK;
		case GK_SCROLL   :return VK_SCROLL;
		case GK_LSHIFT   :return VK_LSHIFT;
		case GK_RSHIFT   :return VK_RSHIFT;
		case GK_LCONTROL :return VK_LCONTROL;
		case GK_RCONTROL :return VK_RCONTROL;
		case GK_LMENU    :return VK_LMENU;
		case GK_RMENU    :return VK_RMENU;
		case GK_MENU     :return VK_LMENU;
		case GK_ALTGR    :return VK_RMENU;
	};

	SHORT code=VkKeyScan(nGameKey);
	BYTE  vk=LOBYTE(code);
	BYTE  state=HIBYTE(code);
	if(vk!=0xFF || state!=0xFF)
	{
		return vk;
	}

	return 0;
}
#elif defined LINUX

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
	
	case XK_Control_R:return GK_RCONTROL;
	case XK_Alt_R:return GK_RMENU;
	case XK_Shift_R:return GK_RSHIFT;
	case XK_F13:return GK_F13;
	case XK_F14:return GK_F14;
	case XK_F15:return GK_F15;
	case XK_F16:return GK_F16;
	case XK_F17:return GK_F17;
	case XK_F18:return GK_F18;
	case XK_F19:return GK_F19;
	case XK_F20:return GK_F20;
	case XK_F21:return GK_F21;
	case XK_F22:return GK_F22;
	case XK_F23:return GK_F23;
	case XK_F24:return GK_F24;
	case XK_Pointer_Button1:return GK_LBUTTON;
	case XK_Pointer_Button3:return GK_RBUTTON;
	case XK_Pointer_Button2:return GK_MBUTTON;
	case XK_Clear:return GK_CLEAR;
	case XK_Caps_Lock:return GK_CAPITAL;
	case XK_Select:return GK_SELECT;
	case XK_Print:return GK_PRINT;
	case XK_Execute:return GK_EXECUTE;
	case XK_Help:return GK_HELP;
	case XK_Super_L:return GK_LWIN;
	case XK_Super_R:return GK_RWIN;
	case XK_KP_Multiply:return GK_MULTIPLY;
	case XK_KP_Add:return GK_ADD;
	case XK_KP_Separator:return GK_SEPARATOR;
	case XK_KP_Subtract:return GK_SUBTRACT;
	case XK_KP_Decimal:return GK_DECIMAL;
	case XK_KP_Divide:return GK_DIVIDE;
	case XK_Num_Lock:return GK_NUMLOCK;
	case XK_Scroll_Lock:return GK_SCROLL;
	case XK_Menu: return GK_MENU;
	case 0xfe03:return GK_ALTGR;
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
	case GK_F6:return XK_F6;
	case GK_F7:return XK_F7;
	case GK_F8:return XK_F8;
	case GK_F9:return XK_F9;
	case GK_F10:return XK_F10;
	case GK_F11:return XK_F11;
	case GK_F12:return XK_F12;
	case GK_PAUSE:return XK_Pause;
	case GK_PAGEUP:return XK_Page_Up;
	case GK_PAGEDOWN:return XK_Page_Down;
	
	case GK_RCONTROL: return XK_Control_R;
	case GK_RMENU: return XK_Alt_R;
	case GK_RSHIFT: return XK_Shift_R;
	case GK_F13: return XK_F13;
	case GK_F14: return XK_F14;
	case GK_F15: return XK_F15;
	case GK_F16: return XK_F16;
	case GK_F17: return XK_F17;
	case GK_F18: return XK_F18;
	case GK_F19: return XK_F19;
	case GK_F20: return XK_F20;
	case GK_F21: return XK_F21;
	case GK_F22: return XK_F22;
	case GK_F23: return XK_F23;
	case GK_F24: return XK_F24;
	case GK_LBUTTON: return XK_Pointer_Button1;
	case GK_RBUTTON: return XK_Pointer_Button3;
	case GK_MBUTTON: return XK_Pointer_Button2;
	case GK_CLEAR: return XK_Clear;
	case GK_CAPITAL: return XK_Caps_Lock;
	case GK_SELECT: return XK_Select;
	case GK_PRINT: return XK_Print;
	case GK_EXECUTE: return XK_Execute;
	case GK_HELP: return XK_Help;
	case GK_LWIN: return XK_Super_L;
	case GK_RWIN: return XK_Super_R;
	case GK_MULTIPLY: return XK_KP_Multiply;
	case GK_ADD: return XK_KP_Add;
	case GK_SEPARATOR: return XK_KP_Separator;
	case GK_SUBTRACT: return XK_KP_Subtract;
	case GK_DECIMAL: return XK_KP_Decimal;
	case GK_DIVIDE: return XK_KP_Divide;
	case GK_NUMLOCK: return XK_Num_Lock;
	case GK_SCROLL: return XK_Scroll_Lock;
	case GK_MENU: return XK_Menu;
	case GK_ALTGR: return 0xfe03;
	};
	return 0;
}

int COpenGLViewport::CustomXIOErrorHandler( Display*)
{
  RTTRACE("COpenGLViewport -> X11 ConnectionLost, Exiting");
  return 0;
}

#elif defined ANDROID

int TranslateKeyFromAndroid(int nAndroidKey)
{
	switch(nAndroidKey)
	{
		case AKEYCODE_HOME:return GK_HOME;
		case AKEYCODE_BACK:return GK_ESCAPE;
		case AKEYCODE_CALL:return GK_RETURN;
		case AKEYCODE_ENDCALL:return GK_ESCAPE;
		case AKEYCODE_0:return GK_NUMPAD0;
		case AKEYCODE_1:return GK_NUMPAD1;
		case AKEYCODE_2:return GK_NUMPAD2;
		case AKEYCODE_3:return GK_NUMPAD3;
		case AKEYCODE_4:return GK_NUMPAD4;
		case AKEYCODE_5:return GK_NUMPAD5;
		case AKEYCODE_6:return GK_NUMPAD6;
		case AKEYCODE_7:return GK_NUMPAD7;
		case AKEYCODE_8:return GK_NUMPAD8;
		case AKEYCODE_9:return GK_NUMPAD9;
		case AKEYCODE_STAR:return GK_MULTIPLY;
		case AKEYCODE_POUND:return 0;
		case AKEYCODE_DPAD_UP:return GK_UP;
		case AKEYCODE_DPAD_DOWN:return GK_DOWN;
		case AKEYCODE_DPAD_LEFT:return GK_LEFT;
		case AKEYCODE_DPAD_RIGHT:return GK_RIGHT;
		case AKEYCODE_DPAD_CENTER:return 0;
		case AKEYCODE_VOLUME_UP:return 0;
		case AKEYCODE_VOLUME_DOWN:return 0;
		case AKEYCODE_POWER:return 0;
		case AKEYCODE_CAMERA:return 0;
		case AKEYCODE_CLEAR:return 0;
		case AKEYCODE_A:return 'a';
		case AKEYCODE_B:return 'b';
		case AKEYCODE_C:return 'c';
		case AKEYCODE_D:return 'd';
		case AKEYCODE_E:return 'e';
		case AKEYCODE_F:return 'f';
		case AKEYCODE_G:return 'g';
		case AKEYCODE_H:return 'h';
		case AKEYCODE_I:return 'i';
		case AKEYCODE_J:return 'j';
		case AKEYCODE_K:return 'k';
		case AKEYCODE_L:return 'l';
		case AKEYCODE_M:return 'm';
		case AKEYCODE_N:return 'n';
		case AKEYCODE_O:return 'o';
		case AKEYCODE_P:return 'p';
		case AKEYCODE_Q:return 'q';
		case AKEYCODE_R:return 'r';
		case AKEYCODE_S:return 's';
		case AKEYCODE_T:return 't';
		case AKEYCODE_U:return 'u';
		case AKEYCODE_V:return 'v';
		case AKEYCODE_W:return 'w';
		case AKEYCODE_X:return 'x';
		case AKEYCODE_Y:return 'y';
		case AKEYCODE_Z:return 'z';
		case AKEYCODE_COMMA:return ',';
		case AKEYCODE_PERIOD:return '.';
		case AKEYCODE_ALT_LEFT:return GK_LMENU;
		case AKEYCODE_ALT_RIGHT:return GK_RMENU;
		case AKEYCODE_SHIFT_LEFT:return GK_LSHIFT;
		case AKEYCODE_SHIFT_RIGHT:return GK_RSHIFT;
		case AKEYCODE_TAB:return '\t';
		case AKEYCODE_SPACE:return ' ';
		case AKEYCODE_SYM:return 0;
		case AKEYCODE_EXPLORER:return 0;
		case AKEYCODE_ENVELOPE:return 0;
		case AKEYCODE_ENTER:return GK_RETURN;
		case AKEYCODE_DEL: return GK_DELETE;
		case AKEYCODE_GRAVE:return 0;
		case AKEYCODE_MINUS:return '-';
		case AKEYCODE_EQUALS:return '=';
		case AKEYCODE_LEFT_BRACKET:return '{';
		case AKEYCODE_RIGHT_BRACKET:return '}';
		case AKEYCODE_BACKSLASH:return '\\';
		case AKEYCODE_SEMICOLON:return ';';
		case AKEYCODE_APOSTROPHE:return '\'';
		case AKEYCODE_SLASH:return '/';
		case AKEYCODE_AT:return '@';
		case AKEYCODE_NUM:return '#';
		case AKEYCODE_HEADSETHOOK:return 0;
		case AKEYCODE_FOCUS:return 0;
		case AKEYCODE_PLUS:return '+';
		case AKEYCODE_MENU:return 0;
		case AKEYCODE_NOTIFICATION:return 0;
		case AKEYCODE_SEARCH:return 0;
		case AKEYCODE_MEDIA_PLAY_PAUSE:return 0;
		case AKEYCODE_MEDIA_STOP:return 0;
		case AKEYCODE_MEDIA_NEXT:return 0;
		case AKEYCODE_MEDIA_PREVIOUS:return 0;
		case AKEYCODE_MEDIA_REWIND:return 0;
		case AKEYCODE_MEDIA_FAST_FORWARD:return 0;
		case AKEYCODE_MUTE:return 0;
		case AKEYCODE_PAGE_UP:return GK_PAGEUP;
		case AKEYCODE_PAGE_DOWN:return GK_PAGEDOWN;
		case AKEYCODE_PICTSYMBOLS:return 0;
		case AKEYCODE_SWITCH_CHARSET:return 0;
		case AKEYCODE_BUTTON_A:return 0;
		case AKEYCODE_BUTTON_B:return 0;
		case AKEYCODE_BUTTON_C:return 0;
		case AKEYCODE_BUTTON_X:return 0;
		case AKEYCODE_BUTTON_Y:return 0;
		case AKEYCODE_BUTTON_Z:return 0;
		case AKEYCODE_BUTTON_L1:return 0;
		case AKEYCODE_BUTTON_R1:return 0;
		case AKEYCODE_BUTTON_L2:return 0;
		case AKEYCODE_BUTTON_R2:return 0;
		case AKEYCODE_BUTTON_THUMBL:return 0;
		case AKEYCODE_BUTTON_THUMBR:return 0;
		case AKEYCODE_BUTTON_START:return 0;
		case AKEYCODE_BUTTON_SELECT:return GK_RETURN;
		case AKEYCODE_BUTTON_MODE:return 0;
	};
	return 0;
}

int TranslateKeyToAndroid(int nGameKey)
{
	switch(nGameKey)
	{
		case GK_HOME:AKEYCODE_HOME;
		case GK_ESCAPE:AKEYCODE_BACK;
		case GK_NUMPAD0:AKEYCODE_0;
		case GK_NUMPAD1:AKEYCODE_1;
		case GK_NUMPAD2:AKEYCODE_2;
		case GK_NUMPAD3:AKEYCODE_3;
		case GK_NUMPAD4:AKEYCODE_4;
		case GK_NUMPAD5:AKEYCODE_5;
		case GK_NUMPAD6:AKEYCODE_6;
		case GK_NUMPAD7:AKEYCODE_7;
		case GK_NUMPAD8:AKEYCODE_8;
		case GK_NUMPAD9:AKEYCODE_9;
		case GK_MULTIPLY:AKEYCODE_STAR;
		case GK_UP:AKEYCODE_DPAD_UP;
		case GK_DOWN:AKEYCODE_DPAD_DOWN;
		case GK_LEFT:AKEYCODE_DPAD_LEFT;
		case GK_RIGHT:AKEYCODE_DPAD_RIGHT;
		case 'a':AKEYCODE_A;
		case 'b':AKEYCODE_B;
		case 'c':AKEYCODE_C;
		case 'd':AKEYCODE_D;
		case 'e':AKEYCODE_E;
		case 'f':AKEYCODE_F;
		case 'g':AKEYCODE_G;
		case 'h':AKEYCODE_H;
		case 'i':AKEYCODE_I;
		case 'j':AKEYCODE_J;
		case 'k':AKEYCODE_K;
		case 'l':AKEYCODE_L;
		case 'm':AKEYCODE_M;
		case 'n':AKEYCODE_N;
		case 'o':AKEYCODE_O;
		case 'p':AKEYCODE_P;
		case 'q':AKEYCODE_Q;
		case 'r':AKEYCODE_R;
		case 's':AKEYCODE_S;
		case 't':AKEYCODE_T;
		case 'u':AKEYCODE_U;
		case 'v':AKEYCODE_V;
		case 'w':AKEYCODE_W;
		case 'x':AKEYCODE_X;
		case 'y':AKEYCODE_Y;
		case 'z':AKEYCODE_Z;
		case ',':AKEYCODE_COMMA;
		case '.':AKEYCODE_PERIOD;
		case GK_LMENU:AKEYCODE_ALT_LEFT;
		case GK_RMENU:AKEYCODE_ALT_RIGHT;
		case GK_LSHIFT:AKEYCODE_SHIFT_LEFT;
		case GK_RSHIFT:AKEYCODE_SHIFT_RIGHT;
		case '\t':AKEYCODE_TAB;
		case ' ':AKEYCODE_SPACE;
		case GK_RETURN:AKEYCODE_ENTER;
		case AKEYCODE_DEL: return GK_DELETE;
		case '-':AKEYCODE_MINUS;
		case '=':AKEYCODE_EQUALS;
		case '{':AKEYCODE_LEFT_BRACKET;
		case '}':AKEYCODE_RIGHT_BRACKET;
		case '\\':AKEYCODE_BACKSLASH;
		case ';':AKEYCODE_SEMICOLON;
		case '\'':AKEYCODE_APOSTROPHE;
		case '/':AKEYCODE_SLASH;
		case '@':AKEYCODE_AT;
		case '#':AKEYCODE_NUM;
		case '+':AKEYCODE_PLUS;
		case GK_PAGEUP:AKEYCODE_PAGE_UP;
		case GK_PAGEDOWN:AKEYCODE_PAGE_DOWN;
	};
	return 0;
}

#endif

COpenGLViewport::COpenGLViewport(void)
{
	m_piCallBack=NULL;
	m_bVerticalSync=false;
	
	m_nJoystickButtons=0;
	m_nJoystickXAxis=0;
	m_nJoystickYAxis=0;
	m_dJoystickDeadZone=0.25;
	
#ifdef WIN32
	m_nLastMouseMoveX=-100000;
	m_nLastMouseMoveY=-100000;
	m_hDC=NULL;
	m_hWnd=NULL;
	m_hRenderContext=NULL;
	m_nPixelFormatIndex=0;
#elif defined LINUX
	m_nJoystickDevice=-1;
	m_bShowSystemMouseCursor=true;
	m_pXDisplay=NULL;
	m_pXVisualInfo=NULL;	
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
	
#elif defined ANDROID
	m_AndroidWidth=0;
	m_AndroidHeight=0;
	m_AndroidRenderContext=EGL_NO_CONTEXT;
	m_AndroidDisplay=EGL_NO_DISPLAY;
	m_AndroidSurface=EGL_NO_SURFACE;
	m_pAndroidApp=NULL;
	m_nLoopDepth=0;
#endif
	
	GetCurrentVideoMode(&m_OriginalVideoMode);
	
	InitializeKeyNames();
}

COpenGLViewport::~COpenGLViewport(void)
{
}

bool COpenGLViewport::Init(std::string sClass,std::string sName,ISystem *piSystem)
{
	bool bOk=CSystemObjectBase::Init(sClass,sName,piSystem);
	if(!bOk){return bOk;}
	
	#ifdef ANDROID
	m_pAndroidApp=g_pAndroidApp;
	m_pAndroidApp->userData = this;
	m_pAndroidApp->onAppCmd = OnAndroidCommand;
	m_pAndroidApp->onInputEvent = OnAndroidInput;
	RTTRACE("COpenGLViewport::InitializeAndroidViewPort -> App %p",m_pAndroidApp);
	RTTRACE("COpenGLViewport::InitializeAndroidViewPort -> Waiting for render context....");
	
	while (m_AndroidRenderContext==EGL_NO_CONTEXT)
	{
		// Read all pending events.
		int events=0;
		int eventId=0;
		bool bBlocking=false;
		struct android_poll_source* source=NULL;
		while ((eventId=ALooper_pollAll(!bBlocking? 0 : -1, NULL, &events,(void**)&source)) >= 0) 
		{
			if (source != NULL) {source->process(m_pAndroidApp, source);}
			
			if (m_pAndroidApp->destroyRequested != 0) 
			{
				RTTRACE("COpenGLViewport::InitializeAndroidViewPort -> Exit, Destroy request received");
				return false;
			}
		}
	}
	
	RTTRACE("COpenGLViewport::InitializeAndroidViewPort -> Exit");
	#endif
	#ifdef LINUX
	m_nJoystickDevice = open("/dev/input/js0", O_RDONLY|O_NONBLOCK);
	if(m_nJoystickDevice!=-1)
	{
		RTTRACE("COpenGLViewport::InitializeAndroidViewPort -> Joystick 0 found");
		UpdateJoystick(false);
	}
	#endif
	
	return  bOk;
}

void COpenGLViewport::InitializeKeyNames()
{
	for(int l=1;l<128;l++)
	{
		if(l=='\t' || l==' '){continue;}
		char sTemp[2]={(char)l,0};m_mKeyNames[l]=sTemp;
	}
	m_mKeyNames['\t']="Tab";
	m_mKeyNames[' ']="Space";
	m_mKeyNames[GK_LBUTTON]="Left Mouse";
	m_mKeyNames[GK_LBUTTON]="Left Mouse";
	m_mKeyNames[GK_RBUTTON]="Right Mouse";
	m_mKeyNames[GK_MBUTTON]="Middle Mouse";
	m_mKeyNames[GK_BACK]="Backspace";
	m_mKeyNames[GK_CLEAR]="Clear";
	m_mKeyNames[GK_RETURN]="Return";
	m_mKeyNames[GK_PAUSE]="Pause";
	m_mKeyNames[GK_CAPITAL]="Caps Lock";
	m_mKeyNames[GK_ESCAPE]="Escape";
	m_mKeyNames[GK_PAGEUP]="Page Up";
	m_mKeyNames[GK_PAGEDOWN]="Page Down";
	m_mKeyNames[GK_END]="End";
	m_mKeyNames[GK_HOME]="Home";
	m_mKeyNames[GK_LEFT]="Left";
	m_mKeyNames[GK_UP]="Up";
	m_mKeyNames[GK_RIGHT]="Right";
	m_mKeyNames[GK_DOWN]="Down";
	m_mKeyNames[GK_SELECT]="Select";
	m_mKeyNames[GK_PRINT]="Print";
	m_mKeyNames[GK_EXECUTE]="Execute";
	m_mKeyNames[GK_INSERT]="Insert";
	m_mKeyNames[GK_DELETE]="Delete";
	m_mKeyNames[GK_HELP]="Help";

	m_mKeyNames[GK_LWIN]="Left Win";
	m_mKeyNames[GK_RWIN]="Right Win";
	
	m_mKeyNames[GK_NUMPAD0]="Numpad 0";
	m_mKeyNames[GK_NUMPAD1]="Numpad 1";
	m_mKeyNames[GK_NUMPAD2]="Numpad 2";
	m_mKeyNames[GK_NUMPAD3]="Numpad 3";
	m_mKeyNames[GK_NUMPAD4]="Numpad 4";
	m_mKeyNames[GK_NUMPAD5]="Numpad 5";
	m_mKeyNames[GK_NUMPAD6]="Numpad 6";
	m_mKeyNames[GK_NUMPAD7]="Numpad 7";
	m_mKeyNames[GK_NUMPAD8]="Numpad 8";
	m_mKeyNames[GK_NUMPAD9]="Numpad 9";
	m_mKeyNames[GK_MULTIPLY]="Numpad *";
	m_mKeyNames[GK_ADD]="Numpad +";
	m_mKeyNames[GK_SEPARATOR]="Separator";
	m_mKeyNames[GK_SUBTRACT]="Numpad -";
	m_mKeyNames[GK_DECIMAL]="Numpad .";
	m_mKeyNames[GK_DIVIDE]="Numpad /";
	m_mKeyNames[GK_F1]="F1";
	m_mKeyNames[GK_F2]="F2";
	m_mKeyNames[GK_F3]="F3";
	m_mKeyNames[GK_F4]="F4";
	m_mKeyNames[GK_F5]="F5";
	m_mKeyNames[GK_F6]="F6";
	m_mKeyNames[GK_F7]="F7";
	m_mKeyNames[GK_F8]="F8";
	m_mKeyNames[GK_F9]="F9";
	m_mKeyNames[GK_F10]="F10";
	m_mKeyNames[GK_F11]="F11";
	m_mKeyNames[GK_F12]="F12";
	m_mKeyNames[GK_F13]="F13";
	m_mKeyNames[GK_F14]="F14";
	m_mKeyNames[GK_F15]="F15";
	m_mKeyNames[GK_F16]="F16";
	m_mKeyNames[GK_F17]="F17";
	m_mKeyNames[GK_F18]="F18";
	m_mKeyNames[GK_F19]="F19";
	m_mKeyNames[GK_F20]="F20";
	m_mKeyNames[GK_F21]="F21";
	m_mKeyNames[GK_F22]="F22";
	m_mKeyNames[GK_F23]="F23";
	m_mKeyNames[GK_F24]="F24";

	m_mKeyNames[GK_NUMLOCK]="Num Lock";
	m_mKeyNames[GK_SCROLL]="Scroll Lock";
	m_mKeyNames[GK_LSHIFT]="Left Shift";
	m_mKeyNames[GK_RSHIFT]="Right Shift";
	m_mKeyNames[GK_LCONTROL]="Left Control";
	m_mKeyNames[GK_RCONTROL]="Right Control";
	m_mKeyNames[GK_LMENU]="Left Alt";
	m_mKeyNames[GK_RMENU]="Right Alt";
	m_mKeyNames[GK_MENU]="Menu";
	m_mKeyNames[GK_ALTGR]="Alt Gr";

	m_mKeyNames[GK_JOYLEFT]="Joystick Left";
	m_mKeyNames[GK_JOYUP]="Joystick Up";
	m_mKeyNames[GK_JOYRIGHT]="Joystick Right";
	m_mKeyNames[GK_JOYDOWN]="Joystick Down";
	m_mKeyNames[GK_JOY0]="Joystick 1";
	m_mKeyNames[GK_JOY1]="Joystick 2";
	m_mKeyNames[GK_JOY2]="Joystick 3";
	m_mKeyNames[GK_JOY3]="Joystick 4";
	m_mKeyNames[GK_JOY4]="Joystick 5";
	m_mKeyNames[GK_JOY5]="Joystick 6";
	m_mKeyNames[GK_JOY6]="Joystick 7";
	m_mKeyNames[GK_JOY7]="Joystick 8";
	m_mKeyNames[GK_JOY8]="Joystick 9";
	m_mKeyNames[GK_JOY9]="Joystick 10";
	m_mKeyNames[GK_JOY10]="Joystick 11";
	m_mKeyNames[GK_JOY11]="Joystick 12";
	m_mKeyNames[GK_JOY12]="Joystick 13";
	m_mKeyNames[GK_JOY13]="Joystick 14";
	m_mKeyNames[GK_JOY14]="Joystick 15";
	m_mKeyNames[GK_JOY15]="Joystick 16";
	m_mKeyNames[GK_JOY16]="Joystick 17";
	m_mKeyNames[GK_JOY17]="Joystick 18";
	m_mKeyNames[GK_JOY18]="Joystick 19";
	m_mKeyNames[GK_JOY19]="Joystick 20";
	m_mKeyNames[GK_JOY20]="Joystick 21";
	m_mKeyNames[GK_JOY21]="Joystick 22";
	m_mKeyNames[GK_JOY22]="Joystick 23";
	m_mKeyNames[GK_JOY23]="Joystick 24";
	m_mKeyNames[GK_JOY24]="Joystick 25";
	m_mKeyNames[GK_JOY25]="Joystick 26";
	m_mKeyNames[GK_JOY26]="Joystick 27";
	m_mKeyNames[GK_JOY27]="Joystick 28";
	m_mKeyNames[GK_JOY28]="Joystick 29";
	m_mKeyNames[GK_JOY29]="Joystick 30";
	m_mKeyNames[GK_JOY30]="Joystick 31";
	m_mKeyNames[GK_JOY31]="Joystick 32";
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
			if(m_hRenderContext)
			{
				wglMakeCurrent(m_hDC,m_hRenderContext);
				SetupBasicRenderOptions();
			}
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
		OnKeyDown(TranslateKeyFromWindows(wParam,lParam));
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		OnKeyUp(TranslateKeyFromWindows(wParam,lParam));
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
		// Avoid spurious WM_MOUSEMOVE messages from windows (ex when the cursor keys are pressed)
		if(m_nLastMouseMoveX!=pointX || m_nLastMouseMoveY!=pointY)
		{
			m_nLastMouseMoveX=pointX;
			m_nLastMouseMoveY=pointY;
			OnMouseMove(pointX,pointY);
		}
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

bool COpenGLViewport::CreateWindowed(unsigned x, unsigned y, unsigned w, unsigned h)
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
	m_hWnd = CreateWindowEx(WS_EX_DLGMODALFRAME,VIEWPORT_CLASSNAME,"Loading...",dwStyle,0,0,0,0,NULL,NULL,NULL,(void *)this);
	if(m_hWnd)
	{
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

		unsigned int dwStyle=GetWindowLong(m_hWnd,GWL_STYLE);
		SetWindowLong(m_hWnd,GWL_STYLE,dwStyle);
		ShowWindow(m_hWnd,SW_SHOW);
		EnableWindow(m_hWnd,TRUE);
		UpdateWindow(m_hWnd);
	}
	bOk=(m_hRenderContext!=NULL);
	if(!bOk){RTTRACE("COpenGLViewport::CreateWindowed -> Failed to get OpenGL render context");}
#elif defined LINUX
	m_pXDisplay=XOpenDisplay(NULL);
	if(m_pXDisplay)
	{
	  XSetIOErrorHandler(CustomXIOErrorHandler);
	  int nScreen = DefaultScreen( m_pXDisplay );
	  int pVisualAttribs[] = {GLX_RGBA,GLX_RED_SIZE,8,GLX_GREEN_SIZE,8,GLX_BLUE_SIZE,8,GLX_ALPHA_SIZE,8,GLX_DEPTH_SIZE,8,GLX_DOUBLEBUFFER,None};
	  m_pXVisualInfo = glXChooseVisual( m_pXDisplay, nScreen,pVisualAttribs);
	}	  
	if(m_pXVisualInfo){m_pGLXContext = glXCreateContext(m_pXDisplay, m_pXVisualInfo,NULL, GL_TRUE);}
	if(m_pGLXContext){m_pXColorMap = XCreateColormap(m_pXDisplay, RootWindow(m_pXDisplay, m_pXVisualInfo->screen),m_pXVisualInfo->visual, AllocNone);}
	if(m_pXColorMap)
	{
		SetWindowed(x,y,w,h);
		bOk=(m_XWindow!=None);
		if(bOk){SetupBasicRenderOptions();}
	}
	if(!bOk){RTTRACE("COpenGLViewport::CreateWindowed -> Failed to get OpenGL render context");}
#else
	bOk=true;
	RTTRACE("COpenGLViewport::CreateWindowed -> Windowed called");
#endif
	return bOk;
}

bool COpenGLViewport::CreateFullScreen(unsigned int w,unsigned int h,unsigned int bpp,unsigned int rate)
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
	SVideoMode mode;
	mode.bpp=bpp;
	mode.h=h;
	mode.w=w;
	mode.rate=rate;

	SetVideoMode(&mode);
	GetCurrentVideoMode(&mode);

	unsigned int dwStyle=WS_POPUP;
	m_hWnd = CreateWindowEx(WS_EX_DLGMODALFRAME,VIEWPORT_CLASSNAME,"Loading...",dwStyle,mode.fullscreenX,mode.fullscreenY,mode.fullscreenW,mode.fullscreenH,NULL,NULL,NULL,(void *)this);
	if(m_hWnd)
	{
		unsigned int dwStyle=GetWindowLong(m_hWnd,GWL_STYLE);
		SetWindowLong(m_hWnd,GWL_STYLE,dwStyle);
		ShowWindow(m_hWnd,SW_MAXIMIZE);
		EnableWindow(m_hWnd,TRUE);
		UpdateWindow(m_hWnd);
	}
	bOk=(m_hRenderContext!=NULL);
	if(!bOk){RTTRACE("COpenGLViewport::CreateFullScreen -> Failed to get OpenGL render context");}
#elif defined LINUX
	m_pXDisplay=XOpenDisplay(NULL);
	if(m_pXDisplay)
	{
		XSetIOErrorHandler(CustomXIOErrorHandler);
		int nScreen = DefaultScreen( m_pXDisplay );
		int pVisualAttribs[] = {GLX_RGBA,GLX_RED_SIZE,8,GLX_GREEN_SIZE,8,GLX_BLUE_SIZE,8,GLX_ALPHA_SIZE,8,GLX_DEPTH_SIZE,8,GLX_DOUBLEBUFFER,None};
		m_pXVisualInfo = glXChooseVisual( m_pXDisplay, nScreen,pVisualAttribs);
	}	  
	if(m_pXVisualInfo){m_pGLXContext = glXCreateContext(m_pXDisplay, m_pXVisualInfo,NULL, GL_TRUE);}
	if(m_pGLXContext){m_pXColorMap = XCreateColormap(m_pXDisplay, RootWindow(m_pXDisplay, m_pXVisualInfo->screen),m_pXVisualInfo->visual, AllocNone);}
	if(m_pXColorMap)
	{
		SetFullScreen(w,h,bpp,rate);
		bOk=(m_XWindow!=None);
		if(bOk){SetupBasicRenderOptions();}
	}
	if(!bOk){RTTRACE("COpenGLViewport::CreateFullScreen -> Failed to get OpenGL render context");}
#else
	bOk=true;
	RTTRACE("COpenGLViewport::CreateFullScreen -> Full screen called");
#endif

	return bOk;
}

void COpenGLViewport::Destroy()
{
 
#ifdef WIN32
	if(m_hWnd){DestroyWindow(m_hWnd);m_hWnd=NULL;}
	SetVideoMode(&m_OriginalVideoMode);
#elif defined LINUX
	if(m_XWindow!=None)
	{
		SetVideoMode(&m_OriginalVideoMode);
	}
	if(m_pXHollowCursor)
	{
		if(m_bShowSystemMouseCursor)
		{
			XUndefineCursor(m_pXDisplay,m_XWindow);
		}
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
	if(m_pXVisualInfo){XFree(m_pXVisualInfo);m_pXVisualInfo=NULL;}
	
	if(m_nJoystickDevice!=-1){close(m_nJoystickDevice);m_nJoystickDevice=-1;}
	
#endif
	
	CSystemObjectBase::Destroy();
}

void COpenGLViewport::GetSize(unsigned *pdwWidth,unsigned *pdwHeight)
{
#ifdef WIN32
	RECT R={0};
	GetClientRect(m_hWnd,&R);
	*pdwWidth=R.right-R.left;
	*pdwHeight=R.bottom-R.top;
#elif defined LINUX
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
#elif defined ANDROID
	*pdwWidth=m_AndroidWidth;
	*pdwHeight=m_AndroidHeight;
#endif
}

void COpenGLViewport::SetCallBack(IGenericViewportCallBack *pCallBack)
{
	m_piCallBack=pCallBack;
}

void COpenGLViewport::SetupBasicRenderOptions()
{
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LESS);
	glClearColor(0,0,0,1.0);
	
#ifndef ANDROID
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
#endif
	
#ifdef ANDROID_GLES1
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
#else
	#ifndef ANDROID
		glEnable(GL_POINT_SPRITE);
	#endif
#endif
}

void COpenGLViewport::Render()
{
	unsigned int nStartTime=GetTimeStamp();
	//RTTRACE("COpenGLViewport::Render --------------------------------------------- %d",nStartTime);
	//GetTimeMeter()->SetActive(true);
	GetTimeMeter()->Start();
#ifdef ANDROID
	if(m_AndroidRenderContext==EGL_NO_CONTEXT){return;}
#endif

	if(m_piCallBack){m_piCallBack->OnRender();}
	
	unsigned int nFlushStartTime=GetTimeStamp();
	
#ifdef WIN32
	SwapBuffers(m_hDC);
#elif defined LINUX
	glXSwapBuffers(m_pXDisplay,m_XWindow);
#elif defined ANDROID
	eglSwapBuffers(m_AndroidDisplay,m_AndroidSurface);
#endif
	unsigned int nEndTime=GetTimeStamp();
	
	GetTimeMeter()->End();
	GetTimeMeter()->Dump(1000);
	
	//RTTRACE("COpenGLViewport::Render -> Total swap time %dms",nEndTime-nFlushStartTime);
	//RTTRACE("COpenGLViewport::Render -> Total frame time %dms",nEndTime-nStartTime);
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
#elif defined LINUX
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
#elif defined LINUX
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

#ifdef WIN32
void COpenGLViewport::UpdateJoystick()
{
	int nJoys=joyGetNumDevs();
	JOYCAPS joyCaps={0};
	for(int id=JOYSTICKID1;id<JOYSTICKID1+nJoys;id++)
	{
		MMRESULT result=joyGetDevCaps(id,&joyCaps,sizeof(joyCaps));
		if(result!=JOYERR_NOERROR)
		{
			continue;
		}
		int xCenter=(joyCaps.wXmin+joyCaps.wXmax)/2;
		int yCenter=(joyCaps.wYmin+joyCaps.wYmax)/2;

		JOYINFOEX joyInfo={0};
		joyInfo.dwSize=sizeof(joyInfo);
		joyInfo.dwFlags=JOY_RETURNALL;
		result=joyGetPosEx(id,&joyInfo);

		int nOldXAxis=m_nJoystickXAxis;
		int nOldYAxis=m_nJoystickYAxis;

		m_nJoystickXAxis=joyInfo.dwXpos-xCenter;
		m_nJoystickYAxis=joyInfo.dwYpos-yCenter;

		int xThreshold=((double)xCenter)*m_dJoystickDeadZone;
		int yThreshold=((double)yCenter)*m_dJoystickDeadZone;
		if(fabs((double)m_nJoystickXAxis)<xThreshold){m_nJoystickXAxis=0;}
		if(fabs((double)m_nJoystickYAxis)<yThreshold){m_nJoystickYAxis=0;}

		if(nOldXAxis>=0 && m_nJoystickXAxis<0){OnKeyDown(GK_JOYLEFT);}
		if(nOldXAxis<0 && m_nJoystickXAxis>=0){OnKeyUp(GK_JOYLEFT);}
		if(nOldXAxis<=0 && m_nJoystickXAxis>0){OnKeyDown(GK_JOYRIGHT);}
		if(nOldXAxis>0 && m_nJoystickXAxis<=0){OnKeyUp(GK_JOYRIGHT);}

		if(nOldYAxis>=0 && m_nJoystickYAxis<0){OnKeyDown(GK_JOYUP);}
		if(nOldYAxis<0 && m_nJoystickYAxis>=0){OnKeyUp(GK_JOYUP);}
		if(nOldYAxis<=0 && m_nJoystickYAxis>0){OnKeyDown(GK_JOYDOWN);}
		if(nOldYAxis>0 && m_nJoystickYAxis<=0){OnKeyUp(GK_JOYDOWN);}

		for(int x=0;x<32;x++)
		{
			if((joyInfo.dwButtons&(1<<x))!=0 && (m_nJoystickButtons&(1<<x))==0){m_nJoystickButtons|=(1<<x);OnKeyUp(GK_JOY_BUTTON_FIRST+x);}
			if((joyInfo.dwButtons&(1<<x))==0 && (m_nJoystickButtons&(1<<x))!=0){m_nJoystickButtons&=~(1<<x);OnKeyDown(GK_JOY_BUTTON_FIRST+x);}
		}
		while(!bChange);
	}
}
#endif

#ifdef LINUX
void COpenGLViewport::UpdateJoystick(bool bGenerateEvents)
{
	if(m_nJoystickDevice==-1){return;}
	
	int nOldXAxis=m_nJoystickXAxis;
	int nOldYAxis=m_nJoystickYAxis;
	unsigned int nNewButtons=m_nJoystickButtons;
	
	SJoystickEvent joystickEvent={0};
	while(read(m_nJoystickDevice,&joystickEvent, sizeof(joystickEvent)) > 0)
	{
		if(joystickEvent.type&JS_EVENT_BUTTON)
		{
			if(joystickEvent.value)
			{
				nNewButtons|=1<<joystickEvent.number;
			}
			else
			{
				nNewButtons&=~(1<<joystickEvent.number);
			}
		}
		else if(joystickEvent.type&JS_EVENT_AXIS)
		{
			if(joystickEvent.number==0)
			{
				m_nJoystickXAxis=joystickEvent.value;
			}
			else if(joystickEvent.number==1)
			{
				m_nJoystickYAxis=joystickEvent.value;
			}
		}
	}
	
	int xThreshold=(32767.0)*m_dJoystickDeadZone;
	int yThreshold=(32767.0)*m_dJoystickDeadZone;
	if(fabs((double)m_nJoystickXAxis)<xThreshold){m_nJoystickXAxis=0;}
	if(fabs((double)m_nJoystickYAxis)<yThreshold){m_nJoystickYAxis=0;}
	
	if(bGenerateEvents)
	{
		if(nOldXAxis>=0 && m_nJoystickXAxis<0){OnKeyDown(GK_JOYLEFT);}
		if(nOldXAxis<0 && m_nJoystickXAxis>=0){OnKeyUp(GK_JOYLEFT);}
		if(nOldXAxis<=0 && m_nJoystickXAxis>0){OnKeyDown(GK_JOYRIGHT);}
		if(nOldXAxis>0 && m_nJoystickXAxis<=0){OnKeyUp(GK_JOYRIGHT);}
		
		if(nOldYAxis>=0 && m_nJoystickYAxis<0){OnKeyDown(GK_JOYUP);}
		if(nOldYAxis<0 && m_nJoystickYAxis>=0){OnKeyUp(GK_JOYUP);}
		if(nOldYAxis<=0 && m_nJoystickYAxis>0){OnKeyDown(GK_JOYDOWN);}
		if(nOldYAxis>0 && m_nJoystickYAxis<=0){OnKeyUp(GK_JOYDOWN);}
		
		for(int x=0;x<32;x++)
		{
			if((nNewButtons&(1<<x))!=0 && (m_nJoystickButtons&(1<<x))==0){m_nJoystickButtons|=(1<<x);OnKeyUp(GK_JOY_BUTTON_FIRST+x);}
			if((nNewButtons&(1<<x))==0 && (m_nJoystickButtons&(1<<x))!=0){m_nJoystickButtons&=~(1<<x);OnKeyDown(GK_JOY_BUTTON_FIRST+x);}
		}
	}
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
			UpdateJoystick();
		}
		if(m_hWnd)
		{
			InvalidateRect(m_hWnd,NULL,FALSE);
		}
	}
#elif defined LINUX
	XEvent event;
	int nLoopId=++m_nLoopDepth;

	while(m_XWindow!=None && m_nLoopDepth>=nLoopId && !m_bXExit)
	{	
		while(m_XWindow!=None && XCheckWindowEvent(m_pXDisplay,m_XWindow,X_WINDOWS_EVENT_MASK,&event))
		{
			
			bool bBreakLoop=false;
			ProcessXEvent(event,&bBreakLoop);
			if(bBreakLoop){return;}
		}
		
		glXMakeCurrent(m_pXDisplay,m_XWindow,m_pGLXContext);
		UpdateJoystick(true);
		Render();
	}

#elif defined ANDROID

	RTTRACE("COpenGLViewport::EnterLoop -> Enter, Depth %d",m_nLoopDepth);

	int nLoopId=++m_nLoopDepth;
	
	while (m_nLoopDepth>=nLoopId /*&& !m_bXExit*/)
	{
		// Read all pending events.
		int events=0;
		int eventId=0;
		bool bBlocking=false;
		struct android_poll_source* source=NULL;
		while ((eventId=ALooper_pollAll(!bBlocking? 0 : -1, NULL, &events,(void**)&source)) >= 0) 
		{
			if (source != NULL) {source->process(m_pAndroidApp, source);}
			
			while(m_vAndroidInputEvents.size())
			{
				SAndroidInputEvent sEvent=m_vAndroidInputEvents.front();
				m_vAndroidInputEvents.pop_front();
				
				if(sEvent.nType==AINPUT_EVENT_TYPE_MOTION) 
				{
					if(sEvent.nAction==AMOTION_EVENT_ACTION_DOWN)
					{
						m_AndroidMousePos=SGamePos(sEvent.x,sEvent.y);
						// Simulate mouse movement to allow some widgets to update state
						OnMouseMove(sEvent.x,sEvent.y);
						OnLButtonDown(sEvent.x,sEvent.y); 
					}
					else if(sEvent.nAction==AMOTION_EVENT_ACTION_UP)
					{
						m_AndroidMousePos=SGamePos(sEvent.x,sEvent.y);
						// Simulate mouse movement to allow some widgets to update state
						OnMouseMove(sEvent.x,sEvent.y);
						OnLButtonUp(sEvent.x,sEvent.y);
					}
					else if(sEvent.nAction==AMOTION_EVENT_ACTION_MOVE)
					{
						m_AndroidMousePos=SGamePos(sEvent.x,sEvent.y);
						OnMouseMove(sEvent.x,sEvent.y);
					}
				}
				else if(sEvent.nType==AINPUT_EVENT_TYPE_KEY) 
				{
					if(sEvent.nAction==AKEY_EVENT_ACTION_DOWN){m_sAndroidPressedKeys.insert(sEvent.nGameKey);OnKeyDown(sEvent.nGameKey);}
					else if(sEvent.nAction==AKEY_EVENT_ACTION_UP){OnKeyUp(sEvent.nGameKey);m_sAndroidPressedKeys.erase(sEvent.nGameKey);}
				}
			}
			if (m_pAndroidApp->destroyRequested != 0) 
			{
				RTTRACE("COpenGLViewport::EnterLoop -> Exit, Destroy request received, Depth %d",m_nLoopDepth);
				return;
			}
		}
		Render();
	}
	
	RTTRACE("COpenGLViewport::EnterLoop -> Exit, Depth %d",m_nLoopDepth);
	
#endif
}
void COpenGLViewport::ExitLoop()
{
#ifdef WIN32
	PostMessage(m_hWnd,WM_GL_VIEWPORT_END_LOOP,0,0);
#elif defined LINUX
	if(m_nLoopDepth>0){m_nLoopDepth--;}
#elif defined ANDROID
	if(m_nLoopDepth>0){m_nLoopDepth--;}
	RTTRACE("COpenGLViewport::ExitLoop -> Exit, Depth %d",m_nLoopDepth);
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
#elif defined LINUX
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
#elif defined ANDROID
	*pX=(int)m_AndroidMousePos.x;
	*pY=(int)m_AndroidMousePos.y;
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
#elif defined LINUX
	if(m_XWindow!=None){XWarpPointer(m_pXDisplay,None,m_XWindow,0,0,0,0,x,y);}
#elif defined ANDROID
	m_AndroidMousePos.x=x;
	m_AndroidMousePos.y=y;
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
	if(nKey==GK_JOYLEFT && m_nJoystickXAxis<0){return true;}
	if(nKey==GK_JOYRIGHT && m_nJoystickXAxis>0){return true;}
	if(nKey==GK_JOYUP && m_nJoystickYAxis<0){return true;}
	if(nKey==GK_JOYDOWN && m_nJoystickYAxis>0){return true;}
	if(nKey>=GK_JOY_BUTTON_FIRST && nKey<=GK_JOY_BUTTON_LAST){return (m_nJoystickButtons&(1<<(nKey-GK_JOY_BUTTON_FIRST)))!=0;}
#ifdef WIN32
	if(m_hWnd!=GetActiveWindow())
	{
		return false;
	}
	USHORT nKeyState=GetKeyState(TranslateKeyToWindows(nKey));
	return (nKeyState&0x8000)!=0;
#elif defined LINUX
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
#elif defined ANDROID
	return m_sAndroidPressedKeys.find(nKey)!=m_sAndroidPressedKeys.end();
#endif
}

bool COpenGLViewport::GetKeyName(int nKey,std::string *psKey)
{
	std::map<unsigned int,std::string>::iterator i;
	i=m_mKeyNames.find(nKey);
	if(i==m_mKeyNames.end()){*psKey="Unknown";return false;}
	*psKey=i->second;
	return true;
}

bool  COpenGLViewport::IsActiveWindow(){return true;}

bool  COpenGLViewport::IsMouseVisible()
{
	return m_bShowSystemMouseCursor;
}
void  COpenGLViewport::ShowMouseCursor(bool bShow)
{
    // en windows el cursor se pone WM_SETCURSOR
#ifdef LINUX
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
#elif defined LINUX
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

#ifdef WIN32
BOOL CALLBACK COpenGLViewport::PrimaryMonitorEnumerationCallBack(HMONITOR hMonitor,HDC hdcMonitor,LPRECT lprcMonitor,LPARAM dwData)
{
	RECT *pRect=(RECT *)dwData;
	MONITORINFOEX info;
	memset(&info,0,sizeof(info));
	info.cbSize=sizeof(info);
	if(GetMonitorInfo(hMonitor,&info) && info.dwFlags&MONITORINFOF_PRIMARY)
	{
		*pRect=info.rcMonitor;
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}
#endif

void COpenGLViewport::GetCurrentVideoMode(SVideoMode *pMode)
{
#ifdef WIN32
	DEVMODE mode={0};
	mode.dmSize=sizeof(DEVMODE);
	EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&mode);
	pMode->bpp=mode.dmBitsPerPel;
	pMode->w=mode.dmPelsWidth;
	pMode->h=mode.dmPelsHeight;
	pMode->fullscreenX=0;
	pMode->fullscreenY=0;
	pMode->fullscreenW=mode.dmPelsWidth;
	pMode->fullscreenH=mode.dmPelsHeight;
	pMode->rate=60;

	RECT sMonitorRect;
	EnumDisplayMonitors(NULL,NULL,PrimaryMonitorEnumerationCallBack,(LPARAM)&sMonitorRect);
	
	pMode->fullscreenX=sMonitorRect.left;
	pMode->fullscreenY=sMonitorRect.top;
	pMode->fullscreenW=sMonitorRect.right-sMonitorRect.left;
	pMode->fullscreenH=sMonitorRect.bottom-sMonitorRect.top;

	if(pMode->fullscreenW==0 || pMode->fullscreenH==0)
	{
		pMode->fullscreenW=pMode->w;
		pMode->fullscreenH=pMode->h;
	}
#elif defined LINUX
	Display *pDisplay=XOpenDisplay(NULL);
	if(pDisplay)
	{
	  int nScreen=DefaultScreen(pDisplay);
	  pMode->w = DisplayWidth(pDisplay,nScreen);
	  pMode->h = DisplayHeight(pDisplay,nScreen);
	  pMode->bpp = DefaultDepth(pDisplay,nScreen);
	  pMode->rate=60;
	  if(!GetFirstXineramaScreen(&pMode->fullscreenX,&pMode->fullscreenY,&pMode->fullscreenW,&pMode->fullscreenH))
	  {
		  pMode->fullscreenX=0;
		  pMode->fullscreenY=0;
		  pMode->fullscreenW=pMode->w;
		  pMode->fullscreenH=pMode->h;
	  }
	  
	  XCloseDisplay(pDisplay);
	  pDisplay=NULL;
	}	
	//RTTRACE("GetCurrentVideoMode : %dx%d : %d : full: %d,%d %dx%d",pMode->w,pMode->h,pMode->bpp,pMode->fullscreenX,pMode->fullscreenY,pMode->fullscreenW,pMode->fullscreenH);
#elif defined ANDROID
	pMode->w = m_AndroidWidth;
	pMode->h = m_AndroidHeight;
	pMode->bpp = 8;//FIXME
	pMode->rate=60;
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
#elif defined LINUX
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

#ifdef LINUX
bool COpenGLViewport::GetFirstXineramaScreen(int *pX,int *pY,int *pW,int *pH)
{
	*pX=*pY=*pW=*pH=0;
	
	bool bRet=false;
	Display *pDisplay=XOpenDisplay(NULL);
	if(pDisplay==NULL){return bRet;}
	
	int nXinEventBase=0, nXinErrorBase=0;
	if (XineramaQueryExtension(pDisplay, &nXinEventBase, &nXinErrorBase) && XineramaIsActive(pDisplay))
	{
		int nXinScreens=0;
		XineramaScreenInfo *pXinScreenInfo=XineramaQueryScreens(pDisplay, &nXinScreens);
		if(nXinScreens)
		{
			*pX=pXinScreenInfo[0].x_org;
			*pY=pXinScreenInfo[0].y_org;
			*pW=pXinScreenInfo[0].width;
			*pH=pXinScreenInfo[0].height;
			bRet=true;
		}
		
		XFree(pXinScreenInfo);
		pXinScreenInfo=NULL;
	}
	XCloseDisplay(pDisplay);
	return bRet;
}

bool COpenGLViewport::WaitForXEvent(int nEventType)
{
	XEvent event;
	do
	{
		XNextEvent(m_pXDisplay, &event);
		{
			bool bBreakLoop=false;
			ProcessXEvent(event,&bBreakLoop);
		}
	}while(event.type!=nEventType);
}

void COpenGLViewport::ProcessXEvent(XEvent &event,bool *pbBreakLoop)
{
	(*pbBreakLoop)=false;
	if(event.xany.display!=m_pXDisplay || event.xany.window!=m_XWindow){return;}
	
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
			*pbBreakLoop=true;
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
				*pbBreakLoop=true;
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
	*pbBreakLoop=false;
	return;
}

void COpenGLViewport::SetupXWindowParameters()
{
	// Free previous cursor
	if(m_pXHollowCursor)
	{
		if(m_bShowSystemMouseCursor)
		{
			XUndefineCursor(m_pXDisplay,m_XWindow);
		}
		XFreeCursor(m_pXDisplay,m_pXHollowCursor);
		m_pXHollowCursor=0;
	}

	if(m_pXDisplay!=NULL && m_XWindow!=None)
	{
		// Setup transparent cursor (i do not know of any other way to hide it)
		Pixmap emptyBitmap;
		XColor black;
		static char pContent[] = { 0,0,0,0,0,0,0,0 };
		black.red = black.green = black.blue = 0;

		emptyBitmap = XCreateBitmapFromData(m_pXDisplay, m_XWindow, pContent, 8, 8);
		m_pXHollowCursor= XCreatePixmapCursor(m_pXDisplay, emptyBitmap,emptyBitmap,&black, &black, 0, 0);
		XFreePixmap(m_pXDisplay,emptyBitmap );

		// Set window caption
		XSetStandardProperties(m_pXDisplay,m_XWindow,m_sCaption.c_str(),m_sCaption.c_str(),None,NULL,0,NULL);
	}	
}
#endif


bool COpenGLViewport::SetFullScreen(unsigned int w,unsigned int h,unsigned int bpp,unsigned int rate)
{
	SVideoMode mode;
	mode.bpp=bpp;
	mode.h=h;
	mode.w=w;
	mode.rate=rate;

#ifdef WIN32
	SetVideoMode(&mode);
	GetCurrentVideoMode(&mode);
	unsigned int dwStyle=GetWindowLong(m_hWnd,GWL_STYLE);
	dwStyle&=~(WS_CAPTION|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME|WS_VISIBLE|WS_OVERLAPPED);
	dwStyle|=WS_POPUP;
	SetWindowLong(m_hWnd,GWL_STYLE,dwStyle);
	SetWindowPos(m_hWnd,NULL,mode.fullscreenX,mode.fullscreenY,mode.fullscreenW,mode.fullscreenH,SWP_NOZORDER);
	ShowWindow(m_hWnd,SW_MAXIMIZE);
	return true;
#elif defined LINUX
	
	SVideoMode oldMode;
	GetCurrentVideoMode(&oldMode);// Get mode to get the fullscreen rect.
	
	if(m_XWindow!=None)
	{
		glXMakeCurrent(m_pXDisplay,None,NULL);	  
		XUnmapWindow(m_pXDisplay,m_XWindow);
		XDestroyWindow(m_pXDisplay,m_XWindow);
		m_XWindow=None;
	}
	
	XSetWindowAttributes windowAttribs;
	windowAttribs.colormap = m_pXColorMap;
	windowAttribs.border_pixel = 0;
	windowAttribs.event_mask = X_WINDOWS_EVENT_MASK;
	windowAttribs.override_redirect=true;
	
	m_XWindow = XCreateWindow(m_pXDisplay,RootWindow(m_pXDisplay,m_pXVisualInfo->screen),
					oldMode.fullscreenX,oldMode.fullscreenY,oldMode.fullscreenW,oldMode.fullscreenH, 0, 
					m_pXVisualInfo->depth,InputOutput,m_pXVisualInfo->visual,
					CWBorderPixel|CWColormap|CWEventMask|CWOverrideRedirect,&windowAttribs);
	
	XMapWindow(m_pXDisplay,m_XWindow);
	WaitForXEvent(MapNotify);
	glXMakeCurrent(m_pXDisplay,m_XWindow,m_pGLXContext);
	
	SetVideoMode(&mode);
	GetCurrentVideoMode(&mode);// Get mode to get the fullscreen rect.
	
	XWindowChanges changes;
	changes.width=mode.fullscreenW;
	changes.height=mode.fullscreenH;
	changes.x=mode.fullscreenX;
	changes.y=mode.fullscreenY;
	changes.border_width=0;
	XConfigureWindow(m_pXDisplay,m_XWindow,CWX|CWY|CWWidth|CWHeight|CWBorderWidth,&changes);
	OnMove(changes.x,changes.y);
	OnSize(changes.width,changes.height);
	
	XRaiseWindow(m_pXDisplay,m_XWindow);
	XSetInputFocus(m_pXDisplay,m_XWindow,RevertToPointerRoot,CurrentTime);
	XGrabKeyboard( m_pXDisplay,m_XWindow, True, GrabModeAsync, GrabModeAsync, CurrentTime );
	
	SetupXWindowParameters();
	
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

	unsigned int dwStyle=GetWindowLong(m_hWnd,GWL_STYLE);
	dwStyle|=WS_CAPTION|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME|WS_VISIBLE|WS_OVERLAPPED;
	dwStyle&=~WS_POPUP;
	SetWindowLong(m_hWnd,GWL_STYLE,dwStyle);
	ShowWindow(m_hWnd,SW_SHOWNORMAL);

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
#elif defined LINUX
	
	if(m_pXDisplay!=NULL && m_XWindow!=None)
	{
		SetVideoMode(&m_OriginalVideoMode);
		
		XUngrabKeyboard( m_pXDisplay,CurrentTime);
		glXMakeCurrent(m_pXDisplay,None,NULL);	  
		XUnmapWindow(m_pXDisplay,m_XWindow);
		XDestroyWindow(m_pXDisplay,m_XWindow);
		m_XWindow=None;
	}
	
	XSetWindowAttributes windowAttribs;
	windowAttribs.colormap = m_pXColorMap;
	windowAttribs.border_pixel = 0;
	windowAttribs.event_mask = X_WINDOWS_EVENT_MASK;
	windowAttribs.override_redirect=false;
	
	m_XWindow = XCreateWindow(m_pXDisplay,RootWindow(m_pXDisplay,m_pXVisualInfo->screen),
							 x,y,w,h, 0, 
							  m_pXVisualInfo->depth,InputOutput,m_pXVisualInfo->visual,
							  CWBorderPixel|CWColormap|CWEventMask|CWOverrideRedirect,&windowAttribs);
	
	XMapWindow(m_pXDisplay,m_XWindow);
	WaitForXEvent(MapNotify);
	glXMakeCurrent(m_pXDisplay,m_XWindow,m_pGLXContext);	
	
	XWindowChanges changes;
	changes.width=w;
	changes.height=h;
	changes.x=x;
	changes.y=y;
	changes.border_width=0;
	XConfigureWindow(m_pXDisplay,m_XWindow,CWX|CWY|CWWidth|CWHeight|CWBorderWidth,&changes);
	OnMove(changes.x,changes.y);
	OnSize(changes.width,changes.height);
	
	XRaiseWindow(m_pXDisplay,m_XWindow);
	XSetInputFocus(m_pXDisplay,m_XWindow,RevertToPointerRoot,CurrentTime);
	
	SetupXWindowParameters();
	
	return true;
#endif
}

void 	COpenGLViewport::SetJoystickDeadZone(double dDeadZone){m_dJoystickDeadZone=dDeadZone;}
double  COpenGLViewport::GetJoystickDeadZone(){return m_dJoystickDeadZone;}

#ifdef ANDROID


int32_t COpenGLViewport::OnAndroidInput(struct android_app *pApplication, AInputEvent *pEvent)
{
	COpenGLViewport *pThis=(COpenGLViewport*)pApplication->userData;
	int nType=AInputEvent_getType(pEvent);
	
	if(nType==AINPUT_EVENT_TYPE_MOTION) 
	{
		SAndroidInputEvent event;
		event.nType=nType;
		event.nAction=AMotionEvent_getAction(pEvent);
		event.x=AMotionEvent_getX(pEvent, 0);
		event.y=AMotionEvent_getY(pEvent, 0);
		pThis->m_vAndroidInputEvents.push_back(event);
		return 1;
	}
	else if(nType==AINPUT_EVENT_TYPE_KEY) 
	{
		SAndroidInputEvent event;
		event.nType=nType;
		event.nAction=AKeyEvent_getAction(pEvent);
		event.nGameKey=TranslateKeyFromAndroid(AKeyEvent_getKeyCode(pEvent));
		if(event.nGameKey){pThis->m_vAndroidInputEvents.push_back(event);return 1;}
		return 0;
	}
	return 0;
}

void COpenGLViewport::AndroidCreateRenderContext() 
{
	const EGLint attribs[] = 
	{
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
#ifndef ANDROID_GLES1
		EGL_RENDERABLE_TYPE,   EGL_OPENGL_ES2_BIT,
#endif
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_DEPTH_SIZE, 16,
		EGL_NONE
	};
	EGLint format=0;
	EGLint numConfigs=0;
	EGLConfig config;
	
	bool bOk=false;
	
	m_AndroidDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	bOk=(m_AndroidDisplay!=EGL_NO_DISPLAY);
	if(!bOk){RTTRACE("COpenGLViewport::AndroidCreateRenderContext -> %d available configs",numConfigs);}

	if(bOk)
	{
		eglInitialize(m_AndroidDisplay, 0, 0);
		eglChooseConfig(m_AndroidDisplay, attribs, &config, 1, &numConfigs);
		//RTTRACE("COpenGLViewport::AndroidCreateRenderContext -> %d available configs",numConfigs);
	}
	
	eglGetConfigAttrib(m_AndroidDisplay, config, EGL_NATIVE_VISUAL_ID, &format);
	
	ANativeWindow_setBuffersGeometry(m_pAndroidApp->window, 0, 0, format);

	if(bOk)
	{
		m_AndroidSurface = eglCreateWindowSurface(m_AndroidDisplay, config, m_pAndroidApp->window, NULL);
		bOk=(m_AndroidSurface!=EGL_NO_SURFACE);
		if(!bOk){RTTRACE("COpenGLViewport::AndroidCreateRenderContext -> Failed to create surface");}
	}
	if(bOk)
	{
		const EGLint contextAttribs[] = 
		{
#ifndef ANDROID_GLES1
			EGL_CONTEXT_CLIENT_VERSION, 2,
#endif			
			EGL_NONE
		};
		
		m_AndroidRenderContext = eglCreateContext(m_AndroidDisplay, config, EGL_NO_CONTEXT, contextAttribs);
		bOk=(m_AndroidRenderContext!=EGL_NO_CONTEXT);
		if(!bOk){RTTRACE("COpenGLViewport::AndroidCreateRenderContext -> Failed to create render context");}
	}
	if(bOk)
	{
		bOk=(eglMakeCurrent(m_AndroidDisplay, m_AndroidSurface, m_AndroidSurface, m_AndroidRenderContext) != EGL_FALSE);
		if(!bOk){RTTRACE("COpenGLViewport::AndroidCreateRenderContext -> Failed to set current render context");}
	}
	if(bOk)
	{
		eglQuerySurface(m_AndroidDisplay, m_AndroidSurface, EGL_WIDTH, &m_AndroidWidth);
		eglQuerySurface(m_AndroidDisplay, m_AndroidSurface, EGL_HEIGHT, &m_AndroidHeight);
		
		SetupBasicRenderOptions();
		RTTRACE("COpenGLViewport::InitializeAndroidViewPort -> Viewport initialized %dx%d",m_AndroidWidth,m_AndroidHeight);
		
		OnMove(0,0);
		OnSize(m_AndroidWidth,m_AndroidHeight);
	}
	else
	{
		RTTRACE("COpenGLViewport::InitializeAndroidViewPort -> Failed to initialize viewport");
	}
	
}

void COpenGLViewport::OnAndroidCommand(struct android_app* pApplication, int32_t nCommand) 
{
	COpenGLViewport *pThis=(COpenGLViewport*)pApplication->userData;
	switch (nCommand) 
	{
		case APP_CMD_SAVE_STATE:
			RTTRACE("COpenGLViewport::OnAndroidCommand -> Save state received");
			break;
		case APP_CMD_INIT_WINDOW:
			RTTRACE("COpenGLViewport::OnAndroidCommand -> Init window received");
			if (pThis->m_pAndroidApp->window != NULL) 
			{
				pThis->AndroidCreateRenderContext();
			}
			break;
		case APP_CMD_TERM_WINDOW:
			RTTRACE("COpenGLViewport::OnAndroidCommand -> Terminate window received");
			break;
		case APP_CMD_GAINED_FOCUS:
			RTTRACE("COpenGLViewport::OnAndroidCommand -> Setfocus received");
			break;
		case APP_CMD_LOST_FOCUS:
			RTTRACE("COpenGLViewport::OnAndroidCommand -> KillFocus received");
			break;
	}
}
#endif

