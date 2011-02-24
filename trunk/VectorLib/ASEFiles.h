// ASEFileType.h: interface for the CASEFileType class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASEFILETYPE_H__9BBA9929_667E_4751_9FEE_B090BDE8AC32__INCLUDED_)
#define AFX_ASEFILETYPE_H__9BBA9929_667E_4751_9FEE_B090BDE8AC32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CASEFileType :public C3DSFileTypeBase
{
	FILE			*m_pFile;
	char			*m_pBuffer;

	unsigned char	ReadByte();
	float			ReadFloat();
	DWORD			ReadDWord();
	WORD			ReadWord();
	CVector			ReadVector();
	std::string		ReadString();
	void			ProcessObjectFrameSubMaterials(S3DSObject *pObject,S3DSFrame *pFrame);
public:

	bool Open(const char *pFile);

	CASEFileType();
};

#endif // !defined(AFX_ASEFILETYPE_H__9BBA9929_667E_4751_9FEE_B090BDE8AC32__INCLUDED_)
