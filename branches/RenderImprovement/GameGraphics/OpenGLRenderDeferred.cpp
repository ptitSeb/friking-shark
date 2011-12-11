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

#include <algorithm>
#include "OpenGLGraphics.h"
#include "OpenGLRenderDeferred.h"

#define DEFERRED_SAMPLER_SUN 0
#define DEFERRED_SAMPLER_DIFFUSE 0
#define DEFERRED_SAMPLER_NORMAL 1
#define DEFERRED_SAMPLER_DEPTH 2
#define DEFERRED_SAMPLER_SHADOW 3
#define DEFERRED_SAMPLER_SKY 4
#define DEFERRED_SAMPLER_NORMAL_MAP 5

#define LIGHT_SIZE_INCREMENT 1.5

#ifdef ANDROID 
	#define DEFAULT_SHADOW_SIZE 512
#else
	#define DEFAULT_SHADOW_SIZE 1024
#endif

COpenGLRenderDeferred::COpenGLRenderDeferred(void)
{
	m_eRenderMode=eDeferredMode_Forward;
	m_bAnyShadowInTheScene=false;
	m_pScene=NULL;
	m_piCurrentViewport=NULL;
	m_fCurrentRealTime=0;
	m_bClippingActive=false;
	
	m_nTextureLevels=0;
	m_nCurrentVertexArrayObject=0;
	m_nCurrentVertexBufferObject=0;
	m_nCurrentIndexesBufferObject=0;
	
	m_bRenderingPoints=false;
	
	m_bSelecting=false;
	m_nSelectionPrecision=0;
	m_pShaderBeforeSelection=NULL;
	
	m_nCurrentActiveTexture=0;
		
	int nAttribIndex=0;
	m_nShadowTextureLevel=3;
	m_nSkyShadowTextureLevel=2;
	m_nNormalMapTextureLevel=1;
	m_RenderMappings.nNormalMapTextureIndex=3;
	m_RenderMappings.nVertexAttribIndex=nAttribIndex++;
	m_RenderMappings.pTextureAttribIndex[0]=nAttribIndex++;
	m_RenderMappings.pTextureAttribIndex[1]=nAttribIndex++;
	m_RenderMappings.nNormalAttribIndex=nAttribIndex++;
	m_RenderMappings.nNormalMapCoordAttribIndex=nAttribIndex++;
	m_RenderMappings.nTangentAttribIndex=nAttribIndex++;
	m_RenderMappings.nBitangentAttribIndex=nAttribIndex++;
	m_RenderMappings.nColorAttribIndex=nAttribIndex++;
	
	m_pCurrentShader=NULL;
	
	m_pLastVertexPointer=NULL;
	m_pLastColorPointer=NULL;
	m_pLastNormalPointer=NULL;
	m_pLastNormalMapPointer=NULL;
	m_pLastTangentPointer=NULL;
	m_pLastBitangentPointer=NULL;
	m_pLastTexPointer[0]=NULL;
	m_pLastTexPointer[1]=NULL;

	m_piEffectiveNormalMap=NULL;

	memset(m_ppiEffectiveTextureLevels,0,sizeof(m_ppiEffectiveTextureLevels));
	memset(m_ppiTextureLevels,0,sizeof(m_ppiTextureLevels));
}

COpenGLRenderDeferred::~COpenGLRenderDeferred(void)
{
}

bool COpenGLRenderDeferred::Setup(IGenericRender *piRender,IGenericViewport *piViewport,SHardwareSupport &support)
{
	bool bOk=true;
	
	REL(m_piCurrentViewport);
	m_piCurrentViewport=ADD(piViewport);
	m_sHardwareSupport=support;
	
	if(m_ShadowTexture.m_piTexture==NULL)
	{
		bOk=m_ShadowTexture.Create(m_piSystem,"Texture","");
		if(bOk)
		{
			bOk=m_ShadowTexture.m_piTexture->CreateDepth(DEFAULT_SHADOW_SIZE,DEFAULT_SHADOW_SIZE,m_piCurrentViewport);
			if(!bOk)
			{
				RTTRACE("COpenGLRenderDeferred::Setup -> Failed to initialize shadow texture");
				m_ShadowTexture.Destroy();
			}
		}
		else
		{
			RTTRACE("COpenGLRenderDeferred::Setup -> Failed to create shadow texture");
		}
	}
	if(m_ShadowShader.m_piShader==NULL)
	{
		bOk=m_ShadowShader.Create(m_piSystem,"Shader","");
		if(bOk)
		{
			m_ShadowShader.m_piShader->Load("Shaders/ShadowBufferShader-Vertex.c","Shaders/ShadowBufferShader-Fragment.c","");
			bOk=m_ShadowShader.m_piShader->Compile();
			if(!bOk)
			{
				RTTRACE("COpenGLRenderDeferred::Setup -> Failed to compile shadow shader");
			}
		}
		else
		{
			RTTRACE("COpenGLRenderDeferred::Setup -> Failed to create shadow shader");
		}
	}

	if(m_DeferredShaderWaterSun.m_piShader==NULL)
	{

		bOk=m_DeferredShaderWaterSun.Create(m_piSystem,"Shader","");
		if(bOk)
		{
			m_DeferredShaderWaterSun.m_piShader->Load("Shaders/RenderShader-Deferred-Water-Sun-Vertex.c","Shaders/RenderShader-Deferred-Water-Sun-Fragment.c","");
			bOk=m_DeferredShaderWaterSun.m_piShader->Compile();
			if(!bOk)
			{
				RTTRACE("COpenGLRenderDeferred::Setup -> Failed to compile deferred sun shader");
			}
			
			m_DeferredShaderWaterSun.m_piShader->AddUniform("SunSceneSampler",(int)DEFERRED_SAMPLER_SUN,false);
			m_DeferredShaderWaterSun.m_piShader->AddUniform("DepthSampler",(int)DEFERRED_SAMPLER_DEPTH,false);
			
			m_DeferredShaderWaterSun.m_piShader->AddAttribute("aVertex",(int)m_RenderMappings.nVertexAttribIndex);
			m_DeferredShaderWaterSun.m_piShader->AddAttribute("aTexCoord0",(int)m_RenderMappings.pTextureAttribIndex[0]);
		}
		else
		{
			RTTRACE("COpenGLRenderDeferred::Setup -> Failed to create deferred sun shader");
		}
	}
	
	if(m_DeferredShaderNoWaterSun.m_piShader==NULL)
	{
		
		bOk=m_DeferredShaderNoWaterSun.Create(m_piSystem,"Shader","");
		if(bOk)
		{			
			m_DeferredShaderNoWaterSun.m_piShader->Load("Shaders/RenderShader-Deferred-NoWater-Sun-Vertex.c","Shaders/RenderShader-Deferred-NoWater-Sun-Fragment.c","");
			bOk=m_DeferredShaderNoWaterSun.m_piShader->Compile();
			if(!bOk)
			{
				RTTRACE("COpenGLRenderDeferred::Setup -> Failed to compile deferred sun shader");
			}
			
			m_DeferredShaderNoWaterSun.m_piShader->AddUniform("DiffuseSampler",(int)DEFERRED_SAMPLER_DIFFUSE,false);
			m_DeferredShaderNoWaterSun.m_piShader->AddUniform("NormalSampler",(int)DEFERRED_SAMPLER_NORMAL,false);
			m_DeferredShaderNoWaterSun.m_piShader->AddUniform("DepthSampler",(int)DEFERRED_SAMPLER_DEPTH,false);
			m_DeferredShaderNoWaterSun.m_piShader->AddUniform("ShadowMap",(int)m_nShadowTextureLevel,false);
			m_DeferredShaderNoWaterSun.m_piShader->AddUniform("SkyShadowMap",(int)DEFERRED_SAMPLER_SKY,false);
			
			m_DeferredShaderNoWaterSun.m_piShader->AddAttribute("aVertex",(int)m_RenderMappings.nVertexAttribIndex);
			m_DeferredShaderNoWaterSun.m_piShader->AddAttribute("aTexCoord0",(int)m_RenderMappings.pTextureAttribIndex[0]);
		}
		else
		{
			RTTRACE("COpenGLRenderDeferred::Setup -> Failed to create deferred sun shader");
		}
	}
	
	if(m_DeferredShaderDynamic.m_piShader==NULL)
	{
		bOk=m_DeferredShaderDynamic.Create(m_piSystem,"Shader","");
		if(bOk)
		{			
			m_DeferredShaderDynamic.m_piShader->Load("Shaders/RenderShader-Deferred-Dynamic-Vertex.c","Shaders/RenderShader-Deferred-Dynamic-Fragment.c","");
			bOk=m_DeferredShaderDynamic.m_piShader->Compile();
			if(!bOk)
			{
				RTTRACE("COpenGLRenderDeferred::Setup -> Failed to compile deferred dynamic shader");
			}
			
			m_DeferredShaderDynamic.m_piShader->AddUniform("DiffuseSampler",(int)DEFERRED_SAMPLER_DIFFUSE,false);
			m_DeferredShaderDynamic.m_piShader->AddUniform("NormalSampler",(int)DEFERRED_SAMPLER_NORMAL,false);
			m_DeferredShaderDynamic.m_piShader->AddUniform("DepthSampler",(int)DEFERRED_SAMPLER_DEPTH,false);
			
			m_DeferredShaderDynamic.m_piShader->AddAttribute("aVertex",(int)m_RenderMappings.nVertexAttribIndex);
			m_DeferredShaderDynamic.m_piShader->AddAttribute("aTexCoord0",(int)m_RenderMappings.pTextureAttribIndex[0]);
		}
		else
		{
			RTTRACE("COpenGLRenderDeferred::Setup -> Failed to create deferred dynamic shader");
		}
	}
	
	const char *ppShadersToPrecompile[]={"1","2","P2","T2","T0","P0","PG0","PG1","GT0","GT1","GTL0","GTL1","SYL1","SYL0","TL0","SYT0","GSYT0","GSYT1","GSYTL0","GSYTL1","GSYL1","SYTL1","SYTL0","GWSYTL1",NULL};
	const char **ppShaderCursor=ppShadersToPrecompile;
	while(bOk && *ppShaderCursor)
	{
		const char *pName=*ppShaderCursor;
		bOk=PrecompileShaderByName(pName);
		if(!bOk){RTTRACE("COpenGLRenderDeferred::Setup -> Failed to precompile shader: '%s'",pName);}
		ppShaderCursor++;
	}
	return bOk;
}

void COpenGLRenderDeferred::Destroy()
{
	m_ShadowTexture.Destroy();
	m_ShadowShader.Destroy();
	m_SelectionShader.Destroy();
	m_DeferredShaderNoWaterSun.Destroy();
	m_DeferredShaderWaterSun.Destroy();
	m_DeferredShaderDynamic.Destroy();
	
	std::map<SDeferredShaderKey,CGenericShaderWrapper>::iterator iShader;
	for(iShader=m_mShaders.begin();iShader!=m_mShaders.end();iShader++)
	{
		iShader->second.Destroy();
	}
	m_mShaders.clear();
	REL(m_piCurrentViewport);

	CSystemObjectBase::Destroy();
}

void COpenGLRenderDeferred::SetProjectionMatrix(CMatrix &matrix)
{
	std::map<SDeferredShaderKey,CGenericShaderWrapper>::iterator iShader;
	for(iShader=m_mShaders.begin();iShader!=m_mShaders.end();iShader++)
	{
		iShader->second.m_piShader->AddUniform("uProjection",matrix,false);
	}
}

void COpenGLRenderDeferred::SetModelViewMatrix(CMatrix &matrix)
{
	CMatrix identity;
	std::map<SDeferredShaderKey,CGenericShaderWrapper>::iterator iShader;
	for(iShader=m_mShaders.begin();iShader!=m_mShaders.end();iShader++)
	{
		iShader->second.m_piShader->AddUniform("uModel",identity,false);
		iShader->second.m_piShader->AddUniform("uView",matrix,false);
	}
}

void COpenGLRenderDeferred::SetupLightsInShaders()
{
	
}

void COpenGLRenderDeferred::PrepareSunShadows()
{
	if(m_piCurrentViewport==NULL){return;}
	if(m_ShadowTexture.m_piTexture==NULL){return;}
	if(m_ShadowShader.m_piShader==NULL){return;}
	
	
	SRenderState sShadowsState=m_sRenderState;
	sShadowsState.bActiveDepth=true;
	sShadowsState.bActiveDepthWrite=true;
	sShadowsState.bActiveSolid=true;
	sShadowsState.bActiveBlending=false;
	sShadowsState.bActiveSkyShadow=false;
	sShadowsState.bActiveLighting=false;
	sShadowsState.bActiveTextures=false;
	sShadowsState.bActiveWater=false;
	sShadowsState.bActiveHeightFog=false;
	SetRenderState(sShadowsState,eDeferredStateChange_DoNotUpdateShader);

	unsigned dwWidth=0,dwHeight=0;
	CMatrix lightProjectionMatrix;
	CMatrix lightModelViewMatrix;
	
	m_ShadowTexture.m_piTexture->GetSize(&dwWidth,&dwHeight);
	double dAspect=(double)dwWidth/(double)dwHeight;
	ComputeCameraMatrix(&lightModelViewMatrix,m_pScene->lighting.m_vSunShadowPos,m_pScene->lighting.m_vSunShadowAngles.c[YAW],m_pScene->lighting.m_vSunShadowAngles.c[PITCH],m_pScene->lighting.m_vSunShadowAngles.c[ROLL]);
	ComputePerspectiveMatrix(&lightProjectionMatrix,m_pScene->lighting.m_dSunShadowViewAngle,dAspect,m_pScene->lighting.m_dSunShadowNearPlane,m_pScene->lighting.m_dSunShadowFarPlane);
	
	if(m_ShadowShader.m_piShader)
	{
		CMatrix identity;
		if(m_pCurrentShader){m_pCurrentShader->m_piShader->Deactivate();}
		m_pCurrentShader=&m_ShadowShader;
		m_ShadowShader.m_piShader->AddUniform("uProjection",lightProjectionMatrix,false);
		m_ShadowShader.m_piShader->AddUniform("uModel",identity,false);
		m_ShadowShader.m_piShader->AddUniform("uView",lightModelViewMatrix,false);
		m_ShadowShader.m_piShader->Activate();
	}
	
	m_ShadowTexture.m_piTexture->StartRenderingToTexture();
	glViewport(0, 0, dwWidth, dwHeight);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	//RTTRACE("Starting rendering shadows");
	RenderModelStages(true,eDeferredState_ALL);
	//RTTRACE("Finished rendering shadows");
	
	glViewport(m_pScene->camera.m_nViewportX,m_pScene->camera.m_nViewportY,m_pScene->camera.m_nViewportW,m_pScene->camera.m_nViewportH);
	m_ShadowTexture.m_piTexture->StopRenderingToTexture();
	
	CMatrix biasMatrix;
	biasMatrix.e[0][0]=0.5f;
	biasMatrix.e[0][3]=0.5f;
	biasMatrix.e[1][1]=0.5f;
	biasMatrix.e[1][3]=0.5f;
	biasMatrix.e[2][2]=0.5f;
	biasMatrix.e[2][3]=0.5f;
	biasMatrix.e[3][3]=1.0f;
	
	CMatrix shadowMatrix;
	shadowMatrix.T(Origin-m_pScene->camera.m_vCameraForward);
	shadowMatrix*=lightModelViewMatrix;
	shadowMatrix*=lightProjectionMatrix;
	shadowMatrix*=biasMatrix;
	
	
	if(m_nCurrentActiveTexture!=m_nShadowTextureLevel){glActiveTexture(GL_TEXTURE0_ARB+m_nShadowTextureLevel);m_nCurrentActiveTexture=m_nShadowTextureLevel;}
	m_ShadowTexture.m_piTexture->PrepareTexture(m_nShadowTextureLevel);
	
	std::map<SDeferredShaderKey,CGenericShaderWrapper>::iterator iShader;
	for(iShader=m_mShaders.begin();iShader!=m_mShaders.end();iShader++)
	{
		if(iShader->first.bShadows && iShader->first.eMode==eDeferredMode_Water)
		{
			iShader->second.m_piShader->AddUniform("uShadowMatrix",shadowMatrix,false);
		}
	}
	if(m_DeferredShaderNoWaterSun.m_piShader)
	{
		m_DeferredShaderNoWaterSun.m_piShader->AddUniform("uShadowMatrix",shadowMatrix,false);
	}
}

void COpenGLRenderDeferred::UnprepareSunShadows()
{
	if(m_nCurrentActiveTexture!=m_nShadowTextureLevel){glActiveTexture(GL_TEXTURE0_ARB+m_nShadowTextureLevel);m_nCurrentActiveTexture=m_nShadowTextureLevel;}
	if(m_ShadowTexture.m_piTexture){m_ShadowTexture.m_piTexture->UnprepareTexture(m_nShadowTextureLevel);}
}

void COpenGLRenderDeferred::AnalyzeStages(bool *pbShadowsPresent,bool *pbWaterPresent,bool *pbSkyPresent,bool *pbLightingPresent)
{
	bool bShadowsPresent=false;
	bool bWaterPresent=false;
	bool bSkyPresent=false;
	bool bLightingPresent=false;
	std::map<SPointStageKey,SPointStage>::iterator iPointStage;
	for(iPointStage=m_pScene->objects.mPointStages.begin();iPointStage!=m_pScene->objects.mPointStages.end();iPointStage++)
	{
		const SPointStageKey *pKey=&iPointStage->first;
		bShadowsPresent|=pKey->sRenderState.bActiveShadowEmission;
		bWaterPresent|=pKey->sRenderState.bActiveWater;
		bSkyPresent|=pKey->sRenderState.bActiveSkyShadow;
		bLightingPresent|=pKey->sRenderState.bActiveLighting;
		if(bShadowsPresent && bWaterPresent && bSkyPresent && bLightingPresent){break;}
	}
	std::map<SLineStageKey,SLineStage>::iterator iLineStage;
	for(iLineStage=m_pScene->objects.mLineStages.begin();iLineStage!=m_pScene->objects.mLineStages.end();iLineStage++)
	{
		const SLineStageKey *pKey=&iLineStage->first;
		bShadowsPresent|=pKey->sRenderState.bActiveShadowEmission;
		bWaterPresent|=pKey->sRenderState.bActiveWater;
		bSkyPresent|=pKey->sRenderState.bActiveSkyShadow;
		bLightingPresent|=pKey->sRenderState.bActiveLighting;
		if(bShadowsPresent && bWaterPresent && bSkyPresent && bLightingPresent){break;}
	}
	std::map<STriangleStageKey,STriangleStage>::iterator iTriangleStage;
	for(iTriangleStage=m_pScene->objects.mTriangleStages.begin();iTriangleStage!=m_pScene->objects.mTriangleStages.end();iTriangleStage++)
	{
		const STriangleStageKey *pKey=&iTriangleStage->first;
		bShadowsPresent|=pKey->sRenderState.bActiveShadowEmission;
		bWaterPresent|=pKey->sRenderState.bActiveWater;
		bSkyPresent|=pKey->sRenderState.bActiveSkyShadow;
		bLightingPresent|=pKey->sRenderState.bActiveLighting;
		if(bShadowsPresent && bWaterPresent && bSkyPresent && bLightingPresent){break;}
	}
	std::multimap<double,std::pair<const SModelStageKey*,const SModelStage*> >::iterator iModelStage;
	for(iModelStage=m_pScene->objects.mModelStages.begin();iModelStage!=m_pScene->objects.mModelStages.end();iModelStage++)
	{
		const SModelStageKey *pKey=iModelStage->second.first;
		bShadowsPresent|=pKey->sRenderState.bActiveShadowEmission;
		bWaterPresent|=pKey->sRenderState.bActiveWater;
		bSkyPresent|=pKey->sRenderState.bActiveSkyShadow;
		bLightingPresent|=pKey->sRenderState.bActiveLighting;
		if(bShadowsPresent && bWaterPresent && bSkyPresent && bLightingPresent){break;}
	}
	*pbWaterPresent=bWaterPresent;
	*pbShadowsPresent=bShadowsPresent;
	*pbSkyPresent=bSkyPresent;
	*pbLightingPresent=bLightingPresent;
}

void COpenGLRenderDeferred::RenderForward()
{
	if(m_pScene->bClear)
	{
		if(m_vClearColor!=m_pScene->vClearColor)
		{
			m_vClearColor=m_pScene->vClearColor;
			glClearColor(m_pScene->vClearColor.c[0],m_pScene->vClearColor.c[1],m_pScene->vClearColor.c[2],1.0);
		}
		glClear(GL_COLOR_BUFFER_BIT);
	}
	RenderAllStages(false);
}

void COpenGLRenderDeferred::RenderDeferred(bool bShadowsPresent,bool bWaterPresent,bool bSkyPresent,bool bLightingPresent)
{
	if(m_GBuffers.nFrameBuffer==0){return;}
	if(m_DeferredShaderNoWaterSun.m_piShader==NULL){return;}
	if(m_DeferredShaderWaterSun.m_piShader==NULL){return;}
	if(m_DeferredShaderDynamic.m_piShader==NULL){return;}
	CMatrix33 mViewRotate(m_pScene->camera.m_ViewMatrix);
	
	RTTIMEMETER_SETGLSTEP("Render-Shader Setup");
	std::vector<IGenericLight *>::iterator i;
	for(i=m_pScene->lighting.m_vLights.begin(); i!=m_pScene->lighting.m_vLights.end();i++)
	{
		IGenericLight *piLight=*i;
		eGenericLightType eType=piLight->GetType();
		
		if(eType==eGenericLightType_Directional)
		{
			CVector vSunEyeDirection=piLight->GetDirectionalDirection();
			
			vSunEyeDirection*=mViewRotate;
			vSunEyeDirection.N();
			
			CVector vHalfVector=piLight->GetDirectionalDirection()-m_pScene->camera.m_vCameraForward;
			vHalfVector.N();
			CVector vEyeHalfVector=vHalfVector;
			vEyeHalfVector*=mViewRotate;
			vEyeHalfVector.N();
			
			std::map<SDeferredShaderKey,CGenericShaderWrapper>::iterator iShader;
			for(iShader=m_mShaders.begin();iShader!=m_mShaders.end();iShader++)
			{
				if(iShader->first.bLighting && iShader->first.eMode==eDeferredMode_Water)
				{
					iShader->second.m_piShader->AddUniform("uAmbient",m_pScene->lighting.m_vAmbientColor,1.0,false);
					iShader->second.m_piShader->AddUniform("uSunDiffuse",piLight->GetDiffuseColor(),1.0,false);
					iShader->second.m_piShader->AddUniform("uSunSpecular",piLight->GetSpecularColor(),1.0,false);
					iShader->second.m_piShader->AddUniform("uSunEyeDirection",vSunEyeDirection,false);
					iShader->second.m_piShader->AddUniform("uSunEyeHalfVector",vEyeHalfVector,false);
				}
			}
			m_DeferredShaderNoWaterSun.m_piShader->AddUniform("uAmbient",m_pScene->lighting.m_vAmbientColor,1.0,false);
			m_DeferredShaderNoWaterSun.m_piShader->AddUniform("uSunDiffuse",piLight->GetDiffuseColor(),1.0,false);
			m_DeferredShaderNoWaterSun.m_piShader->AddUniform("uSunSpecular",piLight->GetSpecularColor(),1.0,false);
			m_DeferredShaderNoWaterSun.m_piShader->AddUniform("uSunEyeDirection",vSunEyeDirection,false);
			m_DeferredShaderNoWaterSun.m_piShader->AddUniform("uSunEyeHalfVector",vEyeHalfVector,false);
			break;
		}
	}

	CVector vEyeWaterBitangent=AxisPosX;
	CVector vEyeWaterTangent=AxisPosZ;
	CVector vEyeWaterNormal=AxisPosY;
	vEyeWaterBitangent*=mViewRotate;
	vEyeWaterTangent*=mViewRotate;
	vEyeWaterNormal*=mViewRotate;
	vEyeWaterBitangent.N();
	vEyeWaterTangent.N();
	vEyeWaterNormal.N();
	
	std::map<SDeferredShaderKey,CGenericShaderWrapper>::iterator iShader;
	for(iShader=m_mShaders.begin();iShader!=m_mShaders.end();iShader++)
	{
		if(iShader->first.eMode==eDeferredMode_Water)
		{
			if(iShader->first.bWater && iShader->first.nTextureUnits)
			{
				iShader->second.m_piShader->AddUniform("uCurrentRealTime",m_pScene->fTime,false);
				iShader->second.m_piShader->AddUniform("uWaterMappingSize",m_pScene->water.dSizeU,m_pScene->water.dSizeV,false);
				iShader->second.m_piShader->AddUniform("uWaterMappingOffset",m_pScene->water.dOffsetU,m_pScene->water.dOffsetV,false);
				iShader->second.m_piShader->AddUniform("uEyeWaterTangent",vEyeWaterTangent,false);
				iShader->second.m_piShader->AddUniform("uEyeWaterBitangent",vEyeWaterBitangent,false);
				iShader->second.m_piShader->AddUniform("uEyeWaterNormal",vEyeWaterNormal,false);
				
			}
			if(iShader->first.bSkyShadow && bSkyPresent){iShader->second.m_piShader->AddUniform("SkyData",CVector(m_pScene->fTime*m_pScene->sky.m_dSkyShadowSpeed,m_pScene->sky.m_dSkyShadowXResolution,m_pScene->sky.m_dSkyShadowZResolution),m_pScene->sky.m_dSkyShadowOpacity,false);}
		}
	}
	
	CMatrix view;
	CMatrix projMatrix;
	ComputeOrthographicMatrix(&projMatrix,0,m_pScene->camera.m_nViewportW,0,m_pScene->camera.m_nViewportH,0.0001,10000);
	ComputeCameraMatrix(&view,Origin,90,90,0);
	
	double dWidthRatio=(double)m_pScene->camera.m_nViewportW/(double)m_GBuffers.nWidth;
	double dHeightRatio=(double)m_pScene->camera.m_nViewportH/(double)m_GBuffers.nHeight;
	
	double dScale=1.0/(m_pScene->camera.m_dPerspectiveFarPlane-m_pScene->camera.m_dPerspectiveNearPlane);
	CMatrix mBias;
	mBias.T(CVector(1,1,1));
	CMatrix mWindowScale;
	mWindowScale.e[0][0]=dWidthRatio*0.5;
	mWindowScale.e[1][1]=dHeightRatio*0.5;
	mWindowScale.e[2][2]=0.5;
	mWindowScale.e[3][3]=1.0;
	CMatrix mZScale;
	mZScale.e[0][0]=dScale;
	mZScale.e[1][1]=dScale;
	mZScale.e[2][2]=dScale;
	mZScale.e[3][3]=dScale;
	CMatrix mCameraInverse;
	mCameraInverse=m_pScene->camera.m_ViewMatrix;
	mCameraInverse*=m_pScene->camera.m_ProjectionMatrix;
	mCameraInverse*=mZScale;
	mCameraInverse*=mBias;
	mCameraInverse*=mWindowScale;
	mCameraInverse.Inverse();
	
	
	
	CMatrix mCameraInverseToEye;
	mCameraInverseToEye=m_pScene->camera.m_ProjectionMatrix;
	mCameraInverseToEye*=mZScale;
	mCameraInverseToEye*=mBias;
	mCameraInverseToEye*=mWindowScale;
	mCameraInverseToEye.Inverse();
	
	SRenderState deferredState=m_sRenderState;
	
	RTTIMEMETER_SETGLSTEP("Render-ShadowMap");
	PrepareSunShadows();

	if(bWaterPresent)
	{
		m_DeferredShaderWaterSun.m_piShader->AddUniform("uProjection",projMatrix,false);
		m_DeferredShaderWaterSun.m_piShader->AddUniform("uView",view,false);
		
		RTTIMEMETER_SETGLSTEP("Render-GBuffer");
		glViewport(0,0,m_pScene->camera.m_nViewportW,m_pScene->camera.m_nViewportH);
		
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,m_GBuffers.nFrameBuffer);
		if(m_pScene->bClear)
		{
			GLenum pClearDrawBuffers[] = { GL_COLOR_ATTACHMENT0_EXT,GL_COLOR_ATTACHMENT2_EXT};
			glDrawBuffers(2, pClearDrawBuffers);
			if(m_vClearColor!=m_pScene->vClearColor)
			{
				m_vClearColor=m_pScene->vClearColor;
				glClearColor(m_pScene->vClearColor.c[0],m_pScene->vClearColor.c[1],m_pScene->vClearColor.c[2],1.0);
			}
			glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
		}
		else
		{
			glClear(GL_DEPTH_BUFFER_BIT);
		}
		
		GLenum pDestinationDrawBuffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT};
		glDrawBuffers(3, pDestinationDrawBuffers);
		
		RenderModelStages(false,eDeferredState_LightingActive);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
		
		RTTIMEMETER_SETGLSTEP("Render-Sun");
		
		if(m_nCurrentVertexArrayObject){glBindVertexArray(0);m_nCurrentVertexArrayObject=0;}
		if(m_nCurrentVertexBufferObject){glBindBuffer(GL_ARRAY_BUFFER,0);m_nCurrentVertexBufferObject=0;}
		if(m_nCurrentIndexesBufferObject){glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);m_nCurrentIndexesBufferObject=0;}
		
		for(int x=0;x<MAX_TEXTURE_LEVELS;x++){REL(m_ppiEffectiveTextureLevels[x]);}
		
		deferredState=m_sRenderState;
		deferredState.bActiveDepthWrite=true;
		deferredState.bActiveSolid=true;
		deferredState.bActiveDepth=true;
		deferredState.bActiveBlending=false;
		deferredState.nDepthFunction=GL_ALWAYS;
		SetRenderState(deferredState,eDeferredStateChange_DoNotUpdateShader);
		
		unsigned int nWidth=m_GBuffers.nWidth;
		unsigned int nHeight=m_GBuffers.nHeight;
		
		CVector vOrigin=CVector(m_pScene->camera.m_nViewportW/2,100,m_pScene->camera.m_nViewportH/2);
		double s1=m_pScene->camera.m_nViewportW,s2=m_pScene->camera.m_nViewportH;
		CVector vAxis1=AxisPosX;
		CVector vAxis2=AxisNegZ;
		
		glViewport(m_pScene->camera.m_nViewportX,m_pScene->camera.m_nViewportY,m_pScene->camera.m_nViewportW,m_pScene->camera.m_nViewportH);
		
		if(m_nCurrentActiveTexture!=DEFERRED_SAMPLER_SUN){glActiveTexture(GL_TEXTURE0_ARB+DEFERRED_SAMPLER_SUN);}
		glBindTexture(GL_TEXTURE_2D,m_GBuffers.nSunSceneBuffer);
		glActiveTexture(GL_TEXTURE0_ARB+DEFERRED_SAMPLER_DEPTH);
		glBindTexture(GL_TEXTURE_2D,m_GBuffers.nDepthBuffer);
		m_nCurrentActiveTexture=DEFERRED_SAMPLER_DEPTH;
		
		if(m_pCurrentShader){m_pCurrentShader->m_piShader->Deactivate();}
		m_pCurrentShader=&m_DeferredShaderWaterSun;
		m_DeferredShaderWaterSun.m_piShader->Activate();
		
		RenderScreenRect(vOrigin,s1,s2,0,0,dWidthRatio,dHeightRatio);
		
		if(m_nCurrentActiveTexture!=DEFERRED_SAMPLER_DIFFUSE){glActiveTexture(GL_TEXTURE0_ARB+DEFERRED_SAMPLER_DIFFUSE);}
		glBindTexture(GL_TEXTURE_2D,m_GBuffers.nDiffuseBuffer);
		glActiveTexture(GL_TEXTURE0_ARB+DEFERRED_SAMPLER_NORMAL);
		glBindTexture(GL_TEXTURE_2D,m_GBuffers.nNormalBuffer);
		m_nCurrentActiveTexture=DEFERRED_SAMPLER_NORMAL;
	}
	else
	{
		float dSkyOpacity=bSkyPresent?m_pScene->sky.m_dSkyShadowOpacity:0.0;
		m_DeferredShaderNoWaterSun.m_piShader->AddUniform("uClearColor",m_pScene->vClearColor,false);
		m_DeferredShaderNoWaterSun.m_piShader->AddUniform("uProjection",projMatrix,false);
		m_DeferredShaderNoWaterSun.m_piShader->AddUniform("uView",view,false);
		m_DeferredShaderNoWaterSun.m_piShader->AddUniform("uUnprojectMatrix",mCameraInverse,false);
		m_DeferredShaderNoWaterSun.m_piShader->AddUniform("uSkyData",CVector(m_pScene->fTime*m_pScene->sky.m_dSkyShadowSpeed,m_pScene->sky.m_dSkyShadowXResolution,m_pScene->sky.m_dSkyShadowZResolution),dSkyOpacity,false);
		
		RTTIMEMETER_SETGLSTEP("Render-GBuffer");
		glViewport(0,0,m_pScene->camera.m_nViewportW,m_pScene->camera.m_nViewportH);
		
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,m_GBuffers.nFrameBuffer);
		glClear(GL_DEPTH_BUFFER_BIT);
		
		GLenum pDestinationDrawBuffers[] = { GL_COLOR_ATTACHMENT2_EXT,GL_COLOR_ATTACHMENT1_EXT};
		glDrawBuffers(2, pDestinationDrawBuffers);
		
		RenderModelStages(false,eDeferredState_LightingActive);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
		
		deferredState=m_sRenderState;
		deferredState.bActiveDepthWrite=true;
		deferredState.bActiveSolid=true;
		deferredState.bActiveDepth=true;
		deferredState.bActiveBlending=false;
		deferredState.nDepthFunction=GL_ALWAYS;
		SetRenderState(deferredState,eDeferredStateChange_DoNotUpdateShader);
		
		RTTIMEMETER_SETGLSTEP("Render-Sun");
		
		if(m_nCurrentVertexArrayObject){glBindVertexArray(0);m_nCurrentVertexArrayObject=0;}
		if(m_nCurrentVertexBufferObject){glBindBuffer(GL_ARRAY_BUFFER,0);m_nCurrentVertexBufferObject=0;}
		if(m_nCurrentIndexesBufferObject){glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);m_nCurrentIndexesBufferObject=0;}
		for(int x=0;x<MAX_TEXTURE_LEVELS;x++){REL(m_ppiEffectiveTextureLevels[x]);}
		
		unsigned int nWidth=m_GBuffers.nWidth;
		unsigned int nHeight=m_GBuffers.nHeight;
		
		CVector vOrigin=CVector(m_pScene->camera.m_nViewportW/2,100,m_pScene->camera.m_nViewportH/2);
		double s1=m_pScene->camera.m_nViewportW,s2=m_pScene->camera.m_nViewportH;
		CVector vAxis1=AxisPosX;
		CVector vAxis2=AxisNegZ;
		
		glViewport(m_pScene->camera.m_nViewportX,m_pScene->camera.m_nViewportY,m_pScene->camera.m_nViewportW,m_pScene->camera.m_nViewportH);
		
		if(m_nCurrentActiveTexture!=DEFERRED_SAMPLER_DIFFUSE){glActiveTexture(GL_TEXTURE0_ARB+DEFERRED_SAMPLER_DIFFUSE);}
		glBindTexture(GL_TEXTURE_2D,m_GBuffers.nDiffuseBuffer);
		glActiveTexture(GL_TEXTURE0_ARB+DEFERRED_SAMPLER_NORMAL);
		glBindTexture(GL_TEXTURE_2D,m_GBuffers.nNormalBuffer);
		glActiveTexture(GL_TEXTURE0_ARB+DEFERRED_SAMPLER_DEPTH);
		glBindTexture(GL_TEXTURE_2D,m_GBuffers.nDepthBuffer);

		if(bSkyPresent)
		{
			glActiveTexture(GL_TEXTURE0_ARB+DEFERRED_SAMPLER_SKY);m_nCurrentActiveTexture=DEFERRED_SAMPLER_SKY;
			if(m_pScene->sky.m_piSkyShadow){m_pScene->sky.m_piSkyShadow->PrepareTexture(DEFERRED_SAMPLER_SKY);}
		}
		
		if(m_pCurrentShader){m_pCurrentShader->m_piShader->Deactivate();}
		m_pCurrentShader=&m_DeferredShaderNoWaterSun;
		m_DeferredShaderNoWaterSun.m_piShader->Activate();
		
		RenderScreenRect(vOrigin,s1,s2,0,0,dWidthRatio,dHeightRatio);
	}
	
	UnprepareSunShadows();
	
	RTTIMEMETER_SETGLSTEP("Render-Dynamic");
	
	if(m_pScene->lighting.m_vLights.size()>1)
	{
		m_DeferredShaderDynamic.m_piShader->AddUniform("uUnprojectMatrix",mCameraInverseToEye,false);
		m_DeferredShaderDynamic.m_piShader->AddUniform("uProjection",projMatrix,false);
		m_DeferredShaderDynamic.m_piShader->AddUniform("uView",view,false);
		
		deferredState=m_sRenderState;
		deferredState.bActiveDepth=false;
		deferredState.bActiveBlending=true;
		deferredState.nBlendOperator1=GL_ONE;
		deferredState.nBlendOperator2=GL_ONE;
		SetRenderState(deferredState,eDeferredStateChange_DoNotUpdateShader);
		
		if(m_pCurrentShader){m_pCurrentShader->m_piShader->Deactivate();}
		m_pCurrentShader=&m_DeferredShaderDynamic;
		m_DeferredShaderDynamic.m_piShader->Activate();
		
		double dAspectRatio=(double)m_pScene->camera.m_nViewportW/(double)m_pScene->camera.m_nViewportH;
		
		for(i=m_pScene->lighting.m_vLights.begin(); i!=m_pScene->lighting.m_vLights.end();i++)
		{
			IGenericLight *piLight=*i;
			eGenericLightType eType=piLight->GetType();
			
			if(eType==eGenericLightType_Omni)
			{
				CVector vPosition=piLight->GetPosition();
				
				double dLightEyeForward=m_pScene->camera.m_CameraForwardPlane.GetSide(vPosition);
				double dLightEyeUp=m_pScene->camera.m_CameraUpPlane.GetSide(vPosition);
				double dLightEyeRight=m_pScene->camera.m_CameraRightPlane.GetSide(vPosition);
				
				double dCameraPlaneRight=dLightEyeForward*tan(DegreesToRadians(m_pScene->camera.m_dPerspectiveViewAngle*0.5))*dAspectRatio;
				double dCameraPlaneUp=dLightEyeForward*tan(DegreesToRadians(m_pScene->camera.m_dPerspectiveViewAngle*0.5));
				
				double dRelRight=(dLightEyeRight/dCameraPlaneRight);
				double dRelUp=(dLightEyeUp/dCameraPlaneUp);
				
				double dLightViewportX=(0.5+dRelRight*0.5)*(double)m_pScene->camera.m_nViewportW;
				double dLightViewportY=(0.5+dRelUp*0.5)*(double)m_pScene->camera.m_nViewportH;
				
				double dPixelsPerUnit=((double)m_pScene->camera.m_nViewportW)/(2.0*dCameraPlaneRight);
				double dSize=dPixelsPerUnit*(piLight->GetOmniRadius()*LIGHT_SIZE_INCREMENT);
				
				double dLightTexCoordU=(0.5+dRelRight*0.5);
				double dLightTexCoordV=(0.5+dRelUp*0.5);
				double dTexSizeU=dSize*2.0/((double)m_pScene->camera.m_nViewportW);
				double dTexSizeV=dSize*2.0/((double)m_pScene->camera.m_nViewportH);
				
				dTexSizeU*=dWidthRatio;
				dTexSizeV*=dHeightRatio;
				dLightTexCoordU*=dWidthRatio;
				dLightTexCoordV*=dHeightRatio;
				
				vPosition*=m_pScene->camera.m_ViewMatrix;
				
				m_DeferredShaderDynamic.m_piShader->AddUniform("uDynamicPosition",vPosition,false);
				m_DeferredShaderDynamic.m_piShader->AddUniform("uDynamicDiffuse",piLight->GetDiffuseColor(),1.0,false);
				m_DeferredShaderDynamic.m_piShader->AddUniform("uDynamicSpecular",piLight->GetSpecularColor(),1.0,false);
				m_DeferredShaderDynamic.m_piShader->AddUniform("uDynamicRange",(float)(piLight->GetOmniRadius()*LIGHT_SIZE_INCREMENT),false);

				CVector vLightOrigin;
				vLightOrigin=CVector(dLightViewportX,100,dLightViewportY);
				
				RenderScreenRect(vLightOrigin,dSize*2.0,dSize*2.0,dLightTexCoordU-dTexSizeU*0.5,dLightTexCoordV-dTexSizeV*0.5,dTexSizeU,dTexSizeV);
			}
		}
	}
	
	RTTIMEMETER_SETGLSTEP("Render-Models (unlighted)");
	RenderModelStages(false,eDeferredState_LightingInactive);
	RTTIMEMETER_SETGLSTEP("Render-Triangles");
	RenderTriangleStages(false);
	RTTIMEMETER_SETGLSTEP("Render-Lines");
	RenderLineStages(false);
	RTTIMEMETER_SETGLSTEP("Render-Points");
	RenderPointStages(false);
	RTTIMEMETER_ENDGLSTEP();
}

void COpenGLRenderDeferred::RenderScreenRect(const CVector &vOrigin,double s1,double s2,double dTexX,double dTexY,double dTexW,double dTexH)
{
	float pVertexBuffer[12];
	float pTextureBuffer[8];
	unsigned short pFaces[]={0,1,2,0,2,3};
	
	CVector vAxis1=CVector(round(s1/2.0),0,0);
	CVector vAxis2=CVector(0,0,round(s2/2.0));
	
	CVector v[4];
	v[0]=vOrigin+vAxis1+vAxis2;
	v[1]=vOrigin-vAxis1+vAxis2;
	v[2]=vOrigin-vAxis1-vAxis2;
	v[3]=vOrigin+vAxis1-vAxis2;
	
	pTextureBuffer[0]=dTexX+dTexW;
	pTextureBuffer[1]=dTexY+dTexH;
	pTextureBuffer[2]=dTexX;
	pTextureBuffer[3]=dTexY+dTexH;
	pTextureBuffer[4]=dTexX;
	pTextureBuffer[5]=dTexY;
	pTextureBuffer[6]=dTexX+dTexW;
	pTextureBuffer[7]=dTexY;

	pVertexBuffer[0]=v[0].c[0];
	pVertexBuffer[1]=v[0].c[1];
	pVertexBuffer[2]=v[0].c[2];
	
	pVertexBuffer[3]=v[1].c[0];
	pVertexBuffer[4]=v[1].c[1];
	pVertexBuffer[5]=v[1].c[2];
	
	pVertexBuffer[6]=v[2].c[0];
	pVertexBuffer[7]=v[2].c[1];
	pVertexBuffer[8]=v[2].c[2];
	
	pVertexBuffer[9]=v[3].c[0];
	pVertexBuffer[10]=v[3].c[1];
	pVertexBuffer[11]=v[3].c[2];
	
	float *pTextureBufferTemp=pTextureBuffer;
	SetVertexPointers(pVertexBuffer,NULL,NULL,1,&pTextureBufferTemp);
	glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_SHORT,pFaces);
}

void COpenGLRenderDeferred::RenderScene(SSceneData &sScene)
{
	if(m_bSelecting)
	{
		m_pScene=&sScene;
		RenderAllStages(false);
		m_pScene=NULL;
		return;
	}
	if(sScene.camera.bProjectionModified){SetProjectionMatrix(sScene.camera.m_ProjectionMatrix);}
	if(sScene.camera.bViewModified){SetModelViewMatrix(sScene.camera.m_ViewMatrix);}
	if(sScene.camera.bViewportModified){glViewport(sScene.camera.m_nViewportX,sScene.camera.m_nViewportY,sScene.camera.m_nViewportW,sScene.camera.m_nViewportH);}
	
	if(sScene.clipping.bEnabled && !m_bClippingActive){m_bClippingActive=true;glEnable(GL_SCISSOR_TEST);}
	else if(!sScene.clipping.bEnabled && m_bClippingActive){m_bClippingActive=false;glDisable(GL_SCISSOR_TEST);}
	if(m_rClipRect!=sScene.clipping.rRect){m_rClipRect=sScene.clipping.rRect;glScissor(m_rClipRect.x,m_rClipRect.y,m_rClipRect.w,m_rClipRect.h);}
	
	m_pScene=&sScene;
	
	bool bShadowsPresent=false,bWaterPresent=false,bSkyPresent=false,bLightingPresent=false;
	AnalyzeStages(&bShadowsPresent,&bWaterPresent,&bSkyPresent,&bLightingPresent);
	m_bAnyShadowInTheScene=bShadowsPresent;
	
	if(!bLightingPresent){m_eRenderMode=eDeferredMode_Forward;}
	else if (bWaterPresent){m_eRenderMode=eDeferredMode_Water;}
	else {m_eRenderMode=eDeferredMode_NoWater;}
		
	m_fCurrentRealTime=sScene.fTime;
	
	if(m_eRenderMode!=eDeferredMode_Forward)
	{
		if(m_GBuffers.nWidth<sScene.camera.m_nViewportW || 
			m_GBuffers.nHeight<sScene.camera.m_nViewportH)
		{
			RTTRACE("COpenGLRenderDeferred::RenderScene -> Resizing GBuffers from %dx%d to %dx%d",m_GBuffers.nWidth,m_GBuffers.nHeight,sScene.camera.m_nViewportW,sScene.camera.m_nViewportH);
			FreeGBuffers(&m_GBuffers);
			m_GBuffers=CreateGBuffers(sScene.camera.m_nViewportW,sScene.camera.m_nViewportH);
		}
		RenderDeferred(bShadowsPresent,bWaterPresent,bSkyPresent,bLightingPresent);
	}
	else
	{
		RenderForward();
	}

	m_bAnyShadowInTheScene=false;
	m_eRenderMode=eDeferredMode_Forward;
	m_pScene=NULL;
}

void COpenGLRenderDeferred::StartFrame()
{
}

void COpenGLRenderDeferred::EndFrame()
{
	for(unsigned int x=0;x<MAX_TEXTURE_LEVELS;x++)
	{
		REL(m_ppiEffectiveTextureLevels[x]);
	}
}

void COpenGLRenderDeferred::SetRenderState( const SRenderState &sNewState,EDeferredStateChangeShader eShader)
{
	m_sRenderState.bActiveShadowEmission=sNewState.bActiveShadowEmission;
	m_sRenderState.bActiveShadowReception=sNewState.bActiveShadowReception;
	m_sRenderState.bActiveWater=sNewState.bActiveWater;
	
	bool bDeferredStateChange=false;
	if(m_sRenderState.bActiveSolid!=sNewState.bActiveSolid)
	{
		if(sNewState.bActiveSolid)
		{
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		}
		bDeferredStateChange=true;
	}
	if(m_sRenderState.bActiveLighting!=sNewState.bActiveLighting)
	{
		bDeferredStateChange=true;
	}
	if(m_sRenderState.bActiveBlending!=sNewState.bActiveBlending)
	{
		if(sNewState.bActiveBlending)
		{
			glEnable(GL_BLEND);
			m_bNormalBlendingEnabled=true;
		}
		else
		{
			glDisable(GL_BLEND);
		}
		bDeferredStateChange=true;
	}
	if(sNewState.bActiveBlending)
	{
		if(m_eRenderMode==eDeferredMode_Water && m_bNormalBlendingEnabled)
		{
			m_bNormalBlendingEnabled=false;
			glDisableIndexedEXT(GL_BLEND,1);
		}
		else if(m_eRenderMode!=eDeferredMode_Water && !m_bNormalBlendingEnabled)
		{
			m_bNormalBlendingEnabled=true;
			glEnableIndexedEXT(GL_BLEND,1);
		}
	}
	if(m_sRenderState.nBlendOperator1!=sNewState.nBlendOperator1 ||
		m_sRenderState.nBlendOperator2!=sNewState.nBlendOperator2)
	{
		glBlendFunc(sNewState.nBlendOperator1,sNewState.nBlendOperator2);
		bDeferredStateChange=true;
	}
	if(m_sRenderState.bActiveDepth!=sNewState.bActiveDepth)
	{
		if(sNewState.bActiveDepth)
		{
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}
		bDeferredStateChange=true;
	}
	if(m_sRenderState.bActiveDepthWrite!=sNewState.bActiveDepthWrite)
	{
		if(sNewState.bActiveDepthWrite)
		{
			glDepthMask(true);
		}
		else
		{
			glDepthMask(false);
		}
		bDeferredStateChange=true;
	}
	if(m_sRenderState.nDepthFunction!=sNewState.nDepthFunction)
	{
		glDepthFunc(sNewState.nDepthFunction);
		bDeferredStateChange=true;
	}
	if(m_sRenderState.bActiveSkyShadow!=sNewState.bActiveSkyShadow)
	{
		if(sNewState.bActiveSkyShadow)
		{
			if(m_pScene->sky.m_piSkyShadow)
			{
				if(m_nCurrentActiveTexture!=m_nSkyShadowTextureLevel){glActiveTexture(GL_TEXTURE0_ARB+m_nSkyShadowTextureLevel);m_nCurrentActiveTexture=m_nSkyShadowTextureLevel;}
				if(m_pScene->sky.m_piSkyShadow){m_pScene->sky.m_piSkyShadow->PrepareTexture(m_nSkyShadowTextureLevel);}
			}
		}
		else
		{
			if(m_pScene->sky.m_piSkyShadow)
			{
				if(m_nCurrentActiveTexture!=m_nSkyShadowTextureLevel){glActiveTexture(GL_TEXTURE0_ARB+m_nSkyShadowTextureLevel);m_nCurrentActiveTexture=m_nSkyShadowTextureLevel;}
				if(m_pScene->sky.m_piSkyShadow){m_pScene->sky.m_piSkyShadow->UnprepareTexture(m_nSkyShadowTextureLevel);}
			}
		}
		bDeferredStateChange=true;
	}
	if(m_sRenderState.bActiveHeightFog!=sNewState.bActiveHeightFog)
	{
		bDeferredStateChange=true;
	}
	if(bDeferredStateChange)
	{
		m_sRenderState=sNewState;
	}
	if(eShader==eDeferredStateChange_UpdateShader)
	{
		SetCurrentRenderStateShader();
	}
}

void COpenGLRenderDeferred::SetCurrentRenderStateShader()
{
	if(m_bSelecting){return;}
	
	bool bTempSkyShadow=m_pScene->sky.m_piSkyShadow && m_pScene->sky.m_piSkyShadow && m_sRenderState.bActiveSkyShadow && m_sRenderState.bActiveShadowReception;
	bool bTempShadow=m_sRenderState.bActiveShadowReception && m_bAnyShadowInTheScene && m_eRenderMode!=eDeferredMode_Forward;
	SDeferredShaderKey key(m_eRenderMode,m_sRenderState.bActiveHeightFog,bTempShadow,m_sRenderState.bActiveTextures && m_nTextureLevels!=0,m_sRenderState.bActiveLighting,m_sRenderState.bActiveWater,/*m_pOptions->bEnableNormalMaps*/m_piEffectiveNormalMap!=NULL,bTempSkyShadow,m_bRenderingPoints);
	std::map<SDeferredShaderKey,CGenericShaderWrapper>::iterator iShader=m_mShaders.find(key);
	CGenericShaderWrapper *pNewShader=(iShader==m_mShaders.end())?NULL:&iShader->second;
	if(!pNewShader)
	{
		AddShader(key);
		iShader=m_mShaders.find(key);
		pNewShader=(iShader==m_mShaders.end())?NULL:&iShader->second;
		if(pNewShader==NULL)
		{
			RTTRACE("COpenGLRenderDeferred::SetRenderState -> Failed to create on-demand shader");
		}
	}
	if(m_pCurrentShader!=pNewShader)
	{
		if(m_pCurrentShader){m_pCurrentShader->m_piShader->Deactivate();}
		m_pCurrentShader=pNewShader;
		
		if(m_pCurrentShader)
		{
			//if(m_pOptions->bEnableStagedRenderingStats){m_sStagedStats.nShaderChanges++;}
			//m_sStagedStats.nShaderChanges++;
			//RTTRACE("Set shader %s",m_mShaderNames[key].c_str());
			m_pCurrentShader->m_piShader->Activate();
			if(key.bHeightFog && key.eMode!=eDeferredMode_Forward)
			{
				m_pCurrentShader->m_piShader->AddUniform("uFogColor",m_sRenderState.vHeightFogColor,false);
				m_pCurrentShader->m_piShader->AddUniform("uFogMins",m_sRenderState.vHeightFogMins,false);
				m_pCurrentShader->m_piShader->AddUniform("uFogMaxs",m_sRenderState.vHeightFogMaxs,false);
			}
		}
	}
}

void COpenGLRenderDeferred::RenderModelStages(bool bRenderingShadow,EDeferredState eDeferredState)
{
	unsigned int pMatrixesToRestore[MAX_TEXTURE_LEVELS];
	unsigned int nMatrixesToRestore=0;
	
	// Flush Model Stages
	std::multimap<double,std::pair<const SModelStageKey*,const SModelStage*> >::iterator iSortedModelStage;
	for(iSortedModelStage=m_pScene->objects.mModelStages.begin();iSortedModelStage!=m_pScene->objects.mModelStages.end();iSortedModelStage++)
	{
		const SModelStageKey *pKey=iSortedModelStage->second.first;
		const SModelStage *pStage=iSortedModelStage->second.second;
		if(bRenderingShadow && !pKey->sRenderState.bActiveShadowEmission){continue;}
		
		bool bLightedStage=(pKey->sRenderState.bActiveLighting || pKey->sRenderState.bActiveWater);
		bool bProcess=(eDeferredState==eDeferredState_ALL || 
						(bLightedStage && eDeferredState==eDeferredState_LightingActive) ||
						(!bLightedStage&& eDeferredState==eDeferredState_LightingInactive));
		if(!bProcess){continue;}
// For each render buffer
		unsigned int nBuffers=pStage->piModel->GetFrameRenderBuffers(pKey->nAnimation,pKey->nFrame);
		for(unsigned int nBuffer=0;nBuffer<nBuffers;nBuffer++)
		{
			bool bRenderBufferPrepared=false;
			if(!bRenderingShadow)
			{
				unsigned long nFaces=0,nVertexes=0;
				pStage->piModel->GetRenderBufferFaces(pKey->nAnimation,pKey->nFrame,nBuffer,&nFaces,0);
				pStage->piModel->GetRenderBufferVertexes(pKey->nAnimation,pKey->nFrame,nBuffer,&nVertexes,0);
			}
			float fMatrixes[16*MAX_OBJECT_INSTANCES];
			CVector vTintings[MAX_OBJECT_INSTANCES];
			float *pfMatrixes=fMatrixes;
			unsigned int nInstances=0;
			
			for(unsigned int nInstance=0;nInstance<pStage->vInstances.size();nInstance++)
			{
				CVector vPos=pStage->vInstances[nInstance].vPos;
				CVector vAngles=pStage->vInstances[nInstance].vAngles;
				bool bSkipRender=pStage->vInstances[nInstance].bSkipRender;
				
				if(!bRenderingShadow && bSkipRender){continue;}
				if(!bRenderBufferPrepared)
				{
					bRenderBufferPrepared=true;
					if(!bRenderingShadow){SetRenderState(pKey->sRenderState,eDeferredStateChange_DoNotUpdateShader);}
					pStage->piGLModel->PrepareRenderBuffer(this,pKey->nAnimation,pKey->nFrame,nBuffer,bRenderingShadow,&m_RenderMappings);
					if(!bRenderingShadow)
					{						
						SetCurrentRenderStateShader();
						
						if(m_pCurrentShader)
						{
							CVector vAmbientColor,vDiffuseColor,vSpecularColor;
							float fShininess,fOpacity;
							float *pColors=NULL;
							unsigned long nLevels=0;
							pStage->piModel->GetRenderBufferMaterial(pKey->nAnimation,pKey->nFrame,nBuffer,&vAmbientColor,&vDiffuseColor,&vSpecularColor, &fShininess, &fOpacity);
							pStage->piModel->GetRenderBufferColors(pKey->nAnimation,pKey->nFrame,nBuffer,&pColors);
							pStage->piModel->GetRenderBufferTextureLevels(pKey->nAnimation,pKey->nFrame,nBuffer,&nLevels);
							for(unsigned long x=0;x<nLevels && x<MAX_TEXTURE_LEVELS;x++)
							{
								bool bIdentity=false;
								pStage->piModel->GetRenderBufferTextureMatrix(pKey->nAnimation,pKey->nFrame,nBuffer,x,NULL,&bIdentity);
								if(!bIdentity)
								{
									CMatrix matrix;
									pStage->piModel->GetRenderBufferTextureMatrix(pKey->nAnimation,pKey->nFrame,nBuffer,x,&matrix,&bIdentity);
									if(m_pCurrentShader){m_pCurrentShader->m_piShader->AddUniform(x==0?"uTexMatrix0":"uTexMatrix1",matrix,false);}
									pMatrixesToRestore[nMatrixesToRestore]=x;
									nMatrixesToRestore++;
								}
							}
							
							if(m_eRenderMode!=eDeferredMode_Forward)
							{
								m_pCurrentShader->m_piShader->AddUniform("uDisableVertexColor",(float)(pColors?0.0:1.0),true);
							}
							if(pKey->sRenderState.bActiveLighting)
							{
								m_pCurrentShader->m_piShader->AddUniform("uMaterialDiffuse",vDiffuseColor,fOpacity,true);
								m_pCurrentShader->m_piShader->AddUniform("uMaterialSpecular",vSpecularColor,fOpacity,true);
							}
						}
					}
				}
				
				CMatrix m,tmp;
				if(vAngles.c[ROLL]){tmp.R(AxisPosX,DegreesToRadians(vAngles.c[ROLL]));m*=tmp;}
				if(vAngles.c[PITCH]){tmp.R(AxisPosZ,DegreesToRadians(vAngles.c[PITCH]));m*=tmp;}
				if(vAngles.c[YAW]){tmp.R(AxisPosY,DegreesToRadians(vAngles.c[YAW]));m*=tmp;}
				if(vPos.c[0] || vPos.c[1] || vPos.c[2]){tmp.T(vPos);m*=tmp;}
				m.Transpose();
				for(int x=0;x<16;x++){*pfMatrixes++=((double*)m.e)[x];}
				vTintings[nInstances]=pStage->vInstances[nInstance].vTinting;
				
				nInstances++;
				
				if(  m_sHardwareSupport.bObjectInstancing==false ||
					(m_sHardwareSupport.bObjectInstancing==true  && nInstances==MAX_OBJECT_INSTANCES))
				{
					CMatrix identity;
					if(m_pCurrentShader){m_pCurrentShader->m_piShader->AddUniformMatrixes("uModel",nInstances,fMatrixes,true);}
					if(m_pCurrentShader && !bRenderingShadow && !m_bSelecting){m_pCurrentShader->m_piShader->AddUniformVectors("uTinting",nInstances,vTintings,true);}
					pStage->piGLModel->CallRenderBuffer(this,pKey->nAnimation,pKey->nFrame,nBuffer,nInstances);
					if(m_pCurrentShader){m_pCurrentShader->m_piShader->AddUniform("uModel",identity,true);}
					
					nInstances=0;
					pfMatrixes=fMatrixes;
				}
			}
			
			if(nInstances)
			{
				CMatrix identity;
				if(m_pCurrentShader){m_pCurrentShader->m_piShader->AddUniformMatrixes("uModel",nInstances,fMatrixes,true);}
				if(m_pCurrentShader && !bRenderingShadow && !m_bSelecting){m_pCurrentShader->m_piShader->AddUniformVectors("uTinting",nInstances,vTintings,true);}
				pStage->piGLModel->CallRenderBuffer(this,pKey->nAnimation,pKey->nFrame,nBuffer,nInstances);
				if(m_pCurrentShader){m_pCurrentShader->m_piShader->AddUniform("uModel",identity,true);}
			}
			
			if(bRenderBufferPrepared)
			{
				for(unsigned int x=0;x<nMatrixesToRestore;x++)
				{
					CMatrix identity;
					if(m_pCurrentShader){m_pCurrentShader->m_piShader->AddUniform(pMatrixesToRestore[x]==0?"uTexMatrix0":"uTexMatrix1",identity,false);}
				}
				nMatrixesToRestore=0;
				
				pStage->piGLModel->UnPrepareRenderBuffer(this,pKey->nAnimation,pKey->nFrame,nBuffer,bRenderingShadow,&m_RenderMappings);
			}
		}
	}
}

void COpenGLRenderDeferred::SetEffectiveTexture(IGenericTexture *pTexture,int nTextureLevel)
{
	if(nTextureLevel>MAX_TEXTURE_LEVELS){return;}
	if(pTexture==NULL){return;}

	IGenericTexture *piOldTexture=m_ppiEffectiveTextureLevels[nTextureLevel];
	if(pTexture==piOldTexture){return;}

	if(m_nCurrentActiveTexture!=nTextureLevel){glActiveTexture(GL_TEXTURE0_ARB+nTextureLevel);m_nCurrentActiveTexture=nTextureLevel;}
	if(piOldTexture){piOldTexture->UnprepareTexture(nTextureLevel);}	

	m_ppiEffectiveTextureLevels[nTextureLevel]=ADD(pTexture);
	pTexture->PrepareTexture(nTextureLevel);
	REL(piOldTexture);
}

void COpenGLRenderDeferred::PrepareTexture(IGenericTexture *piTexture,unsigned int nTextureLevel)
{
	if(!m_sRenderState.bActiveTextures){return;}
	if(nTextureLevel>MAX_TEXTURE_LEVELS){return;}
	if(m_ppiTextureLevels[nTextureLevel]!=NULL)
	{
		REL(m_ppiTextureLevels[nTextureLevel]);
		m_nTextureLevels--;
	}

	m_ppiTextureLevels[m_nTextureLevels]=ADD(piTexture);
	m_nTextureLevels++;
	SetEffectiveTexture(piTexture,nTextureLevel);
}

void COpenGLRenderDeferred::UnprepareTexture(unsigned int nTextureLevel)
{
	if(nTextureLevel>MAX_TEXTURE_LEVELS){return;}
	if(m_ppiTextureLevels[nTextureLevel]==NULL){return;}
	
	REL(m_ppiTextureLevels[nTextureLevel]);
	m_nTextureLevels--;
}

void COpenGLRenderDeferred::PrepareNormalMap(IGenericTexture *piNormalMap)
{	
	if(piNormalMap==NULL){return;}

	IGenericTexture *piOldNormalMap=m_piEffectiveNormalMap;
	if(piNormalMap==piOldNormalMap){return;}

	if(m_nCurrentActiveTexture!=m_nNormalMapTextureLevel){glActiveTexture(GL_TEXTURE0_ARB+m_nNormalMapTextureLevel);m_nCurrentActiveTexture=m_nNormalMapTextureLevel;}
	if(piOldNormalMap){piOldNormalMap->UnprepareTexture(m_nNormalMapTextureLevel);}	

	m_piEffectiveNormalMap=ADD(piNormalMap);
	piNormalMap->PrepareTexture(m_nNormalMapTextureLevel);
	REL(piOldNormalMap);
}

void COpenGLRenderDeferred::UnprepareNormalMap()
{
	if(m_piEffectiveNormalMap)
	{
		m_piEffectiveNormalMap->UnprepareTexture(m_nNormalMapTextureLevel);
		REL(m_piEffectiveNormalMap);
	}
}
void COpenGLRenderDeferred::RenderTriangleStages(bool bRenderingShadow)
{
	// Flush Texture Particles Stages.
	std::map<STriangleStageKey,STriangleStage>::iterator iTriangleStage;
	for(iTriangleStage=m_pScene->objects.mTriangleStages.begin();iTriangleStage!=m_pScene->objects.mTriangleStages.end();iTriangleStage++)
	{
		const STriangleStageKey *pKey=&iTriangleStage->first;
		STriangleStage *pStage=&iTriangleStage->second;
		if(bRenderingShadow && !pKey->sRenderState.bActiveShadowEmission){continue;}

		IGenericTexture *piTexture=pKey->piTexture;
		if(!bRenderingShadow){SetRenderState(pKey->sRenderState,eDeferredStateChange_DoNotUpdateShader);}
		if(piTexture){PrepareTexture(piTexture,0);}
		
		SetCurrentRenderStateShader();

		for(unsigned int x=0;x<pStage->vBuffers.size();x++)
		{
			STriangleBuffer *pBuffer=pStage->vBuffers[x];
			float *pTexVertexBufferTemp=pBuffer->pTexBuffer;
			SetVertexPointers(pBuffer->pVertexBuffer,NULL,pBuffer->pColorBuffer,1,&pTexVertexBufferTemp);
			glDrawArrays(GL_TRIANGLES,0,pBuffer->nUsedElements*3);
		}
		if(piTexture){UnprepareTexture(0);}
	}
}

void COpenGLRenderDeferred::RenderLineStages(bool bRenderingShadow)
{
	if(m_pScene->objects.mLineStages.size()){glEnableLineStipple();}
	
	std::map<SLineStageKey,SLineStage>::iterator iLineStage;
	for(iLineStage=m_pScene->objects.mLineStages.begin();iLineStage!=m_pScene->objects.mLineStages.end();iLineStage++)
	{
		const SLineStageKey *pKey=&iLineStage->first;
		SLineStage *pStage=&iLineStage->second;

		if(bRenderingShadow && !pKey->sRenderState.bActiveShadowEmission){continue;}

		SetRenderState(pKey->sRenderState);
		glLineStipple(1,(unsigned short)pKey->nStipple);

		for(unsigned int x=0;x<pStage->vBuffers.size();x++)
		{
			SLineBuffer *pBuffer=pStage->vBuffers[x];
			SetVertexPointers(pBuffer->pVertexBuffer,NULL,pBuffer->pColorBuffer,0,NULL);
			glDrawArrays(GL_LINES,0,pBuffer->nUsedElements*2);
		}
	}
	if(m_pScene->objects.mLineStages.size()){glDisableLineStipple();}
}

void COpenGLRenderDeferred::RenderPointStages(bool bRenderingShadow)
{
	m_bRenderingPoints=true;
	
	std::map<SPointStageKey,SPointStage>::iterator iPointStage;
	for(iPointStage=m_pScene->objects.mPointStages.begin();iPointStage!=m_pScene->objects.mPointStages.end();iPointStage++)
	{
		const SPointStageKey *pKey=&iPointStage->first;
		SPointStage *pStage=&iPointStage->second;

		if(bRenderingShadow && !pKey->sRenderState.bActiveShadowEmission){continue;}

		SetRenderState(pKey->sRenderState);
		glPointSize((float)pKey->dSize);
		
		for(unsigned int x=0;x<pStage->vBuffers.size();x++)
		{
			SPointBuffer *pBuffer=pStage->vBuffers[x];
			SetVertexPointers(pBuffer->pVertexBuffer,NULL,pBuffer->pColorBuffer,0,NULL);

			glDrawArrays(GL_POINTS,0,pBuffer->nUsedElements);
		}
	}
	
	m_bRenderingPoints=false;
}

void COpenGLRenderDeferred::SetVertexBufferObject(SVertexBufferObject *pVBO)
{
	if(pVBO==NULL)
	{
		glBindVertexArray(0);
		m_nCurrentVertexArrayObject=0;
		return;
	}
	if(pVBO->nVertexArrayObject)
	{
		if(pVBO->nVertexArrayObject!=m_nCurrentVertexArrayObject)
		{
			m_nCurrentVertexArrayObject=pVBO->nVertexArrayObject;
			glBindVertexArray(pVBO->nVertexArrayObject);
		}
	}
	else
	{
		if(pVBO->nBufferObject)
		{
			glBindBuffer(GL_ARRAY_BUFFER,pVBO->nBufferObject);
			m_nCurrentVertexBufferObject=pVBO->nBufferObject;
		}
		if(pVBO->nIndexesBufferObject)
		{	
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,pVBO->nIndexesBufferObject);
			m_nCurrentIndexesBufferObject=pVBO->nIndexesBufferObject;
		}
	}
	
	if(pVBO->nVertexArrayObject==0)
	{
		if(!m_sBufferState.bVertexBufferEnabled && pVBO->nVertexOffset!=-1)     {glEnableVertexAttribArray(m_RenderMappings.nVertexAttribIndex);m_sBufferState.bVertexBufferEnabled=true;}
		else if(m_sBufferState.bVertexBufferEnabled && pVBO->nVertexOffset==-1){glDisableVertexAttribArray(m_RenderMappings.nVertexAttribIndex);m_sBufferState.bVertexBufferEnabled=false;}
		
		if(!m_sBufferState.bColorBufferEnabled && pVBO->nColorOffset!=-1)     {glEnableVertexAttribArray(m_RenderMappings.nColorAttribIndex);m_sBufferState.bColorBufferEnabled=true;}
		else if(m_sBufferState.bColorBufferEnabled && pVBO->nColorOffset==-1){glDisableVertexAttribArray(m_RenderMappings.nColorAttribIndex);m_sBufferState.bColorBufferEnabled=false;}
		
		if(!m_sBufferState.bNormalBufferEnabled && pVBO->nNormalOffset!=-1)     {glEnableVertexAttribArray(m_RenderMappings.nNormalAttribIndex);m_sBufferState.bNormalBufferEnabled=true;}
		else if(m_sBufferState.bNormalBufferEnabled && pVBO->nNormalOffset==-1){glDisableVertexAttribArray(m_RenderMappings.nNormalAttribIndex);m_sBufferState.bNormalBufferEnabled=false;}
		
		if(!m_sBufferState.bNormalMapBufferEnabled && pVBO->nNormalMapOffset!=-1)     {glEnableVertexAttribArray(m_RenderMappings.nNormalMapCoordAttribIndex);m_sBufferState.bNormalMapBufferEnabled=true;}
		else if(m_sBufferState.bNormalMapBufferEnabled && pVBO->nNormalMapOffset==-1){glDisableVertexAttribArray(m_RenderMappings.nNormalMapCoordAttribIndex);m_sBufferState.bNormalMapBufferEnabled=false;}
		
		if(!m_sBufferState.bTangentBufferEnabled && pVBO->nTangentOffset!=-1)     {glEnableVertexAttribArray(m_RenderMappings.nTangentAttribIndex);m_sBufferState.bTangentBufferEnabled=true;}
		else if(m_sBufferState.bTangentBufferEnabled && pVBO->nTangentOffset==-1){glDisableVertexAttribArray(m_RenderMappings.nTangentAttribIndex);m_sBufferState.bTangentBufferEnabled=false;}
		
		if(!m_sBufferState.bBitangentBufferEnabled && pVBO->nBitangentOffset!=-1)     {glEnableVertexAttribArray(m_RenderMappings.nBitangentAttribIndex);m_sBufferState.bBitangentBufferEnabled=true;}
		else if(m_sBufferState.bBitangentBufferEnabled && pVBO->nBitangentOffset==-1){glDisableVertexAttribArray(m_RenderMappings.nBitangentAttribIndex);m_sBufferState.bBitangentBufferEnabled=false;}
		
		if(pVBO->nVertexOffset!=-1){m_pLastVertexPointer=NULL;glVertexAttribPointer(m_RenderMappings.nVertexAttribIndex,3,GL_FLOAT,GL_FALSE,pVBO->nVertexStride,(void*)pVBO->nVertexOffset);}
		if(pVBO->nColorOffset!=-1){m_pLastColorPointer=NULL;glVertexAttribPointer(m_RenderMappings.nColorAttribIndex,4,GL_FLOAT,GL_FALSE,pVBO->nColorStride,(void*)pVBO->nColorOffset);}
		if(pVBO->nNormalOffset!=-1){m_pLastNormalPointer=NULL;glVertexAttribPointer(m_RenderMappings.nNormalAttribIndex,3,GL_FLOAT,GL_FALSE,pVBO->nNormalStride,(void*)pVBO->nNormalOffset);}
		if(pVBO->nNormalMapOffset!=-1){m_pLastNormalMapPointer=NULL;glVertexAttribPointer(m_RenderMappings.nNormalMapCoordAttribIndex,2,GL_FLOAT,GL_FALSE,pVBO->nNormalMapStride,(void*)pVBO->nNormalMapOffset);}
		if(pVBO->nTangentOffset!=-1){m_pLastTangentPointer=NULL;glVertexAttribPointer(m_RenderMappings.nTangentAttribIndex,3,GL_FLOAT,GL_FALSE,pVBO->nTangentStride,(void*)pVBO->nTangentOffset);}
		if(pVBO->nBitangentOffset!=-1){m_pLastBitangentPointer=NULL;glVertexAttribPointer(m_RenderMappings.nBitangentAttribIndex,3,GL_FLOAT,GL_FALSE,pVBO->nBitangentStride,(void*)pVBO->nBitangentOffset);}
		
		for(int x=0;x<MAX_OPENGL_TEXTURE_COORDS;x++)
		{
			bool bEnable=(x<pVBO->nTextures && pVBO->pTexOffsets[x]!=-1);
			if(!m_sBufferState.pbTexCoordEnabled[x] && bEnable)    {glEnableVertexAttribArray(m_RenderMappings.pTextureAttribIndex[x]);m_sBufferState.pbTexCoordEnabled[x]=true;}
			else if(m_sBufferState.pbTexCoordEnabled[x] && !bEnable){glDisableVertexAttribArray(m_RenderMappings.pTextureAttribIndex[x]);m_sBufferState.pbTexCoordEnabled[x]=false;}
			
			if(bEnable){m_pLastTexPointer[x]=NULL;glVertexAttribPointer(m_RenderMappings.pTextureAttribIndex[x],2,GL_FLOAT,GL_FALSE,pVBO->pTexStrides[x],(void*)pVBO->pTexOffsets[x]);}
		}
	}
}

void COpenGLRenderDeferred::SetVertexPointers(float *pVertex,float *pNormal,float *pColor,int nTex,float **pTex, float *pNormalMap,float *pTangent,float *pBiTangent)
{
	if(m_nCurrentVertexArrayObject!=0)
	{
		m_nCurrentVertexArrayObject=0;
		glBindVertexArray(0);
	}
	if(m_nCurrentVertexBufferObject!=0)
	{
		m_nCurrentVertexBufferObject=0;
		glBindBuffer(GL_ARRAY_BUFFER,0);
	}
	if(m_nCurrentIndexesBufferObject!=0)
	{
		m_nCurrentIndexesBufferObject=0;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	}
	

	if(!m_sBufferState.bVertexBufferEnabled && pVertex)     {glEnableVertexAttribArray(m_RenderMappings.nVertexAttribIndex);m_sBufferState.bVertexBufferEnabled=true;}
	else if(m_sBufferState.bVertexBufferEnabled && !pVertex){glDisableVertexAttribArray(m_RenderMappings.nVertexAttribIndex);m_sBufferState.bVertexBufferEnabled=false;}

	if(!m_sBufferState.bColorBufferEnabled && pColor)     {glEnableVertexAttribArray(m_RenderMappings.nColorAttribIndex);m_sBufferState.bColorBufferEnabled=true;}
	else if(m_sBufferState.bColorBufferEnabled && !pColor){glDisableVertexAttribArray(m_RenderMappings.nColorAttribIndex);m_sBufferState.bColorBufferEnabled=false;}

	if(!m_sBufferState.bNormalBufferEnabled && pNormal)     {glEnableVertexAttribArray(m_RenderMappings.nNormalAttribIndex);m_sBufferState.bNormalBufferEnabled=true;}
	else if(m_sBufferState.bNormalBufferEnabled && !pNormal){glDisableVertexAttribArray(m_RenderMappings.nNormalAttribIndex);m_sBufferState.bNormalBufferEnabled=false;}

	if(!m_sBufferState.bNormalMapBufferEnabled && pNormalMap)     {glEnableVertexAttribArray(m_RenderMappings.nNormalMapCoordAttribIndex);m_sBufferState.bNormalMapBufferEnabled=true;}
	else if(m_sBufferState.bNormalMapBufferEnabled && !pNormalMap){glDisableVertexAttribArray(m_RenderMappings.nNormalMapCoordAttribIndex);m_sBufferState.bNormalMapBufferEnabled=false;}
	
	if(!m_sBufferState.bTangentBufferEnabled && pTangent)     {glEnableVertexAttribArray(m_RenderMappings.nTangentAttribIndex);m_sBufferState.bTangentBufferEnabled=true;}
	else if(m_sBufferState.bTangentBufferEnabled && !pTangent){glDisableVertexAttribArray(m_RenderMappings.nTangentAttribIndex);m_sBufferState.bTangentBufferEnabled=false;}
	
	if(!m_sBufferState.bBitangentBufferEnabled && pBiTangent)     {glEnableVertexAttribArray(m_RenderMappings.nBitangentAttribIndex);m_sBufferState.bBitangentBufferEnabled=true;}
	else if(m_sBufferState.bBitangentBufferEnabled && !pBiTangent){glDisableVertexAttribArray(m_RenderMappings.nBitangentAttribIndex);m_sBufferState.bBitangentBufferEnabled=false;}
	
	if(pVertex && pVertex!=m_pLastVertexPointer){m_pLastVertexPointer=pVertex;glVertexAttribPointer(m_RenderMappings.nVertexAttribIndex,3,GL_FLOAT,GL_FALSE,0,pVertex);}
	if(pColor && pColor!=m_pLastColorPointer){m_pLastColorPointer=pColor;glVertexAttribPointer(m_RenderMappings.nColorAttribIndex,4,GL_FLOAT,GL_FALSE,0,pColor);}
	if(pNormal&& pNormal!=m_pLastNormalPointer){m_pLastNormalPointer=pNormal;glVertexAttribPointer(m_RenderMappings.nNormalAttribIndex,3,GL_FLOAT,GL_FALSE,0,pNormal);}
	if(pNormalMap && pNormalMap!=m_pLastNormalMapPointer){m_pLastNormalMapPointer=pNormalMap;glVertexAttribPointer(m_RenderMappings.nNormalMapCoordAttribIndex,2,GL_FLOAT,GL_FALSE,0,pNormalMap);}
	if(pTangent && pTangent!=m_pLastTangentPointer){m_pLastTangentPointer=pTangent;glVertexAttribPointer(m_RenderMappings.nTangentAttribIndex,3,GL_FLOAT,GL_FALSE,0,pTangent);}
	if(pBiTangent && pBiTangent!=m_pLastBitangentPointer){m_pLastBitangentPointer=pBiTangent;glVertexAttribPointer(m_RenderMappings.nBitangentAttribIndex,3,GL_FLOAT,GL_FALSE,0,pBiTangent);}

	for(int x=0;x<MAX_OPENGL_TEXTURE_COORDS;x++)
	{
		bool bEnable=(x<nTex && pTex[x]!=NULL);
		if(!m_sBufferState.pbTexCoordEnabled[x] && bEnable)    {glEnableVertexAttribArray(m_RenderMappings.pTextureAttribIndex[x]);m_sBufferState.pbTexCoordEnabled[x]=true;}
		else if(m_sBufferState.pbTexCoordEnabled[x] && !bEnable){glDisableVertexAttribArray(m_RenderMappings.pTextureAttribIndex[x]);m_sBufferState.pbTexCoordEnabled[x]=false;}
		
		if(bEnable && m_pLastTexPointer[x]!=pTex[x]){m_pLastTexPointer[x]=pTex[x];glVertexAttribPointer(m_RenderMappings.pTextureAttribIndex[x],2,GL_FLOAT,GL_FALSE,0,pTex[x]);}
	}
}

void COpenGLRenderDeferred::RenderAllStages(bool bRenderingShadow)
{
	RenderModelStages(bRenderingShadow,eDeferredState_ALL);
	RenderTriangleStages(bRenderingShadow);
	RenderLineStages(bRenderingShadow);
	RenderPointStages(bRenderingShadow);
}

bool COpenGLRenderDeferred::PrecompileShaderByName(const char *psName)
{
	bool bOk=true;
	
	bool bHeightFog=(strchr(psName,'G')!=NULL);
	bool bShadows=(strchr(psName,'S')!=NULL);
	bool bTextures=(strchr(psName,'T')!=NULL);
	bool bLighting=(strchr(psName,'L')!=NULL);
	bool bWater=(strchr(psName,'W')!=NULL);
	bool bNormalMap=(strchr(psName,'N')!=NULL);
	bool bSky=(strchr(psName,'Y')!=NULL);
	bool bPoints=(strchr(psName,'P')!=NULL);
	EDeferredMode eMode=eDeferredMode_Forward;
	if(strchr(psName,'2')!=NULL){eMode=eDeferredMode_Forward;}
	if(strchr(psName,'1')!=NULL){eMode=eDeferredMode_Water;}
	if(strchr(psName,'0')!=NULL){eMode=eDeferredMode_NoWater;}
	
	SDeferredShaderKey key(eMode,bHeightFog,bShadows,bTextures?1:0,bLighting,bWater,bNormalMap,bSky,bPoints);
	std::map<SDeferredShaderKey,CGenericShaderWrapper>::iterator iShader=m_mShaders.find(key);
	if(iShader==m_mShaders.end())
	{
		AddShader(key);
		iShader=m_mShaders.find(key);
		if(iShader!=m_mShaders.end())
		{
			bOk=iShader->second.m_piShader->Compile();
		}
		else
		{
			bOk=false;
		}
	}
	return bOk;
}

void COpenGLRenderDeferred::AddShader( SDeferredShaderKey &key )
{
	CMatrix identity;
	char sTemp[128];
	std::string sPreprocessor,sDescription="";
	sprintf(sTemp,"#define MAX_OBJECT_INSTANCES %d\n",MAX_OBJECT_INSTANCES);
	sPreprocessor+=sTemp;
	
	if(key.bPoints){sPreprocessor+="#define RENDERING_POINTS\n";sDescription+="P";}
	if(key.bHeightFog){sPreprocessor+="#define ENABLE_FOG\n";sDescription+="G";}
	if(key.bWater){sPreprocessor+="#define ENABLE_WATER\n";sDescription+="W";}
	if(key.bShadows)
	{
		sPreprocessor+="#define ENABLE_SHADOWS\n";
		sPreprocessor+="#define ENABLE_SOFT_SHADOWS\n";
		sDescription+="S";
	}
	if(key.bNormalMap)
	{
		sPreprocessor+="#define ENABLE_NORMAL_MAP\n";
		sDescription+="N";
	}
	if(key.bSkyShadow)
	{
		sPreprocessor+="#define ENABLE_SKY_SHADOW\n";
		sDescription+="Y";
	}
	if(key.nTextureUnits)
	{
		sPreprocessor+="#define ENABLE_TEXTURES\n";
		sprintf(sTemp,"#define TEXTURE_UNITS %d\n",key.nTextureUnits);
		sPreprocessor+=sTemp;
		sDescription+="T";
	}
	if(key.bLighting)
	{
		sPreprocessor+="#define ENABLE_LIGHTING\n";
		sDescription+="L";
	}
	CGenericShaderWrapper wrapper;
	if(wrapper.Create(m_piSystem,"Shader",""))
	{
		if(key.eMode==eDeferredMode_Water)
		{
			wrapper.m_piShader->Load("Shaders/RenderShader-Deferred-Water-DataGathering-Vertex.c","Shaders/RenderShader-Deferred-Water-DataGathering-Fragment.c",sPreprocessor);
			sDescription+="1";
		}
		else if(key.eMode==eDeferredMode_NoWater)
		{
			wrapper.m_piShader->Load("Shaders/RenderShader-Deferred-NoWater-DataGathering-Vertex.c","Shaders/RenderShader-Deferred-NoWater-DataGathering-Fragment.c",sPreprocessor);
			sDescription+="0";
		}
		else
		{
			wrapper.m_piShader->Load("Shaders/RenderShader-Deferred-Forward-Vertex.c","Shaders/RenderShader-Deferred-Forward-Fragment.c",sPreprocessor);
			sDescription+="2";
		}
		
		if(key.nTextureUnits>=1)
		{
			wrapper.m_piShader->AddUniform("Texture0",(int)0,false);
			wrapper.m_piShader->AddUniform("uTexMatrix0",identity,false);
			wrapper.m_piShader->AddAttribute("aTexCoord0",(int)m_RenderMappings.pTextureAttribIndex[0]);
		}
		if(key.nTextureUnits>=2)
		{
			wrapper.m_piShader->AddUniform("Texture1",(int)1,false);
			wrapper.m_piShader->AddUniform("uTexMatrix1",identity,false);
			wrapper.m_piShader->AddAttribute("aTexCoord1",(int)m_RenderMappings.pTextureAttribIndex[1]);
		}
		if(key.bSkyShadow&& key.eMode==eDeferredMode_Water)
		{
			wrapper.m_piShader->AddUniform("SkyShadowMap",(int)m_nSkyShadowTextureLevel,false);
		}
		if(key.bNormalMap)
		{
			wrapper.m_piShader->AddUniform("NormalMap",(int)m_nNormalMapTextureLevel,false);
			wrapper.m_piShader->AddAttribute("aNorCoord",(int)m_RenderMappings.nNormalMapCoordAttribIndex);
			wrapper.m_piShader->AddAttribute("aTangent",(int)m_RenderMappings.nTangentAttribIndex);
			wrapper.m_piShader->AddAttribute("aBitangent",(int)m_RenderMappings.nBitangentAttribIndex);
		}
		wrapper.m_piShader->AddAttribute("aVertex",(int)m_RenderMappings.nVertexAttribIndex);
		wrapper.m_piShader->AddAttribute("aNormal",(int)m_RenderMappings.nNormalAttribIndex);
		wrapper.m_piShader->AddAttribute("aVertexColor",(int)m_RenderMappings.nColorAttribIndex);
		
		wrapper.m_piShader->AddUniform("uModel",identity,false);
		wrapper.m_piShader->AddUniform("uView",m_pScene?m_pScene->camera.m_ViewMatrix:identity,false);
		wrapper.m_piShader->AddUniform("uProjection",m_pScene?m_pScene->camera.m_ProjectionMatrix:identity,false);
		wrapper.m_piShader->AddUniform("uTinting",ColorWhite,false);
		
		if(key.bShadows && key.eMode==eDeferredMode_Water)
		{
			wrapper.m_piShader->AddUniform("ShadowMap",(int)m_nShadowTextureLevel,false);
		}
		if(key.eMode==eDeferredMode_Water)
		{
			wrapper.m_piShader->AddData("oSunScene",(int)0);
			wrapper.m_piShader->AddData("oNormal",(int)1);
			wrapper.m_piShader->AddData("oDiffuse",(int)2);
		}
		else if(key.eMode==eDeferredMode_NoWater)
		{
			wrapper.m_piShader->AddData("oDiffuse",(int)0);
			wrapper.m_piShader->AddData("oNormal",(int)1);
		}
		m_mShaders[key]=wrapper;
		m_mShaderNames[key]=sDescription;
		//RTTRACE("COpenGLRenderDeferred::AddShader -> Added shader '%s'",sDescription.c_str());
	}
}
	
void COpenGLRenderDeferred::StartSelection(SGameRect rWindowRect,IGenericCamera *piCamera,double dx,double dy,double dPrecision)
{
	if(m_SelectionShader.m_piShader==NULL)
	{
		if(m_SelectionShader.Create(m_piSystem,"Shader",""))
		{
			#ifdef ANDROID
			m_SelectionShader.m_piShader->Load("Shaders/Android-SelectionShader-Vertex.c","Shaders/Android-SelectionShader-Fragment.c","");
			#else
			m_SelectionShader.m_piShader->Load("Shaders/SelectionShader-Vertex.c","Shaders/SelectionShader-Fragment.c","");
			#endif
		}
	}
	
	if(m_SelectionShader.m_piShader==NULL){return;}
	
	m_bSelecting=true;
	
	double dNear=0,dFar=0;
	piCamera->GetClippingPlanes(dNear,dFar);
	
	CMatrix mProjectionMatrix,mViewMatrix,identity;
	ComputeCameraMatrix(&mViewMatrix,piCamera->GetPosition(),piCamera->GetAngles().c[YAW],piCamera->GetAngles().c[PITCH],piCamera->GetAngles().c[ROLL]);
	ComputePerspectiveMatrix(&mProjectionMatrix,piCamera->GetViewAngle(),piCamera->GetAspectRatio(),dNear,dFar);
	
	m_SelectionShader.m_piShader->AddUniform("uProjection",mProjectionMatrix,false);
	m_SelectionShader.m_piShader->AddUniform("uView",mViewMatrix,false);
	m_SelectionShader.m_piShader->AddUniform("uModel",identity,false);
		
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	
	m_SelectionPos.x=dx-rWindowRect.x;
	m_SelectionPos.y=dy-rWindowRect.y;
	m_nSelectionPrecision=dPrecision;
	m_pShaderBeforeSelection=m_pCurrentShader;
	if(m_pShaderBeforeSelection){m_pShaderBeforeSelection->m_piShader->Deactivate();}
	m_SelectionShader.m_piShader->Activate();
	m_pCurrentShader=&m_SelectionShader;
		
	SetSelectionId(0);
}

void COpenGLRenderDeferred::SetSelectionId( unsigned int nId )
{
	if(m_SelectionShader.m_piShader)
	{
		CVector vColor;
		double dAlpha=1.0;
		int nTempId=nId+1;
		vColor.c[0]=(double)((nTempId)&0xFF)/255.0;
		vColor.c[1]=(double)((nTempId>>8)&0xFF)/255.0;
		vColor.c[2]=(double)((nTempId>>16)&0xFF)/255.0;
		m_SelectionShader.m_piShader->AddUniform("uSelectionIdColor",vColor,dAlpha,false);
	}
}

int COpenGLRenderDeferred::EndSelection()
{
	unsigned int nSelectionSize=m_nSelectionPrecision+1;
	unsigned int nPrecisionHalf=m_nSelectionPrecision/2;
	int nSelectedId=-1;
	double dSelectionDistanceSquared=nSelectionSize+1.0;
	
	unsigned char *pColorBuffer=new unsigned char [nSelectionSize*nSelectionSize*4];
	memset(pColorBuffer,0,nSelectionSize*nSelectionSize*4);
	glReadPixels(m_SelectionPos.x-nPrecisionHalf,m_SelectionPos.y-nPrecisionHalf,nSelectionSize,nSelectionSize,GL_RGBA,GL_UNSIGNED_BYTE,pColorBuffer);

	CVector vCenter(nPrecisionHalf,nPrecisionHalf,0);
	unsigned char *pColor=pColorBuffer;
	for(unsigned int y=0;y<nSelectionSize;y++)
	{
		for(unsigned int x=0;x<nSelectionSize;x++)
		{
			int nId=pColor[2];
			nId<<=8;nId+=pColor[1];
			nId<<=8;nId+=pColor[0];
			nId-=1;
			if(nId!=-1)
			{
				CVector vPos(x,y,0);
				double dDistance=vPos-vCenter;
				if(dDistance<dSelectionDistanceSquared)
				{
					nSelectedId=nId;
					dSelectionDistanceSquared=dDistance;
				}
			}
			pColor+=4;
		}
	}
	
	m_pCurrentShader=m_pShaderBeforeSelection;
	if(m_SelectionShader.m_piShader){m_SelectionShader.m_piShader->Deactivate();}
	if(m_pCurrentShader){m_pCurrentShader->m_piShader->Activate();}
	m_pShaderBeforeSelection=NULL;
	m_bSelecting=false;
	
	
	delete [] pColorBuffer;
	pColorBuffer=NULL;
	return nSelectedId;
}

void COpenGLRenderDeferred::ReloadShaders()
{
	m_pCurrentShader=NULL;
	
	std::map<SDeferredShaderKey,CGenericShaderWrapper>::iterator iShader;
	for(iShader=m_mShaders.begin();iShader!=m_mShaders.end();iShader++)
	{
		iShader->second.Destroy();
	}
	m_mShaders.clear();
	
	if(m_ShadowShader.m_piShader)
	{
		m_ShadowShader.m_piShader->Load("Shaders/ShadowBufferShader-Vertex.c","Shaders/ShadowBufferShader-Fragment.c","");
		m_ShadowShader.m_piShader->Compile();
	}
	if(m_SelectionShader.m_piShader)
	{
		m_SelectionShader.m_piShader->Load("Shaders/SelectionShader-Vertex.c","Shaders/SelectionShader-Fragment.c","");
		m_SelectionShader.m_piShader->Compile();
	}	
	if(m_DeferredShaderNoWaterSun.m_piShader)
	{
		m_DeferredShaderNoWaterSun.m_piShader->Load("Shaders/RenderShader-Deferred-NoWater-Sun-Vertex.c","Shaders/RenderShader-Deferred-NoWater-Sun-Fragment.c","");
		m_DeferredShaderNoWaterSun.m_piShader->Compile();
	}
	if(m_DeferredShaderWaterSun.m_piShader)
	{
		m_DeferredShaderWaterSun.m_piShader->Load("Shaders/RenderShader-Deferred-Water-Sun-Vertex.c","Shaders/RenderShader-Deferred-Water-Sun-Fragment.c","");
		m_DeferredShaderWaterSun.m_piShader->Compile();
	}
	if(m_DeferredShaderDynamic.m_piShader)
	{
		m_DeferredShaderDynamic.m_piShader->Load("Shaders/RenderShader-Deferred-Dynamic-Vertex.c","Shaders/RenderShader-Deferred-Dynamic-Fragment.c","");
		m_DeferredShaderDynamic.m_piShader->Compile();
	}
}


unsigned int COpenGLRenderDeferred::CreateRenderBuffer( unsigned nWidth,unsigned nHeight,unsigned int nComponent,unsigned int nFormat)
{
	if(nComponent==GL_DEPTH_COMPONENT)
	{
		unsigned int nRenderBuffer=0;
		glGenTextures(1,&nRenderBuffer);
		if(nRenderBuffer)
		{
			glBindTexture(GL_TEXTURE_2D,nRenderBuffer);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,  GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,  GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexImage2D(GL_TEXTURE_2D, 0,  GL_DEPTH_COMPONENT, nWidth,nHeight, 0, nComponent, nFormat, 0);
			glBindTexture(GL_TEXTURE_2D,0);
		}
		return nRenderBuffer;
	}
	else
	{
		unsigned int nTextureIndex=0;
		glGenTextures(1,&nTextureIndex);
		if(nTextureIndex)
		{
			glBindTexture(GL_TEXTURE_2D,nTextureIndex);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,  GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,  GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
			glTexImage2D(GL_TEXTURE_2D, 0,  nComponent, nWidth,nHeight, 0, GL_RGBA, nFormat, 0);
			glBindTexture(GL_TEXTURE_2D,0);
		}
		return nTextureIndex;
	}
}


SDeferredGBuffers COpenGLRenderDeferred::CreateGBuffers( unsigned nWidth,unsigned nHeight)
{
	unsigned int nTextureIndex=0;
	
	SDeferredGBuffers gBuffers;
	gBuffers.nWidth=nWidth;
	gBuffers.nHeight=nHeight;
	gBuffers.nDepthBuffer=CreateRenderBuffer(nWidth,nHeight,GL_DEPTH_COMPONENT,GL_FLOAT);
	gBuffers.nDiffuseBuffer=CreateRenderBuffer(nWidth,nHeight,GL_RGBA,GL_UNSIGNED_BYTE);
	gBuffers.nSunSceneBuffer=CreateRenderBuffer(nWidth,nHeight,GL_RGBA,GL_UNSIGNED_BYTE);
	gBuffers.nNormalBuffer=CreateRenderBuffer(nWidth,nHeight,GL_RGBA,GL_UNSIGNED_BYTE);
	
	bool bOk=(gBuffers.nDepthBuffer!=0 && gBuffers.nDiffuseBuffer!=0 && gBuffers.nNormalBuffer!=0 && gBuffers.nSunSceneBuffer!=0);
	if(bOk)
	{
		glGenFramebuffersEXT(1,&gBuffers.nFrameBuffer);
		bOk=(gBuffers.nFrameBuffer!=0);
		if(bOk)
		{
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,gBuffers.nFrameBuffer);
			
			
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT,GL_TEXTURE_2D,gBuffers.nSunSceneBuffer,0);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT1_EXT,GL_TEXTURE_2D,gBuffers.nNormalBuffer,0);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT2_EXT,GL_TEXTURE_2D,gBuffers.nDiffuseBuffer,0);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,GL_TEXTURE_2D, gBuffers.nDepthBuffer, 0);
			
			GLenum pDestinationDrawBuffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT};
			glDrawBuffers(3, pDestinationDrawBuffers);

			GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
			if(status == GL_FRAMEBUFFER_COMPLETE_EXT)
			{
				RTTRACE("COpenGLRenderDeferred::CreateGBuffers -> GBuffers created, %dx%d",nWidth,nHeight);
			}
			else
			{
				RTTRACE("COpenGLRenderDeferred::CreateGBuffers -> Failed to create FrameBuffer %d (0x%X)",status,status);
			}
 		}
		else
		{
			RTTRACE("COpenGLRenderDeferred::CreateGBuffers -> Failed to create FrameBuffer");
		}
	}
	else
	{
		RTTRACE("COpenGLRenderDeferred::CreateGBuffers -> Failed to create GBuffers");
		
		RTTRACE("COpenGLRenderDeferred::CreateGBuffers -> DepthBuffer %d",gBuffers.nDepthBuffer);
		RTTRACE("COpenGLRenderDeferred::CreateGBuffers -> ColorBuffer %d",gBuffers.nDiffuseBuffer);
		RTTRACE("COpenGLRenderDeferred::CreateGBuffers -> SunSceneBuffer %d",gBuffers.nSunSceneBuffer);
		RTTRACE("COpenGLRenderDeferred::CreateGBuffers -> NormalBuffer %d",gBuffers.nNormalBuffer);
	}
	
	if(!bOk)
	{
		FreeGBuffers(&gBuffers);
	}
	return gBuffers;
}

void COpenGLRenderDeferred::FreeGBuffers(SDeferredGBuffers *pGBuffers)
{
	if(pGBuffers->nFrameBuffer){glDeleteFramebuffersEXT(1,&pGBuffers->nFrameBuffer);pGBuffers->nFrameBuffer=0;}
	if(pGBuffers->nDepthBuffer){glDeleteTextures(1,&pGBuffers->nDepthBuffer);pGBuffers->nDepthBuffer=0;}
	if(pGBuffers->nDiffuseBuffer){glDeleteTextures(1,&pGBuffers->nDiffuseBuffer);pGBuffers->nDiffuseBuffer=0;}
	if(pGBuffers->nSunSceneBuffer){glDeleteTextures(1,&pGBuffers->nSunSceneBuffer);pGBuffers->nSunSceneBuffer=0;}
	if(pGBuffers->nNormalBuffer){glDeleteTextures(1,&pGBuffers->nNormalBuffer);pGBuffers->nNormalBuffer=0;}
}
