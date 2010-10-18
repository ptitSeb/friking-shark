// Log.cpp: implementation of the CLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GameEngine.h"
#include "Log.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLog::CLog()
{
	m_hFile=CreateFile("Log.txt",GENERIC_WRITE,0,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(m_hFile!=INVALID_HANDLE_VALUE)
	{
		DWORD dwLength=GetFileSize(m_hFile,NULL);
		if(dwLength){Log("");}
		SetFilePointer(m_hFile,0,NULL,SEEK_END);
	}
}

CLog::~CLog()
{
	if(m_hFile!=INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
		m_hFile=INVALID_HANDLE_VALUE;
	}
}

void CLog::Log(const char *pText)
{
	if(m_hFile==INVALID_HANDLE_VALUE){return;}
	string sText;
	sText=pText;
	sText+="\r\n";
	OutputDebugString(sText.c_str());
	DWORD dwWritten=0;
	WriteFile(m_hFile,sText.c_str(),sText.size(),&dwWritten,NULL);
}

void CLog::LogErrorAndExit(const char *pCaption,const char *pText)
{
	if(m_hFile==INVALID_HANDLE_VALUE){return;}

	string sText;
	sText="ERROR : ";
	sText+=pText;
	Log(sText.c_str());
	
	ShowCursor(TRUE);
	HWND hWnd=CGlobals::g_pScreen?CGlobals::g_pScreen->GetWindow():NULL;
	MessageBox(CGlobals::g_pScreen->GetWindow(),sText.c_str(),pCaption,MB_ICONSTOP|MB_OK);
	exit(0);
}
