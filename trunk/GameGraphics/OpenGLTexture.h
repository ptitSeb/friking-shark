#pragma once
#include "GameGraphics.h"

class COpenGLTexture : virtual public CSystemObjectBase,virtual public IGenericTexture,virtual public IOpenGLTexture
{
	unsigned m_dwWidth;
	unsigned m_dwHeight;

	unsigned char *m_pBuffer;

	unsigned m_nTextureIndex;

	std::string m_sFileName;
	std::string m_sAlphaFileName;

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

	bool CreateBackBuffer(bool bDepth);
	bool CreatePBuffer(bool bDepth);
	bool CreateFrameBuffer(bool bDepth);

public:
	
	BEGIN_PROP_MAP(COpenGLTexture)
		PROP_FLAGS(m_sFileName,"Archivo",MRPF_NORMAL|MRPF_OPTIONAL)
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


	// IOpenGLTexture

	virtual unsigned GetOpenGLIndex();

	virtual bool Load(string sFileName,CVector *pColorKey,string *pAlphaFile,float fOpacity);
	virtual bool Create( unsigned nWidth,unsigned nHeight );
	virtual bool CreateDepth( unsigned nWidth,unsigned nHeight );
	virtual bool StartRenderingToTexture();
	virtual void StopRenderingToTexture();

	virtual bool PrepareTexture(IGenericRender *piRender,int nTextureLevel);
	virtual void UnprepareTexture(IGenericRender *piRender,int nTextureLevel);

	COpenGLTexture(void);
	~COpenGLTexture(void);
};
