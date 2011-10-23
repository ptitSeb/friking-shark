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
struct SOpenGLRenderMappings
{
	int nVertexAttribIndex;
	int pTextureAttribIndex[2];
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
		pTextureAttribIndex[0]=-1;
		pTextureAttribIndex[1]=-1;
		nNormalAttribIndex=-1;
		nNormalMapCoordAttribIndex=-1;
		nColorAttribIndex=-1;
		nTangentAttribIndex=-1;
		nBitangentAttribIndex=-1;		
	}
};

class IOpenGLModel:virtual public ISystemUnknown
{
public:
	virtual void PrepareRenderBuffer(IGenericRender *piRender,unsigned int nAnimation,unsigned int nFrame, unsigned int nBuffer,bool bRenderingShadow,SOpenGLRenderMappings *pRenderMappings)=0;
	virtual void CallRenderBuffer(IGenericRender *piRender,unsigned int nAnimation,unsigned int nFrame, unsigned int nBuffer,unsigned int nInstances)=0;
	virtual void UnPrepareRenderBuffer(IGenericRender *piRender,unsigned int nAnimation,unsigned int nFrame, unsigned int nBuffer,bool bRenderingShadow,SOpenGLRenderMappings *pRenderMappings)=0;
};
