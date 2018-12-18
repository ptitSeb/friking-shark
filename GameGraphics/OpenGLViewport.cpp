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
  #define VIEWPORT_CLASSNAME "OpenGLViewport"
  #define WM_GL_VIEWPORT_END_LOOP WM_USER+0x001
#elif defined(USE_SDL2)
	#define DETECT_DRAG_SIZE 3
#else
	#include <X11/extensions/Xrandr.h>
	#include <X11/extensions/Xinerama.h>
	#define DETECT_DRAG_SIZE 3
	#define X_WINDOWS_EVENT_MASK ExposureMask|ButtonPressMask|ButtonReleaseMask|PointerMotionMask|KeyPressMask|KeyReleaseMask|StructureNotifyMask
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
#elif defined(USE_SDL2)
#define Button1 1
int TranslateKeyFromSDL(int nSDLKey)
{
	switch(nSDLKey)
	{
	case SDL_SCANCODE_SPACE:return ' ';
	case SDL_SCANCODE_0:return '0';
	case SDL_SCANCODE_1:return '1';
	case SDL_SCANCODE_2:return '2';
	case SDL_SCANCODE_3:return '3';
	case SDL_SCANCODE_4:return '4';
	case SDL_SCANCODE_5:return '5';
	case SDL_SCANCODE_6:return '6';
	case SDL_SCANCODE_7:return '7';
	case SDL_SCANCODE_8:return '8';
	case SDL_SCANCODE_9:return '9';
	case SDL_SCANCODE_A:return 'A';
	case SDL_SCANCODE_B:return 'B';
	case SDL_SCANCODE_C:return 'C';
	case SDL_SCANCODE_D:return 'D';
	case SDL_SCANCODE_E:return 'E';
	case SDL_SCANCODE_F:return 'F';
	case SDL_SCANCODE_G:return 'G';
	case SDL_SCANCODE_H:return 'H';
	case SDL_SCANCODE_I:return 'I';
	case SDL_SCANCODE_J:return 'J';
	case SDL_SCANCODE_K:return 'K';
	case SDL_SCANCODE_L:return 'L';
	case SDL_SCANCODE_M:return 'M';
	case SDL_SCANCODE_N:return 'N';
	case SDL_SCANCODE_O:return 'O';
	case SDL_SCANCODE_P:return 'P';
	case SDL_SCANCODE_Q:return 'Q';
	case SDL_SCANCODE_R:return 'R';
	case SDL_SCANCODE_S:return 'S';
	case SDL_SCANCODE_T:return 'T';
	case SDL_SCANCODE_U:return 'U';
	case SDL_SCANCODE_V:return 'V';
	case SDL_SCANCODE_W:return 'W';
	case SDL_SCANCODE_X:return 'X';
	case SDL_SCANCODE_Y:return 'Y';
	case SDL_SCANCODE_Z:return 'Z';
	case SDL_SCANCODE_PERIOD:return '.';
	case SDL_SCANCODE_SEMICOLON:return ';';
	case SDL_SCANCODE_SLASH:return '/';
	case SDL_SCANCODE_TAB:return '\t';
	case SDL_SCANCODE_UP:return GK_UP;
	case SDL_SCANCODE_DOWN:return GK_DOWN;
	case SDL_SCANCODE_LEFT:return GK_LEFT;
	case SDL_SCANCODE_RIGHT:return GK_RIGHT;
	case SDL_SCANCODE_KP_0:return GK_NUMPAD0;
	case SDL_SCANCODE_KP_1:return GK_NUMPAD1;
	case SDL_SCANCODE_KP_2:return GK_NUMPAD2;
	case SDL_SCANCODE_KP_3:return GK_NUMPAD3;
	case SDL_SCANCODE_KP_4:return GK_NUMPAD4;
	case SDL_SCANCODE_KP_5:return GK_NUMPAD5;
	case SDL_SCANCODE_KP_6:return GK_NUMPAD6;
	case SDL_SCANCODE_KP_7:return GK_NUMPAD7;
	case SDL_SCANCODE_KP_8:return GK_NUMPAD8;
	case SDL_SCANCODE_KP_9:return GK_NUMPAD9;
	case SDL_SCANCODE_LCTRL:return GK_LCONTROL;
	case SDL_SCANCODE_HOME:return GK_HOME;
	case SDL_SCANCODE_END:return GK_END;
	case SDL_SCANCODE_BACKSPACE:return GK_BACK;
	case SDL_SCANCODE_DELETE:return GK_DELETE;
	case SDL_SCANCODE_ESCAPE:return GK_ESCAPE;
	case SDL_SCANCODE_RETURN:return GK_RETURN;
	case SDL_SCANCODE_LALT:return GK_LMENU;
	case SDL_SCANCODE_LSHIFT:return GK_LSHIFT;
	case SDL_SCANCODE_INSERT:return GK_INSERT;
	case SDL_SCANCODE_F1:return GK_F1;
	case SDL_SCANCODE_F2:return GK_F2;
	case SDL_SCANCODE_F3:return GK_F3;
	case SDL_SCANCODE_F4:return GK_F4;
	case SDL_SCANCODE_F5:return GK_F5;
	case SDL_SCANCODE_F6:return GK_F6;
	case SDL_SCANCODE_F7:return GK_F7;
	case SDL_SCANCODE_F8:return GK_F8;
	case SDL_SCANCODE_F9:return GK_F9;
	case SDL_SCANCODE_F10:return GK_F10;
	case SDL_SCANCODE_F11:return GK_F11;
	case SDL_SCANCODE_F12:return GK_F12;
	case SDL_SCANCODE_PAUSE:return GK_PAUSE;
	case SDL_SCANCODE_PAGEUP:return GK_PAGEUP;
	case SDL_SCANCODE_PAGEDOWN:return GK_PAGEDOWN;
	case SDL_SCANCODE_RCTRL:return GK_RCONTROL;
	case SDL_SCANCODE_RALT:return GK_RMENU;
	case SDL_SCANCODE_RSHIFT:return GK_RSHIFT;
	case SDL_SCANCODE_F13:return GK_F13;
	case SDL_SCANCODE_F14:return GK_F14;
	case SDL_SCANCODE_F15:return GK_F15;
	case SDL_SCANCODE_F16:return GK_F16;
	case SDL_SCANCODE_F17:return GK_F17;
	case SDL_SCANCODE_F18:return GK_F18;
	case SDL_SCANCODE_F19:return GK_F19;
	case SDL_SCANCODE_F20:return GK_F20;
	case SDL_SCANCODE_F21:return GK_F21;
	case SDL_SCANCODE_F22:return GK_F22;
	case SDL_SCANCODE_F23:return GK_F23;
	case SDL_SCANCODE_F24:return GK_F24;
	case SDL_SCANCODE_CLEAR:return GK_CLEAR;
	case SDL_SCANCODE_CAPSLOCK:return GK_CAPITAL;
	case SDL_SCANCODE_SELECT:return GK_SELECT;
	case SDL_SCANCODE_PRINTSCREEN:return GK_PRINT;
	case SDL_SCANCODE_EXECUTE:return GK_EXECUTE;
	case SDL_SCANCODE_HELP:return GK_HELP;
	case SDL_SCANCODE_LGUI:return GK_LWIN;
	case SDL_SCANCODE_RGUI:return GK_RWIN;
	case SDL_SCANCODE_KP_MULTIPLY:return GK_MULTIPLY;
	case SDL_SCANCODE_KP_PLUS:return GK_ADD;
	case SDL_SCANCODE_KP_PERIOD:return GK_SEPARATOR;
	case SDL_SCANCODE_KP_LESS:return GK_SUBTRACT;
	case SDL_SCANCODE_KP_DECIMAL:return GK_DECIMAL;
	case SDL_SCANCODE_KP_DIVIDE:return GK_DIVIDE;
	case SDL_SCANCODE_NUMLOCKCLEAR:return GK_NUMLOCK;
	case SDL_SCANCODE_SCROLLLOCK:return GK_SCROLL;
	case SDL_SCANCODE_MENU:return GK_MENU;
	};
	return 0;
}

int TranslateKeyToSDL(int nGameKey)
{
	switch(nGameKey)
	{
	case ' ':return SDL_SCANCODE_SPACE;
	case '0':return SDL_SCANCODE_0;
	case '1':return SDL_SCANCODE_1;
	case '2':return SDL_SCANCODE_2;
	case '3':return SDL_SCANCODE_3;
	case '4':return SDL_SCANCODE_4;
	case '5':return SDL_SCANCODE_5;
	case '6':return SDL_SCANCODE_6;
	case '7':return SDL_SCANCODE_7;
	case '8':return SDL_SCANCODE_8;
	case '9':return SDL_SCANCODE_9;
	case 'A':return SDL_SCANCODE_A;
	case 'B':return SDL_SCANCODE_B;
	case 'C':return SDL_SCANCODE_C;
	case 'D':return SDL_SCANCODE_D;
	case 'E':return SDL_SCANCODE_E;
	case 'F':return SDL_SCANCODE_F;
	case 'G':return SDL_SCANCODE_G;
	case 'H':return SDL_SCANCODE_H;
	case 'I':return SDL_SCANCODE_I;
	case 'J':return SDL_SCANCODE_J;
	case 'K':return SDL_SCANCODE_K;
	case 'L':return SDL_SCANCODE_L;
	case 'M':return SDL_SCANCODE_M;
	case 'N':return SDL_SCANCODE_N;
	case 'O':return SDL_SCANCODE_O;
	case 'P':return SDL_SCANCODE_P;
	case 'Q':return SDL_SCANCODE_Q;
	case 'R':return SDL_SCANCODE_R;
	case 'S':return SDL_SCANCODE_S;
	case 'T':return SDL_SCANCODE_T;
	case 'U':return SDL_SCANCODE_U;
	case 'V':return SDL_SCANCODE_V;
	case 'W':return SDL_SCANCODE_W;
	case 'X':return SDL_SCANCODE_X;
	case 'Y':return SDL_SCANCODE_Y;
	case 'Z':return SDL_SCANCODE_Z;
	case '.':return SDL_SCANCODE_PERIOD;
	case ';':return SDL_SCANCODE_SEMICOLON;
	case '/':return SDL_SCANCODE_SLASH;
	case '\t':return SDL_SCANCODE_TAB;
	case GK_UP:return SDL_SCANCODE_UP;
	case GK_DOWN:return SDL_SCANCODE_DOWN;
	case GK_LEFT:return SDL_SCANCODE_LEFT;
	case GK_RIGHT:return SDL_SCANCODE_RIGHT;
	case GK_NUMPAD0:return SDL_SCANCODE_KP_0;
	case GK_NUMPAD1:return SDL_SCANCODE_KP_1;
	case GK_NUMPAD2:return SDL_SCANCODE_KP_2;
	case GK_NUMPAD3:return SDL_SCANCODE_KP_3;
	case GK_NUMPAD4:return SDL_SCANCODE_KP_4;
	case GK_NUMPAD5:return SDL_SCANCODE_KP_5;
	case GK_NUMPAD6:return SDL_SCANCODE_KP_6;
	case GK_NUMPAD7:return SDL_SCANCODE_KP_7;
	case GK_NUMPAD8:return SDL_SCANCODE_KP_8;
	case GK_NUMPAD9:return SDL_SCANCODE_KP_9;
	case GK_LCONTROL:return SDL_SCANCODE_LCTRL;
	case GK_HOME:return SDL_SCANCODE_HOME;
	case GK_END:return SDL_SCANCODE_END;
	case GK_BACK:return SDL_SCANCODE_BACKSPACE;
	case GK_DELETE:return SDL_SCANCODE_DELETE;
	case GK_ESCAPE:return SDL_SCANCODE_ESCAPE;
	case GK_RETURN:return SDL_SCANCODE_RETURN;
	case GK_LMENU:return SDL_SCANCODE_LALT;
	case GK_LSHIFT:return SDL_SCANCODE_LSHIFT;
	case GK_INSERT:return SDL_SCANCODE_INSERT;
	case GK_F1:return SDL_SCANCODE_F1;
	case GK_F2:return SDL_SCANCODE_F2;
	case GK_F3:return SDL_SCANCODE_F3;
	case GK_F4:return SDL_SCANCODE_F4;
	case GK_F5:return SDL_SCANCODE_F5;
	case GK_F6:return SDL_SCANCODE_F6;
	case GK_F7:return SDL_SCANCODE_F7;
	case GK_F8:return SDL_SCANCODE_F8;
	case GK_F9:return SDL_SCANCODE_F9;
	case GK_F10:return SDL_SCANCODE_F10;
	case GK_F11:return SDL_SCANCODE_F11;
	case GK_F12:return SDL_SCANCODE_F12;
	case GK_PAUSE:return SDL_SCANCODE_PAUSE;
	case GK_PAGEUP:return SDL_SCANCODE_PAGEUP;
	case GK_PAGEDOWN:return SDL_SCANCODE_PAGEDOWN;
	
	case GK_RCONTROL: return SDL_SCANCODE_RCTRL;
	case GK_RMENU: return SDL_SCANCODE_RALT;
	case GK_RSHIFT: return SDL_SCANCODE_RSHIFT;
	case GK_F13: return SDL_SCANCODE_F13;
	case GK_F14: return SDL_SCANCODE_F14;
	case GK_F15: return SDL_SCANCODE_F15;
	case GK_F16: return SDL_SCANCODE_F16;
	case GK_F17: return SDL_SCANCODE_F17;
	case GK_F18: return SDL_SCANCODE_F18;
	case GK_F19: return SDL_SCANCODE_F19;
	case GK_F20: return SDL_SCANCODE_F20;
	case GK_F21: return SDL_SCANCODE_F21;
	case GK_F22: return SDL_SCANCODE_F22;
	case GK_F23: return SDL_SCANCODE_F23;
	case GK_F24: return SDL_SCANCODE_F24;
	case GK_LBUTTON: return 0; //???
	case GK_RBUTTON: return 0; //???
	case GK_MBUTTON: return 0; //???
	case GK_CLEAR: return SDL_SCANCODE_CLEAR;
	case GK_CAPITAL: return SDL_SCANCODE_CAPSLOCK;
	case GK_SELECT: return SDL_SCANCODE_SELECT;
	case GK_PRINT: return SDL_SCANCODE_PRINTSCREEN;
	case GK_EXECUTE: return SDL_SCANCODE_EXECUTE;
	case GK_HELP: return SDL_SCANCODE_HELP;
	case GK_LWIN: return SDL_SCANCODE_LGUI;
	case GK_RWIN: return SDL_SCANCODE_RGUI;
	case GK_MULTIPLY: return SDL_SCANCODE_KP_MULTIPLY;
	case GK_ADD: return SDL_SCANCODE_KP_PLUS;
	case GK_SEPARATOR: return SDL_SCANCODE_KP_PERIOD;
	case GK_SUBTRACT: return SDL_SCANCODE_KP_LESS;
	case GK_DECIMAL: return SDL_SCANCODE_KP_DECIMAL;
	case GK_DIVIDE: return SDL_SCANCODE_KP_DIVIDE;
	case GK_NUMLOCK: return SDL_SCANCODE_NUMLOCKCLEAR;
	case GK_SCROLL: return SDL_SCANCODE_SCROLLLOCK;
	case GK_MENU: return SDL_SCANCODE_MENU;
	case GK_ALTGR: return SDL_SCANCODE_RALT;
	};
	return 0;
}
#else

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

#endif

COpenGLViewport::COpenGLViewport(void)
{
	m_piCallBack=NULL;
	m_bVerticalSync=false;

#ifdef WIN32
	m_nLastMouseMoveX=-100000;
	m_nLastMouseMoveY=-100000;
	m_hDC=NULL;
	m_hWnd=NULL;
	m_hRenderContext=NULL;
	m_nPixelFormatIndex=0;
#elif defined(USE_SDL2)
	m_pWindow = NULL;
	m_pContext = NULL;
	m_nLoopDepth=0;
	m_bSDLExit=false;

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
#else
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
#endif
	
	GetCurrentVideoMode(&m_OriginalVideoMode);
	
	InitializeKeyNames();
}

COpenGLViewport::~COpenGLViewport(void)
{
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
#ifdef AMIGAOS4
	m_mKeyNames[GK_LCONTROL]="Control";
	m_mKeyNames[GK_RCONTROL]="Control";
#else
	m_mKeyNames[GK_LCONTROL]="Left Control";
	m_mKeyNames[GK_RCONTROL]="Right Control";
#endif
	m_mKeyNames[GK_LMENU]="Left Alt";
	m_mKeyNames[GK_RMENU]="Right Alt";
	m_mKeyNames[GK_MENU]="Menu";
	m_mKeyNames[GK_ALTGR]="Alt Gr";
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

bool COpenGLViewport::CreateWindowed(unsigned int x, unsigned int y, unsigned int w, unsigned int h)
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
	m_hWnd = CreateWindowEx(WS_EX_DLGMODALFRAME,VIEWPORT_CLASSNAME,"Loading...",dwStyle,x,y,w,h,NULL,NULL,NULL,(void *)this);
	if(m_hWnd)
	{
		unsigned int dwStyle=GetWindowLong(m_hWnd,GWL_STYLE);
		SetWindowLong(m_hWnd,GWL_STYLE,dwStyle);
		ShowWindow(m_hWnd,SW_SHOW);
		EnableWindow(m_hWnd,TRUE);
		UpdateWindow(m_hWnd);
	}
	bOk=(m_hRenderContext!=NULL);
	if(!bOk){RTTRACE("COpenGLViewport::Create -> Failed to get OpenGL render context");}
#elif defined(USE_SDL2)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 8);
    m_pWindow = SDL_CreateWindow("Friking Shark", x, y, w, h, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	bOk=(m_pWindow!=NULL);
	if(!bOk){RTTRACE("COpenGLViewport::Create -> Failed to create SDL Window"); return bOk;}
    m_pContext = SDL_GL_CreateContext(m_pWindow);
	bOk=(m_pContext!=NULL);
	if(!bOk){RTTRACE("COpenGLViewport::Create -> Failed to create OpenGL Context"); return bOk;}
#else
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
	}
	if(!bOk){RTTRACE("COpenGLViewport::Create -> Failed to get OpenGL render context");}
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
	if(!bOk){RTTRACE("COpenGLViewport::Create -> Failed to get OpenGL render context");}
#elif defined(USE_SDL2)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 8);
    m_pWindow = SDL_CreateWindow("Friking Shark", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);
	bOk=(m_pWindow!=NULL);
	if(!bOk){RTTRACE("COpenGLViewport::Create -> Failed to create SDL Window"); return bOk;}
    m_pContext = SDL_GL_CreateContext(m_pWindow);
	bOk=(m_pContext!=NULL);
	if(!bOk){RTTRACE("COpenGLViewport::Create -> Failed to create OpenGL Context"); return bOk;}
#else
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
	}
	if(!bOk){RTTRACE("COpenGLViewport::Create -> Failed to get OpenGL render context");}
#endif
	return bOk;
}

void COpenGLViewport::Destroy()
{
 
#ifdef WIN32
	if(m_hWnd){DestroyWindow(m_hWnd);m_hWnd=NULL;}
	SetVideoMode(&m_OriginalVideoMode);
#elif defined(USE_SDL2)
	SDL_GL_DeleteContext(m_pContext);
	SDL_DestroyWindow(m_pWindow);
	m_pContext = NULL;
	m_pWindow = NULL;
#else
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
#endif	
	
	CSystemObjectBase::Destroy();
}

void COpenGLViewport::GetSize(unsigned int *pdwWidth,unsigned int *pdwHeight)
{
#ifdef WIN32
	RECT R={0};
	GetClientRect(m_hWnd,&R);
	*pdwWidth=R.right-R.left;
	*pdwHeight=R.bottom-R.top;
#elif defined(USE_SDL2)
	if(m_pWindow)
		SDL_GetWindowSize(m_pWindow, (int*)pdwWidth, (int*)pdwHeight);
	else
	{
		*pdwWidth=0;
		*pdwHeight=0;
	}
#else
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
#endif
}

void COpenGLViewport::SetCallBack(IGenericViewportCallBack *pCallBack)
{
	m_piCallBack=pCallBack;
}

void COpenGLViewport::Render()
{
	glFrontFace(GL_CCW);
	glEnable(GL_POINT_SMOOTH);
	glDisable(GL_POLYGON_SMOOTH);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glCullFace(GL_BACK);
	glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
	//glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	glPolygonMode(GL_FRONT,GL_FILL);

	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	if(m_piCallBack){m_piCallBack->OnRender();}
	
	glFlush();
	glFinish();
	
#ifdef WIN32
	SwapBuffers(m_hDC);
#elif defined(USE_SDL2)
	SDL_GL_SwapWindow(m_pWindow);
#else
	glXSwapBuffers(m_pXDisplay,m_XWindow);
#endif
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
#elif defined(USE_SDL2)
	SDL_GL_SetSwapInterval(m_bVerticalSync);
#else
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
#elif defined(USE_SDL2)
	if(m_pWindow)
		SDL_SetWindowTitle(m_pWindow, m_sCaption.c_str());
#else
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
#elif defined(USE_SDL2)
	SDL_Event event;
	int nLoopId=++m_nLoopDepth;

	while(m_pWindow!=NULL && m_nLoopDepth>=nLoopId && !m_bSDLExit)
	{
		while(m_pWindow!=NULL && SDL_PollEvent(&event))
		{
			bool bBreakLoop=false;
			ProcessSDLEvent(event,&bBreakLoop);
			if(bBreakLoop){return;}
		}
		SDL_GL_MakeCurrent(m_pWindow, m_pContext);
		Render();
	}
#else
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
#elif defined(USE_SDL2)
	if(m_pWindow)
	{
		SDL_GetMouseState(pX, pY);
	}
	else
	{
		*pX=0;
		*pY=0;
	}
#else
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
#elif defined(USE_SDL2)
	if(m_pWindow){SDL_WarpMouseInWindow(m_pWindow, x, y);}
#else
	if(m_XWindow!=None){XWarpPointer(m_pXDisplay,None,m_XWindow,0,0,0,0,x,y);}
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
#ifdef WIN32
	if(m_hWnd!=GetActiveWindow())
	{
		return false;
	}
	USHORT nKeyState=GetKeyState(TranslateKeyToWindows(nKey));
	return (nKeyState&0x8000)!=0;
#elif defined(USE_SDL2)
	if(m_pWindow!=NULL)
	{
		const Uint8 *state = SDL_GetKeyboardState(NULL);
		int k = TranslateKeyToSDL(nKey);
		return state[k];
	}
	return false;
#else
	if(m_pXDisplay!=None)
	{
		char keys[32];
		XQueryKeymap(m_pXDisplay,keys);
		KeyCode nKeyCode=XKeysymToKeycode(m_pXDisplay,TranslateKeyToX11(nKey));
#ifdef PANDORA
		// Hugly hack to make Pandora buttons works
		if (nKey == GK_PAGEDOWN) nKeyCode = 117;
		else if (nKey == GK_PAGEUP) nKeyCode = 112;
		else if (nKey == GK_HOME) nKeyCode = 110;
		else if (nKey == GK_END) nKeyCode = 115;
#endif
		if(nKeyCode<sizeof(keys)*8)
		{  
		  if(keys[nKeyCode>>3]&(1<<(nKeyCode&0x7)))
		  {
			return true;
		  }
		}
	}
	return false;
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
#ifndef WIN32
#ifdef USE_SDL2
	if(m_pWindow)
		SDL_ShowCursor(bShow);
#else
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
#else
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
#elif defined(USE_SDL2)
	SDL_DisplayMode dm;
	if (SDL_GetDesktopDisplayMode(0, &dm) == 0) {
		pMode->fullscreenX=0;
		pMode->fullscreenY=0;
		pMode->w = pMode->fullscreenW=dm.w;
		pMode->h = pMode->fullscreenH=dm.h;
		pMode->rate=0;
		pMode->bpp=SDL_BITSPERPIXEL(dm.format);
	}
#else
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
#elif defined(USE_SDL2)
	SDL_DisplayMode dm={0};
	dm.w = pMode->w;
	dm.h = pMode->h;
	dm.refresh_rate = pMode->rate;
	// set bpp?

	return (SDL_SetWindowDisplayMode(m_pWindow, &dm)==0);
#else
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

#ifndef WIN32
#ifdef USE_SDL2
bool COpenGLViewport::WaitForSDLEvent(int nEventType)
{
	SDL_Event event;
	do
	{
		SDL_WaitEvent(&event);
		{
			bool bBreakLoop=false;
			ProcessSDLEvent(event,&bBreakLoop);
		}
	}while(event.type!=nEventType);
}

void COpenGLViewport::ProcessSDLEvent(SDL_Event &event,bool *pbBreakLoop)
{
	(*pbBreakLoop)=false;
	
	if (event.type==SDL_KEYDOWN) 
	{
		OnKeyDown(TranslateKeyFromSDL(event.key.keysym.scancode));
	}
	else if (event.type==SDL_KEYUP) 
	{
		OnKeyUp(TranslateKeyFromSDL(event.key.keysym.scancode));
	}
	else if (event.type==SDL_TEXTINPUT)
	{
		char c = 0;
		int i =0 ;
		while((c=event.text.text[i++]))
			OnCharacter(c);
	}
	else if (event.type==SDL_MOUSEBUTTONDOWN) 
	{
		int nCurrentTime=GetTimeStamp();
		
		// Regular click processing
		if(event.button.button==SDL_BUTTON_LEFT){OnLButtonDown(event.button.x,event.button.y);}
		else if(event.button.button==SDL_BUTTON_RIGHT){OnRButtonDown(event.button.x,event.button.y);}
		// Double click Detection
		if((event.button.button==SDL_BUTTON_LEFT || event.button.button==SDL_BUTTON_RIGHT) &&
			m_nDblClkDetectLastButton==event.button.button && 
			(nCurrentTime-m_nDblClkDetectLastTime)<=m_nDblClkDetectMilliseconds &&
			fabs(m_nDblClkDetectLastX-event.button.x)<=m_nDblClkDetectDistance &&
			fabs(m_nDblClkDetectLastY-event.button.y)<=m_nDblClkDetectDistance)
		{
			m_nDblClkDetectLastButton=0;
			m_nDblClkDetectLastX=0;
			m_nDblClkDetectLastY=0;
			m_nDblClkDetectLastTime=0;
			
			if(event.button.button==SDL_BUTTON_LEFT){OnLButtonDoubleClick(event.button.x,event.button.y);}
			else if(event.button.button==SDL_BUTTON_RIGHT){OnRButtonDoubleClick(event.button.x,event.button.y);}
		}
		else
		{
			m_nDblClkDetectLastButton=event.button.button;
			m_nDblClkDetectLastX=event.button.x;
			m_nDblClkDetectLastY=event.button.y;
			m_nDblClkDetectLastTime=nCurrentTime;
		}
	}
	else if (event.type==SDL_MOUSEBUTTONUP) 
	{
		if(event.button.button==SDL_BUTTON_LEFT){OnLButtonUp(event.button.x,event.button.y);}
		else if(event.button.button==SDL_BUTTON_RIGHT){OnRButtonUp(event.button.x,event.button.y);}
		
		if(m_nDetectDragButton==event.button.button)
		{
			m_nLoopDepth--;
			*pbBreakLoop=true;
			return;
		}			  
	}
	else if (event.type==SDL_MOUSEWHEEL)
	{
		int mx,my;
		SDL_GetMouseState(&mx, &my);
		if(event.wheel.y<0){OnMouseWheelUp(mx,my);}
		else if(event.wheel.y>0){OnMouseWheelDown(mx,my);}
	}
	else if (event.type==SDL_MOUSEMOTION) 
	{
		OnMouseMove(event.motion.x,event.motion.y);
		
		if(m_nDetectDragButton!=0)
		{
			int nXDist=event.motion.x-m_nDetectDragX;
			int nYDist=event.motion.y-m_nDetectDragY;
			
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
	else if (event.type==SDL_QUIT) 
	{
		m_bSDLExit=true;
	}
	*pbBreakLoop=false;
	return;
}

void COpenGLViewport::SetupSDLWindowParameters()
{
}
#else
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
#elif defined(USE_SDL2)
	if(m_pWindow)
		SDL_SetWindowFullscreen(m_pWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
	return true;
#else
	
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
	// Las coordenadas y tama�os que se gestion siempre son del area cliente.
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
#elif defined(USE_SDL2)
	if(m_pWindow)
		SDL_SetWindowFullscreen(m_pWindow, 0);
	return true;
#else
	
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

