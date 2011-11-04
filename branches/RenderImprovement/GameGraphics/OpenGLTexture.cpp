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

#pragma pack(push,1)
struct BMPFILEHEADER
{
	uint16_t bfType; 
	uint32_t bfSize; 
	uint16_t bfReserved1; 
	uint16_t bfReserved2; 
	uint32_t bfOffBits; 
};
struct BMPINFOHEADER 
{ 
	uint32_t biSize; 
	uint32_t biWidth; 
	uint32_t biHeight; 
	uint16_t biPlanes; 
	uint16_t biBitCount;
	uint32_t biCompression; 
	uint32_t biSizeImage; 
	uint32_t biXPelsPerMeter; 
	uint32_t biYPelsPerMeter; 
	uint32_t biClrUsed; 
	uint32_t biClrImportant; 
}; 
#pragma pack(pop)

bool LoadBMPFile(const char *pFileName,unsigned int *pnWidth,unsigned int *pnHeight,unsigned char **ppPixels,bool bForceAlpha,unsigned int *pdwColorType)
{
	BMPFILEHEADER fileHeader={0};
	BMPINFOHEADER fileInfo={0};

	FILE *pFile=fopen(pFileName,"rb");
	if(!pFile){return false;}
	// Make room for the header
	bool bOk=(fread(&fileHeader,sizeof(fileHeader),1,pFile)==1);
	if(bOk){bOk=(fileHeader.bfType == 0x4d42);}
	if(bOk){bOk=(fread(&fileInfo,sizeof(fileInfo),1,pFile)==1);}
	if(bOk){bOk=(fileInfo.biSize==sizeof(fileInfo));}
	if(bOk){bOk=(fileInfo.biPlanes==1);}
	if(bOk){bOk=(fileInfo.biBitCount==24 || fileInfo.biBitCount==32);}
	if(bOk){bOk=(fileInfo.biCompression==0);} // BI_RGB
	if(bOk){fseek(pFile,fileHeader.bfOffBits,SEEK_SET);}
	if(bOk)
	{
		unsigned char *pFileBits=(unsigned char*)malloc(fileInfo.biSizeImage);
		if(bOk){bOk=(fread(pFileBits,fileInfo.biSizeImage,1,pFile)==1);}
		if(bOk)
		{
			int nRequestedBits=bForceAlpha?32:fileInfo.biBitCount;
			*pdwColorType=(nRequestedBits==32)?GL_RGBA:GL_RGB;
			*pnHeight=fileInfo.biHeight;
			*pnWidth=fileInfo.biWidth;
			*ppPixels = new unsigned char[fileInfo.biHeight * fileInfo.biWidth * nRequestedBits/8];

			int sourceLineSize = fileInfo.biWidth *  fileInfo.biBitCount/8;
			int paddedLineSize = ((sourceLineSize/4)*4);
			int pad=0;
			if(paddedLineSize < sourceLineSize){pad=4;}

			unsigned char *bufferPointer = (*ppPixels);// + (lineSize * (fileInfo.biHeight-1));
			unsigned char *imagePointer = pFileBits;

			int x, y;
			int nSourceExcess=fileInfo.biBitCount>nRequestedBits?1:0;
			int nDestExcess=nRequestedBits>fileInfo.biBitCount?1:0;
			int nCommon=fileInfo.biBitCount==nRequestedBits?fileInfo.biBitCount/8:3;

			// Alpha channel to full opacity if missing.
			if(nDestExcess){memset(*ppPixels,0xFF,fileInfo.biHeight * fileInfo.biWidth * nRequestedBits/8);}

			for(x = fileInfo.biHeight-1; x >= 0; x--)
			{
				for(y = 0; y < (int)fileInfo.biWidth; y++)
				{
					bufferPointer[0] = imagePointer[2];
					bufferPointer[1] = imagePointer[1];
					bufferPointer[2] = imagePointer[0];
					bufferPointer+=nCommon+nDestExcess;
					imagePointer+=nCommon+nSourceExcess;
				}
				imagePointer  += pad;
			}
		}
		free(pFileBits);
		pFileBits=NULL;
	}

	fclose(pFile);
	pFile=NULL;
	return bOk;
}

bool LoadPngFile(const char *pFileName,unsigned int *pnWidth,unsigned int *pnHeight,unsigned char **ppPixels,bool bForceAlpha,unsigned int *pdwColorType)
{
	png_structp pPNGHeader=NULL;
	png_infop pPNGInfo=NULL;

	bool bOk;
	FILE *pFile=fopen(pFileName,"rb");
	bOk=(pFile!=NULL);
	if(bOk){pPNGHeader=png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);bOk=(pPNGHeader!=NULL);}
	if(bOk){pPNGInfo=png_create_info_struct(pPNGHeader);bOk=(pPNGInfo!=NULL);}
	if(bOk){bOk=(setjmp(png_jmpbuf(pPNGHeader))==0);}
	if(bOk){png_init_io(pPNGHeader, pFile);}
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
	if(pFile){fclose(pFile);}	
	return bOk;	
}


bool LoadImageHelper(std::string sFile,unsigned *pOpenGLSkinWidth,unsigned *pOpenGLSkinHeight,unsigned char **ppBuffer,bool bForceAlpha,unsigned int *pdwColorType)
{
	*pOpenGLSkinWidth=0;
	*pOpenGLSkinHeight=0;
	*ppBuffer=NULL;
	*pdwColorType=GL_RGB;

	char sFileName[MAX_PATH]={0};
	strcpy(sFileName,sFile.c_str());

	char sExt[MAX_PATH]={0};
	GetExtension(sFile.c_str(),sExt);

	std::string path=sFile;
	char pFileFolder[MAX_PATH];
	GetFileFolder(sFileName,pFileFolder);
	if(pFileFolder[0]==0 || strcmp(pFileFolder,".")==0)
	{
		std::string sTemp="Textures/";
		sTemp+=path;
		path=sTemp;
	}
	if(sExt[0]==0)
	{
		path+=".bmp";
	}
	else
	{
		if(strcasecmp(sExt,".PNG")==0)
		{
			return LoadPngFile(path.c_str(),pOpenGLSkinWidth,pOpenGLSkinHeight,ppBuffer,bForceAlpha,pdwColorType);
		}
	}
	return LoadBMPFile(path.c_str(),pOpenGLSkinWidth,pOpenGLSkinHeight,ppBuffer,bForceAlpha,pdwColorType);
}


COpenGLTexture::COpenGLTexture(void)
{
	m_bDepth=false;
	m_bRenderTarget=false;
	m_dwWidth=0;
	m_dwHeight=0;
	m_pBuffer=NULL;

	m_dwColorType=GL_RGB;
	m_bColorKey=false;
	m_fOpacity=1.0;

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
	if(m_nTextureIndex)
	{
		glDeleteTextures(1,&m_nTextureIndex);
		m_nTextureIndex=0;
	}
	m_dwColorType=GL_RGB;
}

COpenGLTexture::~COpenGLTexture(void)
{
	Clear();
}

std::string	COpenGLTexture::GetFileName(){return m_sFileName;}
bool		COpenGLTexture::HasAlphaFile(){return m_sAlphaFileName!="";}
std::string COpenGLTexture::GetAlphaFileName(){return m_sAlphaFileName;}
void		COpenGLTexture::GetSize(unsigned *pdwWidth,unsigned *pdwHeight){*pdwWidth=m_dwWidth;*pdwHeight=m_dwHeight;}
bool		COpenGLTexture::HasColorKey(){return m_bColorKey;}
CVector		COpenGLTexture::GetColorKey(){return m_vColorKey;}
bool COpenGLTexture::LoadFromFile()
{
	int nStartTime=GetTimeStamp();
	bool bForceAlpha=(m_bColorKey || m_sAlphaFileName!="" || m_fOpacity<=1.0 || m_bRenderTarget);
	bool bResult=true;
	m_dwColorType=GL_RGB;
	if(LoadImageHelper(m_sFileName,&m_dwWidth,&m_dwHeight,&m_pBuffer,bForceAlpha,&m_dwColorType))
	{
		if(m_sAlphaFileName!="")
		{
			unsigned char	*pAlphaBuffer=NULL;
			unsigned nAlphaOpenGLSkinWidth=0,nAlphaOpenGLSkinHeight=0;

			unsigned int dwAlphaColorType=GL_RGB;
			if(LoadImageHelper(m_sAlphaFileName,&nAlphaOpenGLSkinWidth,&nAlphaOpenGLSkinHeight,&pAlphaBuffer,false,&dwAlphaColorType))
			{
				if(nAlphaOpenGLSkinWidth==m_dwWidth && m_dwHeight==nAlphaOpenGLSkinHeight)
				{
					unsigned char *pTempBuffer=m_pBuffer;
					unsigned char *pTempAlpha=pAlphaBuffer;

					for(unsigned y=0; y < m_dwHeight; y++)
					{
						unsigned int nAlphaColorLen=dwAlphaColorType==GL_RGB?3:4;
						for(unsigned x = 0; x < m_dwWidth; x++,pTempBuffer+=4,pTempAlpha+=nAlphaColorLen)
						{
							pTempBuffer[3] = (unsigned char)((((unsigned int)pTempAlpha[0])+((unsigned int)pTempAlpha[1])+((unsigned int)pTempAlpha[2]))/3);
						}
					}
				}
				else
				{
					bResult=false;
					RTTRACE("COpenGLTexture::LoadFromFile -> Texture %s size does not match with alpha texture %s",m_sFileName.c_str(),m_sAlphaFileName.c_str());
				}
			}
			else
			{
				bResult=false;
				RTTRACE("COpenGLTexture::LoadFromFile -> Failed to load alpha texture %s",m_sAlphaFileName.c_str());
			} 
			if(pAlphaBuffer){delete [] pAlphaBuffer;pAlphaBuffer=NULL;}
		}	
		else if(m_bColorKey)
		{
			unsigned char red=(unsigned char)(m_vColorKey.c[0]*255.0);
			unsigned char green=(unsigned char)(m_vColorKey.c[1]*255.0);
			unsigned char blue=(unsigned char)(m_vColorKey.c[2]*255.0);

			unsigned char *pTempBuffer=m_pBuffer;

			for(unsigned y=0; y < m_dwHeight; y++)
			{
				for(unsigned x = 0; x < m_dwWidth; x++,pTempBuffer+=4)
				{
					pTempBuffer[3] = (pTempBuffer[0]==red && pTempBuffer[1]==green && pTempBuffer[2]==blue)?0:255;
				}
			}
		}
		else if(m_fOpacity<1.0)
		{
			unsigned char *pTempBuffer=m_pBuffer;

			for(unsigned int y=0; y < m_dwWidth; y++)
			{
				for(unsigned int x = 0; x < m_dwHeight; x++,pTempBuffer+=4)
				{
					pTempBuffer[3] = (unsigned char)(255.0*m_fOpacity);
				}
			}
		}
	}
	else
	{
		bResult=false;
	}
	
	if(bResult)
	{
		glGenTextures(1,&m_nTextureIndex);

		if(m_nTextureIndex)
		{
			glBindTexture(GL_TEXTURE_2D,m_nTextureIndex);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_bGenerateMipMaps?GL_NEAREST_MIPMAP_LINEAR:GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_bGenerateMipMaps?GL_LINEAR:GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
#ifdef ANDROID
			glTexImage2D(GL_TEXTURE_2D, 0, m_dwColorType==GL_RGBA?4:3, m_dwWidth,m_dwHeight, 0,m_dwColorType, GL_UNSIGNED_BYTE, m_pBuffer);
#else
			if(m_bGenerateMipMaps)
			{
				gluBuild2DMipmaps(GL_TEXTURE_2D,m_dwColorType==GL_RGBA?4:3,m_dwWidth,m_dwHeight,m_dwColorType,GL_UNSIGNED_BYTE,m_pBuffer);
			}
			else
			{
				glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
				glTexImage2D(GL_TEXTURE_2D, 0, m_dwColorType==GL_RGBA?4:3, m_dwWidth,m_dwHeight, 0,m_dwColorType, GL_UNSIGNED_BYTE, m_pBuffer);
			}
#endif
			RTTRACE("COpenGLTexture::LoadFromFile -> Loaded texture %s (%d ms)",m_sFileName.c_str(),GetTimeStamp()-nStartTime);
		}
		else
		{
			RTTRACE("COpenGLTexture::LoadFromFile -> Failed to create texture id for %s",m_sFileName.c_str());
		}
		m_bRenderTarget=false;
		
	}
	else
	{
		RTTRACE("COpenGLTexture::LoadFromFile -> Failed to load texture %s",m_sFileName.c_str());
	}

	return bResult;
}

bool COpenGLTexture::Unserialize(ISystemPersistencyNode *piNode)
{
	bool bResult=CSystemObjectBase::Unserialize(piNode);
	if(bResult){bResult=LoadFromFile();}
	return bResult;
}
bool COpenGLTexture::HasAlphaChannel(){return (m_dwColorType==GL_RGBA);}

unsigned long COpenGLTexture::GetByteBufferLength(){return HasAlphaChannel()?m_dwHeight*m_dwWidth*4:m_dwHeight*m_dwWidth*3;}
void		  *COpenGLTexture::GetByteBuffer(){return m_pBuffer;}

bool COpenGLTexture::Load(std::string sFileName,CVector *pColorKey,std::string *pAlphaFile,float fOpacity,bool bGenerateMipmaps)
{
	Clear();
	m_sFileName=sFileName;
	m_sAlphaFileName=pAlphaFile?*pAlphaFile:"";
	m_bColorKey=false;
	m_fOpacity=fOpacity;
	m_bGenerateMipMaps=bGenerateMipmaps;
	if(pColorKey)
	{
		m_vColorKey=*pColorKey;
		m_bColorKey=true;
	}
	if(m_pBuffer)
	{
		delete [] m_pBuffer;
		m_pBuffer=NULL;
	}
	return LoadFromFile();
}

CVector COpenGLTexture::GetPixelColor( unsigned long x, unsigned long y )
{
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
	if(!HasAlphaChannel()){return m_fOpacity;}
	if(x<m_dwWidth && y<m_dwHeight)
	{
		unsigned long nPixel=x+(y*m_dwWidth);
		unsigned long nPixelSize=4;
		unsigned char *pPixel=m_pBuffer+(nPixel*nPixelSize);
		return ((double)pPixel[3])/255.0;
	}
	return m_fOpacity;
}

bool COpenGLTexture::CreateFrameBuffer(bool bDepth)
{
	// FrameBuffer Implementation
	
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
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
#endif
			glTexImage2D(GL_TEXTURE_2D, 0,  GL_DEPTH_COMPONENT, m_dwWidth,m_dwHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		}
		else
		{
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#ifndef ANDROID
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
#endif
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_dwWidth,m_dwHeight, 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);
		}
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_nFrameBufferDepth);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, m_dwWidth, m_dwHeight);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,m_nFrameBuffer);
		if(!bDepth)
		{		
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT,GL_TEXTURE_2D,m_nTextureIndex,0);
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,GL_RENDERBUFFER_EXT, m_nFrameBufferDepth);
		}
		else
		{
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,GL_TEXTURE_2D, m_nTextureIndex, 0);
		}

		GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
		bool bOk=(status == GL_FRAMEBUFFER_COMPLETE_EXT);
		if(!bOk){RTTRACE("COpenGLTexture::CreateFrameBuffer -> Failed to complete frame buffer, error 0x%0x",status);}
		return bOk;
	}
	else
	{
		RTTRACE("COpenGLTexture::CreateFrameBuffer -> Failed to create frame buffer object");
		return false;
	}
}


bool COpenGLTexture::Create( unsigned nWidth,unsigned nHeight,IGenericViewport *piViewport)
{
	Clear();

	int nMaxTextureSize=0;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE,&nMaxTextureSize);
	if((int)m_dwWidth>nMaxTextureSize){m_dwWidth=nMaxTextureSize;}
	if((int)m_dwHeight>nMaxTextureSize){m_dwHeight=nMaxTextureSize;}

	m_sFileName="";
	m_sAlphaFileName="";
	m_bColorKey=false;
	m_fOpacity=1.0;
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
	m_sAlphaFileName="";
	m_bColorKey=false;
	m_fOpacity=1.0;
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
	// FrameBuffer Implementation
	if(m_nFrameBuffer)
	{
		glDisable(GL_SCISSOR_TEST);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,m_nFrameBuffer);
		return true;
	}
	else if(m_bDepth && m_nFrameBufferDepth)
	{
		glDisable(GL_SCISSOR_TEST);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,m_nFrameBufferDepth);
		return true;
	}
	
	return false;
}
void COpenGLTexture::StopRenderingToTexture()
{
	// FrameBuffer Implementation
	if(m_nFrameBuffer)
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
	}
	else if(m_bDepth && m_nFrameBufferDepth)
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
	}
}

bool COpenGLTexture::PrepareTexture(IGenericRender *piRender,int nTextureLevel)
{
	if(m_nFrameBuffer)
	{
		glActiveTexture(GL_TEXTURE0_ARB+nTextureLevel);
		glBindTexture(GL_TEXTURE_2D,m_nTextureIndex);
	}
	else if(m_nFrameBufferDepth)
	{
		glActiveTexture(GL_TEXTURE0_ARB+nTextureLevel);
		glBindTexture(GL_TEXTURE_2D,m_nTextureIndex);
	}
	else
	{
		glActiveTexture(GL_TEXTURE0_ARB+nTextureLevel);
		glBindTexture(GL_TEXTURE_2D,m_nTextureIndex);
	}
	return true;
}

void COpenGLTexture::UnprepareTexture(IGenericRender *piRender,int nTextureLevel)
{
	if(m_nFrameBuffer)
	{
		glActiveTexture(GL_TEXTURE0_ARB+nTextureLevel);
		glBindTexture(GL_TEXTURE_2D,0);
	}
	else if(m_nFrameBufferDepth)
	{
		glActiveTexture(GL_TEXTURE0_ARB+nTextureLevel);
		glBindTexture(GL_TEXTURE_2D,0);
	}
	else
	{
		glActiveTexture(GL_TEXTURE0_ARB+nTextureLevel);
		glBindTexture(GL_TEXTURE_2D,m_nTextureIndex);
	}
}
