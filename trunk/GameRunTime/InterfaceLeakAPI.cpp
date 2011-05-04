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


#include <new>
#include <set>
#include <map>
#include <malloc.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "InterfaceLeakAPI.h"
#ifdef WIN32
#include  <windows.h>
#include  <crtdbg.h>
#include  <io.h> // _access


#define _CRT_ERROR          1

#define ALLOCATION_UNIT_TAG	0xA1B2C3D4
#define MONITORIZATION_MAX_STACK_FRAMES 32

#define GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS (0x00000004)
#define GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT (0x00000002)

typedef struct _MODULEINFO {  LPVOID lpBaseOfDll;  DWORD SizeOfImage;  LPVOID EntryPoint;} MODULEINFO,  *LPMODULEINFO;

typedef BOOL(WINAPI*PGET_MODULE_HANDLE_EXA)(IN DWORD dwFlags,LPCSTR lpModuleName,OUT HMODULE*phModule);
typedef BOOL(WINAPI*PGET_MODULE_INFORMATION)(HANDLE hProcess,HMODULE hModule,LPMODULEINFO lpmodinfo,DWORD cb);

typedef struct _MODLOAD_DATA {
    DWORD   ssize;                  // size of this struct
    DWORD   ssig;                   // signature identifying the passed data
    PVOID   data;                   // pointer to passed data
    DWORD   size;                   // size of passed data
    DWORD   flags;                  // options
} MODLOAD_DATA, *PMODLOAD_DATA;

typedef struct _SYMBOL_INFO {  
	ULONG SizeOfStruct;  
	ULONG TypeIndex;  
	ULONG64 Reserved[2];  
	ULONG Index;  
	ULONG Size;  
	ULONG64 ModBase;  
	ULONG Flags;  
	ULONG64 Value;  
	ULONG64 Address;  
	ULONG Register;  
	ULONG Scope;  
	ULONG Tag;  
	ULONG NameLen;  
	ULONG MaxNameLen;  
	TCHAR Name[1];
} SYMBOL_INFO,  *PSYMBOL_INFO;

typedef struct _IMAGEHLP_LINE64 {  
	DWORD SizeOfStruct;  
	PVOID Key;  
	DWORD LineNumber;  
	PTSTR FileName;  
	DWORD64 Address;
} IMAGEHLP_LINE64, *PIMAGEHLP_LINE64;

typedef DWORD64 (WINAPI *PSYM_LOAD_MODULE_EX)(HANDLE hProcess,HANDLE hFile,char * ImageName,char * ModuleName,DWORD64 BaseOfDll,DWORD DllSize,PMODLOAD_DATA Data,DWORD Flags);
typedef BOOL (WINAPI *PSYM_INITIALIZE)(HANDLE hProcess,char *UserSearchPath,BOOL fInvadeProcess);
typedef BOOL (WINAPI *PSYM_FROM_ADDRESS)(HANDLE hProcess,DWORD64 Address,PDWORD64 Displacement,PSYMBOL_INFO Symbol);
typedef BOOL (WINAPI *PSYM_LINE_FROM_ADDRESS)(HANDLE hProcess,DWORD64 Address,PDWORD64 Displacement,PIMAGEHLP_LINE64 Line);


PGET_MODULE_HANDLE_EXA pGetModuleHandleEx=NULL;
PGET_MODULE_INFORMATION pGetModuleInformation=NULL;
PSYM_LOAD_MODULE_EX		pSymLoadModuleEx=NULL;
PSYM_INITIALIZE			pSymInitialize=NULL;
PSYM_FROM_ADDRESS		pSymFromAddress=NULL;
PSYM_LINE_FROM_ADDRESS	pSymLineFromAddress=NULL;

void InitializeListHead(LIST_ENTRY *ListHead)
{
	(ListHead)->Flink = (ListHead)->Blink = (ListHead);
}

char IsListEmpty(const LIST_ENTRY * ListHead)
{
	return (char)(ListHead->Flink == ListHead);
}

char RemoveEntryList(LIST_ENTRY * Entry)
{
	LIST_ENTRY * Blink;
	LIST_ENTRY * Flink;

	Flink = Entry->Flink;
	Blink = Entry->Blink;
	Blink->Flink = Flink;
	Flink->Blink = Blink;
	return (char)(Flink == Blink);
}

LIST_ENTRY *RemoveHeadList(LIST_ENTRY * ListHead)
{
	LIST_ENTRY * Flink;
	LIST_ENTRY * Entry;

	Entry = ListHead->Flink;
	Flink = Entry->Flink;
	ListHead->Flink = Flink;
	Flink->Blink = ListHead;
	return Entry;
}

LIST_ENTRY *RemoveTailList(LIST_ENTRY * ListHead)
{
	LIST_ENTRY * Blink;
	LIST_ENTRY * Entry;

	Entry = ListHead->Blink;
	Blink = Entry->Blink;
	ListHead->Blink = Blink;
	Blink->Flink = ListHead;
	return Entry;
}


void InsertTailList(LIST_ENTRY * ListHead,LIST_ENTRY * Entry)
{
	LIST_ENTRY * Blink;

	Blink = ListHead->Blink;
	Entry->Flink = ListHead;
	Entry->Blink = Blink;
	Blink->Flink = Entry;
	ListHead->Blink = Entry;
}


void InsertHeadList(LIST_ENTRY * ListHead,LIST_ENTRY * Entry)
{
	LIST_ENTRY * Flink;

	Flink = ListHead->Flink;
	Entry->Flink = Flink;
	Entry->Blink = ListHead;
	Flink->Blink = Entry;
	ListHead->Flink = Entry;
}

void AppendTailList(LIST_ENTRY * ListHead,LIST_ENTRY * ListToAppend)
{
	LIST_ENTRY * ListEnd = ListHead->Blink;

	ListHead->Blink->Flink = ListToAppend;
	ListHead->Blink = ListToAppend->Blink;
	ListToAppend->Blink->Flink = ListHead;
	ListToAppend->Blink = ListEnd;
}

struct SModule
{
	LIST_ENTRY  listEntry;
	void	   *pBaseAddress;
	void	   *pEntryPoint;
	unsigned long dwSize;
	char		sModulePath[MAX_PATH];
	char		sModuleName[MAX_PATH];
};

struct SFile
{
	LIST_ENTRY  listEntry;
	char		sFileName[MAX_PATH];
	char		sFilePath[MAX_PATH];
};

struct SSymbol
{
	LIST_ENTRY  listEntry;
	char		sName[MAX_PATH];
};

struct SStackFrame
{
	void	 *pAddress;
	SModule	 *pModule;
	SFile	 *pFile;
	SSymbol  *pSymbol;
	unsigned int nLine;

	SStackFrame()
	{
		pAddress=NULL;
		pModule=NULL;
		pFile=NULL;
		pSymbol=NULL;
		nLine=0;
	}
};

struct SStack
{
	unsigned long nTimes;
	SStackFrame	frames[MONITORIZATION_MAX_STACK_FRAMES];

	bool operator <(const SStack &otherFrame)const 
	{
		for(int x=0;x<MONITORIZATION_MAX_STACK_FRAMES;x++)
		{
			if(frames[x].pAddress<otherFrame.frames[x].pAddress){return true;}
			if(frames[x].pAddress>otherFrame.frames[x].pAddress){return false;}
			if(frames[x].pModule<otherFrame.frames[x].pModule){return true;}
			if(frames[x].pModule>otherFrame.frames[x].pModule){return false;}
			if(frames[x].pFile<otherFrame.frames[x].pFile){return true;}
			if(frames[x].pFile>otherFrame.frames[x].pFile){return false;}
			if(frames[x].pSymbol<otherFrame.frames[x].pSymbol){return true;}
			if(frames[x].pSymbol>otherFrame.frames[x].pSymbol){return false;}
			if(frames[x].nLine<otherFrame.frames[x].nLine){return true;}
			if(frames[x].nLine>otherFrame.frames[x].nLine){return false;}
			if(frames[x].pModule==NULL && otherFrame.frames[x].pModule==NULL){return false;}
		}
		return false;
	}

	SStack()
	{
		nTimes=0;
	}
};

class CInterface
{
public:

	unsigned long			dwReferences;
	void					*pPointer;
	unsigned char			bDumped; // para dumps incrementales
	std::set<SStack>		sAddRefStacks;
	std::set<SStack>		sReleaseStacks;

	CInterface()
	{
		pPointer=NULL;
		dwReferences=0;
		bDumped=false;
	}
};

struct SMonitorizationConfig
{
	unsigned char bPeriodicDump;
	unsigned char bPeriodicDumpIncremental;
	unsigned long dwPeriodicDumpPeriod;
	unsigned char bSymbolLoadErrors;
	unsigned char bCRTAllocations;
	unsigned char bCompactLeaks;
	int			  nFramesToSkip;
	int			  bStopOnMissingSource;
	char		  pFormatString[1024];
	char		  pNoSymbolFormatString[1024];
	char		  pNoCodeFormatString[1024];
} ;

HANDLE g_hMonitorizedInternalHeap=NULL;

std::map<void *,CInterface> g_mInterfaces;
LIST_ENTRY	  g_MonitorizedModuleHead={0};
LIST_ENTRY	  g_MonitorizedFileHead={0};
LIST_ENTRY	  g_MonitorizedSymbolHead={0};
unsigned long g_dwLastPeriodicDump=0;

CRITICAL_SECTION g_csMonitorizationHeap={0};
SMonitorizationConfig g_MonitorizationConfig={0};

#define DEFAULT_PERIODIC_DUMP_ENABLED 0
#define DEFAULT_PERIODIC_DUMP_PERIOD  10
#define DEFAULT_PERIODIC_DUMP_INCREMENTAL 1
#define DEFAULT_FORMAT_STRING "\\t\\t{file}({line}) : \\t{symbol}"
#define DEFAULT_NO_CODE_FORMAT_STRING "\\t{address} \\t{module}!{symbol} + {offset}"
#define DEFAULT_NO_SYMBOL_FORMAT_STRING "\\t{address} \\t{module}!{offset}"
#define DEFAULT_SHOW_SYMBOL_ERRORS 0
#define DEFAULT_SHOW_CRT_ALLOCATIONS 0
#define DEFAULT_SHOW_MEMORY_ASSERTS 1
#define DEFAULT_GROUPED_LEAKS 1
#define DEFAULT_FRAMES_TO_SKIP 2
#define DEFAULT_STOP_ON_MISSING_SOURCE 0

#define FORMAT_TAB_TAG "\\t"
#define FORMAT_ADDRESS_TAG "{address}"
#define FORMAT_MODULE_TAG "{module}"
#define FORMAT_FULL_MODULE_PATH_TAG "{fullmodulepath}"
#define FORMAT_SYMBOL_TAG "{symbol}"
#define FORMAT_OFFSET_TAG "{offset}"
#define FORMAT_SOURCE_FILE_TAG "{file}"
#define FORMAT_FULL_SOURCE_FILE_PATH_TAG "{fullfilepath}"
#define FORMAT_SOURCE_LINE_TAG "{line}"
/*
#ifdef _DEBUG
_CRTIMP void _cdecl _CrtDbgBreak();
_CRTIMP int __cdecl _CrtDbgReport(int nRptType,const char * szFile,int nLine,const char * szModule,const char * szFormat,...);

void ReportError(const char * szFile,int nLine,const char * szModule,const char * szMessage)
{
	if(g_MonitorizationConfig.bMemoryAsserts)
	{
		if(_CrtDbgReport(_CRT_ERROR,szFile,nLine,szModule,szMessage)==1)
		{
			_CrtDbgBreak();
		}
	}
}
#else
_CRTIMP int __cdecl ReportError(const char * szFile,int nLine,const char * szModule,const char * szMessage)
{
	return 0;
}
#endif
*/

void LoadConfig()
{
	g_MonitorizationConfig.bPeriodicDump=GetPrivateProfileInt("PeriodicDump","Enabled",DEFAULT_PERIODIC_DUMP_ENABLED,"ILD.ini");
	g_MonitorizationConfig.bPeriodicDumpIncremental=GetPrivateProfileInt("PeriodicDump","Incremental",DEFAULT_PERIODIC_DUMP_INCREMENTAL,"ILD.ini");
	g_MonitorizationConfig.dwPeriodicDumpPeriod=GetPrivateProfileInt("PeriodicDump","Period",DEFAULT_PERIODIC_DUMP_PERIOD,"ILD.ini");

	g_MonitorizationConfig.bSymbolLoadErrors=GetPrivateProfileInt("Output","SymbolLoadErrors",DEFAULT_SHOW_SYMBOL_ERRORS,"ILD.ini");
	g_MonitorizationConfig.bCRTAllocations=GetPrivateProfileInt("Output","ShowCRTAllocations",DEFAULT_SHOW_CRT_ALLOCATIONS,"ILD.ini");
	g_MonitorizationConfig.bCompactLeaks=GetPrivateProfileInt("Output","GroupedLeaks",DEFAULT_GROUPED_LEAKS,"ILD.ini");
	g_MonitorizationConfig.nFramesToSkip=GetPrivateProfileInt("Output","FramesToSkip",DEFAULT_FRAMES_TO_SKIP,"ILD.ini");
	g_MonitorizationConfig.bStopOnMissingSource=GetPrivateProfileInt("Output","StopOnMissingSource",DEFAULT_STOP_ON_MISSING_SOURCE,"ILD.ini");
	GetPrivateProfileString("Output","Format",DEFAULT_FORMAT_STRING,g_MonitorizationConfig.pFormatString,sizeof(g_MonitorizationConfig.pFormatString)-1,"ILD.ini");
	GetPrivateProfileString("Output","NoSymbolFormat",DEFAULT_NO_SYMBOL_FORMAT_STRING,g_MonitorizationConfig.pNoSymbolFormatString,sizeof(g_MonitorizationConfig.pNoSymbolFormatString)-1,"ILD.ini");
	GetPrivateProfileString("Output","NoCodeFormat",DEFAULT_NO_CODE_FORMAT_STRING,g_MonitorizationConfig.pNoCodeFormatString,sizeof(g_MonitorizationConfig.pNoCodeFormatString)-1,"ILD.ini");
}

void InitMonitorization()
{
	HMODULE hK32=NULL;
	HMODULE hPSAPI=NULL;
	HMODULE hDbgHelp=NULL;

	LoadConfig();

	InitializeCriticalSection(&g_csMonitorizationHeap);
	g_hMonitorizedInternalHeap=HeapCreate(0,0,0);
	InitializeListHead(&g_MonitorizedModuleHead);
	InitializeListHead(&g_MonitorizedFileHead);
	InitializeListHead(&g_MonitorizedSymbolHead);

	hK32=LoadLibrary("kernel32.dll");
	pGetModuleHandleEx=(PGET_MODULE_HANDLE_EXA)GetProcAddress(hK32,"GetModuleHandleExA");

	hPSAPI=LoadLibrary("psapi.dll");
	pGetModuleInformation=(PGET_MODULE_INFORMATION)GetProcAddress(hPSAPI,"GetModuleInformation");

	hDbgHelp=LoadLibrary("Dbghelp.dll");
	pSymInitialize=(PSYM_INITIALIZE)GetProcAddress(hDbgHelp,"SymInitialize");
	pSymLoadModuleEx=(PSYM_LOAD_MODULE_EX)GetProcAddress(hDbgHelp,"SymLoadModuleEx");
	pSymFromAddress=(PSYM_FROM_ADDRESS)GetProcAddress(hDbgHelp,"SymFromAddr");
	pSymLineFromAddress=(PSYM_LINE_FROM_ADDRESS)GetProcAddress(hDbgHelp,"SymGetLineFromAddr64");

	if(!pGetModuleHandleEx){OutputDebugString("Unable to load function GetModuleHandleEx from kernel32.dll\n");}
	if(!pGetModuleInformation){OutputDebugString("Unable to load function GetModuleInformation from psapi.dll\n");}
	if(!pSymInitialize){OutputDebugString("Unable to load function SymInitialize from dbghelp.dll\n");}
	if(!pSymLoadModuleEx){OutputDebugString("Unable to load function SymLoadModuleEx from dbghelp.dll\n");}
	if(!pSymFromAddress){OutputDebugString("Unable to load function SymFromAddr from dbghelp.dll\n");}
	if(!pSymLineFromAddress){OutputDebugString("Unable to load function SymGetLineFromAddr64 from dbghelp.dll\n");}
	if(pSymInitialize)
	{
		pSymInitialize(GetCurrentProcess(),NULL,FALSE);
	}
}

SSymbol *AddSymbol(char *pName)
{
	LIST_ENTRY *pEntry=NULL;
	SSymbol *pSymbol=NULL;

	// search for Symbol in Symbol list.
	pEntry=g_MonitorizedSymbolHead.Flink;
	while(pEntry!=&g_MonitorizedSymbolHead)
	{
		SSymbol *pTempSymbol=CONTAINING_RECORD(pEntry,SSymbol,listEntry);
		if(strcmp(pTempSymbol->sName,pName)==0)
		{
			pSymbol=pTempSymbol;
			break;
		}
		pEntry=pEntry->Flink;
	}
	if(pSymbol==NULL)
	{
		pSymbol=(SSymbol*)HeapAlloc(g_hMonitorizedInternalHeap,0,sizeof(SSymbol));
		strcpy(pSymbol->sName,pName);
		InsertHeadList(&g_MonitorizedSymbolHead,&pSymbol->listEntry);
	}
	return pSymbol;
}

SFile *AddFile(char *pFileName)
{
	LIST_ENTRY *pEntry=NULL;
	SFile *pFile=NULL;
	
	// search for File in File list.
	pEntry=g_MonitorizedFileHead.Flink;
	while(pEntry!=&g_MonitorizedFileHead)
	{
		SFile *pTempFile=CONTAINING_RECORD(pEntry,SFile,listEntry);
		if(strcmp(pTempFile->sFilePath,pFileName)==0)
		{
			pFile=pTempFile;
			break;
		}
		pEntry=pEntry->Flink;
	}
	if(pFile==NULL)
	{
		char sTempName[MAX_PATH]={0},sTempExt[MAX_PATH]={0};
		_splitpath(pFileName,NULL,NULL,sTempName,sTempExt);
		
		pFile=(SFile*)HeapAlloc(g_hMonitorizedInternalHeap,0,sizeof(SFile));
		strcpy(pFile->sFilePath,pFileName);
		strcpy(pFile->sFileName,sTempName);
		strcat(pFile->sFileName,sTempExt);
		InsertHeadList(&g_MonitorizedFileHead,&pFile->listEntry);
	}
	return pFile;	
}

SModule *AddModule(void *pAddress)
{
	LIST_ENTRY *pEntry=NULL;
	SModule *pModule=NULL;
	HMODULE hModule=NULL;
	char sModuleName[MAX_PATH]={0};
	void *pModuleBase=NULL;

	if(pGetModuleHandleEx)
	{
		pGetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,(LPCTSTR)pAddress,&hModule);
		GetModuleFileName(hModule,sModuleName,MAX_PATH);
	}

	// search for module in module list.
	pEntry=g_MonitorizedModuleHead.Flink;
	while(pEntry!=&g_MonitorizedModuleHead)
	{
		SModule *pTempModule=CONTAINING_RECORD(pEntry,SModule,listEntry);
		if(strcmp(pTempModule->sModulePath,sModuleName)==0)
		{
			pModule=pTempModule;
			break;
		}
		pEntry=pEntry->Flink;
	}

	if(pModule==NULL)
	{
		unsigned long dwModuleSize=0;
		void *pModuleEntryPoint=NULL;
		MODLOAD_DATA modLoadData={0};
		char sTempName[MAX_PATH]={0},sTempExt[MAX_PATH]={0};
		
		pModule=(SModule*)HeapAlloc(g_hMonitorizedInternalHeap,0,sizeof(SModule));
		strcpy(pModule->sModulePath,sModuleName);

		_splitpath(pModule->sModulePath,NULL,NULL,sTempName,sTempExt);
		strcpy(pModule->sModuleName,sTempName);
		strcat(pModule->sModuleName,sTempExt);
		
		if(pGetModuleInformation)
		{
			MODULEINFO moduleInfo={0};
			pGetModuleInformation(GetCurrentProcess(),hModule,&moduleInfo,sizeof(moduleInfo));
			pModuleBase=(void*)GetModuleHandle(sModuleName);
			dwModuleSize=moduleInfo.SizeOfImage;
			pModuleEntryPoint=moduleInfo.EntryPoint;
		}
		else
		{
			pModuleBase=(void*)hModule;
		}
		pModule->dwSize=dwModuleSize;
		pModule->pEntryPoint=pModuleEntryPoint;
		pModule->pBaseAddress=pModuleBase;
		
		InsertHeadList(&g_MonitorizedModuleHead,&pModule->listEntry);
		
		if(pSymLoadModuleEx(GetCurrentProcess(),NULL,pModule->sModulePath,NULL,0,0,NULL,0)==0)
		{
			//if(g_MonitorizationConfig.bSymbolLoadErrors)
			{
				char sTemp[1024];
				sprintf(sTemp,"Failed to load symbols for '%s', error %d\n",pModule->sModulePath,GetLastError());
				OutputDebugString(sTemp);
			}
		}
	}

	return pModule;	
}


void GetStackFrameFromCodeAddress(void *pAddress,SStackFrame *pFrame)
{
	SModule		*pModule=NULL;
	DWORD64		 displacement=0;
	char		 pFunctionSymbolBuffer[sizeof(SYMBOL_INFO)+1024];
	SYMBOL_INFO *pFunctionSymbol=(SYMBOL_INFO *)pFunctionSymbolBuffer;
	IMAGEHLP_LINE64 functionLine={0};

	// First, add module based on code address to load symbol table if necesary
	pModule=AddModule(pAddress);

	memset(pFunctionSymbol,0,sizeof(SYMBOL_INFO)+1024);
	pFunctionSymbol->SizeOfStruct=sizeof(SYMBOL_INFO);
	pFunctionSymbol->MaxNameLen=1024;
	if(!pSymFromAddress(GetCurrentProcess(),(DWORD64)pAddress,&displacement,pFunctionSymbol))
	{
		if(g_MonitorizationConfig.bSymbolLoadErrors)
		{
			char sTemp[1024];
			sprintf(sTemp,"Failed to resolve symbol for address 0x%p of '%s', error %d\n",pAddress,pModule->sModulePath,GetLastError());
			OutputDebugString(sTemp);
		}
	}

	functionLine.SizeOfStruct=sizeof(IMAGEHLP_LINE64);
	if(!pSymLineFromAddress(GetCurrentProcess(),(DWORD64)pAddress,&displacement,&functionLine))
	{
		if(g_MonitorizationConfig.bSymbolLoadErrors)
		{
			char sTemp[1024];
			sprintf(sTemp,"Failed to resolve source code for address 0x%p of '%s', error %d\n",pAddress,pModule->sModulePath,GetLastError());
			OutputDebugString(sTemp);
		}
	}

	pFrame->pSymbol=AddSymbol(pFunctionSymbol->Name);
	pFrame->pFile=AddFile(functionLine.FileName?(char*)functionLine.FileName:"");
	pFrame->nLine=functionLine.LineNumber;
	pFrame->pAddress=pAddress;
	pFrame->pModule=pModule;
}

void GetStack(SStack *pStack)
{
	unsigned *pEBP=0;
	unsigned *pRETDIR=0;
	int nToSkip=g_MonitorizationConfig.nFramesToSkip;
	int nFrames=0;
	memset(pStack->frames,0,sizeof(pStack->frames));

	__try
	{
		_asm
		{
			mov eax,ebp;
			mov pEBP,eax;
		}
		do
		{
			if(IsBadReadPtr(pEBP,sizeof(void*)))
			{
				break;
			}

			pRETDIR=(unsigned *)(((char *)pEBP)+sizeof(void*));

			if(IsBadReadPtr(pRETDIR,sizeof(void*)))
			{
				break;
			}
			if(pEBP)
			{
				if(nToSkip==0)
				{
					if(*pRETDIR)
					{
						GetStackFrameFromCodeAddress((void*)*pRETDIR,&pStack->frames[nFrames]);
						nFrames++;
					}
				}
				else
				{
					nToSkip--;
				}
				pEBP=(unsigned*)(*pEBP);
			}
		}
		while(pEBP!=0 && nFrames<MONITORIZATION_MAX_STACK_FRAMES);
	}
	__except(1){}
}

void FormatFrame(char *pFinalBuffer,SStackFrame *pFrame)
{
	unsigned long dwOffset=(unsigned long)(((char*)pFrame->pAddress)-(unsigned long)pFrame->pModule->pBaseAddress);
	char sTemp[1024]={0};
	char *pCursor=NULL;
	char *pBuffer=pFinalBuffer;
	pBuffer[0]=0;
	if(pFrame->pSymbol->sName[0]==0)
	{
		pCursor=g_MonitorizationConfig.pNoSymbolFormatString;
	}
	else if(pFrame->pFile->sFileName[0]==0)
	{
		pCursor=g_MonitorizationConfig.pNoCodeFormatString;
	}
	else
	{
		pCursor=g_MonitorizationConfig.pFormatString;
	}

	while(pCursor[0])
	{
		if(strncmp(pCursor,FORMAT_TAB_TAG,strlen(FORMAT_TAB_TAG))==0)
		{
			pBuffer[0]='\t';
			pBuffer++;
			pCursor+=strlen(FORMAT_TAB_TAG);
		}
		else if(strncmp(pCursor,FORMAT_ADDRESS_TAG,strlen(FORMAT_ADDRESS_TAG))==0)
		{
			int nLen=sprintf(sTemp,"0x%08x",pFrame->pAddress);
			memcpy(pBuffer,sTemp,nLen);
			pBuffer[nLen]=0;
			pBuffer+=nLen;
			pCursor+=strlen(FORMAT_ADDRESS_TAG);
		}
		else if(strncmp(pCursor,FORMAT_OFFSET_TAG,strlen(FORMAT_OFFSET_TAG))==0)
		{
			int nLen=sprintf(sTemp,"0x%x",dwOffset);
			memcpy(pBuffer,sTemp,nLen);
			pBuffer[nLen]=0;
			pBuffer+=nLen;
			pCursor+=strlen(FORMAT_OFFSET_TAG);
		}
		else if(strncmp(pCursor,FORMAT_MODULE_TAG,strlen(FORMAT_MODULE_TAG))==0)
		{
			int nLen=strlen(pFrame->pModule->sModuleName);
			memcpy(pBuffer,pFrame->pModule->sModuleName,nLen);
			pBuffer[nLen]=0;
			pBuffer+=nLen;
			pCursor+=strlen(FORMAT_MODULE_TAG);
		}
		else if(strncmp(pCursor,FORMAT_FULL_MODULE_PATH_TAG,strlen(FORMAT_FULL_MODULE_PATH_TAG))==0)
		{
			int nLen=strlen(pFrame->pModule->sModulePath);
			memcpy(pBuffer,pFrame->pModule->sModulePath,nLen);
			pBuffer[nLen]=0;
			pBuffer+=nLen;
			pCursor+=strlen(FORMAT_FULL_MODULE_PATH_TAG);
		}
		else if(strncmp(pCursor,FORMAT_SYMBOL_TAG,strlen(FORMAT_SYMBOL_TAG))==0)
		{
			char *pString=pFrame->pSymbol->sName[0]==0?"?":pFrame->pSymbol->sName;
			int nLen=strlen(pString);
			memcpy(pBuffer,pString,nLen);
			pBuffer[nLen]=0;
			pBuffer+=nLen;
			pCursor+=strlen(FORMAT_SYMBOL_TAG);
		}
		else if(strncmp(pCursor,FORMAT_SOURCE_FILE_TAG,strlen(FORMAT_SOURCE_FILE_TAG))==0)
		{
			char *pString=pFrame->pFile->sFileName[0]==0?"?":pFrame->pFile->sFileName;
			int nLen=sprintf(sTemp,"%s",pString);
			memcpy(pBuffer,sTemp,nLen);
			pBuffer[nLen]=0;
			pBuffer+=nLen;
			pCursor+=strlen(FORMAT_SOURCE_FILE_TAG);
		}
		else if(strncmp(pCursor,FORMAT_FULL_SOURCE_FILE_PATH_TAG,strlen(FORMAT_FULL_SOURCE_FILE_PATH_TAG))==0)
		{
			char *pString=pFrame->pFile->sFilePath[0]==0?"?":pFrame->pFile->sFilePath;
			int nLen=sprintf(sTemp,"%s",pString);
			memcpy(pBuffer,sTemp,nLen);
			pBuffer[nLen]=0;
			pBuffer+=nLen;
			pCursor+=strlen(FORMAT_FULL_SOURCE_FILE_PATH_TAG);
		}
		else if(strncmp(pCursor,FORMAT_SOURCE_LINE_TAG,strlen(FORMAT_SOURCE_LINE_TAG))==0)
		{
			int nLen=sprintf(sTemp,"%d",pFrame->nLine);
			memcpy(pBuffer,sTemp,nLen);
			pBuffer[nLen]=0;
			pBuffer+=nLen;
			pCursor+=strlen(FORMAT_SOURCE_LINE_TAG);
		}
		else
		{
			*pBuffer=*pCursor;
			pCursor++;
			pBuffer++;
		}
	}
	pBuffer[0]='\n';
	pBuffer[1]=0;
}


void DumpStack(SStack *pStack)
{
	char sTemp[1024];
	int nFrame=0;
	for(nFrame=0;nFrame<MONITORIZATION_MAX_STACK_FRAMES && pStack->frames[nFrame].pModule!=NULL;nFrame++)
	{
		if(g_MonitorizationConfig.bStopOnMissingSource && _access(pStack->frames[nFrame].pFile->sFileName,0)!=0)
		{
			break;
		}
		FormatFrame(sTemp,&pStack->frames[nFrame]);
		OutputDebugString(sTemp);
	}
}

void DumpStackList(std::set<SStack> *pStacks,bool bAddRef)
{
	std::set<SStack>::iterator i;
	for(i=pStacks->begin();i!=pStacks->end();i++)
	{
		char A[128];
		if(bAddRef)
		{
			sprintf(A,"\t%d AddRefs from...\n",i->nTimes);
		}
		else
		{
			sprintf(A,"\t%d Releases from...\n",i->nTimes);
		}
		OutputDebugString(A);
		SStack *pStack=const_cast<SStack*>(&(*i));
		DumpStack(pStack);
		OutputDebugString("\n");
	}
}

void DumpCurrentStack()
{
	SStack stack;
	GetStack(&stack);
	DumpStack(&stack);
}
/*
void GetLostBytesFromSameStack(unsigned char bIncremental,SAllocationUnitHeader *pUnit,SAllocationStats *pStats)
{
	LIST_ENTRY *pEntry=NULL;
	pStats->dwBytes=pUnit->dwSize;
	pStats->dwUnits=1;
	pStats->dwMinUnit=pStats->dwMaxUnit=pUnit->dwSize;
	pUnit->bTemporallyGrouped=1;
	pUnit->bDumped=1;

	pEntry=g_MonitorizedAllocationHead.Flink;
	while(pEntry!=&g_MonitorizedAllocationHead)
	{
		SAllocationUnitHeader *pTempUnit=NULL;
		pTempUnit=(SAllocationUnitHeader *)CONTAINING_RECORD(pEntry,SAllocationUnitHeader,listEntry);

		if(!bIncremental || !pTempUnit->bDumped)
		{
			if(!pTempUnit->bTemporallyGrouped && (pTempUnit->eBlockType!=EMBT_CRT || g_MonitorizationConfig.bCRTAllocations))
			{
				unsigned char bSameStack=1;
				int x;
				for(x=0;x<MONITORIZATION_MAX_STACK_FRAMES;x++)
				{
					if(memcmp(&pTempUnit->stack.frames[x],&pUnit->stack.frames[x],sizeof(pUnit->stack.frames[0]))!=0)
					{
						bSameStack=0;
						break;
					}
				}
				if(bSameStack)
				{
					pTempUnit->bDumped=1;
					pTempUnit->bTemporallyGrouped=1;
					pStats->dwBytes+=pTempUnit->dwSize;
					pStats->dwUnits++;
					if(pTempUnit->dwSize<pStats->dwMinUnit){pStats->dwMinUnit=pTempUnit->dwSize;}
					if(pTempUnit->dwSize>pStats->dwMaxUnit){pStats->dwMaxUnit=pTempUnit->dwSize;}
				}
			}
		}
		pEntry=pEntry->Flink;
	}
}
*/
void GAMERUNTIME_API DumpMonitorizationLeaksInternal(unsigned char bIncremental)
{	
	char sTemp[1024];
	LIST_ENTRY *pEntry=NULL;

	EnterCriticalSection(&g_csMonitorizationHeap);

	if(g_mInterfaces.size()==0)
	{
		if(bIncremental)
		{
			sprintf(sTemp,"No new interfaces detected (incremental dump)!\n");
		}
		else
		{
			sprintf(sTemp,"No interface leaks detected!\n");
		}
		OutputDebugString(sTemp);
	}
	else
	{
		if(bIncremental)
		{
			sprintf(sTemp,"Dumping new allocations...\n");
		}
		else
		{
			sprintf(sTemp,"Dumping memory leaks...\n");
		}
		OutputDebugString(sTemp);
	}
	

	std::map<void *,CInterface>::iterator i;
	for(i=g_mInterfaces.begin();i!=g_mInterfaces.end();i++)
	{
		CInterface *pInterface=const_cast<CInterface*>(&(i->second));
		if(!bIncremental || !pInterface->bDumped)
		{
			sprintf(sTemp,"Object leaked: %d references %p\n",i->second.dwReferences,i->first);
			OutputDebugString(sTemp);
			DumpStackList(&pInterface->sAddRefStacks,true);
			DumpStackList(&pInterface->sReleaseStacks,false);
		}
	}

	LeaveCriticalSection(&g_csMonitorizationHeap);
}

void GAMERUNTIME_API DumpMonitorizationLeaks()
{	
	DumpMonitorizationLeaksInternal(0);
}

void GAMERUNTIME_API EndMonitorization()
{
	DeleteCriticalSection(&g_csMonitorizationHeap);
	HeapDestroy(g_hMonitorizedInternalHeap);
	g_hMonitorizedInternalHeap=NULL;
	InitializeListHead(&g_MonitorizedModuleHead);
	InitializeListHead(&g_MonitorizedFileHead);
	InitializeListHead(&g_MonitorizedSymbolHead);
}

void DumpIfNecesary()
{
	if(g_MonitorizationConfig.bPeriodicDump)
	{
		if(GetTickCount()>(g_dwLastPeriodicDump+g_MonitorizationConfig.dwPeriodicDumpPeriod*1000))
		{
			DumpMonitorizationLeaksInternal(g_MonitorizationConfig.bPeriodicDumpIncremental);
			g_dwLastPeriodicDump=GetTickCount();
		}
	}
}

void GAMERUNTIME_API RegisterAddRef(void *pPointer)
{
	if(pPointer==NULL){return;}
	EnterCriticalSection(&g_csMonitorizationHeap);
	CInterface *pInterface=&g_mInterfaces[pPointer];
	pInterface->dwReferences++;
	SStack sStack;
	GetStack(&sStack);
	std::set<SStack>::iterator i=pInterface->sAddRefStacks.find(sStack);
	if(i==pInterface->sAddRefStacks.end())
	{
		sStack.nTimes=1;
		g_mInterfaces[pPointer].sAddRefStacks.insert(sStack);
	}
	else
	{
		SStack *pStack=const_cast<SStack *>(&(*i));
		pStack->nTimes++;
	}
	LeaveCriticalSection(&g_csMonitorizationHeap);
}

void GAMERUNTIME_API RegisterRelease(void *pPointer)
{
	if(pPointer==NULL){return;}
	EnterCriticalSection(&g_csMonitorizationHeap);
	CInterface *pInterface=&g_mInterfaces[pPointer];
	pInterface->dwReferences--;
	if(pInterface->dwReferences<=0)
	{
		g_mInterfaces.erase(pPointer);
	}
	else
	{
		SStack sStack;
		GetStack(&sStack);
		std::set<SStack>::iterator i=pInterface->sReleaseStacks.find(sStack);
		if(i==pInterface->sReleaseStacks.end())
		{
			sStack.nTimes++;
			g_mInterfaces[pPointer].sReleaseStacks.insert(sStack);
		}
		else
		{
			SStack *pStack=const_cast<SStack *>(&(*i));
			pStack->nTimes++;
		}
	}

	LeaveCriticalSection(&g_csMonitorizationHeap);
}

#else // WIN32

void GAMERUNTIME_API InitMonitorization(){}
void GAMERUNTIME_API EndMonitorization(){}

void GAMERUNTIME_API RegisterAddRef(void *pInterface){}
void GAMERUNTIME_API RegisterRelease(void *pInterface){}
void GAMERUNTIME_API DumpMonitorizationLeaks(){}

#endif