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

#include "./stdafx.h"
#include <stdint.h>
#include "OpenGLGraphics.h"
#include "OpenGLTexture.h"
#define PNG_SKIP_SETJMP_CHECK
#define PNG_NO_PEDANTIC_WARNINGS
#include <png.h>

bool LoadPngFile(const char *pFileName,unsigned int *pnWidth,unsigned int *pnHeight,unsigned char **ppPixels,bool bForceAlpha,unsigned int *pdwColorType)
{
	png_structp pPNGHeader=NULL;
	png_infop pPNGInfo=NULL;

	bool bOk;
	AFILE *pFile=afopen(pFileName,"rb");
	bOk=(pFile!=NULL);
	if(bOk){pPNGHeader=png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);bOk=(pPNGHeader!=NULL);}
	if(bOk){pPNGInfo=png_create_info_struct(pPNGHeader);bOk=(pPNGInfo!=NULL);}
	if(bOk){bOk=(setjmp(png_jmpbuf(pPNGHeader))==0);}
	if(bOk){png_init_io(pPNGHeader, afgetfile(pFile));}
	if(bOk){png_set_sig_bytes(pPNGHeader, 0);}
	if(bOk){png_read_png(pPNGHeader, pPNGInfo, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);}
	if(bOk){bOk=(pPNGInfo->color_type==PNG_COLOR_TYPE_RGBA || pPNGInfo->color_type==PNG_COLOR_TYPE_RGB);}
	if(bOk)
	{
		*pnWidth= pPNGInfo->width;
		*pnHeight= pPNGInfo->height;
		unsigned int nFileBits=pPNGInfo->color_type==PNG_COLOR_TYPE_RGBA?32:24;
		unsigned int nRequestedBits=(bForceAlpha)?32:nFileBits;
		*pdwColorType=(nRequestedBits==32)?GL_RGBA:GL_RGB;
	
		if(nFileBits==nRequestedBits)
		{
			unsigned int nRowSize = png_get_rowbytes(pPNGHeader, pPNGInfo);
			*ppPixels = new unsigned char [nRowSize* pPNGInfo->height];
			png_bytepp ppRows = png_get_rows(pPNGHeader, pPNGInfo);
			for(unsigned int y=0;y<pPNGInfo->height;y++) 
			{
				memcpy(*ppPixels+(nRowSize* (pPNGInfo->height-1-y)), ppRows[y], nRowSize);
			}
		}
		else
		{
			int nDestBytesPerColor=(nRequestedBits>>3);
			int nFileBytesPerColor=(nFileBits>>3);
			
			png_get_rowbytes(pPNGHeader, pPNGInfo);
			unsigned int nDestRowSize = nDestBytesPerColor*pPNGInfo->width;
			*ppPixels = new unsigned char [nDestBytesPerColor*pPNGInfo->width*pPNGInfo->height];

			if(nDestBytesPerColor>nFileBytesPerColor)
			{
				// Alpha channel to full opacity if missing.
				memset(*ppPixels,0xFF,nDestBytesPerColor*pPNGInfo->width*pPNGInfo->height);
			}
		
			png_bytepp ppRows = png_get_rows(pPNGHeader, pPNGInfo);
			for(unsigned int y=0;y<pPNGInfo->height;y++) 
			{
				unsigned char *pFile=ppRows[y];
				unsigned char *pDest=(*ppPixels)+(nDestRowSize* (pPNGInfo->height-1-y));
				
				for(unsigned int x=0;x<pPNGInfo->width;x++) 
				{
					pDest[0]=pFile[0];
					pDest[1]=pFile[1];
					pDest[2]=pFile[2];
					pDest+=nDestBytesPerColor;
					pFile+=nFileBytesPerColor;
				}
			}
		}
	}

	if(pPNGHeader){png_destroy_read_struct(&pPNGHeader, &pPNGInfo,NULL);}
	if(pFile){afclose(pFile);}	
	return bOk;	
}


bool LoadImageHelper(std::string sFile,unsigned *pOpenGLSkinWidth,unsigned *pOpenGLSkinHeight,unsigned char **ppBuffer,bool bForceAlpha,unsigned int *pdwColorType)
{
	*pOpenGLSkinWidth=0;
	*pOpenGLSkinHeight=0;
	*ppBuffer=NULL;
	*pdwColorType=GL_RGB;

	std::string path=sFile;
	char pFileFolder[MAX_PATH];
	GetFileFolder(sFile.c_str(),pFileFolder);
	if(pFileFolder[0]==0 || strcmp(pFileFolder,".")==0)
	{
		std::string sTemp="Textures/";
		sTemp+=path;
		path=sTemp;
	}
	return LoadPngFile(path.c_str(),pOpenGLSkinWidth,pOpenGLSkinHeight,ppBuffer,bForceAlpha,pdwColorType);
}

COpenGLTexture::COpenGLTexture(void)
{
	m_bOpenGLSetupPending=false;
	m_bResident=false;
	m_bDepth=false;
	m_bRenderTarget=false;
	m_dwWidth=0;
	m_dwHeight=0;
	m_pBuffer=NULL;

	m_dwColorType=GL_RGB;

	m_nTextureIndex=0;
	m_nFrameBuffer=0;
	m_nFrameBufferDepth=0;
	m_bGenerateMipMaps=true;
}

void COpenGLTexture::Clear()
{
	
	if(m_pBuffer)
	{
		delete [] m_pBuffer;
		m_pBuffer=NULL;
	}
#ifndef ANDROID_GLES1
	if(m_nFrameBuffer)
	{
		glDeleteFramebuffersEXT(1,&m_nFrameBuffer);
		m_nFrameBuffer=0;
	}
	if(m_nFrameBufferDepth)
	{
		glDeleteRenderbuffersEXT(1,&m_nFrameBufferDepth);
		m_nFrameBufferDepth=0;
	}
#endif
	if(m_nTextureIndex)
	{
		glDeleteTextures(1,&m_nTextureIndex);
		m_nTextureIndex=0;
	}
	m_dwColorType=GL_RGB;
	
	m_bOpenGLSetupPending=false;
	m_bResident=false;
}

COpenGLTexture::~COpenGLTexture(void)
{
	Clear();
}

std::string	COpenGLTexture::GetFileName(){return m_sFileName;}
void		COpenGLTexture::GetSize(unsigned *pdwWidth,unsigned *pdwHeight)
{
	*pdwWidth=m_dwWidth;
	*pdwHeight=m_dwHeight;
}

bool COpenGLTexture::LoadFromFile(bool bResident)
{
	int nStartTime=GetTimeStamp();
	bool bForceAlpha=(m_bRenderTarget);
	bool bResult=true;
	m_dwColorType=GL_RGB;
	bResult=LoadImageHelper(m_sFileName,&m_dwWidth,&m_dwHeight,&m_pBuffer,bForceAlpha,&m_dwColorType);
	
	if(bResult)
	{
		unsigned int nWidthBits=0,nHeightBits=0;
		unsigned int nTempWidth=m_dwWidth,nTempHeight=m_dwHeight;
		while(nTempWidth){nWidthBits+=nTempWidth&1;nTempWidth>>=1;}
		while(nTempHeight){nHeightBits+=nTempHeight&1;nTempHeight>>=1;}
		
		if(nWidthBits!=1 || nHeightBits!=1)
		{
			RTTRACE("COpenGLTexture::LoadFromFile -> WARNING: Texture %s has non power of two dimensions %dx%d, this can be problematic on some systems",m_sFileName.c_str(),m_dwWidth,m_dwHeight);
		}
		if(m_dwWidth>1024 || m_dwHeight>1024)
		{
			RTTRACE("COpenGLTexture::LoadFromFile -> WARNING: Texture %s has dimensions greater than 1024 pixels %dx%d, this can be problematic on some systems",m_sFileName.c_str(),m_dwWidth,m_dwHeight);
		}
		
		RTTRACE("COpenGLTexture::LoadFromFile -> Loaded texture %s, %dx%d (%d ms)",m_sFileName.c_str(),m_dwWidth,m_dwHeight,GetTimeStamp()-nStartTime);
		
		m_bRenderTarget=false;
		m_bResident=bResident;
		
		// OpenGL setup can fail if the texture is loaded before a render context has been created
		// if this is the case we must retry the first time the texture is prepared for rendering.
		
		m_bOpenGLSetupPending=!SetupOpenGL(true);
		if(!m_bOpenGLSetupPending)
		{
			if(!m_bResident){delete [] m_pBuffer;m_pBuffer=NULL;}
		}
	}
	else
	{
		RTTRACE("COpenGLTexture::LoadFromFile -> Failed to load texture %s",m_sFileName.c_str());
	}
	
	return bResult;
}

bool COpenGLTexture::SetupOpenGL(bool bFailureAllowed)
{
	if(m_nTextureIndex!=0){return true;}
	
	glGenTextures(1,&m_nTextureIndex);
	if(m_nTextureIndex)
	{
		glBindTexture(GL_TEXTURE_2D,m_nTextureIndex);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		#ifdef ANDROID
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_bGenerateMipMaps?GL_LINEAR:GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, m_dwColorType, m_dwWidth,m_dwHeight, 0,m_dwColorType, GL_UNSIGNED_BYTE, m_pBuffer);
		
		#ifdef ANDROID_GLES1
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_bGenerateMipMaps?GL_LINEAR:GL_NEAREST);
		#else
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_bGenerateMipMaps?GL_LINEAR_MIPMAP_LINEAR:GL_NEAREST);
		if(m_bGenerateMipMaps){glGenerateMipmap(GL_TEXTURE_2D);}
		#endif
		//int error=glGetError();
		//if(error!=GL_NO_ERROR){RTTRACE("COpenGLTexture::SetupOpenGL -> glTexImage2D, error for %s: %d",m_sFileName.c_str(),error);}
		#else
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_bGenerateMipMaps?GL_LINEAR_MIPMAP_LINEAR:GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_bGenerateMipMaps?GL_LINEAR:GL_NEAREST);
		if(m_bGenerateMipMaps)
		{
			gluBuild2DMipmaps(GL_TEXTURE_2D,m_dwColorType,m_dwWidth,m_dwHeight,m_dwColorType,GL_UNSIGNED_BYTE,m_pBuffer);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, m_dwColorType, m_dwWidth,m_dwHeight, 0,m_dwColorType, GL_UNSIGNED_BYTE, m_pBuffer);
		}
		#endif
	}
	else
	{
		if(!bFailureAllowed)
		{
			RTTRACE("COpenGLTexture::SetupOpenGL -> Failed to create texture id for %s",m_sFileName.c_str());
		}
	}
	return m_nTextureIndex!=0;
}

bool COpenGLTexture::Unserialize(ISystemPersistencyNode *piNode)
{
	bool bResult=CSystemObjectBase::Unserialize(piNode);
	if(bResult){bResult=LoadFromFile(false);}
	return bResult;
}
bool COpenGLTexture::HasAlphaChannel(){return (m_dwColorType==GL_RGBA);}

unsigned long COpenGLTexture::GetByteBufferLength()
{
	return HasAlphaChannel()?m_dwHeight*m_dwWidth*4:m_dwHeight*m_dwWidth*3;
}
void *COpenGLTexture::GetByteBuffer()
{
	return m_pBuffer;
}

bool COpenGLTexture::Load(std::string sFileName,bool bGenerateMipmaps, bool bResident)
{
	Clear();
	m_sFileName=sFileName;
	m_bGenerateMipMaps=bGenerateMipmaps;
	if(m_pBuffer)
	{
		delete [] m_pBuffer;
		m_pBuffer=NULL;
	}
	return LoadFromFile(bResident);
}

CVector COpenGLTexture::GetPixelColor( unsigned long x, unsigned long y )
{
	if(m_pBuffer==NULL){return Origin;}
	CVector vResult;
	if(x<m_dwWidth && y<m_dwHeight)
	{
		unsigned long nPixel=x+(y*m_dwWidth);
		unsigned long nPixelSize=(HasAlphaChannel()?4:3);
		unsigned char *pPixel=m_pBuffer+(nPixel*nPixelSize);
		for(int p=0;p<3;p++){vResult.c[p]=((double)pPixel[p])/255.0;}		 
	}
	return vResult;
}

double COpenGLTexture::GetPixelAlpha( unsigned long x, unsigned long y )
{
	if(m_pBuffer==NULL){return 1.0;}
	if(!HasAlphaChannel()){return 1.0;}
	if(x<m_dwWidth && y<m_dwHeight)
	{
		unsigned long nPixel=x+(y*m_dwWidth);
		unsigned long nPixelSize=4;
		unsigned char *pPixel=m_pBuffer+(nPixel*nPixelSize);
		return ((double)pPixel[3])/255.0;
	}
	return 1.0;
}

bool COpenGLTexture::CreateFrameBuffer(bool bDepth)
{
	// FrameBuffer Implementation
#ifdef ANDROID_GLES1
	return false;
#else
	
	glGenTextures(1,&m_nTextureIndex);
	if(m_nTextureIndex==0){RTTRACE("COpenGLTexture::CreateFrameBuffer -> Failed to create texture id for frame buffer object");}
	if(!bDepth)
	{
		glGenRenderbuffersEXT(1, &m_nFrameBufferDepth);
		if(m_nFrameBufferDepth==0){RTTRACE("COpenGLTexture::CreateFrameBuffer -> Failed to create depth buffer id for frame buffer object");}
	}
	glGenFramebuffersEXT(1,&m_nFrameBuffer);
	if(m_nFrameBuffer==0){RTTRACE("COpenGLTexture::CreateFrameBuffer -> Failed to create frame buffer id");}
	
	if(m_nTextureIndex && m_nFrameBuffer && (m_nFrameBufferDepth|| bDepth))
	{
		m_bRenderTarget=true;

		glBindTexture(GL_TEXTURE_2D,m_nTextureIndex);
		if(bDepth)
		{
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,  GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,  GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#ifndef ANDROID
			glTexImage2D(GL_TEXTURE_2D, 0,  GL_DEPTH_COMPONENT, m_dwWidth,m_dwHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
#else
			glTexImage2D(GL_TEXTURE_2D, 0,  GL_DEPTH_COMPONENT, m_dwWidth,m_dwHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, 0);
#endif
		}
		else
		{
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_dwWidth,m_dwHeight, 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);
		}

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,m_nFrameBuffer);
		if(!bDepth)
		{		
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT,GL_TEXTURE_2D,m_nTextureIndex,0);
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,GL_RENDERBUFFER_EXT, m_nFrameBufferDepth);
		}
		else
		{
#ifndef ANDROID
			// Some systems require no draw or read buffers for depth-only frame buffers
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
#endif
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,GL_TEXTURE_2D, m_nTextureIndex, 0);
		}

		GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
		bool bOk=(status == GL_FRAMEBUFFER_COMPLETE_EXT);
		if(!bOk){RTTRACE("COpenGLTexture::CreateFrameBuffer -> Failed to complete frame buffer, error 0x%08x",status);}
		return bOk;
	}
	else
	{
		RTTRACE("COpenGLTexture::CreateFrameBuffer -> Failed to create frame buffer object");
		return false;
	}
#endif
}


bool COpenGLTexture::Create( unsigned nWidth,unsigned nHeight,IGenericViewport *piViewport)
{
	Clear();

	int nMaxTextureSize=0;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE,&nMaxTextureSize);
	if((int)m_dwWidth>nMaxTextureSize){m_dwWidth=nMaxTextureSize;}
	if((int)m_dwHeight>nMaxTextureSize){m_dwHeight=nMaxTextureSize;}

	m_sFileName="";
	m_dwWidth=nWidth;
	m_dwHeight=nHeight;
	m_bDepth=false;

	bool bOk=false;
	if(!bOk){bOk=CreateFrameBuffer(false);}
	if(!bOk){Clear();}
	return bOk;
}

bool COpenGLTexture::CreateDepth( unsigned nWidth,unsigned nHeight,IGenericViewport *piViewport)
{
	Clear();

	m_sFileName="";
	m_dwWidth=nWidth;
	m_dwHeight=nHeight;
	m_bDepth=true;

	bool bOk=false;
	if(!bOk){bOk=CreateFrameBuffer(true);}
	if(!bOk){Clear();}
	return bOk;
}

bool COpenGLTexture::StartRenderingToTexture()
{
#ifndef ANDROID_GLES1
	// FrameBuffer Implementation
	if(m_nFrameBuffer)
	{
		//glDisable(GL_SCISSOR_TEST);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,m_nFrameBuffer);
		return true;
	}
	else if(m_bDepth && m_nFrameBufferDepth)
	{
		//glDisable(GL_SCISSOR_TEST);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,m_nFrameBufferDepth);
		return true;
	}
#endif
	return false;
}
void COpenGLTexture::StopRenderingToTexture()
{
	// FrameBuffer Implementation
#ifndef ANDROID_GLES1
	if(m_nFrameBuffer)
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
	}
	else if(m_bDepth && m_nFrameBufferDepth)
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
	}
#endif
}

bool COpenGLTexture::PrepareTexture(int nTextureLevel)
{
	if(m_bOpenGLSetupPending)
	{
		SetupOpenGL(false);
		m_bOpenGLSetupPending=false;
		
		if(!m_bResident){delete [] m_pBuffer;m_pBuffer=NULL;}
	}
	
	if(m_nTextureIndex)
	{
		glBindTexture(GL_TEXTURE_2D,m_nTextureIndex);
	}
	return true;
}

void COpenGLTexture::UnprepareTexture(int nTextureLevel)
{
}

void COpenGLTexture::ReleaseResidentData()
{
	if(m_bOpenGLSetupPending)
	{
		m_bResident=false;
	}
	else
	{
		delete [] m_pBuffer;
		m_pBuffer=NULL;
	}
}
