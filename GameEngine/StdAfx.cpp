// stdafx.cpp : source file that includes just the standard includes
//	GameEngine.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#ifdef WIN32
  #ifdef _DEBUG
	  #pragma comment (lib, "..\\VectorLib\\Debug\\VectorLib.lib")
  #else
	  #pragma comment (lib, "..\\VectorLib\\Release\\VectorLib.lib")
  #endif

  #ifdef _DEBUG
	  #pragma comment (lib, "..\\GameLib\\Debug\\GameLib.lib")
  #else
	  #pragma comment (lib, "..\\GameLib\\Release\\GameLib.lib")
  #endif

  #pragma comment (lib, "opengl32.lib")
  #pragma comment (lib, "glu32.lib")
#endif