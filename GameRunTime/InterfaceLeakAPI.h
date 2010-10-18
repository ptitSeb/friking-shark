#ifdef __cplusplus
extern "C" {
#endif
#ifndef GAMERUNTIME_API 
#ifdef GAMERUNTIME_EXPORTS
#define GAMERUNTIME_API __declspec(dllexport)
#else
#define GAMERUNTIME_API __declspec(dllimport)
#ifndef GAMERUNTIMELIB_COMPILATION
#endif
#endif
#endif


void GAMERUNTIME_API InitMonitorization();
void GAMERUNTIME_API EndMonitorization();

void GAMERUNTIME_API RegisterAddRef(void *pInterface);
void GAMERUNTIME_API RegisterRelease(void *pInterface);
void GAMERUNTIME_API DumpMonitorizationLeaks();
#ifdef __cplusplus
};
#endif
