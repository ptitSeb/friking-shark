// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#ifndef __STDAFX__
#define __STDAFX__

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define GAME_LIB_PROYECT

#define GL_GLEXT_PROTOTYPES
#include <windows.h>
#include <crtdbg.h>
#include <GL/gl.h>
#include <GL/glext.h>
#ifdef WIN32
#include <GL/glu.h>
#else
#include <GL/glut.h>
#endif

#endif 
