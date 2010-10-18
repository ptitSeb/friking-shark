// IniFiles.cpp: implementation of the CIniFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IniFiles.h"
#include "VectorLib.h"
#include <io.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIniFile::CIniFile()
{
	m_sFileName[0]=0;
}

CIniFile::~CIniFile()
{
	Close();
}

bool CIniFile::GetString_Internal(char *pString,std::string &sResult)
{
	char path[MAX_PATH]={0};
	char filename[_MAX_FNAME]={0};
	char sValue[MAX_PATH];
	_splitpath(pString,NULL,path,filename,NULL);
	int nPath=strlen(path);
	if(nPath){if(path[nPath-1]=='\\'){path[nPath-1]=0;}}
	int nBytes=GetPrivateProfileString(path,filename,"",sValue,sizeof(sValue),m_sFileName);
	if(nBytes==0){return false;}
	sResult=sValue;
	return true;
}

bool CIniFile::SetString_Internal(char *pString,const char *pValue)
{
	char path[MAX_PATH]={0};
	char filename[_MAX_FNAME]={0};
	_splitpath(pString,NULL,path,filename,NULL);
	int nPath=strlen(path);
	if(nPath){if(path[nPath-1]=='\\'){path[nPath-1]=0;}}
	return WritePrivateProfileString(path,filename,pValue,m_sFileName)!=0;
}

std::string	CIniFile::GetString(char *pString,char *pDefault)
{
	std::string str;
	if(!GetString_Internal(pString,str) && pDefault){str=pDefault;}
	return str;
}

bool	CIniFile::GetBool(char *pString,bool bDefault)
{
	std::string str;
	if(!GetString_Internal(pString,str)){return bDefault;}
	return atoi(str.c_str())!=0;
}

int		CIniFile::GetInt(char *pString,int nDefault)
{
	std::string str;
	if(!GetString_Internal(pString,str)){return nDefault;}
	return atoi(str.c_str());
}

double	CIniFile::GetDouble(char *pString,double dDefault)
{
	std::string str;
	if(!GetString_Internal(pString,str)){return dDefault;}
	return atof(str.c_str());
}

COLORREF CIniFile::GetColor(char *pString,COLORREF cDefault,bool bIsRGB)
{
	CVector vColor=GetVector(pString,RGBToVector(cDefault));
	if(bIsRGB)
	{
		return RGB(vColor[0],vColor[1],vColor[2]);
	}
	else
	{
		return VectorToRGB(&vColor);
	}
}

CVector CIniFile::GetVector(char *pString,CVector vDefaultPos)
{
	std::string str;
	if(!GetString_Internal(pString,str)){return vDefaultPos;}
	char sTemp[1024]={0};
	strcpy(sTemp,str.c_str());
	CVector vPos;
	char *pToken=strtok(sTemp,", ");
	if(pToken){vPos.c[0]=atof(pToken);pToken=strtok(NULL,", ");}
	if(pToken){vPos.c[1]=atof(pToken);pToken=strtok(NULL,", ");}
	if(pToken){vPos.c[2]=atof(pToken);}
	return vPos;
}

void CIniFile::SetString(char *pString,const char *pValue)
{
	SetString_Internal(pString,pValue);
}

void CIniFile::SetBool(char *pString,bool bValue)
{
	char sTemp[MAX_PATH];
	sprintf(sTemp,"%d",bValue);
	SetString_Internal(pString,sTemp);
}

void CIniFile::SetInt(char *pString,int nValue)
{
	char sTemp[MAX_PATH];
	sprintf(sTemp,"%d",nValue);
	SetString_Internal(pString,sTemp);
}

void CIniFile::SetDouble(char *pString,double dValue)
{
	char sTemp[MAX_PATH];
	sprintf(sTemp,"%f",dValue);
	SetString_Internal(pString,sTemp);
}

void CIniFile::SetColor(char *pString,COLORREF cValue,bool bIsRGB)
{
	char sTemp[MAX_PATH];
	if(bIsRGB)
	{
		sprintf(sTemp,"%d,%d,%d",GetRValue(cValue),GetGValue(cValue),GetBValue(cValue));
	}
	else
	{
		float fColor[3];
		RGBToFloat(cValue,fColor);
		sprintf(sTemp,"%f,%f,%f",fColor[0],fColor[1],fColor[2]);
	}
	SetString_Internal(pString,sTemp);
}

void CIniFile::SetVector(char *pString,CVector vValue)
{
	char sTemp[MAX_PATH];
	sprintf(sTemp,"%f,%f,%f",vValue.c[0],vValue.c[1],vValue.c[2]);
	SetString_Internal(pString,sTemp);
}


bool CIniFile::Open(char *pFileName)
{
	char sDrive[_MAX_DRIVE]={0};
	char sPath[MAX_PATH]={0};
	_splitpath(pFileName,sDrive,NULL,NULL,NULL);
	if(sDrive[0]==0)
	{
		GetCurrentDirectory(MAX_PATH,sPath);
		strcat(sPath,"\\");
	}
	strcat(sPath,pFileName);
	strcpy(m_sFileName,sPath);
	return true;
}

void CIniFile::Close()
{
	m_sFileName[0]=0;
}

