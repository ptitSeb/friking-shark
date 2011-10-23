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


// GCMFiles.cpp: Game Compiled Model file loader/saver.
// .gcm files are files designed to store models to be loaded fast.
//
//////////////////////////////////////////////////////////////////////


#include "./StdAfx.h"
#include "VectorLib.h"
#include "GCMFiles.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

SGCMHeader::SGCMHeader()
{
	memcpy(sMagic,GCM_FILE_MAGIC,GCM_FILE_MAGIC_LENGTH);
	nVersion=GCM_FILE_VERSION;
	nFrames=0;
}

SGCMTextureLevel::SGCMTextureLevel()
{
	pTexVertexArray=NULL;
}

SGCMTextureLevel::~SGCMTextureLevel()
{
	delete [] pTexVertexArray;
	pTexVertexArray=NULL;
}

SGCMBufferData::SGCMBufferData()
{
	vAmbientColor[0]=vAmbientColor[1]=vAmbientColor[2]=0.5;
	vDiffuseColor[0]=vDiffuseColor[1]=vDiffuseColor[2]=0.5;
	vSpecularColor[0]=vSpecularColor[1]=vSpecularColor[2]=0.5;

	fShininess=0;
	fOpacity=1;
	
	nVertexes=0;
	nFaces=0;
}

SGCMBuffer::SGCMBuffer()
{
	pVertexArray=NULL;
	pNormalArray=NULL;
	pNormalMapArray=NULL;
	pColorArray=NULL;
	pFaceVertexIndexes=NULL;
	pTangentArray=NULL;
	pBitangentArray=NULL;
}

SGCMBuffer::~SGCMBuffer()
{
	delete [] pVertexArray;
	delete [] pNormalArray;
	delete [] pNormalMapArray;
	delete [] pColorArray;
	delete [] pFaceVertexIndexes;
	delete [] pTangentArray;
	delete [] pBitangentArray;
	
	pVertexArray=NULL;
	pNormalArray=NULL;
	pNormalMapArray=NULL;
	pColorArray=NULL;
	pFaceVertexIndexes=NULL;
	pTangentArray=NULL;
	pBitangentArray=NULL;

	for(unsigned int x=0;x<vTextureLevels.size();x++)
	{
		delete vTextureLevels[x];
	}
}

SGCMFrameData::SGCMFrameData()
{
	vMaxs[0]=vMaxs[1]=vMaxs[2]=0;
	vMins[0]=vMins[1]=vMins[2]=0;
	vSize[0]=vSize[1]=vSize[2]=0;
	dRadius=0;
}

SGCMFrame::SGCMFrame()
{
}

SGCMFrame::~SGCMFrame()
{
	for(unsigned int x=0;x<vBuffers.size();x++)
	{
		delete vBuffers[x];
	}
}

CGCMFileType::CGCMFileType()
{
}

CGCMFileType::~CGCMFileType()
{
	Clear();
}

bool CGCMFileType::Open(const char *psFileName)
{
	Clear();

	bool bOk=true;
	FILE *pFile=fopen(psFileName,"rb");
	bOk=(pFile!=NULL);

	SGCMHeader header;

	// Read and check file header,

	if(bOk){bOk=(fread(&header,sizeof(header),1,pFile)==1);}
	if(bOk){bOk=(memcmp(&header.sMagic,GCM_FILE_MAGIC,GCM_FILE_MAGIC_LENGTH)==0 && header.nVersion<=GCM_FILE_VERSION);}

	// Read frames.
	
	for(unsigned int f=0;bOk && f<header.nFrames;f++)
	{
		SGCMFrame *pFrame=new SGCMFrame;
		unsigned int nBuffers=0;

		if(bOk){bOk=(fread(&pFrame->data,sizeof(pFrame->data),1,pFile)==1);}
		if(bOk){bOk=(fread(&nBuffers,sizeof(nBuffers),1,pFile)==1);}

		for(unsigned int b=0;bOk && b<nBuffers;b++)
		{
			SGCMBuffer *pBuffer=new SGCMBuffer;
			unsigned int nTextureLevels=0;
			unsigned int nFlags=0;

			if(bOk){bOk=(fread(&pBuffer->data,sizeof(pBuffer->data),1,pFile)==1);}
			if(bOk){bOk=(fread(&nTextureLevels,sizeof(nTextureLevels),1,pFile)==1);}
			if(bOk){bOk=(fread(&nFlags,sizeof(nFlags),1,pFile)==1);}

			if(bOk)
			{
				pBuffer->pVertexArray=new float [pBuffer->data.nVertexes*3];
				bOk=(fread(pBuffer->pVertexArray,sizeof(float)*pBuffer->data.nVertexes*3,1,pFile)==1);
			}
			if(bOk)
			{
				pBuffer->pFaceVertexIndexes=new unsigned int [pBuffer->data.nFaces*3];
				bOk=(fread(pBuffer->pFaceVertexIndexes,sizeof(unsigned int)*pBuffer->data.nFaces*3,1,pFile)==1);
			}
			if(bOk && nFlags&GCM_BUFFER_FLAG_HAS_COLORS)
			{
				pBuffer->pColorArray=new float [pBuffer->data.nVertexes*4];
				bOk=(fread(pBuffer->pColorArray,sizeof(float)*pBuffer->data.nVertexes*4,1,pFile)==1);
			}			
			if(bOk && nFlags&GCM_BUFFER_FLAG_HAS_NORMALS)
			{
				pBuffer->pNormalArray=new float [pBuffer->data.nVertexes*3];
				bOk=(fread(pBuffer->pNormalArray,sizeof(float)*pBuffer->data.nVertexes*3,1,pFile)==1);
			}
			if(bOk && nFlags&GCM_BUFFER_FLAG_HAS_NORMAL_MAP)
			{
				unsigned int nNormalMapFileSize=0;
				if(bOk){bOk=(fread(&nNormalMapFileSize,sizeof(nNormalMapFileSize),1,pFile)==1);}
				if(bOk)
				{
					char *pNormalMapFile=new char [nNormalMapFileSize+1];
					if(nNormalMapFileSize){bOk=(fread(pNormalMapFile,nNormalMapFileSize,1,pFile)==1);}
					pNormalMapFile[nNormalMapFileSize]=0;
					pBuffer->sNormalMap=pNormalMapFile;
					delete [] pNormalMapFile;
					pNormalMapFile=NULL;
				}
				if(bOk)
				{
					pBuffer->pNormalMapArray=new float [pBuffer->data.nVertexes*2];
					bOk=(fread(pBuffer->pNormalMapArray,sizeof(float)*pBuffer->data.nVertexes*2,1,pFile)==1);
				}
			}

			if(bOk && nFlags&GCM_BUFFER_FLAG_HAS_NORMAL_BASIS)
			{
				pBuffer->pTangentArray=new float [pBuffer->data.nVertexes*3];
				pBuffer->pBitangentArray=new float [pBuffer->data.nVertexes*3];
				bOk=(fread(pBuffer->pTangentArray,sizeof(float)*pBuffer->data.nVertexes*3,1,pFile)==1);
				if(bOk){(fread(pBuffer->pBitangentArray,sizeof(float)*pBuffer->data.nVertexes*3,1,pFile)==1);}
			}
			for(unsigned int l=0;bOk && l<nTextureLevels;l++)
			{
				SGCMTextureLevel *pTextureLevel=new SGCMTextureLevel;
				unsigned int nFileNameSize=0;
				if(bOk){bOk=(fread(&nFileNameSize,sizeof(nFileNameSize),1,pFile)==1);}
				if(bOk)
				{
					char *pFileName=new char [nFileNameSize+1];
					bOk=(fread(pFileName,nFileNameSize,1,pFile)==1);
					pFileName[nFileNameSize]=0;
					pTextureLevel->sTexture=pFileName;
					delete [] pFileName;
					pFileName=NULL;
				}
				if(bOk)
				{
					pTextureLevel->pTexVertexArray=new float [pBuffer->data.nVertexes*2];
					bOk=(fread(pTextureLevel->pTexVertexArray,sizeof(float)*pBuffer->data.nVertexes*2,1,pFile)==1);
				}
				pBuffer->vTextureLevels.push_back(pTextureLevel);
			}

			pFrame->vBuffers.push_back(pBuffer);
		}
		m_vFrames.push_back(pFrame);
	}
	if(!bOk){Clear();}

	if(pFile)
	{
		fclose(pFile);
		pFile=NULL;
	}
	return bOk;
}

bool CGCMFileType::Save(const char *psFileName)
{
	UpdateSizes();

	bool bOk=true;
	FILE *pFile=fopen(psFileName,"wb");
	bOk=(pFile!=NULL);
	
	SGCMHeader header;
	
	// Write file header,
	
	header.nFrames=m_vFrames.size();
	if(bOk){bOk=(fwrite(&header,sizeof(header),1,pFile)==1);}
	
	// Write frames.
	
	for(unsigned int f=0;bOk && f<m_vFrames.size();f++)
	{
		SGCMFrame *pFrame=m_vFrames[f];
		unsigned int nBuffers=pFrame->vBuffers.size();
		
		if(bOk){bOk=(fwrite(&pFrame->data,sizeof(pFrame->data),1,pFile)==1);}
		if(bOk){bOk=(fwrite(&nBuffers,sizeof(nBuffers),1,pFile)==1);}
		
		for(unsigned int b=0;bOk && b<nBuffers;b++)
		{
			SGCMBuffer *pBuffer=pFrame->vBuffers[b];
			unsigned int nTextureLevels=pBuffer->vTextureLevels.size();
			unsigned int nFlags=0;
			nFlags|=pBuffer->pColorArray?GCM_BUFFER_FLAG_HAS_COLORS:0;
			nFlags|=pBuffer->pNormalArray?GCM_BUFFER_FLAG_HAS_NORMALS:0;
			nFlags|=pBuffer->pNormalMapArray?GCM_BUFFER_FLAG_HAS_NORMAL_MAP:0;
			nFlags|=pBuffer->pTangentArray && pBuffer->pBitangentArray?GCM_BUFFER_FLAG_HAS_NORMAL_BASIS:0;
			
			if(bOk){bOk=(fwrite(&pBuffer->data,sizeof(pBuffer->data),1,pFile)==1);}			
			if(bOk){bOk=(fwrite(&nTextureLevels,sizeof(nTextureLevels),1,pFile)==1);}
			if(bOk){bOk=(fwrite(&nFlags,sizeof(nFlags),1,pFile)==1);}
			if(bOk){bOk=(fwrite(pBuffer->pVertexArray,sizeof(float)*pBuffer->data.nVertexes*3,1,pFile)==1);}
			if(bOk){bOk=(fwrite(pBuffer->pFaceVertexIndexes,sizeof(unsigned int)*pBuffer->data.nFaces*3,1,pFile)==1);}
			if(bOk && nFlags&GCM_BUFFER_FLAG_HAS_COLORS)
			{
				bOk=(fwrite(pBuffer->pColorArray,sizeof(float)*pBuffer->data.nVertexes*4,1,pFile)==1);
			}			
			if(bOk && nFlags&GCM_BUFFER_FLAG_HAS_NORMALS)
			{
				bOk=(fwrite(pBuffer->pNormalArray,sizeof(float)*pBuffer->data.nVertexes*3,1,pFile)==1);
			}
			if(bOk && nFlags&GCM_BUFFER_FLAG_HAS_NORMAL_MAP)
			{
				unsigned int nNormalMapFileSize=pBuffer->sNormalMap.length();
				if(bOk){bOk=(fwrite(&nNormalMapFileSize,sizeof(nNormalMapFileSize),1,pFile)==1);}
				if(bOk && nNormalMapFileSize){bOk=(fwrite(pBuffer->sNormalMap.c_str(),nNormalMapFileSize,1,pFile)==1);}
				if(bOk){bOk=(fwrite(pBuffer->pNormalMapArray,sizeof(float)*pBuffer->data.nVertexes*2,1,pFile)==1);}
			}
			if(bOk && nFlags&GCM_BUFFER_FLAG_HAS_NORMAL_BASIS)
			{
				bOk=(fwrite(pBuffer->pTangentArray,sizeof(float)*pBuffer->data.nVertexes*3,1,pFile)==1);
				if(bOk){bOk=(fwrite(pBuffer->pBitangentArray,sizeof(float)*pBuffer->data.nVertexes*3,1,pFile)==1);}
			}
			for(unsigned int l=0;bOk && l<nTextureLevels;l++)
			{
				SGCMTextureLevel *pTextureLevel=pBuffer->vTextureLevels[l];
				unsigned int nFileNameSize=pTextureLevel->sTexture.length();
				if(bOk){bOk=(fwrite(&nFileNameSize,sizeof(nFileNameSize),1,pFile)==1);}
				if(bOk && nFileNameSize){bOk=(fwrite(pTextureLevel->sTexture.c_str(),nFileNameSize,1,pFile)==1);}
				if(bOk){bOk=(fwrite(pTextureLevel->pTexVertexArray,sizeof(float)*pBuffer->data.nVertexes*2,1,pFile)==1);}
			}
		}
	}
	if(pFile)
	{
		fclose(pFile);
		pFile=NULL;
	}
	return bOk;
}

void CGCMFileType::Clear()
{
	RemoveFrames();
}

unsigned long CGCMFileType::GetFrames()
{
	return m_vFrames.size();
}	

unsigned long CGCMFileType::GetFrameBuffers(unsigned long nFrame )
{
	if(nFrame>=m_vFrames.size()){return 0;}
	return m_vFrames[nFrame]->vBuffers.size();
}

void CGCMFileType::GetFrameBBox(unsigned long nFrame,CVector *pvMins,CVector *pvMaxs)
{
	if(nFrame>=m_vFrames.size()){return;}
	for(unsigned int x=0;x<3;x++)
	{
		pvMins->c[x]=m_vFrames[nFrame]->data.vMins[x];
		pvMaxs->c[x]=m_vFrames[nFrame]->data.vMaxs[x];
	}
}

CVector CGCMFileType::GetFrameSize(unsigned long nFrame )
{
	if(nFrame>=m_vFrames.size()){return Origin;}
	CVector vSize;
	for(unsigned int x=0;x<3;x++){vSize.c[x]=m_vFrames[nFrame]->data.vSize[x];}
	return vSize;
}

double CGCMFileType::GetFrameRadius(unsigned long nFrame )
{
	if(nFrame>=m_vFrames.size()){return 0;}
	return m_vFrames[nFrame]->data.dRadius;
}


unsigned long CGCMFileType::AddFrame()
{
	SGCMFrame *pFrame=new SGCMFrame;
	m_vFrames.push_back(pFrame);
	return (m_vFrames.size()-1);
}

unsigned long CGCMFileType::AddBuffer(unsigned long nFrame )
{
	if(nFrame>=m_vFrames.size()){return 0;}
	SGCMBuffer *pBuffer=new SGCMBuffer;
	m_vFrames[nFrame]->vBuffers.push_back(pBuffer);
	return (m_vFrames[nFrame]->vBuffers.size()-1);
}

SGCMBuffer *CGCMFileType::GetBuffer( unsigned long nFrame, unsigned long nBuffer )
{
	if(nFrame>=m_vFrames.size()){return NULL;}
	if(nBuffer>=m_vFrames[nFrame]->vBuffers.size()){return NULL;}
	return m_vFrames[nFrame]->vBuffers[nBuffer];
}

void CGCMFileType::SetBufferMaterial(unsigned long nFrame,unsigned long nBuffer,CVector vAmbientColor,CVector vDiffuseColor,CVector vSpecularColor, float fShininess, float fOpacity )
{
	SGCMBuffer *pBuffer=GetBuffer(nFrame, nBuffer);
	if(pBuffer==NULL){return;}

	for(int c=0;c<3;c++)
	{
		pBuffer->data.vAmbientColor[c]=vAmbientColor.c[c];
		pBuffer->data.vDiffuseColor[c]=vDiffuseColor.c[c];
		pBuffer->data.vSpecularColor[c]=vSpecularColor.c[c];
	}
	pBuffer->data.fOpacity=fOpacity;
	pBuffer->data.fShininess=fShininess;
}

void CGCMFileType::SetBufferTexture(unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,const std::string &sTexture)
{
	SGCMBuffer *pBuffer=GetBuffer(nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	while(nTextureLevel>=pBuffer->vTextureLevels.size()){pBuffer->vTextureLevels.push_back(new SGCMTextureLevel);}
	SGCMTextureLevel *pTextureLevel=pBuffer->vTextureLevels[nTextureLevel];
	pTextureLevel->sTexture=sTexture;
}

void CGCMFileType::SetBufferNormalMap(unsigned long nFrame,unsigned long nBuffer,const std::string &sTexture)
{
	SGCMBuffer *pBuffer=GetBuffer(nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	pBuffer->sNormalMap=sTexture;
}

void CGCMFileType::SetBufferVertexes(unsigned long nFrame,unsigned long nBuffer,unsigned long nVertexes,float *pVertexes )
{
	SGCMBuffer *pBuffer=GetBuffer(nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	delete [] pBuffer->pVertexArray;
	pBuffer->data.nVertexes=nVertexes;
	pBuffer->pVertexArray=pVertexes;
}

void CGCMFileType::SetBufferFaces(unsigned long nFrame,unsigned long nBuffer,unsigned long nFaces,unsigned int *pFacesVertexes )
{
	SGCMBuffer *pBuffer=GetBuffer(nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	delete [] pBuffer->pFaceVertexIndexes;
	pBuffer->data.nFaces=nFaces;
	pBuffer->pFaceVertexIndexes=pFacesVertexes;
}

void CGCMFileType::SetBufferNormals(unsigned long nFrame,unsigned long nBuffer,float *pNormals )
{
	SGCMBuffer *pBuffer=GetBuffer(nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	delete [] pBuffer->pNormalArray;
	pBuffer->pNormalArray=pNormals;
}

void CGCMFileType::SetBufferColors(unsigned long nFrame,unsigned long nBuffer,float *pColors )
{
	SGCMBuffer *pBuffer=GetBuffer(nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	delete [] pBuffer->pColorArray;
	pBuffer->pColorArray=pColors;
}

void CGCMFileType::SetBufferTextureCoords(unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,float *pTexVertexes )
{
	SGCMBuffer *pBuffer=GetBuffer(nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	while(nTextureLevel>=pBuffer->vTextureLevels.size()){pBuffer->vTextureLevels.push_back(new SGCMTextureLevel);}
	SGCMTextureLevel *pTextureLevel=pBuffer->vTextureLevels[nTextureLevel];
	delete [] pTextureLevel->pTexVertexArray;
	pTextureLevel->pTexVertexArray=pTexVertexes;
}

void CGCMFileType::SetBufferNormalMapCoords(unsigned long nFrame,unsigned long nBuffer,float *pNormalMapArray)
{
	SGCMBuffer *pBuffer=GetBuffer(nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	delete [] pBuffer->pNormalMapArray;
	pBuffer->pNormalMapArray=pNormalMapArray;
}

void CGCMFileType::GetBufferMaterial(unsigned long nFrame,unsigned long nBuffer,CVector *pvAmbientColor,CVector *pvDiffuseColor,CVector *pvSpecularColor, float *pfShininess, float *pfOpacity )
{
	if(pvAmbientColor){*pvAmbientColor=Origin;}
	if(pvDiffuseColor){*pvDiffuseColor=Origin;}
	if(pvSpecularColor){*pvSpecularColor=Origin;}
	if(pfShininess){*pfShininess=0;}
	if(pfOpacity){*pfOpacity=1.0;}

	SGCMBuffer *pBuffer=GetBuffer(nFrame, nBuffer);
	if(pBuffer==NULL){return;}

	for(int c=0;c<3;c++)
	{
		if(pvAmbientColor){pvAmbientColor->c[c]=pBuffer->data.vAmbientColor[c];}
		if(pvDiffuseColor){pvDiffuseColor->c[c]=pBuffer->data.vDiffuseColor[c];}
		if(pvSpecularColor){pvSpecularColor->c[c]=pBuffer->data.vSpecularColor[c];}
	}
	if(pfShininess){*pfShininess=pBuffer->data.fShininess;}
	if(pfOpacity){*pfOpacity=pBuffer->data.fOpacity;}
}

void CGCMFileType::GetBufferTexture(unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,std::string *psTexture)
{
	if(psTexture){*psTexture="";}
	SGCMBuffer *pBuffer=GetBuffer(nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	if(nTextureLevel>=pBuffer->vTextureLevels.size()){return;}
	if(psTexture){*psTexture=pBuffer->vTextureLevels[nTextureLevel]->sTexture;}
}

void CGCMFileType::GetBufferNormalMap(unsigned long nFrame,unsigned long nBuffer,std::string *psTexture)
{
	if(psTexture){*psTexture="";}
	SGCMBuffer *pBuffer=GetBuffer(nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	if(psTexture){*psTexture=pBuffer->sNormalMap;}
}

void CGCMFileType::GetBufferTextureLevels(long unsigned int nFrame, long unsigned int nBuffer, long unsigned int* pnTextureLevels)
{
	if(pnTextureLevels){*pnTextureLevels=0;}
	SGCMBuffer *pBuffer=GetBuffer(nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	*pnTextureLevels=pBuffer->vTextureLevels.size();	
}

void CGCMFileType::GetBufferVertexes(unsigned long nFrame,unsigned long nBuffer,unsigned long *pVertexes,float **ppVertexes )
{
	if(pVertexes){*pVertexes=0;}
	if(ppVertexes){*ppVertexes=NULL;}
	SGCMBuffer *pBuffer=GetBuffer(nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	if(pVertexes){*pVertexes=pBuffer->data.nVertexes;}
	if(ppVertexes){*ppVertexes=pBuffer->pVertexArray;}
}

void CGCMFileType::GetBufferFaces(unsigned long nFrame,unsigned long nBuffer,unsigned long *pFaces,unsigned int **ppFacesVertexes )
{
	if(pFaces){*pFaces=0;}
	if(ppFacesVertexes){*ppFacesVertexes=NULL;}
	SGCMBuffer *pBuffer=GetBuffer(nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	if(pFaces){*pFaces=pBuffer->data.nFaces;}
	if(ppFacesVertexes){*ppFacesVertexes=pBuffer->pFaceVertexIndexes;}
}

void CGCMFileType::GetBufferNormals(unsigned long nFrame,unsigned long nBuffer,float **ppNormals )
{
	if(ppNormals){*ppNormals=NULL;}
	SGCMBuffer *pBuffer=GetBuffer(nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	if(ppNormals){*ppNormals=pBuffer->pNormalArray;}
}

void CGCMFileType::GetBufferColors(unsigned long nFrame,unsigned long nBuffer,float **ppColors )
{
	if(ppColors){*ppColors=NULL;}
	SGCMBuffer *pBuffer=GetBuffer(nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	if(ppColors){*ppColors=pBuffer->pColorArray;}
}

void CGCMFileType::GetBufferTextureCoords(unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,float **ppTexVertexes )
{
	if(ppTexVertexes){*ppTexVertexes=NULL;}
	SGCMBuffer *pBuffer=GetBuffer(nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	if(nTextureLevel>=pBuffer->vTextureLevels.size()){return;}
	if(ppTexVertexes){*ppTexVertexes=pBuffer->vTextureLevels[nTextureLevel]->pTexVertexArray;}
}

void CGCMFileType::GetBufferNormalMapCoords(unsigned long nFrame,unsigned long nBuffer,float **ppNormalMapVertexes )
{
	if(ppNormalMapVertexes){*ppNormalMapVertexes=NULL;}
	SGCMBuffer *pBuffer=GetBuffer(nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	if(ppNormalMapVertexes){*ppNormalMapVertexes=pBuffer->pNormalMapArray;}
}

void CGCMFileType::GetBufferTangents(unsigned long nFrame,unsigned long nBuffer,float **ppTangents )
{
	if(ppTangents){*ppTangents=NULL;}
	SGCMBuffer *pBuffer=GetBuffer(nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	if(ppTangents){*ppTangents=pBuffer->pTangentArray;}
}

void CGCMFileType::GetBufferBitangents(unsigned long nFrame,unsigned long nBuffer,float **ppBitangents )
{
	if(ppBitangents){*ppBitangents=NULL;}
	SGCMBuffer *pBuffer=GetBuffer(nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	if(ppBitangents){*ppBitangents=pBuffer->pBitangentArray;}
}

void CGCMFileType::SetBufferTangents(unsigned long nFrame,unsigned long nBuffer,float *pTangents )
{
	SGCMBuffer *pBuffer=GetBuffer(nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	delete [] pBuffer->pTangentArray;
	pBuffer->pTangentArray=pTangents;
}

void CGCMFileType::SetBufferBitangents(unsigned long nFrame,unsigned long nBuffer,float *pBitangents )
{
	SGCMBuffer *pBuffer=GetBuffer(nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	delete [] pBuffer->pBitangentArray;
	pBuffer->pBitangentArray=pBitangents;
}

void CGCMFileType::RemoveFrames()
{
	for(unsigned x=0;x<m_vFrames.size();x++){delete m_vFrames[x];}
	m_vFrames.clear();
}

void CGCMFileType::RemoveBuffers(unsigned long nFrame )
{
	if(nFrame>=m_vFrames.size()){return;}
	for(unsigned x=0;x<m_vFrames[nFrame]->vBuffers.size();x++){delete m_vFrames[nFrame]->vBuffers[x];}
	m_vFrames[nFrame]->vBuffers.clear();
}

void CGCMFileType::GetTextures(std::set<std::string> *pvTextures)
{
	for(unsigned int f=0;f<m_vFrames.size();f++)
	{
		SGCMFrame *pFrame=m_vFrames[f];
		for(unsigned int b=0;b<pFrame->vBuffers.size();b++)
		{
			SGCMBuffer *pBuffer=pFrame->vBuffers[b];
			for(unsigned int l=0;l<pBuffer->vTextureLevels.size();l++)
			{
				SGCMTextureLevel *pLevel=pBuffer->vTextureLevels[l];
				pvTextures->insert(pLevel->sTexture);
			}	
		}
	}
}

void CGCMFileType::UpdateSizes()
{
	bool bFirstVertex=true;
	for(unsigned int f=0;f<m_vFrames.size();f++)
	{
		CVector vMins,vMaxs,vSize;

		SGCMFrame *pFrame=m_vFrames[f];
		for(unsigned int b=0;b<pFrame->vBuffers.size();b++)
		{
			SGCMBuffer *pBuffer=pFrame->vBuffers[b];
			float *pCoordinate=pBuffer->pVertexArray;
			for(unsigned int v=0;v<pBuffer->vTextureLevels.size();v++)
			{
				if(bFirstVertex)
				{
					bFirstVertex=false;
					for(int c=0;c<3;c++)
					{
						vMins.c[c]=pCoordinate[c];
						vMaxs.c[c]=pCoordinate[c];
					}
				}
				else
				{
					for(int c=0;c<3;c++)
					{
						vMins.c[c]=std::min(vMins.c[c],(double)pCoordinate[c]);
						vMaxs.c[c]=std::max(vMins.c[c],(double)pCoordinate[c]);
					}
					pCoordinate+=3;
				}
			}
		}
		vSize=vMaxs-vMins;
		for(int c=0;c<3;c++)
		{
			pFrame->data.vMins[c]=vMins.c[c];
			pFrame->data.vMaxs[c]=vMaxs.c[c];
			pFrame->data.vSize[c]=vSize.c[c];
		}
		pFrame->data.dRadius=std::max(vSize.c[0],std::max(vSize.c[1],vSize.c[2]));
	}
}
