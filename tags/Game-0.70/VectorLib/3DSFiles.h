// 3DSFiles.h: interface for the C3DSFiles class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_3DSFILES_H__F896600F_BB9E_4A01_81D9_84C4F67C0C23__INCLUDED_)
#define AFX_3DSFILES_H__F896600F_BB9E_4A01_81D9_84C4F67C0C23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class C3DSFileType:public C3DSFileTypeBase
{
	FILE			*m_pFile;
	char			*m_pBuffer;
	bool			m_bBinaryFile;

	unsigned char	ReadByte();
	float			ReadFloat();
	unsigned int			ReadDWord();
	unsigned short			ReadWord();
	CVector			ReadVector();
	std::string		ReadString();

	bool Open3DS(const char *sFileName);
	bool OpenASE(const char *sFileName);

public:

	bool Open(const char *pFile);

	C3DSFileType();
};

#endif // !defined(AFX_3DSFILES_H__F896600F_BB9E_4A01_81D9_84C4F67C0C23__INCLUDED_)
