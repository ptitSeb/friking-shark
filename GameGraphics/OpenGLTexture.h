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
	unsigned m_dwWidth;
	unsigned m_dwHeight;

	unsigned char *m_pBuffer;

	unsigned int m_nTextureIndex;
	unsigned int m_dwColorType;

	std::string m_sFileName;
	std::string m_sAlphaFileName;
	
	bool m_bGenerateMipMaps;

	bool		m_bColorKey;
	CRGBColor	m_vColorKey;
	float		m_fOpacity;

	bool		m_bRenderTarget;
	unsigned int m_nFrameBuffer;
	unsigned int m_nFrameBufferDepth;
	
#ifdef WIN32
// PBuffers are currently supporter only in windows
	HDC			m_hPBufferDC;
	HGLRC		m_hPBufferRC;
	int			m_nPBufferPixelFormatIndex;
	HPBUFFERARB m_hPBuffer;

	HDC			m_hPBufferOldDC;
	HGLRC		m_hPBufferOldRC;
#endif

	bool		m_bDepth;

	void Clear();

	bool LoadFromFile();
	bool Unserialize(ISystemPersistencyNode *piNode);

	bool CreateBackBuffer(bool bDepth,IGenericViewport *piViewport);
	bool CreatePBuffer(bool bDepth);
	bool CreateFrameBuffer(bool bDepth);

public:
	
	BEGIN_PROP_MAP(COpenGLTexture)
		PROP_FLAGS(m_sFileName,"Archivo",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_bGenerateMipMaps,"GenerateMipmaps",true,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_sAlphaFileName,"ArchivoAlpha","",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_bColorKey,"UsarColorKey",0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_vColorKey,"ColorKey",CVector(0,0,0),MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP();
	// IGenericTexture

	std::string	 GetFileName();

	bool				 HasAlphaFile();
	std::string  GetAlphaFileName();

	void GetSize(unsigned *pdwWidth,unsigned *pdwHeight);

	bool		 HasColorKey();
	CVector		 GetColorKey();

	bool		HasAlphaChannel();

	unsigned long GetByteBufferLength();
	void		  *GetByteBuffer();

	CVector		GetPixelColor(unsigned long x, unsigned long y);
	double		GetPixelAlpha(unsigned long x, unsigned long y);
	
	virtual bool Load(std::string sFileName,CVector *pColorKey,std::string *pAlphaFile,float fOpacity,bool bGenerateMipmaps);
	virtual bool Create( unsigned nWidth,unsigned nHeight,IGenericViewport *piViewport );
	virtual bool CreateDepth( unsigned nWidth,unsigned nHeight ,IGenericViewport *piViewport);
	virtual bool StartRenderingToTexture();
	virtual void StopRenderingToTexture();

	virtual bool PrepareTexture(IGenericRender *piRender,int nTextureLevel);
	virtual void UnprepareTexture(IGenericRender *piRender,int nTextureLevel);

	COpenGLTexture(void);
	~COpenGLTexture(void);
};
