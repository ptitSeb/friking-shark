// stdafx.cpp : source file that includes just the standard includes
// GameRunTimeLib.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include <stdio.h>
#include <stdarg.h>

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

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
#ifdef WIN32
    OutputDebugString(pTempBuffer);
#else
	printf("%s",pTempBuffer);
#endif 
}
