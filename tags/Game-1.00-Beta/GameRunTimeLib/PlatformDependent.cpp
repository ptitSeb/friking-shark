//  Friking Shark, a Flying Shark arcade remake.
//  Copyright (C) 2011  Javier Martin Garcia (javiermartingarcia@gmail.com)
//	
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//  


#include "./stdafx.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <stdarg.h>
#include <deque>
#include "PlatformDependent.h"

#include <sys/stat.h>

#ifndef WIN32
#include <sys/time.h>
#include <libgen.h>
#include <glob.h>
#else
#include <io.h>
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
	std::string sFolder=sPath;
	std::string sFileName=GetFileName(sPath);
	
	std::deque<std::string> sFolders;
	std::string sFolderName;
	
	int nToSkip=0;
	while((sFolderName=GetFileName(sFolder))!="")
	{
		if(sFolder=="."){break;}
		if(sFolderName==".."){nToSkip++;}
		else if(sFolderName!="."){if(nToSkip){nToSkip--;}else{sFolders.push_back(sFolderName);}}
		sFolder=GetFileFolder(sFolder);
	}
	// Aqui sFolder contiene / en Linux y C:\ en windows
	std::string sNormalized=sFolder;
	
	std::deque<std::string>::reverse_iterator i;
	for(i=sFolders.rbegin();i!=sFolders.rend();i++)
	{
		sNormalized=AppendPathSeparator(sNormalized);
		sNormalized+=(*i);
	}
	if(FileIsDirectory(sNormalized.c_str())){sNormalized=AppendPathSeparator(sNormalized);}
	return sNormalized;
}

#ifdef WIN32

void GetFileFolder(const char *pFilePath,char *pFolder)
{
	char sDrive[MAX_PATH]={0};
	char sFolder[MAX_PATH]={0};
	char pTempFilePath[MAX_PATH]={0};
	strcpy(pTempFilePath,pFilePath);
	int nLen=strlen(pTempFilePath);
	if(nLen && pTempFilePath[nLen-1]==PATH_SEPARATOR_CHAR){pTempFilePath[nLen-1]=0;}
	// SplitPath devuelve FileName "" para c:\Temp\ en lugar de Temp
	// Para evitarlo se le quita la barra final
	_splitpath(pTempFilePath,sDrive,sFolder,NULL,NULL);
	strcpy(pFolder,sDrive);
	strcat(pFolder,sFolder);
}

void GetFileName(const char *pFilePath,char *pFileName)
{
	char sFile[MAX_PATH]={0};
	char sExt[MAX_PATH]={0};
	char pTempFilePath[MAX_PATH]={0};
	// SplitPath devuelve FileName "" para c:\Temp\ en lugar de Temp
	// Para evitarlo se le quita la barra final
	strcpy(pTempFilePath,pFilePath);
	int nLen=strlen(pTempFilePath);
	if(nLen && pTempFilePath[nLen-1]==PATH_SEPARATOR_CHAR){pTempFilePath[nLen-1]=0;}
	_splitpath(pFilePath,NULL,NULL,sFile,sExt);
	strcpy(pFileName,sFile);
	strcat(pFileName,sExt);
}

std::string GetFileFolder(std::string sFilePath)
{
	std::string sFileFolder;
	char sDrive[MAX_PATH]={0};
	char sFolder[MAX_PATH]={0};
	
	// SplitPath devuelve Folder "C:\Temp\" para C:\Temp\ en lugar de C:\.
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
	return sFileName;
}
void RTTRACE(const char *format, ...)
{
	va_list vargs;
	va_start (vargs,format);
	char pTempBuffer[1024*16];
	int res=vsnprintf(pTempBuffer,sizeof(pTempBuffer)-1,format,vargs);
	if(res==-1){pTempBuffer[sizeof(pTempBuffer)-2]=0;res=sizeof(pTempBuffer)-2;}
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
	DWORD dwAttrib=GetFileAttributes(pFileName);
	if(dwAttrib==0xFFFFFFFF){return false;}
	return ((dwAttrib&FILE_ATTRIBUTE_DIRECTORY)!=0);
}

time_t GetFileTimeStamp(const char *pFileName)
{
	struct stat data;
	if(stat(pFileName,&data)!=0){return 0;}
	return data.st_mtime;
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
	if(strcmp(pFileName,"/")==0){pFileName[0]=0;}
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
	sFileName=basename(pTemp);
	if(sFileName=="/"){return "";}
	free(pTemp);
	return sFileName;
}

void RTTRACE(const char *format, ...)
{
	va_list vargs;
	va_start (vargs,format);
	char pTempBuffer[1024*16];
	int res=vsnprintf(pTempBuffer,sizeof(pTempBuffer)-1,format,vargs);
	if(res==-1){pTempBuffer[sizeof(pTempBuffer)-2]=0;res=sizeof(pTempBuffer)-2;}
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
	if(eMode!=eFindFilesMode_OnlyFiles )
	{
	  std::string sParent=GetFileFolder(psPattern)+PATH_SEPARATOR+".."+PATH_SEPARATOR;
	  if(FileExists(sParent.c_str())){psFiles->insert(sParent);}
	}
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
		case eFindFilesMode_OnlyDirs:	if(bDirectory){psFiles->insert(pFile);};break;
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

#endif

