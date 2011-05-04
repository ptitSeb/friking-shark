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

#ifdef WIN32

	#define WINVER 0x0501
	#define WIN32_LEAN_AND_MEAN

	#include <windows.h>
	// warning C4250: 'xxx' : inherits 'CSystemObjectBase::AddReference' via dominance
	#pragma warning ( disable : 4250 )

	#define PATH_SEPARATOR "\\"
	#define PATH_SEPARATOR_CHAR '\\'
	#define strcasecmp _stricmp
	#define strtok_r strtok_s

	#define RTASSERT(x) _ASSERTE((x))
	#define __UNUSED_ATTRIB__
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

unsigned int GetTimeStamp();

void RTTRACE(const char *format, ...);

#endif
