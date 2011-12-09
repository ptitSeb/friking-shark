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

#include "OpenGLRender.h"

#define SELECTION_BUFFER_SIZE		 1024

struct SRenderForwardFixedVertexBufferState
{
	bool bVertexBufferEnabled;
	bool bColorBufferEnabled;
	bool bNormalBufferEnabled;
	bool bNormalMapBufferEnabled;
	bool bTangentBufferEnabled;
	bool bBitangentBufferEnabled;
	bool pbTexCoordEnabled[MAX_OPENGL_TEXTURE_COORDS];
	
	SRenderForwardFixedVertexBufferState()
	{
		bVertexBufferEnabled=false;
		bColorBufferEnabled=false;
		bNormalBufferEnabled=false;
		bNormalMapBufferEnabled=false;
		bTangentBufferEnabled=false;
		bBitangentBufferEnabled=false;
		for(int x=0;x<MAX_OPENGL_TEXTURE_COORDS;x++){pbTexCoordEnabled[x]=false;}
	}
};

class COpenGLRenderForwardFixed: virtual public CSystemObjectBase, virtual public IOpenGLRender
{
	SSceneData     *m_pScene;
	CVector         m_vClearColor;
	bool			m_bClippingActive;
	SGameRect		m_rClipRect;
	
	SHardwareSupport m_sHardwareSupport;
	
	SRenderState                         m_sRenderState;
	SRenderForwardFixedVertexBufferState m_sBufferState;
		
	bool 		  m_bLightingPrepared;
	unsigned long m_nActiveLights;
	unsigned int  m_nCurrentActiveTexture;
	
	int	              m_nSkyShadowTextureLevel;
	int 	          m_nNormalMapTextureLevel;
	int 	          m_nShadowTextureLevel;

	SOpenGLRenderMappings m_RenderMappings;
		
	float *m_pLastVertexPointer;
	float *m_pLastColorPointer;
	float *m_pLastNormalPointer;
	float *m_pLastNormalMapPointer;
	float *m_pLastTangentPointer;
	float *m_pLastBitangentPointer;
	float *m_pLastTexPointer[2];
	
	unsigned int 		m_nCurrentVertexBufferObject;
	unsigned int 		m_nCurrentIndexesBufferObject;
	
	IGenericTexture *				m_ppiEffectiveTextureLevels[MAX_TEXTURE_LEVELS];
	IGenericTexture *				m_ppiTextureLevels[MAX_TEXTURE_LEVELS];
	unsigned int 					m_nTextureLevels;
		
	bool         m_bSelecting;
	GLuint		 m_pSelectionBuffer[SELECTION_BUFFER_SIZE];
	
	IGenericViewport *m_piCurrentViewport;
	CGenericTextureWrapper m_ShadowTexture;
	
	void PrepareSunShadows();
	void UnprepareSunShadows();
	
	void SetRenderState(const SRenderState &sNewState);
	void PrepareLighting();
	void UnPrepareLighting();

	void RenderAllStages(bool bRenderingShadow);
	void RenderPointStages(bool bRenderingShadow);
	void RenderLineStages(bool bRenderingShadow);
	void RenderTriangleStages(bool bRenderingShadow);
	void RenderModelStages(bool bRenderingShadow);
		
	void SetVertexPointers(float *pVertex,float *pNormal,float *pColor,int nTex,float **pTex, float *pNormalTex=NULL,float *pTangent=NULL,float *pBiTangent=NULL);
	void SetVertexBufferObject(SVertexBufferObject *pVBO);
	
	void PrepareTexture(IGenericTexture *piTexture,unsigned int nTextureLevel);
	void UnprepareTexture(unsigned int nTextureLevel);
	void SetEffectiveTexture(IGenericTexture *pTexture,int nTextureLevel);
	
	void AnalyzeStages(bool *pbShadowsPresent,bool *pbLightingPresent);
	
public:
	
	bool Setup(IGenericRender *piRender,IGenericViewport *piViewport,SHardwareSupport &support);
	void Destroy();

	void StartFrame();
	void EndFrame();
	
	void ReloadShaders();
	
	void RenderScene(SSceneData &pScene);
	
	void StartSelection(SGameRect rWindowRect,IGenericCamera *piCamera,double dx,double dy,double dPrecision);
	void SetSelectionId(unsigned int nId);
	int EndSelection();
	
	COpenGLRenderForwardFixed(void);
	~COpenGLRenderForwardFixed(void);
};
