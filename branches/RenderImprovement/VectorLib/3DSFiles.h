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
	AFILE			*m_pFile;
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
