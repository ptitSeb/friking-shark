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
#include "OpenGLRenderForwardShader.h"

COpenGLRenderForwardShader::COpenGLRenderForwardShader(void)
{
	m_nLastDesiredSunShadowWidth=0;
	m_nLastDesiredSunShadowHeight=0;

	m_eShadowQuality=eShadowQuality_High;
	m_bClippingActive=false;
	m_bAnyShadowInTheScene=false;
	m_pScene=NULL;
	m_piCurrentViewport=NULL;

	m_fCurrentRealTime=0;
	
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
	m_nShadowTextureLevel=1;
	m_nSkyShadowTextureLevel=2;
	m_nNormalMapTextureLevel=3;
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

COpenGLRenderForwardShader::~COpenGLRenderForwardShader(void)
{
}

bool COpenGLRenderForwardShader::Setup(IGenericRender *piRender,IGenericViewport *piViewport,SHardwareSupport &support)
{
	bool bOk=true;
	
	REL(m_piCurrentViewport);
	m_piCurrentViewport=ADD(piViewport);
	m_sHardwareSupport=support;
	
	if(bOk && m_ShadowShader.m_piShader==NULL)
	{
		bOk=m_ShadowShader.Create(m_piSystem,"Shader","");
		if(bOk)
		{
			#ifdef ANDROID
			m_ShadowShader.m_piShader->Load("Shaders/Android-ShadowBufferShader-Vertex.c","Shaders/Android-ShadowBufferShader-Fragment.c","");
			#else
			m_ShadowShader.m_piShader->Load("Shaders/ShadowBufferShader-Vertex.c","Shaders/ShadowBufferShader-Fragment.c","");
			#endif
			
			bOk=m_ShadowShader.m_piShader->Compile();
			if(!bOk)
			{
				RTTRACE("COpenGLRenderForwardShader::Setup -> Failed to compile shadow shader");
			}
		}
		else
		{
			RTTRACE("COpenGLRenderForwardShader::Setup -> Failed to create shadow shader");
		}
	}
	
	const char *ppShadersToPrecompile[]={"P","T","GSYL","GSYTL","GWSYT","GTL","GWT","GSTL","GT","GST","PG","GWST","TL","SYT","SYL","SYTL",NULL};
	const char **ppShaderCursor=ppShadersToPrecompile;
	while(bOk && *ppShaderCursor)
	{
		const char *pName=*ppShaderCursor;
		bOk=PrecompileShaderByName(pName);
		if(!bOk){RTTRACE("COpenGLRenderForwardShader::Setup -> Failed to precompile shader: '%s'",pName);}
		ppShaderCursor++;
	}
	return bOk;
}

void COpenGLRenderForwardShader::Cleanup()
{
	for(unsigned int x=0;x<MAX_TEXTURE_LEVELS;x++)
	{
		if(m_ppiEffectiveTextureLevels[x])
		{
			m_ppiEffectiveTextureLevels[x]->UnprepareTexture(x);
			REL(m_ppiEffectiveTextureLevels[x]);
		}
		REL(m_ppiTextureLevels[x]);
	}
	REL(m_piEffectiveNormalMap);
	m_nTextureLevels=0;
	if(m_nCurrentActiveTexture!=0){glActiveTexture(GL_TEXTURE0_ARB+0);m_nCurrentActiveTexture=0;}

	float *pCleanTextures[MAX_TEXTURE_LEVELS]={0};
	SetVertexPointers(NULL,NULL,NULL,MAX_TEXTURE_LEVELS,pCleanTextures,NULL,NULL,NULL);

	SRenderState sResetState;
	SetRenderState(sResetState,eStateChange_DoNotUpdateShader);
	if(m_pCurrentShader){m_pCurrentShader->m_piShader->Deactivate();m_pCurrentShader=NULL;}
	glUseProgramObjectARB(0);

	if(m_vClearColor!=ColorBlack){m_vClearColor=ColorBlack;glClearColor(0,0,0,1);}

	m_ShadowTexture.Destroy();
	m_ShadowShader.Destroy();
	m_SelectionShader.Destroy();

	std::map<SShaderKey,CGenericShaderWrapper>::iterator iShader;
	for(iShader=m_mShaders.begin();iShader!=m_mShaders.end();iShader++)
	{
		iShader->second.Destroy();
	}
	m_mShaders.clear();
	REL(m_piCurrentViewport);

	m_nLastDesiredSunShadowHeight=0;
	m_nLastDesiredSunShadowWidth=0;
}

void COpenGLRenderForwardShader::Destroy()
{
	CSystemObjectBase::Destroy();
}

void COpenGLRenderForwardShader::SetProjectionMatrix(CMatrix &matrix)
{
	std::map<SShaderKey,CGenericShaderWrapper>::iterator iShader;
	for(iShader=m_mShaders.begin();iShader!=m_mShaders.end();iShader++)
	{
		iShader->second.m_piShader->AddUniform("uProjection",matrix,false);
	}
}

void COpenGLRenderForwardShader::SetModelViewMatrix(CMatrix &matrix)
{
	CMatrix identity;
	std::map<SShaderKey,CGenericShaderWrapper>::iterator iShader;
	for(iShader=m_mShaders.begin();iShader!=m_mShaders.end();iShader++)
	{
		iShader->second.m_piShader->AddUniform("uModel",identity,false);
		iShader->second.m_piShader->AddUniform("uView",matrix,false);
	}
}

void COpenGLRenderForwardShader::SetupLightsInShaders()
{
	CVector vSunDiffuse;
	CVector vSunSpecular;
	CVector vSunDirection;
	
	CVector pDynamicPosition[MAX_DYNAMIC_LIGHTS];
	CVector pDynamicDiffuse[MAX_DYNAMIC_LIGHTS];
	CVector pDynamicSpecular[MAX_DYNAMIC_LIGHTS];
	float   pDynamicOpacity[MAX_DYNAMIC_LIGHTS];
	float   pDynamicRange[MAX_DYNAMIC_LIGHTS];
	
	int nDynamicLights=0;
	
	std::vector<IGenericLight *>::iterator i;
	for(i=m_pScene->lighting.m_vLights.begin(); i!=m_pScene->lighting.m_vLights.end();i++)
	{
		IGenericLight *piLight=*i;
		eGenericLightType eType=piLight->GetType();
		
		if(eType==eGenericLightType_Directional)
		{
			vSunDiffuse=piLight->GetDiffuseColor();
			vSunSpecular=piLight->GetSpecularColor();
			vSunDirection=piLight->GetDirectionalDirection();
		}
		else if(eType==eGenericLightType_Omni && nDynamicLights<MAX_DYNAMIC_LIGHTS)
		{
			pDynamicPosition[nDynamicLights]=piLight->GetPosition();
			pDynamicDiffuse[nDynamicLights]=piLight->GetDiffuseColor();
			pDynamicSpecular[nDynamicLights]=piLight->GetSpecularColor();
			pDynamicOpacity[nDynamicLights]=1.0;
			pDynamicRange[nDynamicLights]=piLight->GetOmniRadius();
			nDynamicLights++;
		}
	}
	
	std::map<SShaderKey,CGenericShaderWrapper>::iterator iShader;
	for(iShader=m_mShaders.begin();iShader!=m_mShaders.end();iShader++)
	{
		if(!iShader->first.bLighting){continue;}
		CGenericShaderWrapper *pShader=&iShader->second;
		
		pShader->m_piShader->AddUniform("uWorldEyeDir",m_pScene->camera.m_vCameraForward,false);
		pShader->m_piShader->AddUniform("uAmbient",m_pScene->lighting.m_vAmbientColor,1.0,false);
		pShader->m_piShader->AddUniform("uSunDiffuse",vSunDiffuse,1.0,false);
		pShader->m_piShader->AddUniform("uSunSpecular",vSunSpecular,1.0,false);
		pShader->m_piShader->AddUniform("uSunDirection",vSunDirection,false);
		
		pShader->m_piShader->AddUniform("uDynamicLights",nDynamicLights,false);
		pShader->m_piShader->AddUniformVectors("uDynamicPosition",nDynamicLights,pDynamicPosition,false);
		pShader->m_piShader->AddUniformColors("uDynamicDiffuse",nDynamicLights,pDynamicDiffuse,pDynamicOpacity,false);
		pShader->m_piShader->AddUniformColors("uDynamicSpecular",nDynamicLights,pDynamicSpecular,pDynamicOpacity,false);
		pShader->m_piShader->AddUniformFloats("uDynamicRange",nDynamicLights,pDynamicRange,false);
	}
}

void COpenGLRenderForwardShader::PrepareSunShadows()
{
	if(m_nLastDesiredSunShadowWidth!=m_pScene->lighting.m_nDesiredSunShadowWidth || 
		m_nLastDesiredSunShadowHeight!=m_pScene->lighting.m_nDesiredSunShadowHeight)
	{
		if(m_ShadowTexture.m_piTexture){m_ShadowTexture.Destroy();}

		bool bOk=m_ShadowTexture.Create(m_piSystem,"Texture","");
		if(bOk)
		{
			bOk=m_ShadowTexture.m_piTexture->CreateDepth(m_pScene->lighting.m_nDesiredSunShadowWidth,m_pScene->lighting.m_nDesiredSunShadowHeight,m_piCurrentViewport);
			if(!bOk)
			{
				RTTRACE("COpenGLRenderForwardShader::PrepareSunShadows -> Failed to initialize shadow texture");
				m_ShadowTexture.Destroy();
			}
		}
		else
		{
			RTTRACE("COpenGLRenderForwardShader::PrepareSunShadows -> Failed to create shadow texture");
		}
		m_nLastDesiredSunShadowWidth=m_pScene->lighting.m_nDesiredSunShadowWidth;
		m_nLastDesiredSunShadowHeight=m_pScene->lighting.m_nDesiredSunShadowHeight;
	}

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
	SetRenderState(sShadowsState,eStateChange_DoNotUpdateShader);

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
	RenderModelStages(true);
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
	
	std::map<SShaderKey,CGenericShaderWrapper>::iterator iShader;
	for(iShader=m_mShaders.begin();iShader!=m_mShaders.end();iShader++)
	{
		if(iShader->first.bShadows)
		{
			iShader->second.m_piShader->AddUniform("uShadowMatrix",shadowMatrix,false);
		}
	}	
}

void COpenGLRenderForwardShader::UnprepareSunShadows()
{
	if(m_nCurrentActiveTexture!=m_nShadowTextureLevel){glActiveTexture(GL_TEXTURE0_ARB+m_nShadowTextureLevel);m_nCurrentActiveTexture=m_nShadowTextureLevel;}
	if(m_ShadowTexture.m_piTexture){m_ShadowTexture.m_piTexture->UnprepareTexture(m_nShadowTextureLevel);}
}

void COpenGLRenderForwardShader::AnalyzeStages(bool *pbShadowsPresent,bool *pbSkyPresent,bool *pbLightingPresent)
{
	bool bShadowsPresent=false;
	bool bSkyPresent=false;
	bool bLightingPresent=false;
	std::map<SPointStageKey,SPointStage>::iterator iPointStage;
	for(iPointStage=m_pScene->objects.mPointStages.begin();iPointStage!=m_pScene->objects.mPointStages.end();iPointStage++)
	{
		const SPointStageKey *pKey=&iPointStage->first;
		bShadowsPresent|=pKey->sRenderState.bActiveShadowEmission;
		bSkyPresent|=pKey->sRenderState.bActiveSkyShadow;
		bLightingPresent|=pKey->sRenderState.bActiveLighting;
		if(bShadowsPresent && bSkyPresent && bLightingPresent){break;}
	}
	std::map<SLineStageKey,SLineStage>::iterator iLineStage;
	for(iLineStage=m_pScene->objects.mLineStages.begin();iLineStage!=m_pScene->objects.mLineStages.end();iLineStage++)
	{
		const SLineStageKey *pKey=&iLineStage->first;
		bShadowsPresent|=pKey->sRenderState.bActiveShadowEmission;
		bSkyPresent|=pKey->sRenderState.bActiveSkyShadow;
		bLightingPresent|=pKey->sRenderState.bActiveLighting;
		if(bShadowsPresent && bSkyPresent && bLightingPresent){break;}
	}
	std::map<STriangleStageKey,STriangleStage>::iterator iTriangleStage;
	for(iTriangleStage=m_pScene->objects.mTriangleStages.begin();iTriangleStage!=m_pScene->objects.mTriangleStages.end();iTriangleStage++)
	{
		const STriangleStageKey *pKey=&iTriangleStage->first;
		bShadowsPresent|=pKey->sRenderState.bActiveShadowEmission;
		bSkyPresent|=pKey->sRenderState.bActiveSkyShadow;
		bLightingPresent|=pKey->sRenderState.bActiveLighting;
		if(bShadowsPresent && bSkyPresent && bLightingPresent){break;}
	}
	std::multimap<double,std::pair<const SModelStageKey*,const SModelStage*> >::iterator iModelStage;
	for(iModelStage=m_pScene->objects.mModelStages.begin();iModelStage!=m_pScene->objects.mModelStages.end();iModelStage++)
	{
		const SModelStageKey *pKey=iModelStage->second.first;
		bShadowsPresent|=pKey->sRenderState.bActiveShadowEmission;
		bSkyPresent|=pKey->sRenderState.bActiveSkyShadow;
		bLightingPresent|=pKey->sRenderState.bActiveLighting;
		if(bShadowsPresent && bSkyPresent && bLightingPresent){break;}
	}
	*pbShadowsPresent=bShadowsPresent;
	*pbSkyPresent=bSkyPresent;
	*pbLightingPresent=bLightingPresent;
}

void COpenGLRenderForwardShader::RenderScene(SSceneData &sScene)
{
	if(m_bSelecting)
	{
		m_pScene=&sScene;
		RenderAllStages(false);
		m_pScene=NULL;
		return;
	}

	RTTIMEMETER_SETGLSTEP("Render-Setup");
	if(sScene.camera.bProjectionModified){SetProjectionMatrix(sScene.camera.m_ProjectionMatrix);}
	if(sScene.camera.bViewModified){SetModelViewMatrix(sScene.camera.m_ViewMatrix);}
	if(sScene.camera.bViewportModified){glViewport(sScene.camera.m_nViewportX,sScene.camera.m_nViewportY,sScene.camera.m_nViewportW,sScene.camera.m_nViewportH);}

	m_pScene=&sScene;

	bool bShadowsPresent=false,bSkyPresent=false,bLightingPresent=false;
	AnalyzeStages(&bShadowsPresent,&bSkyPresent,&bLightingPresent);
	m_bAnyShadowInTheScene=bShadowsPresent;

	std::map<SShaderKey,CGenericShaderWrapper>::iterator iShader;
	
	if(bLightingPresent){SetupLightsInShaders();}
	
	for(iShader=m_mShaders.begin();iShader!=m_mShaders.end();iShader++)
	{
		if(iShader->first.bSkyShadow && bSkyPresent){iShader->second.m_piShader->AddUniform("SkyData",CVector(sScene.fTime*sScene.sky.m_dSkyShadowSpeed,sScene.sky.m_dSkyShadowXResolution,sScene.sky.m_dSkyShadowZResolution),sScene.sky.m_dSkyShadowOpacity,false);}
		if(iShader->first.bWater)
		{
			if(sScene.fTime!=m_fCurrentRealTime){iShader->second.m_piShader->AddUniform("CurrentRealTime",sScene.fTime,false);}
			iShader->second.m_piShader->AddUniform("WaterMappingSize",sScene.water.dSizeU,sScene.water.dSizeV,false);
			iShader->second.m_piShader->AddUniform("WaterMappingOffset",sScene.water.dOffsetU,sScene.water.dOffsetV,false);
		}
	}
	m_fCurrentRealTime=sScene.fTime;

	if(bSkyPresent)
	{
		if(m_nCurrentActiveTexture!=m_nSkyShadowTextureLevel){glActiveTexture(GL_TEXTURE0_ARB+m_nSkyShadowTextureLevel);m_nCurrentActiveTexture=m_nSkyShadowTextureLevel;}
		if(sScene.sky.m_piSkyShadow){sScene.sky.m_piSkyShadow->PrepareTexture(m_nSkyShadowTextureLevel);}
	}

	if(bShadowsPresent)
	{
		if(m_bClippingActive){m_bClippingActive=false;glDisable(GL_SCISSOR_TEST);}
		
		RTTIMEMETER_SETGLSTEP("Render-ShadowMap");
		PrepareSunShadows();
		RTTIMEMETER_ENDGLSTEP();
	}
		
	if(sScene.clipping.bEnabled && !m_bClippingActive){m_bClippingActive=true;glEnable(GL_SCISSOR_TEST);}
	else if(!sScene.clipping.bEnabled && m_bClippingActive){m_bClippingActive=false;glDisable(GL_SCISSOR_TEST);}
	if(m_rClipRect!=sScene.clipping.rRect){m_rClipRect=sScene.clipping.rRect;glScissor(m_rClipRect.x,m_rClipRect.y,m_rClipRect.w,m_rClipRect.h);}
	if(sScene.bClear || sScene.bClearDepth)
	{
		if(sScene.bClear && m_vClearColor!=sScene.vClearColor)
		{
			m_vClearColor=sScene.vClearColor;
			glClearColor(sScene.vClearColor.c[0],sScene.vClearColor.c[1],sScene.vClearColor.c[2],1.0);
		}
		unsigned int nBuffersToClear=0;
		if(sScene.bClear     ){nBuffersToClear|=GL_COLOR_BUFFER_BIT;}
		if(sScene.bClearDepth){nBuffersToClear|=GL_DEPTH_BUFFER_BIT;}
		glClear(nBuffersToClear);
	}
	
	RTTIMEMETER_SETGLSTEP("Render-AllStages-BackBuffer");                                                                                                              
	RenderAllStages(false);          
	RTTIMEMETER_ENDGLSTEP();       
	if(bShadowsPresent){UnprepareSunShadows();}
 		
	if(bSkyPresent)
	{
		if(m_nCurrentActiveTexture!=m_nSkyShadowTextureLevel){glActiveTexture(GL_TEXTURE0_ARB+m_nSkyShadowTextureLevel);m_nCurrentActiveTexture=m_nSkyShadowTextureLevel;}
		if(sScene.sky.m_piSkyShadow){sScene.sky.m_piSkyShadow->UnprepareTexture(m_nSkyShadowTextureLevel);}
	}

	m_bAnyShadowInTheScene=false;
	m_pScene=NULL;
}

void COpenGLRenderForwardShader::StartFrame()
{
}

void COpenGLRenderForwardShader::EndFrame()
{
	for(unsigned int x=0;x<MAX_TEXTURE_LEVELS;x++)
	{
		REL(m_ppiEffectiveTextureLevels[x]);
	}
}

void COpenGLRenderForwardShader::SetRenderState( const SRenderState &sNewState,EStateChangeShader eShader)
{
	m_sRenderState.bActiveShadowEmission=sNewState.bActiveShadowEmission;
	m_sRenderState.bActiveShadowReception=sNewState.bActiveShadowReception;
	m_sRenderState.bActiveWater=sNewState.bActiveWater;
	
	bool bStateChange=false;
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
		bStateChange=true;
	}
	if(m_sRenderState.bActiveLighting!=sNewState.bActiveLighting)
	{
		bStateChange=true;
	}
	if(m_sRenderState.bActiveBlending!=sNewState.bActiveBlending)
	{
		if(sNewState.bActiveBlending)
		{
			glEnable(GL_BLEND);
		}
		else
		{
			glDisable(GL_BLEND);
		}
		bStateChange=true;
	}
	if(m_sRenderState.nBlendOperator1!=sNewState.nBlendOperator1 ||
		m_sRenderState.nBlendOperator2!=sNewState.nBlendOperator2)
	{
		glBlendFunc(sNewState.nBlendOperator1,sNewState.nBlendOperator2);
		bStateChange=true;
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
		bStateChange=true;
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
		bStateChange=true;
	}
	if(m_sRenderState.nDepthFunction!=sNewState.nDepthFunction)
	{
		glDepthFunc(sNewState.nDepthFunction);
		bStateChange=true;
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
		bStateChange=true;
	}
	if(m_sRenderState.bActiveHeightFog!=sNewState.bActiveHeightFog)
	{
		bStateChange=true;
	}
	if(bStateChange)
	{
		m_sRenderState=sNewState;
	}
	if(eShader==eStateChange_UpdateShader)
	{
		SetCurrentRenderStateShader();
	}
}

void COpenGLRenderForwardShader::SetCurrentRenderStateShader()
{
	if(m_bSelecting){return;}
	
	bool bTempSkyShadow=m_pScene->sky.m_piSkyShadow && m_pScene->sky.m_piSkyShadow && m_sRenderState.bActiveSkyShadow && m_sRenderState.bActiveShadowReception;
	SShaderKey key(m_sRenderState.bActiveHeightFog,m_sRenderState.bActiveShadowReception && m_bAnyShadowInTheScene,m_sRenderState.bActiveTextures && m_nTextureLevels!=0,m_sRenderState.bActiveLighting,m_sRenderState.bActiveWater,/*m_pOptions->bEnableNormalMaps*/m_piEffectiveNormalMap!=NULL,bTempSkyShadow,m_bRenderingPoints);
	std::map<SShaderKey,CGenericShaderWrapper>::iterator iShader=m_mShaders.find(key);
	CGenericShaderWrapper *pNewShader=(iShader==m_mShaders.end())?NULL:&iShader->second;
	if(!pNewShader)
	{
		AddShader(key);
		iShader=m_mShaders.find(key);
		pNewShader=(iShader==m_mShaders.end())?NULL:&iShader->second;
		if(pNewShader==NULL)
		{
			RTTRACE("COpenGLRenderForwardShader::SetRenderState -> Failed to create on-demand shader");
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
			if(key.bHeightFog)
			{
				m_pCurrentShader->m_piShader->AddUniform("uFogColor",m_sRenderState.vHeightFogColor,false);
				m_pCurrentShader->m_piShader->AddUniform("uFogMins",m_sRenderState.vHeightFogMins,false);
				m_pCurrentShader->m_piShader->AddUniform("uFogMaxs",m_sRenderState.vHeightFogMaxs,false);
			}
		}
	}
}

void COpenGLRenderForwardShader::RenderModelStages(bool bRenderingShadow)
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
		//if(!bRenderingShadow && m_pOptions->bEnableStagedRenderingStats){m_sStagedStats.nModels+=pStage->vInstances.size();}
		
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
				/*if(m_pOptions->bEnableStagedRenderingStats)
				{
					m_sStagedStats.nTotalFaces+=nFaces;
					m_sStagedStats.nTotalVertexes+=nVertexes;
					m_sStagedStats.nBufferedFaces+=nFaces;
					m_sStagedStats.nBufferedVertexes+=nVertexes;
				}*/
			}
			float fMatrixes[16*MAX_OBJECT_INSTANCES];
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
					if(!bRenderingShadow){SetRenderState(pKey->sRenderState,eStateChange_DoNotUpdateShader);}
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
							
							m_pCurrentShader->m_piShader->AddUniform("uDisableVertexColor",(float)(pColors?0.0:1.0),true);
							m_pCurrentShader->m_piShader->AddUniform("uMaterialDiffuse",vDiffuseColor,fOpacity,true);
							if(pKey->sRenderState.bActiveLighting)
							{
								m_pCurrentShader->m_piShader->AddUniform("uMaterialSpecular",vSpecularColor,fOpacity,true);
								m_pCurrentShader->m_piShader->AddUniform("uMaterialShininess",fShininess,true);
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
				nInstances++;
				
				if(  m_sHardwareSupport.bObjectInstancing==false ||
					(m_sHardwareSupport.bObjectInstancing==true  && nInstances==MAX_OBJECT_INSTANCES))
				{
					CMatrix identity;
					if(m_pCurrentShader){m_pCurrentShader->m_piShader->AddUniformMatrixes("uModel",nInstances,fMatrixes,true);}
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

void COpenGLRenderForwardShader::SetEffectiveTexture(IGenericTexture *pTexture,int nTextureLevel)
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

void COpenGLRenderForwardShader::PrepareTexture(IGenericTexture *piTexture,unsigned int nTextureLevel)
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

void COpenGLRenderForwardShader::UnprepareTexture(unsigned int nTextureLevel)
{
	if(nTextureLevel>MAX_TEXTURE_LEVELS){return;}
	if(m_ppiTextureLevels[nTextureLevel]==NULL){return;}
	
	REL(m_ppiTextureLevels[nTextureLevel]);
	m_nTextureLevels--;
}

void COpenGLRenderForwardShader::PrepareNormalMap(IGenericTexture *piNormalMap)
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

void COpenGLRenderForwardShader::UnprepareNormalMap()
{
	if(m_piEffectiveNormalMap)
	{
		m_piEffectiveNormalMap->UnprepareTexture(m_nNormalMapTextureLevel);
		REL(m_piEffectiveNormalMap);
	}
}

void COpenGLRenderForwardShader::RenderTriangleStages(bool bRenderingShadow)
{
	// Flush Texture Particles Stages.
	std::map<STriangleStageKey,STriangleStage>::iterator iTriangleStage;
	for(iTriangleStage=m_pScene->objects.mTriangleStages.begin();iTriangleStage!=m_pScene->objects.mTriangleStages.end();iTriangleStage++)
	{
		const STriangleStageKey *pKey=&iTriangleStage->first;
		STriangleStage *pStage=&iTriangleStage->second;
		if(bRenderingShadow && !pKey->sRenderState.bActiveShadowEmission){continue;}

		IGenericTexture *piTexture=pKey->piTexture;
		if(piTexture){PrepareTexture(piTexture,0);}
		
		SetRenderState(pKey->sRenderState);

		for(unsigned int x=0;x<pStage->vBuffers.size();x++)
		{
			STriangleBuffer *pBuffer=pStage->vBuffers[x];
			float *pTexVertexBufferTemp=pBuffer->pTexBuffer;
			SetVertexPointers(pBuffer->pVertexBuffer,NULL,pBuffer->pColorBuffer,1,&pTexVertexBufferTemp);
			glDrawArrays(GL_TRIANGLES,0,pBuffer->nUsedElements*3);
			/*//RTTRACE("Rendering %d triangles",pBuffer->nUsedElements);
			if(!bRenderingShadow && m_pOptions->bEnableStagedRenderingStats)
			{
				m_sStagedStats.nParticles+=pBuffer->nUsedElements;
				m_sStagedStats.nTotalFaces+=pBuffer->nUsedElements;
				m_sStagedStats.nTotalVertexes+=pBuffer->nUsedElements*3;
				m_sStagedStats.nInmediateFaces+=pBuffer->nUsedElements;
				m_sStagedStats.nInmediateVertexes+=pBuffer->nUsedElements*3;
			}*/
		}
		if(piTexture){UnprepareTexture(0);}
	}
}

void COpenGLRenderForwardShader::RenderLineStages(bool bRenderingShadow)
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
			/*//RTTRACE("Rendering %d lines",pBuffer->nUsedElements);
			if(!bRenderingShadow && m_pOptions->bEnableStagedRenderingStats)
			{
				m_sStagedStats.nLines+=pBuffer->nUsedElements;
				m_sStagedStats.nTotalVertexes+=pBuffer->nUsedElements*2;
				m_sStagedStats.nInmediateVertexes+=pBuffer->nUsedElements*2;				
			}*/
		}
	}
	if(m_pScene->objects.mLineStages.size()){glDisableLineStipple();}
}

void COpenGLRenderForwardShader::RenderPointStages(bool bRenderingShadow)
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
			/*//RTTRACE("Rendering %d points",pBuffer->nUsedElements);
			if(!bRenderingShadow && m_pOptions->bEnableStagedRenderingStats)
			{
				m_sStagedStats.nPoints+=pBuffer->nUsedElements;
				m_sStagedStats.nTotalVertexes+=pBuffer->nUsedElements;
				m_sStagedStats.nInmediateVertexes+=pBuffer->nUsedElements;				
			}*/
		}
	}
	
	m_bRenderingPoints=false;
}

void COpenGLRenderForwardShader::SetVertexBufferObject(SVertexBufferObject *pVBO)
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

void COpenGLRenderForwardShader::SetVertexPointers(float *pVertex,float *pNormal,float *pColor,int nTex,float **pTex, float *pNormalMap,float *pTangent,float *pBiTangent)
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

void COpenGLRenderForwardShader::RenderAllStages(bool bRenderingShadow)
{
	RenderModelStages(bRenderingShadow);
	RenderTriangleStages(bRenderingShadow);
	RenderLineStages(bRenderingShadow);
	RenderPointStages(bRenderingShadow);
}

bool COpenGLRenderForwardShader::PrecompileShaderByName(const char *psName)
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
	SShaderKey key(bHeightFog,bShadows,bTextures?1:0,bLighting,bWater,bNormalMap,bSky,bPoints);
	std::map<SShaderKey,CGenericShaderWrapper>::iterator iShader=m_mShaders.find(key);
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

void COpenGLRenderForwardShader::AddShader( SShaderKey &key )
{
	CMatrix identity;
	char sTemp[128];
	std::string sPreprocessor,sDescription=":";
	sprintf(sTemp,"#define MAX_OBJECT_INSTANCES %d\n",MAX_OBJECT_INSTANCES);
	sPreprocessor+=sTemp;
	sprintf(sTemp,"#define MAX_DYNAMIC_LIGHTS %d\n",MAX_DYNAMIC_LIGHTS);
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
#ifdef ANDROID
		wrapper.m_piShader->Load("Shaders/Android-RenderShader-Vertex.c","Shaders/Android-RenderShader-Fragment.c",sPreprocessor);
#else
		wrapper.m_piShader->Load("Shaders/RenderShader-Forward-Vertex.c","Shaders/RenderShader-Forward-Fragment.c",sPreprocessor);
#endif
		
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
		if(key.bSkyShadow){wrapper.m_piShader->AddUniform("SkyShadowMap",(int)m_nSkyShadowTextureLevel,false);}
		if(key.bShadows){wrapper.m_piShader->AddUniform("ShadowMap",(int)m_nShadowTextureLevel,false);}
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
	
	
		m_mShaders[key]=wrapper;
		m_mShaderNames[key]=sDescription;
	}
}
	
void COpenGLRenderForwardShader::StartSelection(SGameRect rWindowRect,IGenericCamera *piCamera,double dx,double dy,double dPrecision)
{
	glClearStencil(0);
	
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

void COpenGLRenderForwardShader::SetSelectionId( unsigned int nId )
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

int COpenGLRenderForwardShader::EndSelection()
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

void COpenGLRenderForwardShader::ReloadShaders()
{
	m_pCurrentShader=NULL;
	
	std::map<SShaderKey,CGenericShaderWrapper>::iterator iShader;
	for(iShader=m_mShaders.begin();iShader!=m_mShaders.end();iShader++)
	{
		iShader->second.Destroy();
	}
	m_mShaders.clear();
	
	if(m_ShadowShader.m_piShader)
	{
#ifdef ANDROID
		m_ShadowShader.m_piShader->Load("Shaders/Android-ShadowBufferShader-Vertex.c","Shaders/Android-ShadowBufferShader-Fragment.c","");
#else
		m_ShadowShader.m_piShader->Load("Shaders/ShadowBufferShader-Vertex.c","Shaders/ShadowBufferShader-Fragment.c","");
#endif
		m_ShadowShader.m_piShader->Compile();
	}
	
	if(m_SelectionShader.m_piShader)
	{
#ifdef ANDROID
		m_SelectionShader.m_piShader->Load("Shaders/Android-SelectionShader-Vertex.c","Shaders/Android-SelectionShader-Fragment.c","");
#else
		m_SelectionShader.m_piShader->Load("Shaders/SelectionShader-Vertex.c","Shaders/SelectionShader-Fragment.c","");
#endif
		m_SelectionShader.m_piShader->Compile();
	}
}
