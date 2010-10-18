// VectorLib.cpp: implementation of the VectorLib class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <windows.h>
#include <string>
#include "utilities.h"


void _MRT(const char *format, ...)
{
	va_list vargs;
	va_start (vargs,format);
	char pTempBuffer[2400];
	int res=_vsnprintf(pTempBuffer,1023,format,vargs);
	if(res==-1){pTempBuffer[1022]=0;res=1022;}
	va_end (vargs);

	pTempBuffer[res]='\n';
	pTempBuffer[res+1]=0;
	OutputDebugString(pTempBuffer);
}

void SkipCommentsStringsAndSpaces(char *pBuffer,DWORD *pOffset,DWORD bufLen)
{
	bool inLineComment=false;
	bool inParagraphComment=false;
	bool inString=false;
	DWORD x;

	for(x=(*pOffset);x<bufLen;x++)
	{
		if(inLineComment)
		{
			if(x!=0 && pBuffer[x]=='\n')
			{inLineComment=false;}
		}
		else
			if(inParagraphComment)
			{
				if(x!=0 && pBuffer[x-1]=='*' && pBuffer[x]=='/')
				{inParagraphComment=false;}
			}
			else
				if(inString)
				{
					// this -> \" is a colon character inside a string.
					if(x==0 || pBuffer[x-1]!='\\' || pBuffer[x]!='"')
					{if(pBuffer[x]=='"'){inString=false;}}
				}
				else
				{
					if(x+1<bufLen)
					{
						if(pBuffer[x]=='/' && pBuffer[x+1]=='*'){inParagraphComment =true;}
						if(pBuffer[x]=='/' && pBuffer[x+1]=='/'){inLineComment =true;}
					}	
					if(pBuffer[x]=='"'){inString=true;}
					
					if(!inParagraphComment && !inLineComment && !inString)
					{
						if(pBuffer[x]!=' ' && pBuffer[x]!='\n' && pBuffer[x]!='\r'  && pBuffer[x]!='\t'){break;}
					}
				}
	}
	(*pOffset)=x;
}

bool GetFileModificationTime(char *pfile,FILETIME *pModificationTime)
{
	bool bOk=FALSE;
	memset(pModificationTime,0,sizeof(FILETIME));
	HANDLE hFile=CreateFile(pfile,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile!=INVALID_HANDLE_VALUE)
	{
		bOk=GetFileTime(hFile,NULL,NULL,pModificationTime);
		CloseHandle(hFile);
		hFile=INVALID_HANDLE_VALUE;
	}
	return bOk;
}