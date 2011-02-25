#include "./stdafx.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <stdarg.h>
#include "PlatformDependent.h"

#ifndef WIN32
#include <sys/time.h>
#include <libgen.h>
#endif

void ReplaceExtension(char *pFileName,const char *pExt)
{
  char *pExtStart=strrchr(pFileName,'.');
  if(pExtStart){strcpy(pExtStart,pExt);}
  else{strcat(pFileName,pExt);}
}

void GetExtension(const char *pFileName,char *pExt)
{
  const char *pExtStart=strrchr(pFileName,'.');
  if(pExtStart){strcpy(pExt,pExtStart);}
  else{*pExt=0;}
}

#ifdef WIN32

void GetFileFolder(const char *pFilePath,char *pFolder)
{
	char sDrive[MAX_PATH]={0};
	char sFolder[MAX_PATH]={0};
	_splitpath(pFilePath,sDrive,sFolder,NULL,NULL);
	strcpy(pFolder,sDrive);
	strcat(pFolder,sFolder);
}

void GetFileName(const char *pFilePath,char *pFileName)
{
	char sName[MAX_PATH]={0};
	char sExt[MAX_PATH]={0};
	_splitpath(pFilePath,NULL,NULL,sName,sExt);
	strcpy(pFileName,sName);
	strcat(pFileName,sExt);
}

void RTTRACE(const char *format, ...)
{
	va_list vargs;
	va_start (vargs,format);
	char pTempBuffer[2400];
	int res=vsnprintf(pTempBuffer,1023,format,vargs);
	if(res==-1){pTempBuffer[1022]=0;res=1022;}
	va_end (vargs);

	pTempBuffer[res]='\n';
	pTempBuffer[res+1]=0;
	OutputDebugString(pTempBuffer);
}

unsigned int GetTimeStamp()
{
	LARGE_INTEGER ldNow={0};
	LARGE_INTEGER ldPerformanceFrequency={0};
	QueryPerformanceFrequency(&ldPerformanceFrequency);
	QueryPerformanceCounter(&ldNow);
	return (unsigned int)(ldNow.QuadPart*1000/ldPerformanceFrequency.QuadPart);
}

#else

void GetFileFolder(const char *pFilePath,char *pFolder)
{
	char *pTemp=strdup(pFilePath);
	strcpy(pFolder,dirname(pTemp));
	free(pTemp);
}

void GetFileName(const char *pFilePath,char *pFileName)
{
	char *pTemp=strdup(pFilePath);
	strcpy(pFileName,basename(pTemp));
	free(pTemp);
}

void RTTRACE(const char *format, ...)
{
	va_list vargs;
	va_start (vargs,format);
	char pTempBuffer[2400];
	int res=vsnprintf(pTempBuffer,1023,format,vargs);
	if(res==-1){pTempBuffer[1022]=0;res=1022;}
	va_end (vargs);

	pTempBuffer[res]='\n';
	pTempBuffer[res+1]=0;
	printf("%s",pTempBuffer);
}

unsigned int GetCurrentTime()
{
	timeval tNow;
	gettimeofday(&tNow, NULL);
	return ((double)tNow.tv_sec)*1000.0+((double)tNow.tv_usec)/1000.0;
}
#endif

