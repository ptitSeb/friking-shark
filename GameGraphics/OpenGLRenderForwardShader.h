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

struct SRenderForwardShaderVertexBufferState
{
	bool bVertexBufferEnabled;
	bool bColorBufferEnabled;
	bool bNormalBufferEnabled;
	bool bNormalMapBufferEnabled;
	bool bTangentBufferEnabled;
	bool bBitangentBufferEnabled;
	bool pbTexCoordEnabled[MAX_OPENGL_TEXTURE_COORDS];
	
	SRenderForwardShaderVertexBufferState()
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

struct SShaderKey
{
	bool bHeightFog;
	bool bShadows;
	int  nTextureUnits;
	bool bLighting;
	bool  bWater;
	bool  bNormalMap;
	bool  bSkyShadow;
	bool  bPoints;
	std::string sDescription;
	
	EShadingModel eShadingModel;
	
	bool operator <(const SShaderKey &otherKey) const
	{
		if(eShadingModel<otherKey.eShadingModel){return true;}
		if(eShadingModel>otherKey.eShadingModel){return false;}
		if(bPoints<otherKey.bPoints){return true;}
		if(bPoints>otherKey.bPoints){return false;}
		if(bHeightFog<otherKey.bHeightFog){return true;}
		if(bHeightFog>otherKey.bHeightFog){return false;}
		if(bShadows<otherKey.bShadows){return true;}
		if(bShadows>otherKey.bShadows){return false;}
		if(bSkyShadow<otherKey.bSkyShadow){return true;}
		if(bSkyShadow>otherKey.bSkyShadow){return false;}
		if(nTextureUnits<otherKey.nTextureUnits){return true;}
		if(nTextureUnits>otherKey.nTextureUnits){return false;}
		if(bNormalMap<otherKey.bNormalMap){return true;}
		if(bNormalMap>otherKey.bNormalMap){return false;}
		if(bLighting<otherKey.bLighting){return true;}
		if(bLighting>otherKey.bLighting){return false;}
		if(bWater<otherKey.bWater){return true;}
		if(bWater>otherKey.bWater){return false;}
		return false;
	}

SShaderKey(){eShadingModel=eShadingModel_Gouraud;bHeightFog=false;bShadows=false;nTextureUnits=0;bLighting=false;bWater=false;bNormalMap=false;bSkyShadow=false;bPoints=false;}
SShaderKey(EShadingModel shading,bool heightFog,bool shadows,int textureUnits,bool lighting,bool water,bool normalMap,bool sky,bool points){eShadingModel=shading;bHeightFog=heightFog;bShadows=shadows;nTextureUnits=textureUnits;bLighting=lighting;bWater=water;bNormalMap=normalMap;bSkyShadow=sky;bPoints=points;}
};

enum EStateChangeShader
{
	eStateChange_DoNotUpdateShader,
	eStateChange_UpdateShader,
};

class COpenGLRenderForwardShader: virtual public CSystemObjectBase, virtual public IOpenGLRender
{
	SSceneData     *m_pScene;
	bool            m_bAnyShadowInTheScene;
	CVector         m_vClearColor;
	bool			m_bClippingActive;
	SGameRect		m_rClipRect;

	SHardwareSupport m_sHardwareSupport;

	SRenderState                          m_sRenderState;
	SRenderForwardShaderVertexBufferState m_sBufferState;
	
	int	              m_nSkyShadowTextureLevel;
	int 	          m_nNormalMapTextureLevel;
	int 	          m_nShadowTextureLevel;
	float             m_fCurrentRealTime;

	SOpenGLRenderMappings m_RenderMappings;
	
	bool 		 m_bSelecting;
	SGamePos 	 m_SelectionPos;
	unsigned int m_nSelectionPrecision;
	
	float *m_pLastVertexPointer;
	float *m_pLastColorPointer;
	float *m_pLastNormalPointer;
	float *m_pLastNormalMapPointer;
	float *m_pLastTangentPointer;
	float *m_pLastBitangentPointer;
	float *m_pLastTexPointer[2];
	
	int m_nLastVertexStride;
	int m_nLastColorStride;
	int m_nLastNormalStride;
	int m_nLastNormalMapStride;
	int m_nLastTangentStride;
	int m_nLastBitangentStride;
	int m_pLastTexStride[2];
	
	bool 		 m_bRenderingPoints;
	
	CGenericShaderWrapper m_ShadowShader;
	CGenericShaderWrapper m_SelectionShader;
	CGenericShaderWrapper *m_pCurrentShader;
	CGenericShaderWrapper *m_pShaderBeforeSelection;
	
	CGenericTextureWrapper m_ShadowTexture;

	unsigned int		m_nCurrentVertexArrayObject;
	unsigned int 		m_nCurrentVertexBufferObject;
	unsigned int 		m_nCurrentIndexesBufferObject;
		
	std::map<SShaderKey,CGenericShaderWrapper> m_mShaders;
	std::map<SShaderKey,std::string> m_mShaderNames;
	
	IGenericTexture *				m_ppiEffectiveTextureLevels[MAX_TEXTURE_LEVELS];
	IGenericTexture *				m_ppiTextureLevels[MAX_TEXTURE_LEVELS];
	unsigned int 					m_nTextureLevels;
	unsigned int 					m_nCurrentActiveTexture;
	
	IGenericViewport *m_piCurrentViewport;
	

	void PrepareSunShadows();
	void UnprepareSunShadows();

	void SetProjectionMatrix(CMatrix &matrix);
	void SetModelViewMatrix(CMatrix &matrix);

	void SetRenderState(const SRenderState &sNewState,EStateChangeShader eShader=eStateChange_UpdateShader);
	void SetCurrentRenderStateShader();
	
	void SetupLightsInShaders();

	void RenderAllStages(bool bRenderingShadow);
	void RenderPointStages(bool bRenderingShadow);
	void RenderLineStages(bool bRenderingShadow);
	void RenderTriangleStages(bool bRenderingShadow);
	void RenderModelStages(bool bRenderingShadow);

	void AnalyzeStages(bool *pbShadowsPresent,bool *pbSkyPresent,bool *pbLightingPresent);
	
	void AddShader(SShaderKey &key);
	bool PrecompileShaderByName(const char *psName,EShadingModel shading);
	
	
	void SetVertexPointers(float *pVertex,float *pNormal,float *pColor,int nTex,float **pTex, float *pNormalTex=NULL,float *pTangent=NULL,float *pBiTangent=NULL);
	void SetVertexBufferObject(SVertexBufferObject *pVBO);
	
	void PrepareTexture(IGenericTexture *piTexture,unsigned int nTextureLevel);
	void UnprepareTexture(unsigned int nTextureLevel);
	void SetEffectiveTexture(IGenericTexture *pTexture,int nTextureLevel);
	
	void InternalSetCamera(const CVector &vPosition,double dYaw, double dPitch, double dRoll);
	
public:

	bool Setup(IGenericRender *piRender,IGenericViewport *piViewport,SHardwareSupport &support);
	void Destroy();

	void StartFrame();
	void EndFrame();

	void RenderScene(SSceneData &pScene);

	// Selection

	void StartSelection(SGameRect rWindowRect,IGenericCamera *piCamera,double dx,double dy,double dPrecision);
	void SetSelectionId(unsigned int nId);
	int EndSelection();
	
	void ReloadShaders();
		
	COpenGLRenderForwardShader(void);
	~COpenGLRenderForwardShader(void);
};
