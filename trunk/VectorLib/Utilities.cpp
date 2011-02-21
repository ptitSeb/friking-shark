// VectorLib.cpp: implementation of the VectorLib class.
//
//////////////////////////////////////////////////////////////////////

#include "./StdAfx.h"
#include <windows.h>
#include <string>
#include <stdarg.h>
#include "Utilities.h"

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

