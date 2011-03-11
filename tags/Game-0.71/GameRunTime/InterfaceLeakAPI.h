#ifdef __cplusplus
extern "C" {
#endif

#ifdef GAMERUNTIME_EXPORTS
	#ifdef WIN32 
		#define GAMERUNTIME_API __declspec(dllexport)
	#else
		#define GAMERUNTIME_API 
	#endif
	
#else
	#ifdef WIN32 
		#define GAMERUNTIME_API __declspec(dllimport)
	#else
		#define GAMERUNTIME_API 
	#endif
#ifndef GAMERUNTIMELIB_COMPILATION
//#pragma comment (lib,"GameRunTime.lib")
#endif
#endif


void GAMERUNTIME_API InitMonitorization();
void GAMERUNTIME_API EndMonitorization();

void GAMERUNTIME_API RegisterAddRef(void *pInterface);
void GAMERUNTIME_API RegisterRelease(void *pInterface);
void GAMERUNTIME_API DumpMonitorizationLeaks();
#ifdef __cplusplus
}
#endif
