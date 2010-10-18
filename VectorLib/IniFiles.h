// IniFiles.h: interface for the CIniFiles class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INIFILES_H__6A94D4A2_2853_4A71_B382_E03330C36E64__INCLUDED_)
#define AFX_INIFILES_H__6A94D4A2_2853_4A71_B382_E03330C36E64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <string>
#include "VectorLib.h"

class CIniFile  
{
	char m_sFileName[MAX_PATH];

	bool GetString_Internal(char *pString,std::string &sResult);
	bool SetString_Internal(char *pString,const char *pValue);
public:

	std::string	GetString(char *pString,char *pDefault=NULL);
	bool		GetBool(char *pString,bool bDefault=false);
	int			GetInt(char *pString,int nDefault=0);
	double		GetDouble(char *pString,double dDefault=0.0);
	COLORREF	GetColor(char *pString,COLORREF cDefault,bool bIsRGB);
	CVector		GetVector(char *pString,CVector vDefaultPos);

	void 		SetString(char *pString,const char *pValue);
	void 		SetBool(char *pString,bool bValue);
	void 		SetInt(char *pString,int nValue);
	void 		SetDouble(char *pString,double dValue);
	void 		SetColor(char *pString,COLORREF cValue,bool bIsRGB);
	void 		SetVector(char *pString,CVector vValue);

	bool Open(char *pFileName);
	void Close();

	CIniFile();
	virtual ~CIniFile();

};

#endif // !defined(AFX_INIFILES_H__6A94D4A2_2853_4A71_B382_E03330C36E64__INCLUDED_)
