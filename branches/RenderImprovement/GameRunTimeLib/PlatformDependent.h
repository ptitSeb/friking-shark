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


#ifndef _PLATFORM_DEPENDENT_
#define _PLATFORM_DEPENDENT_

#if !defined WIN32 && !defined ANDROID
#define LINUX
#endif

#ifdef WIN32

	#define WINVER 0x0501
	#define WIN32_LEAN_AND_MEAN

	#include <windows.h>
	// warning C4250: 'xxx' : inherits 'CSystemObjectBase::AddReference' via dominance
	#pragma warning ( disable : 4250 )

	#define PATH_SEPARATOR "/"
	#define PATH_SEPARATOR_CHAR '/'
	
	#define strcasecmp _stricmp
	#define strtok_r strtok_s

	#define round(x) floor((x)+0.5)

	#define RTASSERT(x) _ASSERTE((x))
	#define __UNUSED_ATTRIB__

	void WindowsPathToGamePath(char *pPath);
#else
	#include <libgen.h>

	#define MAX_PATH 260
	#define PATH_SEPARATOR "/"
	#define PATH_SEPARATOR_CHAR '/'

	#ifdef NDEBUG
		#define RTASSERT(x)
	#else
		#include <assert.h>
		#define RTASSERT(x) assert(x)
	#endif

	#define __UNUSED_ATTRIB__ __attribute__((unused))
#endif
#include <set>
#include <string>

enum EFindFilesMode
{
	eFindFilesMode_Unknown,
	eFindFilesMode_OnlyFiles,
	eFindFilesMode_OnlyDirs,
	eFindFilesMode_DirsAndFiles
};

void 		ReplaceExtension(char *pFileName,const char *pExt);
void 		GetExtension(const char *pFileName,char *pExt);
void 		GetFileFolder(const char *pFilePath,char *pFolder);
std::string GetFileFolder(std::string sFilePath);
void 		GetFileName(const char *pFilePath,char *pFileName);
std::string GetFileName(std::string sFilePath);
std::string AppendPathSeparator(std::string sFile);

bool 		FindFiles(const char *psPattern, EFindFilesMode eMode,std::set<std::string> *psFiles);

time_t 		GetFileTimeStamp(const char *pFileName);
bool 		FileExists(const char *pFileName);
bool 		FileIsDirectory(const char *pFileName);
std::string GetWorkingFolder();
bool 		SetWorkingFolder(std::string sFolder);
std::string NormalizePath(std::string sPath);

// Asset file encapsulation, needed for android.

#ifdef ANDROID
#include "../GameEngine/android_native_app_glue.h"
void setAssetManager(AAssetManager *pAssetManager);
struct AFILE;
bool afexists(const char * _Filename);
AFILE *afopen(const char * _Filename, const char * _Mode);
size_t afread(void * _DstBuf, size_t _ElementSize, size_t _Count, AFILE * _File);
size_t afwrite(const void * _DstBuf, size_t _ElementSize, size_t _Count, AFILE * _File);
char  *afgets(char *s, int size, AFILE *_File);
FILE  *afgetfile(AFILE *file);
int afseek(AFILE * _File, long _Offset, int _Origin);
int aftell(AFILE * _File);
int afclose(AFILE * _File);
#else
struct AFILE;
bool afexists(const char * _Filename);
AFILE *afopen(const char * _Filename, const char * _Mode);
size_t afread(void * _DstBuf, size_t _ElementSize, size_t _Count, AFILE * _File);
size_t afwrite(const void * _DstBuf, size_t _ElementSize, size_t _Count, AFILE * _File);
char  *afgets(char *s, int size, AFILE *_File);
FILE  *afgetfile(AFILE *file);
int afseek(AFILE * _File, long _Offset, int _Origin);
int aftell(AFILE * _File);
int afclose(AFILE * _File);
#endif

unsigned int GetTimeStamp();
double       GetMicrosecondTimeStamp();

void RTTRACE(const char *format, ...);

#endif
