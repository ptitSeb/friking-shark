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

struct SDeferredVertexBufferState
{
	bool bVertexBufferEnabled;
	bool bColorBufferEnabled;
	bool bNormalBufferEnabled;
	bool bNormalMapBufferEnabled;
	bool bTangentBufferEnabled;
	bool bBitangentBufferEnabled;
	bool pbTexCoordEnabled[MAX_OPENGL_TEXTURE_COORDS];
	
	SDeferredVertexBufferState()
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

enum EDeferredMode
{
	eDeferredMode_None,
	eDeferredMode_NoWater,
	eDeferredMode_Water,
	eDeferredMode_Forward,
	eDeferredMode_Count
};

struct SDeferredShaderKey
{
	EDeferredMode eMode;
	bool bHeightFog;
	bool bShadows;
	int  nTextureUnits;
	bool bLighting;
	bool  bWater;
	bool  bNormalMap;
	bool  bSkyShadow;
	bool  bPoints;
	std::string sDescription;
	
	bool operator <(const SDeferredShaderKey &otherKey) const
	{
		if(eMode<otherKey.eMode){return true;}
		if(eMode>otherKey.eMode){return false;}
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

	SDeferredShaderKey(){eMode=eDeferredMode_Forward;bHeightFog=false;bShadows=false;nTextureUnits=0;bLighting=false;bWater=false;bNormalMap=false;bSkyShadow=false;bPoints=false;}
	SDeferredShaderKey(EDeferredMode mode,bool heightFog,bool shadows,int textureUnits,bool lighting,bool water,bool normalMap,bool sky,bool points){eMode=mode;bHeightFog=heightFog;bShadows=shadows;nTextureUnits=textureUnits;bLighting=lighting;bWater=water;bNormalMap=normalMap;bSkyShadow=sky;bPoints=points;}
};

enum EDeferredState
{
	eDeferredState_ALL,
	eDeferredState_LightingActive,
	eDeferredState_LightingInactive
};

enum EDeferredStateChangeShader
{
	eDeferredStateChange_DoNotUpdateShader,
	eDeferredStateChange_UpdateShader,
};

struct SDeferredGBuffers
{
	unsigned int nDepthBuffer;
	unsigned int nDiffuseBuffer;
	unsigned int nSunSceneBuffer;
	unsigned int nNormalBuffer;
	unsigned int nFrameBuffer;
	
	unsigned int nWidth;
	unsigned int nHeight;
	
	SDeferredGBuffers(){nWidth=nHeight=0;nDepthBuffer=nDiffuseBuffer=nNormalBuffer=nSunSceneBuffer=0;nFrameBuffer=0;}
};

class COpenGLRenderDeferred: virtual public CSystemObjectBase, virtual public IOpenGLRender
{
	SSceneData     *m_pScene;
	bool            m_bAnyShadowInTheScene;
	EDeferredMode m_eRenderMode;
	CVector         m_vClearColor;
	bool			m_bClippingActive;
	SGameRect		m_rClipRect;
	
	SDeferredGBuffers       m_GBuffers;
	
	SHardwareSupport m_sHardwareSupport;

	SRenderState                          m_sRenderState;
	SDeferredVertexBufferState m_sBufferState;
	
	int	              m_nSkyShadowTextureLevel;
	int 	          m_nNormalMapTextureLevel;
	int 	          m_nShadowTextureLevel;
	unsigned int      m_nLastDesiredSunShadowWidth;
	unsigned int      m_nLastDesiredSunShadowHeight;

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
	bool		 m_bNormalBlendingEnabled;
	
	CGenericShaderWrapper m_ShadowShader;
	CGenericShaderWrapper m_SelectionShader;
	CGenericShaderWrapper m_DeferredShaderWaterSun;
	CGenericShaderWrapper m_DeferredShaderNoWaterSun;
	CGenericShaderWrapper m_DeferredShaderDynamic;
	CGenericShaderWrapper *m_pCurrentShader;
	CGenericShaderWrapper *m_pShaderBeforeSelection;
	
	CGenericTextureWrapper m_ShadowTexture;

	unsigned int		m_nCurrentVertexArrayObject;
	unsigned int 		m_nCurrentVertexBufferObject;
	unsigned int 		m_nCurrentIndexesBufferObject;
		
	std::map<SDeferredShaderKey,CGenericShaderWrapper> m_mShaders;
	std::map<SDeferredShaderKey,std::string> m_mShaderNames;
	
	IGenericTexture *				m_ppiEffectiveTextureLevels[MAX_TEXTURE_LEVELS];
	IGenericTexture *				m_ppiTextureLevels[MAX_TEXTURE_LEVELS];
	IGenericTexture *				m_piEffectiveNormalMap;
	unsigned int 					m_nTextureLevels;
	unsigned int 					m_nCurrentActiveTexture;
	
	IGenericViewport *m_piCurrentViewport;

	EShadowQuality m_eShadowQuality;
	
	void RenderDeferred(bool bShadowsPresent,bool bWaterPresent,bool bSkyPresent,bool bLightingPresent);
	void RenderForward();
	
	void PrepareSunShadows();
	void UnprepareSunShadows();

	void SetProjectionMatrix(CMatrix &matrix);
	void SetModelViewMatrix(CMatrix &matrix);

	void SetRenderState(const SRenderState &sNewState,EDeferredStateChangeShader eShader=eDeferredStateChange_UpdateShader);
	void SetCurrentRenderStateShader();
	
	void RenderAllStages(bool bRenderingShadow);
	void RenderPointStages(bool bRenderingShadow);
	void RenderLineStages(bool bRenderingShadow);
	void RenderTriangleStages(bool bRenderingShadow);
	void RenderModelStages(bool bRenderingShadow,EDeferredState eLightingState);

	void AnalyzeStages(bool *pbShadowsPresent,bool *pbWaterPresent,bool *pbSkyPresent,bool *pbLightingPresent);
	
	void AddShader(SDeferredShaderKey &key);
	bool PrecompileShaderByName(const char *psName);
	
	
	void SetVertexPointers(float *pVertex,float *pNormal,float *pColor,int nTex,float **pTex, float *pNormalTex=NULL,float *pTangent=NULL,float *pBiTangent=NULL);
	void SetVertexBufferObject(SVertexBufferObject *pVBO);
	
	void PrepareTexture(IGenericTexture *piTexture,unsigned int nTextureLevel);
	void UnprepareTexture(unsigned int nTextureLevel);
	void SetEffectiveTexture(IGenericTexture *pTexture,int nTextureLevel);

	void PrepareNormalMap(IGenericTexture *piNormalMap);
	void UnprepareNormalMap();
	
	void InternalSetCamera(const CVector &vPosition,double dYaw, double dPitch, double dRoll);
	
	void          FreeGBuffers(SDeferredGBuffers *pGBuffers);
	SDeferredGBuffers     CreateGBuffers( unsigned nWidth,unsigned nHeight);
	unsigned int  CreateRenderBuffer( unsigned nWidth,unsigned nHeight,unsigned int nComponent,unsigned int nFormat);
	
	void RenderScreenRect(double x,double y,double w,double h,double dTexX,double dTexY,double dTexW,double dTexH);
	
public:

	bool Setup(IGenericRender *piRender,IGenericViewport *piViewport,SHardwareSupport &support);
	void Cleanup();

	std::string GetFriendlyName(){return "Deferred";}
	void Destroy();

	void StartFrame();
	void EndFrame();

	void RenderScene(SSceneData &pScene);

	// Selection

	void StartSelection(SGameRect rWindowRect,IGenericCamera *piCamera,double dx,double dy,double dPrecision);
	void SetSelectionId(unsigned int nId);
	int EndSelection();
	
	void ReloadShaders();

	EShadingModel GetShadingModel(){return eShadingModel_Fragment;}
	
	COpenGLRenderDeferred(void);
	~COpenGLRenderDeferred(void);
};
