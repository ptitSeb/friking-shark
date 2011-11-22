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

#define MAX_OPENGL_TEXTURE_COORDS 2
struct SOpenGLRenderMappings
{
	int nVertexAttribIndex;
	int pTextureAttribIndex[MAX_OPENGL_TEXTURE_COORDS];
	int nNormalAttribIndex;
	int nNormalMapCoordAttribIndex;
	int nColorAttribIndex;
	int nNormalMapTextureIndex;
	int nTangentAttribIndex;
	int nBitangentAttribIndex;
	
	SOpenGLRenderMappings()
	{
		nNormalMapTextureIndex=-1;
		
		nVertexAttribIndex=-1;
		for(int x=0;x<MAX_OPENGL_TEXTURE_COORDS;x++){pTextureAttribIndex[x]=-1;}
		nNormalAttribIndex=-1;
		nNormalMapCoordAttribIndex=-1;
		nColorAttribIndex=-1;
		nTangentAttribIndex=-1;
		nBitangentAttribIndex=-1;		
	}
};

struct SVertexBufferObject
{
	unsigned int nVertexArrayObject;
	unsigned int nBufferObject;
	unsigned int nIndexesBufferObject;
	int nVertexOffset;
	int nColorOffset;
	int nNormalOffset;
	int nTextures;
	int pTexOffsets[MAX_OPENGL_TEXTURE_COORDS];
	int nNormalMapOffset;
	int nTangentOffset;
	int nBitangentOffset;

	int nVertexStride;
	int nColorStride;
	int nNormalStride;
	int pTexStrides[MAX_OPENGL_TEXTURE_COORDS];
	int nNormalMapStride;
	int nTangentStride;
	int nBitangentStride;
	
	SVertexBufferObject()
	{
		nBufferObject=0;
		nIndexesBufferObject=0;
		nVertexArrayObject=0;
		nVertexOffset=-1;
		nColorOffset=-1;
		nNormalOffset=-1;
		nTextures=0;
		nNormalMapOffset=-1;
		nTangentOffset=-1;
		nBitangentOffset=-1;

		nVertexStride=0;
		nColorStride=0;
		nNormalStride=0;
		nNormalMapStride=0;
		nTangentStride=0;
		nBitangentStride=0;

		for(int x=0;x<MAX_OPENGL_TEXTURE_COORDS;x++)
		{
			pTexOffsets[x]=-1;
			pTexStrides[x]=0;
		}
	}
};

class IOpenGLRender:virtual public IGenericRender
{
public:
	virtual void SetVertexPointers(float *pVertex,float *pNormal,float *pColor,int nTex,float **pTex, float *pNormalTex,float *pTangent,float *pBiTangent)=0;
	virtual void SetVertexBufferObject(SVertexBufferObject *pVBO)=0;
	
	virtual void PrepareTexture(IGenericTexture *piTexture,unsigned int nTextureLevel)=0;
	virtual void UnprepareTexture(unsigned int nTextureLevel)=0;
};

class IOpenGLModel:virtual public ISystemUnknown
{
public:
	
	virtual void PrepareRenderBuffer(IOpenGLRender *piRender,unsigned int nAnimation,unsigned int nFrame, unsigned int nBuffer,bool bRenderingShadow,SOpenGLRenderMappings *pRenderMappings)=0;
	virtual void CallRenderBuffer(IOpenGLRender *piRender,unsigned int nAnimation,unsigned int nFrame, unsigned int nBuffer,unsigned int nInstances)=0;
	virtual void UnPrepareRenderBuffer(IOpenGLRender *piRender,unsigned int nAnimation,unsigned int nFrame, unsigned int nBuffer,bool bRenderingShadow,SOpenGLRenderMappings *pRenderMappings)=0;
};
