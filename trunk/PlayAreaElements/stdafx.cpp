#include "./stdafx.h"

#ifdef WIN32
#pragma comment (lib, "VectorLib.lib")
#pragma comment (lib, "GameLib.lib")
#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")
#endif

CSingletonWrapper<IEntityManager> g_EntityManagerWrapper("GameSystem","EntityManager");
CSingletonWrapper<IPlayAreaManager> g_PlayAreaManagerWrapper("GameSystem","PlayAreaManager");
CSingletonWrapper<IFrameManager>  g_FrameManagerSingleton("GameSystem","FrameManager");
