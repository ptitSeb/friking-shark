#include "./stdafx.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <stdarg.h>
#include <deque>
#include "PlatformDependent.h"

#ifndef WIN32
#include <sys/stat.h>
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

std::string NormalizePath(std::string sPath)
{
#ifdef WIN32
	char sDrive[MAX_PATH]={0},sFolder[MAX_PATH]={0},sFile[MAX_PATH]={0},sExt[MAX_PATH]={0};
	_splitpath(sPath.c_str(),sDrive,sFolder,sFile,sExt);
	std::string sNormalized=sDrive;
	std::string sFileName=sFile;
	sFileName+=sExt;
	sNormalized+=PATH_SEPARATOR;
#else
	std::string sFileName=GetFileName(sPath);
	GetFileFolder(sPath.c_str(),sFolder);
#endif

	std::deque<std::string> sFolders;
	char *pContext=NULL;
	char *pToken=strtok_r(sFolder,PATH_SEPARATOR,&pContext);
	while(pToken)
	{
		if(strcmp(pToken,".")==0){continue;}
		else if(strcmp(pToken,"..")==0){if(sFolders.size()){sFolders.pop_back();}}
		else {sFolders.push_back(pToken);}

		pToken=strtok_r(NULL,PATH_SEPARATOR,&pContext);
	}
	std::deque<std::string>::iterator i;
	for(i=sFolders.begin();i!=sFolders.end();i++)
	{
		sNormalized+=*i;
		sNormalized+=PATH_SEPARATOR;
	}
	sNormalized+=sFileName;
	return sNormalized;
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
	char sFile[MAX_PATH]={0};
	char sExt[MAX_PATH]={0};
	_splitpath(pFilePath,NULL,NULL,sFile,sExt);
	strcpy(pFileName,sFile);
	strcat(pFileName,sExt);
}

std::string GetFileFolder(std::string sFilePath)
{
	std::string sFileFolder;
	char sDrive[MAX_PATH]={0};
	char sFolder[MAX_PATH]={0};

	// SplitPath devuelve Folder "C:\Temp\" para C:\Temp\ en lugar de C:\
	// Para evitarlo se le quita la barra final
	if(sFilePath.length() && sFilePath.at(sFilePath.length()-1)==PATH_SEPARATOR_CHAR)
	{
		sFilePath.replace(sFilePath.length()-1,1,"");
	}

	_splitpath(sFilePath.c_str(),sDrive,sFolder,NULL,NULL);
	sFileFolder=sDrive;
	sFileFolder+=sFolder;
	return sFileFolder;
}

std::string GetFileName(std::string sFilePath)
{
	std::string sFileName;
	char sFile[MAX_PATH]={0};
	char sExt[MAX_PATH]={0};
	// SplitPath devuelve FileName "" para c:\Temp\ en lugar de Temp
	// Para evitarlo se le quita la barra final
	bool bSeparatorRemoved=false;
	std::string sTempPath=sFilePath;
	if(sFilePath.length() && sFilePath.at(sFilePath.length()-1)==PATH_SEPARATOR_CHAR)
	{
		sFilePath.replace(sFilePath.length()-1,1,"");
		bSeparatorRemoved=true;
	}
	_splitpath(sFilePath.c_str(),NULL,NULL,sFile,sExt);
	sFileName=sFile;
	sFileName+=sExt;
	if(bSeparatorRemoved){sFileName+=PATH_SEPARATOR;}
	return sFileName;
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
			if(strcmp(FileData.cFileName,".")!=0)
			{
				std::string sFile=AppendPathSeparator(sFolder)+FileData.cFileName;
				bool bDirectory=((GetFileAttributes(sFile.c_str())&FILE_ATTRIBUTE_DIRECTORY)!=0);
				std::string sFileName=GetFileName(sFile);
				if(bDirectory){sFile=AppendPathSeparator(sFile);}
				switch(eMode)
				{
				case eFindFilesMode_OnlyFiles:	if(!bDirectory){psFiles->insert(sFile);};break;
				case eFindFilesMode_OnlyDirs:	if(bDirectory){psFiles->insert(sFile);}break;
				case eFindFilesMode_DirsAndFiles:psFiles->insert(sFile);break;
				}
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

bool FileExists(const char *pFileName)
{
	return (_access(pFileName,0)==0);
}

bool FileIsDirectory(const char *pFileName)
{
	return ((GetFileAttributes(pFileName)&FILE_ATTRIBUTE_DIRECTORY)!=0);
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

std::string GetFileFolder(std::string sFilePath)
{
	std::string sFileFolder;
	char *pTemp=strdup(sFilePath.c_str());
	sFileFolder=dirname(pTemp);
	free(pTemp);
	return sFileFolder;
}

std::string GetFileName(std::string sFilePath)
{
	std::string sFileName;
	char *pTemp=strdup(sFilePath.c_str());
	sFileName=basename(pTemp));
	free(pTemp);
	return sFileName;
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

bool FindFiles(const char *psPattern, EFindFilesMode eMode,std::set<std::string> *psFiles)
{
	glob_t globbuf={0};
	unsigned int nFlags=GLOB_MARK;
	nFlags|=((eMode==eFindFilesMode_OnlyDirs)?GLOB_ONLYDIR:0);
	glob(psPattern,nFlags,NULL,&globbuf);
	//liberamos la memoria	
	for (unsigned int i=0; i <globbuf.gl_pathc; i++)
	{
		const char *pFile=globbuf.gl_pathv[i];
		if(pFile[0]==0){continue;}
		bool bDirectory=(pFile[strlen(pFile)-1]==PATH_SEPARATOR_CHAR);
		switch(eMode)
		{
		case eFindFilesMode_Unknown:break;
		case eFindFilesMode_OnlyFiles:	if(!bDirectory){psFiles->insert(pFile);};break;
		case eFindFilesMode_OnlyDirs:	if(!bDirectory){psFiles->insert(pFile);};break;
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
bool FileExists(const char *pFileName)
{
	return (access(pFileName,F_OK)==0);
}
bool FileIsDirectory(const char *pFileName)
{
	struct stat data;
	if(stat(pFileName,&data)!=0){return false;}
	return S_ISDIR(data.st_mode);
}
time_t GetFileTimeStamp(const char *pFileName)
{
	struct stat data;
	if(stat(pFileName,&data)!=0){return 0;}
	return data.st_mtime;
}


std::string NormalizePath(std::string sPath)
{
	char sTemp[MAX_PATH]={0};
	int nLength=readlink(sPath.c_str(),sTemp,sizeof(sTemp)-1);
	if(nLength==-1){return sPath;}
	sTemp[nLength]=0;
	return sTemp;
}
#endif

