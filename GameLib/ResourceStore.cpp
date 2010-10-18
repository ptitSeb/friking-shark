// GameRender.cpp: implementation of the GameRender class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <float.h>
#include <crtdbg.h>
#include <windows.h>
#include "..\VectorLib\VectorLib.h"
#include "GameRunTimeLib.h"
#include "ResourceStore.h"
#include "GLProcs.h"
#include "..\..\JPEGLibrary\inc\jpegdecoder.h"
#ifdef _DEBUG
#pragma comment(lib,"..\\..\\JPEGLibrary\\lib\\jpgdlib_d.lib")
#else
#pragma comment(lib,"..\\..\\JPEGLibrary\\lib\\jpgdlib.lib")
#endif
#pragma warning ( disable : 4244 4018 )

#define FILE_TYPE_3DS 1
#define FILE_TYPE_ASE 2

//me falta liberar el resourcestore y liberar los objetos del gamelevel cuando se cambia de archivo

CLight::CLight(string sName,DWORD dwType,CVector vPos,COLORREF cColor)
{
	m_bIsActive=true;
	m_sName=sName;
	m_dwType=dwType;
	m_vPosition=vPos;
	m_cColor=cColor;
	m_fAttenuationConstant=1;
	m_fAttenuationLinear=0;
	m_fAttenuationQuadratic=0;

	m_fSpotExponent=0;
	m_fSpotAngle=0;
	m_nOpenGlIndex=-1;
}

CLight::~CLight()
{
}

CPreprocessedRenderBuffer::CPreprocessedRenderBuffer()
{
	nBufferObject=0;
	nIndexesBufferObject=0;
	nTriangles=0;
	nVertexes=0;
	pVertexArray=NULL;
	pNormalArray=NULL;
	pTexVertexArray=NULL;
	pTex1VertexArray=NULL;
	pTex2VertexArray=NULL;
	pColorArray=NULL;
	pTriangleVertexIndexes=NULL;
	pMaterial=NULL;
}

CPreprocessedRenderBuffer::~CPreprocessedRenderBuffer()
{
	if(pVertexArray){delete [] pVertexArray;pVertexArray=NULL;}
	if(pNormalArray){delete [] pNormalArray;pNormalArray=NULL;}
	if(pTexVertexArray){delete [] pTexVertexArray;pTexVertexArray=NULL;}
	if(pTex1VertexArray){delete [] pTex1VertexArray;pTex1VertexArray=NULL;}
	if(pTex2VertexArray){delete [] pTex2VertexArray;pTex2VertexArray=NULL;}
	if(pColorArray){delete [] pColorArray;pColorArray=NULL;}
	if(pTriangleVertexIndexes){delete [] pTriangleVertexIndexes;pTriangleVertexIndexes=NULL;}
	if(nBufferObject!=0){glDeleteBuffers(1,&nBufferObject);nBufferObject=0;}
	if(nIndexesBufferObject!=0){glDeleteBuffers(1,&nIndexesBufferObject);nIndexesBufferObject=0;}
}

CFrame::CFrame()
{
	m_nPolygons=0;
	m_pPolygons=NULL;
	m_nPreprocessedRenderBuffers=0;
	m_pPreprocessedRenderBuffers=NULL;
}

CFrame::~CFrame()
{
	if(m_pPolygons){delete [] m_pPolygons;m_pPolygons=NULL;}
	if(m_pPreprocessedRenderBuffers){delete [] m_pPreprocessedRenderBuffers;m_pPreprocessedRenderBuffers=NULL;}
	m_nPolygons=0;
	m_nPreprocessedRenderBuffers=0;
}

void CFrame::CalcBBox()
{
	m_vMins=Origin;
	m_vMaxs=Origin;

	unsigned x,v,c;
	for(x=0;x<m_nPolygons;x++)
	{
		CPolygon *pPolygon=&m_pPolygons[x];
		for(v=0;v<pPolygon->m_nVertexes;v++)
		{
			if(x==0 && v==0)
			{
				m_vMins=pPolygon->m_pVertexes[v];
				m_vMaxs=pPolygon->m_pVertexes[v];
			}
			else
			{
				for(c=0;c<3;c++)
				{
					if(pPolygon->m_pVertexes[v].c[c]<m_vMins.c[c]){m_vMins.c[c]=pPolygon->m_pVertexes[v].c[c];}
					if(pPolygon->m_pVertexes[v].c[c]>m_vMaxs.c[c]){m_vMaxs.c[c]=pPolygon->m_pVertexes[v].c[c];}
				}
			}
		}
	}
}

class CVertexKey
{
public:
	double c[3]; // Coordenadas
	double t[2];
	double n[3];
	double col[3];
	unsigned int i;

	bool operator<(const CVertexKey &v2) const
	{
		if(c[0]>v2.c[0]){return false;}
		if(c[0]<v2.c[0]){return true;}
		if(c[1]>v2.c[1]){return false;}
		if(c[1]<v2.c[1]){return true;}
		if(c[2]>v2.c[2]){return false;}
		if(c[2]<v2.c[2]){return true;}
		if(t[0]>v2.t[0]){return false;}
		if(t[0]<v2.t[0]){return true;}
		if(t[1]>v2.t[1]){return false;}
		if(t[1]<v2.t[1]){return true;}
		if(n[0]>v2.n[0]){return false;}
		if(n[0]<v2.n[0]){return true;}
		if(n[1]>v2.n[1]){return false;}
		if(n[1]<v2.n[1]){return true;}
		if(n[2]>v2.n[2]){return false;}
		if(n[2]<v2.n[2]){return true;}
		return false;
	}

	CVertexKey(CVector vc,CVector vt,CVector vn,CVector vcol,int vi)
	{
		int x;
		for(x=0;x<3;x++){c[x]=vc.c[x];}
		for(x=0;x<2;x++){t[x]=vt.c[x];}
		for(x=0;x<3;x++){n[x]=vn.c[x];}
		for(x=0;x<3;x++){col[x]=vcol.c[x];}
		i=vi;
	}
	CVertexKey(){memset(c,0,sizeof(c));memset(t,0,sizeof(t));memset(n,0,sizeof(n));memset(col,0,sizeof(col));i=0;}
};

void CFrame::GeneratePreprocessedRenderBuffer()
{
	set<CMaterial *> mMaterials;
	set<CMaterial *>::iterator i;
	int x,m,v;
	for(x=0;x<m_nPolygons;x++)
	{
		if(m_pPolygons[x].m_pMaterial){mMaterials.insert(m_pPolygons[x].m_pMaterial);}
	}

	m_nPreprocessedRenderBuffers=mMaterials.size();
	if(m_nPreprocessedRenderBuffers)
	{
		m_pPreprocessedRenderBuffers=new CPreprocessedRenderBuffer[m_nPreprocessedRenderBuffers];
	}

	for(i=mMaterials.begin(),m=0;i!=mMaterials.end();i++,m++)
	{
		map<CVertexKey,DWORD> mVertexes;
		map<CVertexKey,DWORD>::iterator iVertexes;

		CMaterial					*pMaterial=*i;
		CPreprocessedRenderBuffer	*pBuffer=&m_pPreprocessedRenderBuffers[m];

		for(x=0;x<m_nPolygons;x++)
		{
			if(m_pPolygons[x].m_pMaterial==pMaterial)
			{
				pBuffer->nTriangles++;

				CMaterialPolygon *pPolygon=&m_pPolygons[x];
				for(v=0;v<pPolygon->m_nVertexes;v++)
				{
					CVertexKey key(pPolygon->m_pVertexes[v],pPolygon->m_pTextureCoords[v],pPolygon->m_pVertexNormals[v],pPolygon->m_pVertexColors?pPolygon->m_pVertexColors[v]:RGBToVector(pMaterial->cAmbientColor),mVertexes.size());
					if(mVertexes.find(key)==mVertexes.end())
					{
						mVertexes[key]=mVertexes.size();
					}
				}
			}
		}

		pBuffer->nVertexes=mVertexes.size();
		pBuffer->pVertexArray=new GLfloat[pBuffer->nVertexes*3];
		pBuffer->pTexVertexArray=new GLfloat[pBuffer->nVertexes*2];
		pBuffer->pNormalArray=new GLfloat[pBuffer->nVertexes*3];
		pBuffer->pColorArray=new GLfloat[pBuffer->nVertexes*4];
		pBuffer->pTriangleVertexIndexes=new unsigned int[pBuffer->nTriangles*3];
		pBuffer->pMaterial=pMaterial;

		GLfloat color[4];
		RGBToFloat(pMaterial->cAmbientColor,color);

		int nCurVert=0;
		int nCurTex=0;
		int nCurCol=0;
		for(iVertexes=mVertexes.begin();iVertexes!=mVertexes.end();iVertexes++)
		{
			const CVertexKey *pVertex=&iVertexes->first;

			pBuffer->pVertexArray[pVertex->i*3]=pVertex->c[0];
			pBuffer->pVertexArray[pVertex->i*3+1]=pVertex->c[1];
			pBuffer->pVertexArray[pVertex->i*3+2]=pVertex->c[2];

			pBuffer->pNormalArray[pVertex->i*3]=pVertex->n[0];
			pBuffer->pNormalArray[pVertex->i*3+1]=pVertex->n[1];
			pBuffer->pNormalArray[pVertex->i*3+2]=pVertex->n[2];

			pBuffer->pColorArray[pVertex->i*4]=pVertex->col[0];
			pBuffer->pColorArray[pVertex->i*4+1]=pVertex->col[1];
			pBuffer->pColorArray[pVertex->i*4+2]=pVertex->col[2];
			pBuffer->pColorArray[pVertex->i*4+3]=pMaterial->fOpacity;

			pBuffer->pTexVertexArray[pVertex->i*2]=pVertex->t[0];
			pBuffer->pTexVertexArray[pVertex->i*2+1]=pVertex->t[1];

			nCurVert+=3;
			nCurTex+=2;
			nCurCol+=4;
		}

		int nIndex=0;
		for(x=0;x<m_nPolygons;x++)
		{
			if(m_pPolygons[x].m_pMaterial==pMaterial)
			{
				CMaterialPolygon *pPolygon=&m_pPolygons[x];
				for(v=0;v<pPolygon->m_nVertexes;v++)
				{
					CVertexKey key(pPolygon->m_pVertexes[v],pPolygon->m_pTextureCoords[v],pPolygon->m_pVertexNormals[v],Origin,0);
					pBuffer->pTriangleVertexIndexes[nIndex+v]=mVertexes[key];
				}
				nIndex+=3;
			}
		}

		//_MRT("Vertex Number for %d polygons Decremented from %d to %d",m_nPolygons,m_nPolygons*3,pBuffer->nVertexes);
	}
	GenerateBufferObjects();
}

void CFrame::GenerateBufferObjects()
{

	for(unsigned long x=0;x<m_nPreprocessedRenderBuffers;x++)
	{
		CPreprocessedRenderBuffer *pBuffer=&m_pPreprocessedRenderBuffers[x];

		if(pBuffer->nBufferObject!=0){glDeleteBuffers(1,&pBuffer->nBufferObject);pBuffer->nBufferObject=0;}
		if(pBuffer->nIndexesBufferObject!=0){glDeleteBuffers(1,&pBuffer->nIndexesBufferObject);pBuffer->nIndexesBufferObject=0;}

		int nDataPerVertex=3;
		if(pBuffer->pTexVertexArray){nDataPerVertex+=2;}
		if(pBuffer->pNormalArray){nDataPerVertex+=3;}
		if(pBuffer->pColorArray){nDataPerVertex+=4;}
		if(pBuffer->pTex1VertexArray){nDataPerVertex+=2;}
		if(pBuffer->pTex2VertexArray){nDataPerVertex+=2;}

		// Generacion del buffer object
		glGenBuffers(1,&pBuffer->nBufferObject);
		int nError=glGetError();
		if(pBuffer->nBufferObject)
		{
			glBindBuffer(GL_ARRAY_BUFFER,pBuffer->nBufferObject);
			glBufferData(GL_ARRAY_BUFFER,pBuffer->nVertexes*nDataPerVertex*sizeof(GLfloat),NULL,GL_STATIC_DRAW);
			GLfloat *pBufferObject = (GLfloat *)glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
			DWORD dwOffset=0;
			memcpy(((BYTE*)pBufferObject)+dwOffset,pBuffer->pVertexArray,pBuffer->nVertexes*3*sizeof(GLfloat));
			dwOffset+=pBuffer->nVertexes*3*sizeof(GLfloat);
			if(pBuffer->pTexVertexArray)
			{
				memcpy(((BYTE*)pBufferObject)+dwOffset,pBuffer->pTexVertexArray,pBuffer->nVertexes*2*sizeof(GLfloat));
				dwOffset+=pBuffer->nVertexes*2*sizeof(GLfloat);
			}
			if(pBuffer->pNormalArray)
			{
				memcpy(((BYTE*)pBufferObject)+dwOffset,pBuffer->pNormalArray,pBuffer->nVertexes*3*sizeof(GLfloat));
				dwOffset+=pBuffer->nVertexes*3*sizeof(GLfloat);
			}
			if(pBuffer->pColorArray)
			{
				memcpy(((BYTE*)pBufferObject)+dwOffset,pBuffer->pColorArray,pBuffer->nVertexes*4*sizeof(GLfloat));
				dwOffset+=pBuffer->nVertexes*4*sizeof(GLfloat);
			}
			if(pBuffer->pTex1VertexArray)
			{
				memcpy(((BYTE*)pBufferObject)+dwOffset,pBuffer->pTex1VertexArray,pBuffer->nVertexes*2*sizeof(GLfloat));
				dwOffset+=pBuffer->nVertexes*2*sizeof(GLfloat);
			}
			if(pBuffer->pTex2VertexArray)
			{
				memcpy(((BYTE*)pBufferObject)+dwOffset,pBuffer->pTex2VertexArray,pBuffer->nVertexes*2*sizeof(GLfloat));
				dwOffset+=pBuffer->nVertexes*2*sizeof(GLfloat);
			}
			glUnmapBuffer(GL_ARRAY_BUFFER);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		glGenBuffers(1,&pBuffer->nIndexesBufferObject);
		nError=glGetError();
		if(pBuffer->nIndexesBufferObject)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pBuffer->nIndexesBufferObject);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, pBuffer->nTriangles*3*sizeof(GLuint),NULL,GL_STATIC_DRAW);
			GLuint *pBufferObjectIndexes = (GLuint *)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER,GL_WRITE_ONLY);
			memcpy(pBufferObjectIndexes,pBuffer->pTriangleVertexIndexes,sizeof(GLuint)*pBuffer->nTriangles*3);
			glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
	}
}

void CFrame::GetGeometry( vector<CPolygon *> *pvGeometry )
{
	for(unsigned long x=0;x<m_nPolygons;x++)
	{
		CPolygon *pPolygon=new CPolygon;
		*pPolygon=m_pPolygons[x];
		pPolygon->CalcPlane();
		pvGeometry->push_back(pPolygon);
	}
}

CModelAnimation::CModelAnimation()
{
	m_nFrames=0;
	m_pFrames=NULL;
}

CModelAnimation::~CModelAnimation()
{
	if(m_pFrames){delete [] m_pFrames;m_pFrames=NULL;}
	m_nFrames=0;
}


CModel::CModel()
{
	m_dwType=MODEL_TYPE_NONE;
	m_nAnimations=0;
	m_pAnimations=NULL;
	m_nLights=0;
	memset(m_pLights,sizeof(m_pLights),0);
}

CModel::~CModel()
{
	Reset();
}

void CModel::Reset()
{
	int x;
	for(x=0;x<m_nLights;x++)
	{
		if(m_pLights[x]){delete m_pLights[x];m_pLights[x]=NULL;}
	}
	m_nLights=0;
	if(m_pAnimations){delete [] m_pAnimations;m_pAnimations=NULL;}
	m_nAnimations=0;
	m_sName="";
	m_dwType=MODEL_TYPE_NONE;
}

CModelAnimation *CModel::GetAnimation(int nAnimation)
{
	if(nAnimation>=m_nAnimations){return NULL;}
	return &m_pAnimations[nAnimation];
}

CFrame	*CModel::GetFrame(int nAnimation,int nFrame)
{
	if(nAnimation>=m_nAnimations){return NULL;}
	CModelAnimation *pAnimation=&m_pAnimations[nAnimation];
	if(pAnimation==NULL){return NULL;}
	if(nFrame>=pAnimation->m_nFrames){return NULL;}
	return &pAnimation->m_pFrames[nFrame];
}

CResourceStore::CResourceStore()
{
	m_hDC=NULL;
	m_hRenderContext=NULL;
    m_sName="ResourceStore";
}

CResourceStore::~CResourceStore()
{
	Destroy();
}

bool LoadJPEGImageHelper(string sFile,DWORD dwColorType,unsigned *pOpenGLSkinWidth,unsigned *pOpenGLSkinHeight,BYTE **ppBuffer)
{
  Pjpeg_decoder_file_stream Pinput_stream = new jpeg_decoder_file_stream();

  if (Pinput_stream->open(sFile.c_str()))
  {
    delete Pinput_stream;
    return false;
  }

  Pjpeg_decoder Pd = new jpeg_decoder(Pinput_stream, false);
  if (Pd->get_error_code() != 0)
  {
    delete Pd;
    delete Pinput_stream;
    return false;
  }
  
  if (Pd->begin())
  {
    delete Pd;
    delete Pinput_stream;
    return false;
  }

  uchar *Pbuf = NULL;
  if (Pd->get_num_components() == 3)
  {
    Pbuf = (uchar *)malloc(Pd->get_width() * 3);
    if (!Pbuf)
    {
      printf("Error: Out of memory!\n");

      delete Pd;
      delete Pinput_stream;
      return false;
    }
  }

  DWORD destBytesPerPixel=dwColorType==GL_RGB?3:4;
  DWORD dwSize = Pd->get_width() * Pd->get_height() * destBytesPerPixel;
  *pOpenGLSkinWidth = Pd->get_width();
  *pOpenGLSkinHeight = Pd->get_height();
  *ppBuffer = new BYTE[dwSize];

  memset(*ppBuffer,255,dwSize); // esto se hace para inicializar la imagen y el canal RGBA

  int lines_decoded = 0;
  for ( ; ; )
  {
    void *Pscan_line_ofs;
    uint scan_line_len;

    if (Pd->decode(&Pscan_line_ofs, &scan_line_len))
      break;

    if (Pd->get_num_components() == 3)
    {
      uchar *Psb = (uchar *)Pscan_line_ofs;
      uchar *Pdb = (*ppBuffer)+(Pd->get_height()-(lines_decoded+1))*Pd->get_width()*destBytesPerPixel;
      int src_bpp = Pd->get_bytes_per_pixel();

      for (int x = Pd->get_width(); x > 0; x--, Psb += src_bpp, Pdb += destBytesPerPixel)
      {
        Pdb[0] = Psb[0];
        Pdb[1] = Psb[1];
        Pdb[2] = Psb[2];
      }
    }
    lines_decoded++;
  }

  free(Pbuf);
  delete Pd;
  delete Pinput_stream;
  return true;
}


bool LoadTextureImageHelper(string sFile,DWORD dwColorType,unsigned *pOpenGLSkinWidth,unsigned *pOpenGLSkinHeight,BYTE **ppBuffer)
{
	*pOpenGLSkinWidth=0;
	*pOpenGLSkinHeight=0;
	*ppBuffer=NULL;

	std::string path;
	char sDrive[MAX_PATH]={0};
	char sPath[MAX_PATH]={0};
	char sExt[MAX_PATH]={0};
	path=sFile;
	_splitpath(sFile.c_str(),sDrive,sPath,NULL,sExt);
	if(sDrive[0]==0 && sPath[0]==0)
	{
		string sTemp="Textures\\";
		sTemp+=path;
		path=sTemp;
	}
	if(sExt[0]==0)
	{
		path+=".bmp";
	}
	else
	{
		_strupr(sExt);
		if(strcmp(sExt,".JPG")==0)
		{
		/*	char drive[_MAX_DRIVE]={0},tpath[MAX_PATH]={0},name[_MAX_FNAME]={0};
			_splitpath(path.c_str(),drive,tpath,name,NULL);
			path=drive;
			path+=tpath;
			path+=name;
			path+=".BMP";*/
      return LoadJPEGImageHelper(path,dwColorType,pOpenGLSkinWidth,pOpenGLSkinHeight,ppBuffer);
		}
	}
	CMRImage image;
	if(image.LoadFromFile(path.c_str()))
	{
		unsigned nOpenGLSkinWidth=0,nOpenGLSkinHeight=0,i=0,j=0;
		while(nOpenGLSkinWidth<image.Width()){nOpenGLSkinWidth=pow(2.0,(double)i);i++;}
		while(nOpenGLSkinHeight<image.Height()){nOpenGLSkinHeight=pow(2.0,(double)j);j++;}

		image.Resize(nOpenGLSkinWidth,nOpenGLSkinHeight);
		image.Flip_Vertical();
		DWORD	dwSize=0;
		BYTE	*pBuffer=NULL;
		if(dwColorType==GL_RGB)
		{
			image.GetRawRGB(&dwSize,&pBuffer);
		}
		else
		{
			image.GetRawRGBA(&dwSize,&pBuffer);
		}
		*pOpenGLSkinWidth=nOpenGLSkinWidth;
		*pOpenGLSkinHeight=nOpenGLSkinHeight;
		*ppBuffer=pBuffer;
		return true;
	}
	return false;
}

CTexture *CResourceStore::LoadTexture(string sName,COLORREF *pColorKey,string *pAlphaFile,float fOpacity,CTexture *pExistingTexture)
{
	if(m_hRenderContext){wglMakeCurrent(m_hDC,m_hRenderContext);	}

	CTexture *pTexture=NULL;
	if(pExistingTexture!=NULL)
	{
		pTexture=pExistingTexture;
	}
	else
	{
		pTexture=FindTexture(sName);
		if(pTexture){return pTexture;}
	}

	BYTE	*pBuffer=NULL;
	unsigned nOpenGLSkinWidth=0,nOpenGLSkinHeight=0;
	DWORD	 dwColorType=(pColorKey!=NULL || pAlphaFile!=NULL || fOpacity<=1.0)?GL_RGBA:GL_RGB;

	if(LoadTextureImageHelper(sName,dwColorType,&nOpenGLSkinWidth,&nOpenGLSkinHeight,&pBuffer))
	{
		if(pAlphaFile)
		{
			BYTE	*pAlphaBuffer=NULL;
			unsigned nAlphaOpenGLSkinWidth=0,nAlphaOpenGLSkinHeight=0;

			if(LoadTextureImageHelper(*pAlphaFile,GL_RGB,&nAlphaOpenGLSkinWidth,&nAlphaOpenGLSkinHeight,&pAlphaBuffer))
			{
				if(nAlphaOpenGLSkinWidth==nOpenGLSkinWidth && nOpenGLSkinHeight==nAlphaOpenGLSkinHeight)
				{
					BYTE *pTempBuffer=pBuffer;
					BYTE *pTempAlpha=pAlphaBuffer;

					for(int y=0; y < nOpenGLSkinHeight; y++)
					{
						for(int x = 0; x < nOpenGLSkinWidth; x++,pTempBuffer+=4,pTempAlpha+=3)
						{
							pTempBuffer[3] = (((DWORD)pTempAlpha[0])+((DWORD)pTempAlpha[1])+((DWORD)pTempAlpha[2]))/3;
						}
					}
				}
				else
				{
					//LOG_ERROR_AND_EXIT("Error","El tamaño de la textura de alpha \"%s\" y el de la textura \"%s\" deben ser iguales",pAlphaFile->c_str(),path.c_str());
				}
			}
			else
			{
				//LOG_ERROR_AND_EXIT("Error","No se ha podido cargar la textura de alpha \"%s\" para la textura \"%s\"",pAlphaFile->c_str(),path.c_str());
			}
			if(pAlphaBuffer){delete [] pAlphaBuffer;pAlphaBuffer=NULL;}
		}	
		else if(pColorKey)
		{
			BYTE blue,green,red;
			blue=GetBValue(*pColorKey);
			green=GetGValue(*pColorKey);
			red=GetRValue(*pColorKey);

			BYTE *pTempBuffer=pBuffer;

			for(int y=0; y < nOpenGLSkinHeight; y++)
			{
				for(int x = 0; x < nOpenGLSkinWidth; x++,pTempBuffer+=4)
				{
					pTempBuffer[3] = (pTempBuffer[0]==red && pTempBuffer[1]==green && pTempBuffer[2]==blue)?0:255;
				}
			}
		}
		else if(fOpacity<1.0)
		{
			BYTE *pTempBuffer=pBuffer;

			for(int y=0; y < nOpenGLSkinHeight; y++)
			{
				for(int x = 0; x < nOpenGLSkinWidth; x++,pTempBuffer+=4)
				{
					pTempBuffer[3] = (BYTE)(255.0*fOpacity);
				}
			}
		}
	
		if(pExistingTexture)
		{
			void *pOldPixels=pTexture->m_pPixels;
			pTexture->m_pPixels=pBuffer;
			pTexture->m_sName=sName;
			pTexture->m_fOpacity=fOpacity;
			pTexture->m_bAlphaFile=pAlphaFile!=NULL;
			pTexture->m_sAlphaFileName=pAlphaFile?*pAlphaFile:"";
			pTexture->m_bColorKey=pColorKey!=NULL;
			pTexture->m_cColorKey=pColorKey?*pColorKey:RGB(0,0,0);
			pTexture->m_nWidth=nOpenGLSkinWidth;
			pTexture->m_nHeight=nOpenGLSkinHeight;
			pTexture->m_dwColorType=dwColorType;
			UpdateTextureBuffers(pTexture);
			if(pOldPixels){delete [] pOldPixels;pOldPixels=NULL;}
		}
		else
		{
			pTexture=new CTexture(sName,nOpenGLSkinWidth,nOpenGLSkinHeight,pBuffer,dwColorType);
			pTexture->m_fOpacity=fOpacity;
			pTexture->m_bAlphaFile=pAlphaFile!=NULL;
			pTexture->m_sAlphaFileName=pAlphaFile?*pAlphaFile:"";
			pTexture->m_bColorKey=pColorKey!=NULL;
			pTexture->m_cColorKey=pColorKey?*pColorKey:RGB(0,0,0);
			AddTexture(pTexture);
		}
	}
	else
	{
		#pragma message ("Falta el control de errores en la carga de texturas LOG_ERROR_AND_EXIT")
		//LOG_ERROR_AND_EXIT("Error","No se ha podido carga la textura %s",path.c_str());
	}

	return pTexture;
}

int CResourceStore::GetTextureIndex(string sName)
{
	map<string,CTexture*>::iterator i;
	i=m_mTextures.find(sName);
	if(i!=m_mTextures.end())
	{
		CTexture *pTexture=i->second;
		return pTexture->m_nOpenGlIndex;
	}
	return -1;
}

CTexture *CResourceStore::FindTexture(string sName)
{
	map<string,CTexture*>::iterator i;
	i=m_mTextures.find(sName);
	if(i!=m_mTextures.end())
	{
		CTexture *pTexture=i->second;
		return pTexture;
	}
	return NULL;
}

void CResourceStore::AddTexture(CTexture *pTexture)
{
	_ASSERTE(GetTextureIndex(pTexture->m_sName)==-1);
	m_mTextures[pTexture->m_sName]=pTexture;
	UpdateTextureBuffers(pTexture);
}

void CResourceStore::UpdateTextureBuffers(CTexture *pTexture)
{
	if(m_hRenderContext){wglMakeCurrent(m_hDC,m_hRenderContext);}
	HGLRC hRenderContext=wglGetCurrentContext();
	_ASSERTE(hRenderContext!=NULL);
	if(pTexture->m_nOpenGlIndex==0)
	{
		glGenTextures(1,&pTexture->m_nOpenGlIndex);
	}
	glBindTexture(GL_TEXTURE_2D,pTexture->m_nOpenGlIndex);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	gluBuild2DMipmaps(GL_TEXTURE_2D,pTexture->m_dwColorType==GL_RGBA?4:3,pTexture->m_nWidth,pTexture->m_nHeight,pTexture->m_dwColorType,GL_UNSIGNED_BYTE,pTexture->m_pPixels);
}

void CResourceStore::FreeTextures()
{
	map<string,CTexture*>::iterator i;
	for(i=m_mTextures.begin();i!=m_mTextures.end();i++)
	{
		CTexture *pTexture=i->second;
		if(pTexture->m_nOpenGlIndex){glDeleteTextures(1,&pTexture->m_nOpenGlIndex);}
		if(pTexture){delete pTexture;pTexture=NULL;}
	}
	m_mTextures.clear();
}

int CResourceStore::GetMaterialCount()
{
    return m_lMaterials.size();
}

CMaterial *CResourceStore::GetMaterialAt(int index)
{
    if(index<m_lMaterials.size()){return m_lMaterials[index];}
    return NULL;
}

CModel *CResourceStore::LoadModel(string sFileName,CModel *pExistingModel)
{
	if(m_hRenderContext)
	{
		wglMakeCurrent(m_hDC,m_hRenderContext);
	}

	char sDrive[MAX_PATH]={0};
	char sPath[MAX_PATH]={0};
	char sExt[MAX_PATH]={0};
	_splitpath(sFileName.c_str(),sDrive,sPath,NULL,sExt);
	if(sDrive[0]==0 && sPath[0]==0)
	{
		string sTemp="Models\\";
		sTemp+=sFileName;
		sFileName=sTemp;
	}

	CModel *pModel=pExistingModel;
	if(!pExistingModel)
	{
		pModel=FindModel(sFileName);
		if(pModel){return pModel;}
		pModel=new CModel;
	}
	else
	{
		pModel->Reset();
	}

	bool bLoaded=false;
	_strupr(sExt);
	if(strcmp(sExt,".MAP")==0)		{bLoaded=LoadMap(pModel,sFileName);}
	else if(strcmp(sExt,".MDL")==0)	{bLoaded=LoadMdl(pModel,sFileName);}
	else if(strcmp(sExt,".3DS")==0)	{bLoaded=Load3DS(pModel,sFileName,FILE_TYPE_3DS);}
	else if(strcmp(sExt,".ASE")==0)	{bLoaded=Load3DS(pModel,sFileName,FILE_TYPE_ASE);}
	else{MessageBox(NULL,"Tipo de extension de modelo desconocida",sFileName.c_str(),MB_ICONSTOP|MB_OK);}
	if(bLoaded)
	{
		if(!pExistingModel)
		{
			AddModel(pModel);
		}
		//_MRT("Loaded Model %s, %d polygons",sFileName.c_str(),pModel->m_pAnimations[0].m_pFrames[0].m_nPolygons);
	}
	else
	{
		if(!pExistingModel)
		{
			delete pModel;
		}
		pModel=NULL;
	}
	return pModel;
}

bool CResourceStore::ReloadModel(CModel *pModel)
{
	return LoadModel(pModel->m_sName,pModel)!=NULL;
}

bool CResourceStore::ReloadTexture(CTexture *pTexture)
{
	return LoadTexture(pTexture->m_sName,pTexture->m_bColorKey?&pTexture->m_cColorKey:NULL,pTexture->m_bAlphaFile?&pTexture->m_sAlphaFileName:NULL,pTexture->m_fOpacity,pTexture)!=NULL;
}

void CResourceStore::ReloadAllTextures()
{
	map<string,CTexture*>::iterator i;
	for(i=m_mTextures.begin();i!=m_mTextures.end();i++)
	{
		CTexture *pTexture=i->second;
		ReloadTexture(pTexture);
	}
}

void CResourceStore::ReloadAllModels()
{
	ReloadAllTextures();

	map<string,CModel*>::iterator i;
	for(i=m_mModels.begin();i!=m_mModels.end();i++)
	{
		CModel *pModel=i->second;
		ReloadModel(pModel);
	}
}

bool CResourceStore::LoadMap(CModel *pModel,string sFileName)
{
	bool bResult=false;
	CMapFileType	mapFile;
	if(mapFile.Open(sFileName.c_str()))
	{
		bResult=true;
		CModelAnimation	*pAnimation	=new CModelAnimation[1];
		CFrame		    *pFrame		=new CFrame[1];
		pModel->m_dwType		=MODEL_TYPE_MAP;
		pModel->m_sName=sFileName;
		pModel->m_nAnimations=1;
		pModel->m_pAnimations=pAnimation;
		pModel->m_pAnimations[0].m_nFrames=1;
		pModel->m_pAnimations[0].m_pFrames=pFrame;
		list<CMapFileBrush*>::iterator i;
		for(i=mapFile.m_lBrushes.begin();i!=mapFile.m_lBrushes.end();i++){pFrame->m_nPolygons+=(*i)->m_vPolygons.size();}
		
		if(pFrame->m_nPolygons)
		{
			int x=0;
			double step=(0.7/(double)pFrame->m_nPolygons);
			pFrame->m_pPolygons=new CMaterialPolygon [pFrame->m_nPolygons];
			for(i=mapFile.m_lBrushes.begin();i!=mapFile.m_lBrushes.end();i++)
			{
				CMapFileBrush *pBrush=*i;
				vector<CMapFilePolygon *>::iterator i2;
				for(i2=pBrush->m_vPolygons.begin();i2!=pBrush->m_vPolygons.end();i2++)
				{
					CMapFilePolygon *pPolygon=*i2;

					CTexture  *pTexture=LoadTexture(pPolygon->m_sTextName);
					CMaterial *pMaterial=new CMaterial;
					pMaterial->cAmbientColor=RGB((0.3+x*step)*255.0,(0.3+x*step)*255.0,(0.3+x*step)*255.0);
					pMaterial->cDiffuseColor=RGB((0.3+x*step)*255.0,(0.3+x*step)*255.0,(0.3+x*step)*255.0);
					pMaterial->cSpecularColor=RGB((0.3+x*step)*255.0,(0.3+x*step)*255.0,(0.3+x*step)*255.0);
					pMaterial->dwMaterialType=MATERIAL_TEXTURED;
					pMaterial->nTextureIndex=pTexture?pTexture->m_nOpenGlIndex:-1;
					pMaterial->pTexture=pTexture;

					CMaterial *pTest=FindMaterial(pMaterial);
					if(pTest)
						{delete pMaterial;pMaterial=pTest;}
					else
						{AddMaterial(pMaterial);}
					

					pFrame->m_pPolygons[x]=*pPolygon;
					pFrame->m_pPolygons[x].m_pMaterial=pMaterial;

					int z;
					for(z=0;z<pFrame->m_pPolygons[x].m_nVertexes;z++)
					{
						pFrame->m_pPolygons[x].m_pTextureCoords[z].c[0]/=(double)pTexture->m_nWidth;
						pFrame->m_pPolygons[x].m_pTextureCoords[z].c[1]/=(double)pTexture->m_nHeight;
					}
					x++;
				}
			}
		}
		pFrame->CalcBBox();
		pFrame->GeneratePreprocessedRenderBuffer();
	}
	return bResult;
}

bool CResourceStore::Load3DS(CModel *pModel,string sFileName,DWORD dwFileType)
{
	bool bResult=false;

	C3DSFileType	file3DS;
	CASEFileType	fileASE;
	C3DSFileTypeBase *pFile=&fileASE;
	if(dwFileType==FILE_TYPE_3DS){pFile=&file3DS;}

	//_MRT("Loading 3DS file %s",sFileName.c_str());
	if(pFile->Open(sFileName.c_str()))
	{
		bResult=true;
		CModelAnimation	*pAnimations=new CModelAnimation[1];
		CFrame		    *pFrames	=new CFrame[pFile->m_Scene.nFrameCount];

		pModel->m_dwType		=MODEL_TYPE_3DS;
		pModel->m_sName=sFileName;
		pModel->m_nAnimations=1;
		pModel->m_pAnimations=pAnimations;
		pModel->m_pAnimations[0].m_nFrames=pFile->m_Scene.nFrameCount;
		pModel->m_pAnimations[0].m_pFrames=pFrames;

		map<string,CMaterial *> mTranslatedMaterials;
		map<string,S3DSMaterial *> m3DSMaterials;
		for(int m=0;m<pFile->m_vMaterials.size();m++)
		{
			S3DSMaterial *p3DSMaterial=pFile->m_vMaterials[m];
			string sName=p3DSMaterial->sName;
			CMaterial *pMaterial=new CMaterial;
			pMaterial->cAmbientColor=p3DSMaterial->cAmbientColor;
			pMaterial->cDiffuseColor=p3DSMaterial->cDiffuseColor;
			pMaterial->cSpecularColor=p3DSMaterial->cSpecularColor;
			pMaterial->fShininess=p3DSMaterial->fShininess;
			pMaterial->dwMaterialType=p3DSMaterial->dwMaterialType;
			pMaterial->fOpacity=1.0-p3DSMaterial->fTranparency;
			pMaterial->bTwoSided=p3DSMaterial->bTwoSided;
			if(strcmp(p3DSMaterial->sFile,"")!=0)
			{
				string sTemp=p3DSMaterial->sAlphaFile;
				CTexture *pTexture=LoadTexture(p3DSMaterial->sFile,NULL,sTemp==""?NULL:&sTemp,pMaterial->fOpacity);
				pMaterial->nTextureIndex=pTexture?pTexture->m_nOpenGlIndex:-1;
				pMaterial->pTexture=pTexture;
			}

			CMaterial *pTest=FindMaterial(pMaterial);
			if(pTest)
			{delete pMaterial;pMaterial=pTest;}
			else
			{AddMaterial(pMaterial);}

			m3DSMaterials[sName]=p3DSMaterial;
			mTranslatedMaterials[sName]=pMaterial;
		}

		for(int f=0;f<pFile->m_Scene.nFrameCount;f++)
		{
			CFrame *pFrame=&pFrames[f];

			vector<S3DSObject *>::iterator i;
			for(i=pFile->m_vObjects.begin();i!=pFile->m_vObjects.end();i++)
			{
				S3DSObject *pObject=*i;
				if(pObject->bVisible)
				{
					pFrame->m_nPolygons+=pObject->vAnimationFrames[f]->nFaces;
				}
			}
			if(pFrame->m_nPolygons)
			{
				int nPol=0;
				pFrame->m_pPolygons=new CMaterialPolygon[pFrame->m_nPolygons];

				for(int x=0;x<pFile->m_vObjects.size();x++)
				{
					int nBasePol=nPol;
					S3DSObject *pObject=pFile->m_vObjects[x];
					S3DSFrame  *p3DSFrame=pObject->vAnimationFrames[f];

					//_MRT("Loaded Object %s, %d polygons",pObject->sName,p3DSFrame->nFaces);
					if(pObject->bVisible && p3DSFrame->nVertexes && p3DSFrame->nFaces)
					{
						double step=(0.7/(double)p3DSFrame->nFaces);
						for(int y=0;y<p3DSFrame->nFaces;y++)
						{
							pFrame->m_pPolygons[nPol].m_nVertexes=3;
							pFrame->m_pPolygons[nPol].m_pVertexes=new CVector[3];
							pFrame->m_pPolygons[nPol].m_pTextureCoords=new CVector[3];
							pFrame->m_pPolygons[nPol].m_pVertexNormals=new CVector[3];
							pFrame->m_pPolygons[nPol].m_pVertexes[0]=p3DSFrame->pVertexes[p3DSFrame->pFaces[y*3]];
							pFrame->m_pPolygons[nPol].m_pVertexes[1]=p3DSFrame->pVertexes[p3DSFrame->pFaces[y*3+1]];
							pFrame->m_pPolygons[nPol].m_pVertexes[2]=p3DSFrame->pVertexes[p3DSFrame->pFaces[y*3+2]];
							if(p3DSFrame->pColorVertexes)
							{
								pFrame->m_pPolygons[nPol].m_pVertexColors=new CVector[3];
								for(int z=0;z<p3DSFrame->nColorFaces;z++)
								{
									if(p3DSFrame->pColorFaces[z].nFaceIndex==y)
									{
										pFrame->m_pPolygons[nPol].m_pVertexColors[0]=p3DSFrame->pColorVertexes[p3DSFrame->pColorFaces[z].pColorVertexes[0]];
										pFrame->m_pPolygons[nPol].m_pVertexColors[1]=p3DSFrame->pColorVertexes[p3DSFrame->pColorFaces[z].pColorVertexes[1]];
										pFrame->m_pPolygons[nPol].m_pVertexColors[2]=p3DSFrame->pColorVertexes[p3DSFrame->pColorFaces[z].pColorVertexes[2]];
										break;
									}
								}
							}
							for(int z=0;z<p3DSFrame->nTextFaces;z++)
							{
								if(p3DSFrame->pTextFaces[z].nFaceIndex==y)
								{
									pFrame->m_pPolygons[nPol].m_pTextureCoords[0]=p3DSFrame->pTextVertexes[p3DSFrame->pTextFaces[z].pTextVertexes[0]];
									pFrame->m_pPolygons[nPol].m_pTextureCoords[1]=p3DSFrame->pTextVertexes[p3DSFrame->pTextFaces[z].pTextVertexes[1]];
									pFrame->m_pPolygons[nPol].m_pTextureCoords[2]=p3DSFrame->pTextVertexes[p3DSFrame->pTextFaces[z].pTextVertexes[2]];
									break;
								}
							}

							for(int a=0;a<3;a++)
							{
								if(p3DSFrame->pVertexNormals)
								{
									pFrame->m_pPolygons[nPol].m_pVertexNormals[a]=p3DSFrame->pVertexNormals[p3DSFrame->pFaces[y*3+a]];
								}
								else
								{
									CVector vNormal;
									for(int b=0;b<p3DSFrame->nFaces;b++)
									{
										if(p3DSFrame->pFaces[b*3]==p3DSFrame->pFaces[y*3+a] || p3DSFrame->pFaces[b*3+1]==p3DSFrame->pFaces[y*3+a] || p3DSFrame->pFaces[b*3+2]==p3DSFrame->pFaces[y*3+a])
										{
											CPlane plane(p3DSFrame->pVertexes[p3DSFrame->pFaces[b*3]],p3DSFrame->pVertexes[p3DSFrame->pFaces[b*3+1]],p3DSFrame->pVertexes[p3DSFrame->pFaces[b*3+2]]);
											vNormal+=(CVector)plane;
										}
									}
									vNormal.N();
									pFrame->m_pPolygons[nPol].m_pVertexNormals[a]=vNormal;
								}
							}
							nPol++;
						}
					}
					if(pObject->dwMaterialId!=-1)
					{
						for(int y=0;y<p3DSFrame->sObjectMaterials.size();y++)
						{
							S3DSObjectMaterial	*pObjectMaterial=p3DSFrame->sObjectMaterials[y];
							S3DSMaterial		*p3DSMaterial=m3DSMaterials[pObjectMaterial->sName];
							CMaterial			*pMaterial=mTranslatedMaterials[pObjectMaterial->sName];
							for(int z=0;z<pObjectMaterial->nFaces;z++)
							{
								int nPolygonIndex=nBasePol+pObjectMaterial->pFaces[z];
								CMaterialPolygon *pPolygon=&pFrame->m_pPolygons[nPolygonIndex];
								pPolygon->m_pMaterial=pMaterial;
								for(int a=0;a<pFrame->m_pPolygons[nPolygonIndex].m_nVertexes;a++)
								{
									CVector vTextureCenter(0.5,0.5,0);
									CMatrix m;
									pPolygon->m_pTextureCoords[a].c[0]-=p3DSMaterial->fTextureUOffset;
									pPolygon->m_pTextureCoords[a].c[1]-=p3DSMaterial->fTextureVOffset;
									m.S(p3DSMaterial->fTextureUScale,p3DSMaterial->fTextureVScale,1,vTextureCenter);
									pPolygon->m_pTextureCoords[a]*=m;
									m.R(CVector(0,0,1),p3DSMaterial->fTextureAngle,vTextureCenter);
									pPolygon->m_pTextureCoords[a]*=m;
								}
							}
						}
					}
					else
					{
						char sName[512];
						sprintf(sName,"<Color Material %d,%d,%d>",GetRValue(pObject->cWireframeColor),GetGValue(pObject->cWireframeColor),GetBValue(pObject->cWireframeColor));
						CMaterial *pMaterial=new CMaterial;
						pMaterial->cAmbientColor=pObject->cWireframeColor;
						pMaterial->cDiffuseColor=pObject->cWireframeColor;
						pMaterial->cSpecularColor=pObject->cWireframeColor;
						pMaterial->dwMaterialType=MATERIAL_SOLID;
						pMaterial->fOpacity=1.0;

						CMaterial *pTest=FindMaterial(pMaterial);
						if(pTest)
							{delete pMaterial;pMaterial=pTest;}
						else
							{AddMaterial(pMaterial);}

						for(int a=0;a<p3DSFrame->nFaces;a++)
						{
							int nPolygonIndex=nBasePol+a;
							pFrame->m_pPolygons[nBasePol+a].m_pMaterial=pMaterial;
						}
					}
				}
			}
			pFrame->CalcBBox();
			pFrame->GeneratePreprocessedRenderBuffer();
		}

		int x;
		for(x=0;x<pFile->m_vLights.size();x++)
		{
			S3DSLight *p3DSLight=pFile->m_vLights[x];
			DWORD dwLightType=0;
			if(p3DSLight->dwType==S3DS_LIGHT_TYPE_FREE){dwLightType=LIGHT_TYPE_AMBIENT;}
			if(p3DSLight->dwType==S3DS_LIGHT_TYPE_OMNI){dwLightType=LIGHT_TYPE_OMNI;}
			if(p3DSLight->dwType==S3DS_LIGHT_TYPE_SPOT){dwLightType=LIGHT_TYPE_SPOT;}

			COLORREF color;
			color=p3DSLight->cColor;
			
			if(p3DSLight->dwType==S3DS_LIGHT_TYPE_FREE)
			{
				// La unica forma de controlar la intensidad de las luces de ambiente es quitarles mucha intensidad
				// de color y despues corregir el cambio de intensidad con el multiplicador.
				// Con el resto de luces se usa el multiplicador para cambiar la atenuacion constante.

				CVector vColor=RGBToVector(p3DSLight->cColor);
				for(int x=0;x<3;x++)
				{
					vColor.c[x]*=0.2*p3DSLight->fMultiplier;
					if(vColor.c[x]>1.0){vColor.c[x]=1.0;}
				}
				color=VectorToRGB(&vColor);
			}

			
			CLight *pLight=new CLight(p3DSLight->sName,dwLightType,p3DSLight->vPosition,color);
			pLight->m_fAttenuationConstant=1.0/p3DSLight->fMultiplier;
			if(p3DSLight->bAttenuationOn)
			{
				pLight->m_fAttenuationLinear=1.0/(p3DSLight->fAttenuationEnd);
				pLight->m_fAttenuationQuadratic=1.0/((p3DSLight->fAttenuationEnd*p3DSLight->fAttenuationEnd));
			}
			float fAngle=p3DSLight->fSpotFallOff/2;
			float fFocusAngle=(p3DSLight->fSpotHotSpot+1)/2;
			float fExponent=((fAngle-fFocusAngle)/fAngle)*128.0;

			pLight->m_bIsActive=!p3DSLight->bIsOff;
			if(p3DSLight->dwType==S3DS_LIGHT_TYPE_SPOT)
			{
				pLight->m_fSpotAngle=fAngle;
				pLight->m_fSpotExponent=fExponent;
				pLight->m_vSpotDir=p3DSLight->vSpotTarget-p3DSLight->vPosition;
			}

			if(pModel->m_nLights<MAX_LIGHTS)
			{
				pModel->m_pLights[pModel->m_nLights]=pLight;
				pModel->m_nLights++;
			}
		}
	}
	return bResult;
}

bool CResourceStore::LoadMdl(CModel *pModel,string sFileName)
{
	bool bResult=false;
	CMdlFileType	mdlFile;
	if(mdlFile.Open(sFileName.c_str()))
	{
		bResult=true;
		//_MRT("Loading MDL file %s",sFileName.c_str());

		CTexture  *pTexture=NULL;
		CMaterial *pMaterial=new CMaterial;
		pMaterial->cAmbientColor=RGB(128,128,128);
		pMaterial->cDiffuseColor=RGB(128,128,128);
		pMaterial->cSpecularColor=RGB(128,128,128);
		pMaterial->dwMaterialType=MATERIAL_TEXTURED;

		if(mdlFile.m_nSkins)
		{
			void *pBuffer=new unsigned char [mdlFile.m_nOpenGLSkinWidth*mdlFile.m_nOpenGLSkinHeigth*3];
			double x,y,rx,ry;
			rx=((double)mdlFile.m_Header.nSkinWidth)/mdlFile.m_nOpenGLSkinWidth;
			ry=((double)mdlFile.m_Header.nSkinHeight)/mdlFile.m_nOpenGLSkinHeigth;
			for(y=0;y<mdlFile.m_nOpenGLSkinHeigth;y++)
			{
				for(x=0;x<mdlFile.m_nOpenGLSkinWidth;x++)
				{
					int ox,oy;
					ox=x*rx;
					oy=y*ry;
					((unsigned char *)pBuffer)[0+(int)(x+y*mdlFile.m_nOpenGLSkinWidth)*3]=g_pPaletteLmp[mdlFile.m_pSkins[0]->pPixels[(int)(ox+((int)oy)*mdlFile.m_Header.nSkinWidth)]].cRed;
					((unsigned char *)pBuffer)[1+(int)(x+y*mdlFile.m_nOpenGLSkinWidth)*3]=g_pPaletteLmp[mdlFile.m_pSkins[0]->pPixels[(int)(ox+((int)oy)*mdlFile.m_Header.nSkinWidth)]].cGreen;
					((unsigned char *)pBuffer)[2+(int)(x+y*mdlFile.m_nOpenGLSkinWidth)*3]=g_pPaletteLmp[mdlFile.m_pSkins[0]->pPixels[(int)(ox+((int)oy)*mdlFile.m_Header.nSkinWidth)]].cBlue;
				}
			}

			pTexture=FindTexture(sFileName+":0");
			if(pTexture){FreeTexture(sFileName+":0");}
			pTexture=new CTexture(sFileName+":0",mdlFile.m_nOpenGLSkinWidth,mdlFile.m_nOpenGLSkinHeigth,pBuffer,GL_RGB);
			AddTexture(pTexture);
		}		

		pMaterial->nTextureIndex=pTexture?pTexture->m_nOpenGlIndex:-1;
		pMaterial->pTexture=pTexture;
		CMaterial *pTest=FindMaterial(pMaterial);
		if(pTest)
			{delete pMaterial;pMaterial=pTest;}
		else
			{AddMaterial(pMaterial);}

		CModelAnimation	*pAnimation	=new CModelAnimation[1];
		CFrame		    *pFrames	=new CFrame[mdlFile.m_nFrames];
		pModel->m_sName=sFileName;
		pModel->m_dwType=MODEL_TYPE_MDL;
		pModel->m_nAnimations=1;
		pModel->m_pAnimations=pAnimation;
		pModel->m_pAnimations[0].m_nFrames=mdlFile.m_nFrames;
		pModel->m_pAnimations[0].m_pFrames=pFrames;

		for(int f=0;f<mdlFile.m_nFrames;f++)
		{
			int x;
			pFrames[f].m_pPolygons=new CMaterialPolygon[mdlFile.m_Header.nTriangles];
			CVector *pVertexes=new CVector[mdlFile.m_Header.nVertexes];
			CVector *pNormals=new CVector[mdlFile.m_Header.nVertexes];

			for(x=0;x<mdlFile.m_Header.nVertexes;x++)
			{
				pVertexes[x].c[0]=((double)mdlFile.m_pFrames[f]->pnVertexIndexes[x].x)*((double)mdlFile.m_Header.pfScale[0])+(double)mdlFile.m_Header.pfOrigin[0];
				pVertexes[x].c[1]=((double)mdlFile.m_pFrames[f]->pnVertexIndexes[x].z)*((double)mdlFile.m_Header.pfScale[2])+(double)mdlFile.m_Header.pfOrigin[2];
				pVertexes[x].c[2]=((double)mdlFile.m_pFrames[f]->pnVertexIndexes[x].y)*((double)mdlFile.m_Header.pfScale[1])+(double)mdlFile.m_Header.pfOrigin[1];
			}

			for(x=0;x<mdlFile.m_Header.nVertexes;x++)
			{
				for(int a=0;a<3;a++)
				{
					CVector vNormal;
					for(int b=0;b<mdlFile.m_Header.nTriangles;b++)
					{
						if(	 mdlFile.m_pTriangles[b].pnVertexIndexes[0]==x || 
							 mdlFile.m_pTriangles[b].pnVertexIndexes[1]==x || 
							 mdlFile.m_pTriangles[b].pnVertexIndexes[2]==x)
						{
							CPlane plane(pVertexes[mdlFile.m_pTriangles[b].pnVertexIndexes[0]],pVertexes[mdlFile.m_pTriangles[b].pnVertexIndexes[1]],pVertexes[mdlFile.m_pTriangles[b].pnVertexIndexes[2]]);
							vNormal+=(CVector)plane;
						}
					}
					vNormal.N();
					pNormals[x]=vNormal;
				}
			}
			
			for(int n=0;n<mdlFile.m_Header.nTriangles;n++)
			{
				CVector vTriangleVertexes[3];
				CVector vTextCoord[3];
				vTriangleVertexes[0].c[0]=((double)mdlFile.m_pFrames[f]->pnVertexIndexes[mdlFile.m_pTriangles[n].pnVertexIndexes[0]].x)*((double)mdlFile.m_Header.pfScale[0])+(double)mdlFile.m_Header.pfOrigin[0];
				vTriangleVertexes[0].c[1]=((double)mdlFile.m_pFrames[f]->pnVertexIndexes[mdlFile.m_pTriangles[n].pnVertexIndexes[0]].z)*((double)mdlFile.m_Header.pfScale[2])+(double)mdlFile.m_Header.pfOrigin[2];
				vTriangleVertexes[0].c[2]=((double)mdlFile.m_pFrames[f]->pnVertexIndexes[mdlFile.m_pTriangles[n].pnVertexIndexes[0]].y)*((double)mdlFile.m_Header.pfScale[1])+(double)mdlFile.m_Header.pfOrigin[1];
				vTriangleVertexes[1].c[0]=((double)mdlFile.m_pFrames[f]->pnVertexIndexes[mdlFile.m_pTriangles[n].pnVertexIndexes[1]].x)*((double)mdlFile.m_Header.pfScale[0])+(double)mdlFile.m_Header.pfOrigin[0];
				vTriangleVertexes[1].c[1]=((double)mdlFile.m_pFrames[f]->pnVertexIndexes[mdlFile.m_pTriangles[n].pnVertexIndexes[1]].z)*((double)mdlFile.m_Header.pfScale[2])+(double)mdlFile.m_Header.pfOrigin[2];
				vTriangleVertexes[1].c[2]=((double)mdlFile.m_pFrames[f]->pnVertexIndexes[mdlFile.m_pTriangles[n].pnVertexIndexes[1]].y)*((double)mdlFile.m_Header.pfScale[1])+(double)mdlFile.m_Header.pfOrigin[1];
				vTriangleVertexes[2].c[0]=((double)mdlFile.m_pFrames[f]->pnVertexIndexes[mdlFile.m_pTriangles[n].pnVertexIndexes[2]].x)*((double)mdlFile.m_Header.pfScale[0])+(double)mdlFile.m_Header.pfOrigin[0];
				vTriangleVertexes[2].c[1]=((double)mdlFile.m_pFrames[f]->pnVertexIndexes[mdlFile.m_pTriangles[n].pnVertexIndexes[2]].z)*((double)mdlFile.m_Header.pfScale[2])+(double)mdlFile.m_Header.pfOrigin[2];
				vTriangleVertexes[2].c[2]=((double)mdlFile.m_pFrames[f]->pnVertexIndexes[mdlFile.m_pTriangles[n].pnVertexIndexes[2]].y)*((double)mdlFile.m_Header.pfScale[1])+(double)mdlFile.m_Header.pfOrigin[1];

				vTextCoord[0].c[0]=((double)mdlFile.m_pSkinVertexes[mdlFile.m_pTriangles[n].pnVertexIndexes[0]].x)/((double)mdlFile.m_Header.nSkinWidth);
				vTextCoord[0].c[1]=((double)mdlFile.m_pSkinVertexes[mdlFile.m_pTriangles[n].pnVertexIndexes[0]].y)/((double)mdlFile.m_Header.nSkinHeight);
				vTextCoord[1].c[0]=((double)mdlFile.m_pSkinVertexes[mdlFile.m_pTriangles[n].pnVertexIndexes[1]].x)/((double)mdlFile.m_Header.nSkinWidth);
				vTextCoord[1].c[1]=((double)mdlFile.m_pSkinVertexes[mdlFile.m_pTriangles[n].pnVertexIndexes[1]].y)/((double)mdlFile.m_Header.nSkinHeight);
				vTextCoord[2].c[0]=((double)mdlFile.m_pSkinVertexes[mdlFile.m_pTriangles[n].pnVertexIndexes[2]].x)/((double)mdlFile.m_Header.nSkinWidth);
				vTextCoord[2].c[1]=((double)mdlFile.m_pSkinVertexes[mdlFile.m_pTriangles[n].pnVertexIndexes[2]].y)/((double)mdlFile.m_Header.nSkinHeight);

				if(mdlFile.m_pSkinVertexes[mdlFile.m_pTriangles[n].pnVertexIndexes[0]].nBack==0x20 && !mdlFile.m_pTriangles[n].nFront){vTextCoord[0].c[0]+=0.5;}
				if(mdlFile.m_pSkinVertexes[mdlFile.m_pTriangles[n].pnVertexIndexes[1]].nBack==0x20 && !mdlFile.m_pTriangles[n].nFront){vTextCoord[1].c[0]+=0.5;}
				if(mdlFile.m_pSkinVertexes[mdlFile.m_pTriangles[n].pnVertexIndexes[2]].nBack==0x20 && !mdlFile.m_pTriangles[n].nFront){vTextCoord[2].c[0]+=0.5;}
				// A veces se cargan poligonos no validos (como por ejemplo 3 puntos en linea)
				// ha pasado con el poligono 1 del frame 0 del modelo soldier.mdl
				// Por lo tanto hay que comprbar la valided del poligono antes de meterlo en el poliedro
				// la comprobacion que se hace es la de calcular el std::vector normal al poligono, no se introduce si alguna
				// de sus componentes es NaN o las tres son cero.

				CVector t1;
				CVector t2;
				CVector t3;
				t1=vTriangleVertexes[1]-vTriangleVertexes[0];
				t2=vTriangleVertexes[2]-vTriangleVertexes[0];
				t3=t1^t2;
				if(!_isnan(t3.c[0]) && !_isnan(t3.c[1]) && !_isnan(t3.c[2]) && 
					(t3.c[0]!=0 || t3.c[1]!=0 || t3.c[2]!=0))
				{

					CMaterialPolygon *pPolygon=&pFrames[f].m_pPolygons[n];
					pPolygon->m_pMaterial=pMaterial;
					pPolygon->m_nVertexes=3;
					pPolygon->m_pVertexes=new CVector[3];
					pPolygon->m_pVertexes[0]=vTriangleVertexes[0];
					pPolygon->m_pVertexes[1]=vTriangleVertexes[1];
					pPolygon->m_pVertexes[2]=vTriangleVertexes[2];
					pPolygon->m_pTextureCoords=new CVector[3];
					pPolygon->m_pTextureCoords[0]=vTextCoord[0];
					pPolygon->m_pTextureCoords[1]=vTextCoord[1];
					pPolygon->m_pTextureCoords[2]=vTextCoord[2];
					pPolygon->m_pVertexNormals=new CVector[3];
					pPolygon->m_pVertexNormals[0]=pNormals[mdlFile.m_pTriangles[n].pnVertexIndexes[0]];
					pPolygon->m_pVertexNormals[1]=pNormals[mdlFile.m_pTriangles[n].pnVertexIndexes[1]];
					pPolygon->m_pVertexNormals[2]=pNormals[mdlFile.m_pTriangles[n].pnVertexIndexes[2]];
					pFrames[f].m_nPolygons++;
				}
			}
			pFrames[f].CalcBBox();
			pFrames[f].GeneratePreprocessedRenderBuffer();
			delete [] pVertexes;
			delete [] pNormals;
		}
	}
	return bResult;
}

void CResourceStore::AddModel(CModel *pModel)
{
	m_mModels[pModel->m_sName]=pModel;
}

void CResourceStore::FreeModels()
{
	map<string,CModel*>::iterator i;
	for(i=m_mModels.begin();i!=m_mModels.end();i++)
	{
		CModel *pModel=i->second;
		if(pModel){delete pModel;pModel=NULL;}
	}
	m_mModels.clear();
}

void CResourceStore::FreeMaterials()
{
	deque<CMaterial*>::iterator i;
	for(i=m_lMaterials.begin();i!=m_lMaterials.end();i++)
	{
		CMaterial *pMaterial=*i;
		if(pMaterial){delete pMaterial;pMaterial=NULL;}
	}
	m_lMaterials.clear();
}

void CResourceStore::Destroy()
{
	FreeAllResources();
}

void CResourceStore::FreeAllResources()
{
	FreeModels();
	FreeTextures();
	FreeMaterials();
}

CMaterial *CResourceStore::FindMaterial(CMaterial *pMaterial)
{
	deque<CMaterial*>::iterator i;
	for(i=m_lMaterials.begin();i!=m_lMaterials.end();i++)
	{
		CMaterial *pExisting=*i;
		if(*pExisting==*pMaterial){return pExisting;}
	}
	return NULL;
}

CModel *CResourceStore::FindModel(string sName)
{
	map<string,CModel*>::iterator i;
	i=m_mModels.find(sName);
	if(i!=m_mModels.end())
	{
		CModel *pModel=i->second;
		return pModel;
	}
	return NULL;
}

void CResourceStore::AddMaterial(CMaterial *pMaterial)
{
	pMaterial->CreateList();
	m_lMaterials.push_back(pMaterial);
}

void CResourceStore::Init(HDC hDC,HGLRC hRenderContext)
{
	m_hDC=hDC;
	m_hRenderContext=hRenderContext;
}

HGLRC CResourceStore::GetRenderContext()
{
	return m_hRenderContext;
}

void CResourceStore::ReplaceFilePath(string sOldFileName,string sNewFileName)
{
	map<string,CTexture*>::iterator iTexture=m_mTextures.find(sOldFileName);
	map<string,CModel*>::iterator	iModel=m_mModels.find(sOldFileName);
	if(iTexture!=m_mTextures.end())	
	{
		CTexture *pTexture=iTexture->second;
		m_mTextures.erase(iTexture);
		m_mTextures[sNewFileName]=pTexture;
		pTexture->m_sName=sNewFileName;
	}
	if(iModel!=m_mModels.end())
	{
		CModel *pModel=iModel->second;
		m_mModels.erase(iModel);
		m_mModels[sNewFileName]=pModel;
		pModel->m_sName=sNewFileName;
	}
}
void CResourceStore::FreeTexture(string sFileName)
{
	map<string,CTexture*>::iterator i;
	for(i=m_mTextures.begin();i!=m_mTextures.end();i++)
	{
		CTexture *pTexture=i->second;
		if(i->first==sFileName)
		{
			if(pTexture->m_nOpenGlIndex){glDeleteTextures(1,&pTexture->m_nOpenGlIndex);}
			delete pTexture;pTexture=NULL;
			m_mTextures.erase(i);
			break;
		}
	}
}
