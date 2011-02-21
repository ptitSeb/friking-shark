// stdafx.cpp : source file that includes just the standard includes
// ParticleSystems.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "./stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
#ifdef WIN32
	#pragma comment (lib, "VectorLib.lib")
	#pragma comment (lib, "GameLib.lib")
	#pragma comment (lib, "GameRuntime.lib")
	#pragma comment (lib, "GameRuntimeLib.lib")
	#pragma comment (lib, "opengl32.lib")
	#pragma comment (lib, "glu32.lib")

	#ifdef _DEBUG
		#pragma comment(lib,"jpgdlib_d.lib")
	#else
		#pragma comment(lib,"jpgdlib.lib")
	#endif
#endif
