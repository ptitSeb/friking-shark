#include "./stdafx.h"

#ifdef WIN32
  #ifdef _DEBUG
	  #pragma comment (lib, "..\\VectorLib\\Debug\\VectorLib.lib")
  #else
	  #pragma comment (lib, "..\\VectorLib\\Release\\VectorLib.lib")
  #endif

  #pragma comment (lib, "GameRuntime.lib")
  #pragma comment (lib, "GameRuntimeLib.lib")
#endif