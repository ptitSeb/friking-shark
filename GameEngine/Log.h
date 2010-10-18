// Log.h: interface for the CLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOG_H__02BED279_0A43_4DD6_96F6_B1ED9EFCA6CE__INCLUDED_)
#define AFX_LOG_H__02BED279_0A43_4DD6_96F6_B1ED9EFCA6CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CLog  
{
	HANDLE	m_hFile;

public:

	void Log(const char *pText);
	void LogErrorAndExit(const char *pCaption,const char *pText);

	CLog();
	virtual ~CLog();
};

#endif // !defined(AFX_LOG_H__02BED279_0A43_4DD6_96F6_B1ED9EFCA6CE__INCLUDED_)
