#include "./stdafx.h"

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

  #pragma comment (lib, "GameRuntime.lib")
  #pragma comment (lib, "GameRuntimeLib.lib")
#endif