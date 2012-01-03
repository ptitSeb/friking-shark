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


#pragma once
#include "GameGraphics.h"

class COpenGLTexture : virtual public CSystemObjectBase,virtual public IGenericTexture
{
	bool m_bOpenGLSetupPending;
	bool m_bResident;
	
	unsigned m_dwWidth;
	unsigned m_dwHeight;

	unsigned char *m_pBuffer;

	unsigned int m_nTextureIndex;
	unsigned int m_dwColorType;
	
	std::string m_sFileName;
	
	bool m_bGenerateMipMaps;

	bool		m_bRenderTarget;
	unsigned int m_nFrameBuffer;
	unsigned int m_nFrameBufferDepth;
	
	bool		m_bDepth;

	void Clear();

	bool LoadFromFile(bool bResident);
	bool Unserialize(ISystemPersistencyNode *piNode);

	bool CreateBackBuffer(bool bDepth,IGenericViewport *piViewport);
	bool CreatePBuffer(bool bDepth);
	bool CreateFrameBuffer(bool bDepth);
	
	bool SetupOpenGL(bool bFailureAllowed);

public:
	
	BEGIN_PROP_MAP(COpenGLTexture)
		PROP_FLAGS(m_sFileName,"Archivo",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_bGenerateMipMaps,"GenerateMipmaps",true,MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP();
	// IGenericTexture

	std::string	 GetFileName();

	void GetSize(unsigned *pdwWidth,unsigned *pdwHeight);

	bool		HasAlphaChannel();

	unsigned long GetByteBufferLength();
	void		  *GetByteBuffer();

	CVector		GetPixelColor(unsigned long x, unsigned long y);
	double		GetPixelAlpha(unsigned long x, unsigned long y);
	
	bool Load(std::string sFileName,bool bGenerateMipmaps, bool bResident);
	bool Create( unsigned nWidth,unsigned nHeight,IGenericViewport *piViewport );
	bool CreateDepth( unsigned nWidth,unsigned nHeight ,IGenericViewport *piViewport);
	bool StartRenderingToTexture();
	void StopRenderingToTexture();

	void ReleaseResidentData();
	bool PrepareTexture(int nTextureLevel);
	void UnprepareTexture(int nTextureLevel);

	COpenGLTexture(void);
	~COpenGLTexture(void);
};
