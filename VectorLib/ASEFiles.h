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
	unsigned int			ReadDWord();
	unsigned short			ReadWord();
	CVector			ReadVector();
	std::string		ReadString();
	void			ProcessObjectFrameSubMaterials(S3DSObject *pObject,S3DSFrame *pFrame);
public:

	bool Open(const char *pFile);

	CASEFileType();
};

#endif // !defined(AFX_ASEFILETYPE_H__9BBA9929_667E_4751_9FEE_B090BDE8AC32__INCLUDED_)
