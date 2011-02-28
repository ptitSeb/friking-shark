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
#include <glob.h>
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

std::string AppendPathSeparator(std::string sFile)
{
	int nLen=sFile.length();
	if(nLen && sFile[nLen-1]!=PATH_SEPARATOR_CHAR){sFile+=PATH_SEPARATOR;}
	return sFile;
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


bool FindFiles(const char *psPattern, EFindFilesMode eMode,std::set<std::string> *psFiles)
{
	char sFolder[MAX_PATH];
	GetFileFolder(psPattern,sFolder);

	WIN32_FIND_DATA FileData;
	HANDLE hFind = FindFirstFile(psPattern, &FileData);
	if(hFind != INVALID_HANDLE_VALUE) 
	{
		do
		{
			std::string sFile=AppendPathSeparator(sFolder)+FileData.cFileName;
			bool bDirectory=((GetFileAttributes(sFile.c_str())&FILE_ATTRIBUTE_DIRECTORY)!=0);
			switch(eMode)
			{
			case eFindFilesMode_OnlyFiles:	if(!bDirectory){psFiles->insert(sFile);};break;
			case eFindFilesMode_OnlyDirs:	psFiles->insert(sFile);break;
			case eFindFilesMode_DirsAndFiles:psFiles->insert(sFile);break;
			}
		}
		while(FindNextFile(hFind, &FileData));

		FindClose(hFind);
		hFind=INVALID_HANDLE_VALUE;
	}
	return true;
}

std::string GetWorkingFolder()
{
	char sCurrentPath[MAX_PATH]={0};
	GetCurrentDirectory(MAX_PATH,sCurrentPath);
	return sCurrentPath;
}

bool SetWorkingFolder(std::string sFolder)
{
	return (SetCurrentDirectory(sFolder.c_str())==TRUE);
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

unsigned int GetTimeStamp()
{
	timeval tNow;
	gettimeofday(&tNow, NULL);
	return ((double)tNow.tv_sec)*1000.0+((double)tNow.tv_usec)/1000.0;
}

bool FindFiles(const char *psPattern, EFindFilesMode eMode,std::set<std::string> *psFiles);
{
	glob_t globbuf={0};
	int nFlags=GLOB_MARK;
	nFlags|=((eMode==eFindFilesMode_OnlyDirs)?GLOB_ONLYDIR:0);
	glob(psPattern,nFlags,NULL,&globbuf);
	//liberamos la memoria	
	for (int i=0; i <globbuf.gl_pathc; i++)
	{
		const char *pFile=globbuf.gl_pathv[i];
		if(pFile[0]==0){continue;}
		bool bDirectory=(pFile[strlen(pFile)-1]==PATH_SEPARATOR_CHAR);
		switch(eMode)
		{
		case eFindFilesMode_OnlyFiles:	if(!bDirectory){psFiles->insert(pFile);};break;
		case eFindFilesMode_OnlyDirs:	psFiles->insert(pFile);break;
		case eFindFilesMode_DirsAndFiles:psFiles->insert(pFile);break;
		}
	}
	globfree(&globbuf);
	return true;
}

std::string GetWorkingFolder()
{
	char sCurrentPath[MAX_PATH]={0};
	if(!getcwd(sCurrentPath,MAX_PATH)){sCurrentPath[0]=0;}
	return sCurrentPath;
}

bool SetWorkingFolder(std::string sFolder)
{
	return (chdir(sFolder.c_str())==0);
}
#endif

