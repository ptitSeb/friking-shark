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
#include "OpenGLRenderForwardFixed.h"

#ifdef ANDROID 
#define DEFAULT_SHADOW_SIZE 512
#else
#define DEFAULT_SHADOW_SIZE 1024
#endif

COpenGLRenderForwardFixed::COpenGLRenderForwardFixed(void)
{
	m_pScene=NULL;
	m_piCurrentViewport=NULL;
	m_bClippingActive=false;
	m_bSelecting=false;
	m_nTextureLevels=0;
	m_nCurrentVertexBufferObject=0;
	m_nCurrentIndexesBufferObject=0;
	
	m_bLightingPrepared=false;
	m_nCurrentActiveTexture=0;
	
	int nAttribIndex=0;
	m_nShadowTextureLevel=1;
	m_nSkyShadowTextureLevel=2;
	
	m_pLastVertexPointer=NULL;
	m_pLastColorPointer=NULL;
	m_pLastNormalPointer=NULL;
	m_pLastNormalMapPointer=NULL;
	m_pLastTangentPointer=NULL;
	m_pLastBitangentPointer=NULL;
	m_pLastTexPointer[0]=NULL;
	m_pLastTexPointer[1]=NULL;
	
	memset(m_ppiEffectiveTextureLevels,0,sizeof(m_ppiEffectiveTextureLevels));
	memset(m_ppiTextureLevels,0,sizeof(m_ppiTextureLevels));
}

COpenGLRenderForwardFixed::~COpenGLRenderForwardFixed(void)
{
}

bool COpenGLRenderForwardFixed::Setup(IGenericRender *piRender,IGenericViewport *piViewport,SHardwareSupport &support)
{
	REL(m_piCurrentViewport);
	m_piCurrentViewport=ADD(piViewport);
	m_sHardwareSupport=support;
	return true;
}

void COpenGLRenderForwardFixed::Destroy()
{
	m_ShadowTexture.Destroy();
	REL(m_piCurrentViewport);
	
	CSystemObjectBase::Destroy();
}

void COpenGLRenderForwardFixed::PrepareLighting()
{
	if(m_bLightingPrepared){return;}
	
	const GLfloat pfAmbienColor[]  = {(float)m_pScene->lighting.m_vAmbientColor.c[0], (float)m_pScene->lighting.m_vAmbientColor.c[1], (float)m_pScene->lighting.m_vAmbientColor.c[2], 1.0};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,pfAmbienColor);
	glEnable(GL_LIGHTING);
	
	std::vector<IGenericLight *>::iterator i;
	int nlIndex=GL_LIGHT0;
	for(i=m_pScene->lighting.m_vLights.begin(); nlIndex<(GL_LIGHT0+MAX_DYNAMIC_LIGHTS) && i!=m_pScene->lighting.m_vLights.end();i++)
	{
		IGenericLight *piLight=*i;
		eGenericLightType eType=piLight->GetType();
		glDisable(nlIndex);
		
		if(eType==eGenericLightType_Omni)
		{
			float dRadius=(float)piLight->GetOmniRadius();
			glLightf(nlIndex,GL_CONSTANT_ATTENUATION,0);
			glLightf(nlIndex,GL_LINEAR_ATTENUATION,(float)(1.0/(dRadius)));
			glLightf(nlIndex,GL_QUADRATIC_ATTENUATION,(float)(1.0/(dRadius*dRadius)));
		}
		else if (eType==eGenericLightType_Spot)
		{
			double dExponent=0,dAttenuationConstant=0,dAttenuationLinear=0,dAttenuationQuadratic=0;
			CVector dir=piLight->GetSpotDirection();
			float vDir[3]={0};
			vDir[0]=(float)dir.c[0];
			vDir[1]=(float)dir.c[1];
			vDir[2]=(float)dir.c[2];
			piLight->GetSpotParameters(&dExponent,&dAttenuationConstant,&dAttenuationLinear,&dAttenuationQuadratic);
			
			glLightf(nlIndex,GL_CONSTANT_ATTENUATION,(float)dAttenuationConstant);
			glLightf(nlIndex,GL_LINEAR_ATTENUATION,(float)dAttenuationLinear);
			glLightf(nlIndex,GL_QUADRATIC_ATTENUATION,(float)dAttenuationQuadratic);
			
			glLightfv(nlIndex,GL_SPOT_DIRECTION	,vDir);
			glLightf(nlIndex,GL_SPOT_CUTOFF		,(float)piLight->GetSpotAngle());
			glLightf(nlIndex,GL_SPOT_EXPONENT	,(float)dExponent);
		}
		
		CVector ambient=piLight->GetAmbientColor();
		CVector diffuse=piLight->GetDiffuseColor();
		CVector specular=piLight->GetSpecularColor();
		CVector position=(eType==eGenericLightType_Directional)?piLight->GetDirectionalDirection():piLight->GetPosition();
		float vAmbientColor[4]={0},vDiffuseColor[4]={0},vSpecularColor[4]={0},vPos[4]={0};
		vAmbientColor[0]=(float)ambient.c[0];
		vAmbientColor[1]=(float)ambient.c[1];
		vAmbientColor[2]=(float)ambient.c[2];
		vAmbientColor[3]=1.0;
		vDiffuseColor[0]=(float)diffuse.c[0];
		vDiffuseColor[1]=(float)diffuse.c[1];
		vDiffuseColor[2]=(float)diffuse.c[2];
		vDiffuseColor[3]=1.0;
		vSpecularColor[0]=(float)specular.c[0];
		vSpecularColor[1]=(float)specular.c[1];
		vSpecularColor[2]=(float)specular.c[2];
		vSpecularColor[3]=1.0;
		vPos[0]=(float)position.c[0];
		vPos[1]=(float)position.c[1];
		vPos[2]=(float)position.c[2];
		vPos[3]=(eType==eGenericLightType_Directional)?(float)0.0:(float)1.0;
		
		glLightfv(nlIndex,GL_POSITION,vPos);
		glLightfv(nlIndex,GL_AMBIENT,vAmbientColor);
		glLightfv(nlIndex,GL_DIFFUSE ,vDiffuseColor);
		glLightfv(nlIndex,GL_SPECULAR,vSpecularColor);
		glEnable(nlIndex);
		nlIndex++;
		m_nActiveLights++;
	}
	m_bLightingPrepared=true;
}

void COpenGLRenderForwardFixed::UnPrepareLighting()
{
	if(!m_bLightingPrepared){return;}
	std::set<IGenericLight *>::iterator i;
	for(int nlIndex=GL_LIGHT0;nlIndex<(GL_LIGHT0+MAX_DYNAMIC_LIGHTS) && nlIndex<(GL_LIGHT0+(int)m_nActiveLights);nlIndex++)
	{
		float vColor[4]={0};
		glLightfv(nlIndex,GL_AMBIENT,vColor);
		glLightfv(nlIndex,GL_DIFFUSE ,vColor);
		glLightfv(nlIndex,GL_SPECULAR,vColor);
		glDisable(nlIndex);
	}
	m_nActiveLights=0;
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	m_bLightingPrepared=false;
}

#ifdef ANDROID_GLES1
void COpenGLRenderForwardFixed::PrepareSunShadows(){}
void COpenGLRenderForwardFixed::UnprepareSunShadows(){}
#else
void COpenGLRenderForwardFixed::PrepareSunShadows()
{
	if(m_piCurrentViewport==NULL){return;}
	if(m_ShadowTexture.m_piTexture==NULL)
	{
		if(m_ShadowTexture.Create(m_piSystem,"Texture",""))
		{
			if(!m_ShadowTexture.m_piTexture->CreateDepth(DEFAULT_SHADOW_SIZE,DEFAULT_SHADOW_SIZE,m_piCurrentViewport))
			{
				m_ShadowTexture.Destroy();
			}
		}
	}
	if(m_ShadowTexture.m_piTexture==NULL){return;}
		
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
	SetRenderState(sShadowsState);
	
	m_ShadowTexture.m_piTexture->StartRenderingToTexture();
	
	unsigned dwWidth=0,dwHeight=0;
	m_ShadowTexture.m_piTexture->GetSize(&dwWidth,&dwHeight);
	
	CMatrix lightProjectionMatrix;
	CMatrix lightModelViewMatrix;
	double dAspect=(double)dwWidth/(double)dwHeight;
	ComputeCameraMatrix(&lightModelViewMatrix,m_pScene->lighting.m_vSunShadowPos,m_pScene->lighting.m_vSunShadowAngles.c[YAW],m_pScene->lighting.m_vSunShadowAngles.c[PITCH],m_pScene->lighting.m_vSunShadowAngles.c[ROLL]);
	ComputePerspectiveMatrix(&lightProjectionMatrix,m_pScene->lighting.m_dSunShadowViewAngle,dAspect,m_pScene->lighting.m_dSunShadowNearPlane,m_pScene->lighting.m_dSunShadowFarPlane);
	
	glViewport(0, 0, dwWidth, dwHeight);
	
	float fMatrix[16];
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	ToOpenGLMatrix(&lightProjectionMatrix,fMatrix);
	glLoadMatrixf(fMatrix);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRotatef((0-m_pScene->lighting.m_vSunShadowAngles.c[ROLL])	,AxisPosZ.c[0],AxisPosZ.c[1],AxisPosZ.c[2]);
	glRotatef((0-m_pScene->lighting.m_vSunShadowAngles.c[PITCH]),AxisPosX.c[0],AxisPosX.c[1],AxisPosX.c[2]);
	glRotatef((0-(m_pScene->lighting.m_vSunShadowAngles.c[YAW]-90))	,AxisPosY.c[0],AxisPosY.c[1],AxisPosY.c[2]);
	glTranslatef(-m_pScene->lighting.m_vSunShadowPos.c[0],-m_pScene->lighting.m_vSunShadowPos.c[1],-m_pScene->lighting.m_vSunShadowPos.c[2]);
	
	glClear(GL_DEPTH_BUFFER_BIT);
	
	//RTTRACE("Starting rendering shadows");
	RenderModelStages(true);
	
	//RTTRACE("Finished rendering shadows");
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
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
	
	if(m_nCurrentActiveTexture!=1){glActiveTexture(GL_TEXTURE0_ARB+1);m_nCurrentActiveTexture=1;}
	m_ShadowTexture.m_piTexture->PrepareTexture(1);
	//Set up texture coordinate generation.
	
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGendv(GL_S, GL_EYE_PLANE,shadowMatrix.e[0]); 
	glEnable(GL_TEXTURE_GEN_S);
	
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGendv(GL_T, GL_EYE_PLANE,shadowMatrix.e[1]);
	glEnable(GL_TEXTURE_GEN_T);
	
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGendv(GL_R, GL_EYE_PLANE,shadowMatrix.e[2]);
	glEnable(GL_TEXTURE_GEN_R);
	
	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGendv(GL_Q, GL_EYE_PLANE,shadowMatrix.e[3]);
	glEnable(GL_TEXTURE_GEN_Q);
	
	//Enable shadow comparison
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY);
	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE0);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
	
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_PRIMARY_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
	
	if(m_sHardwareSupport.nMaxTextureUnits>2)
	{
		const GLfloat pfAmbienColor[]  = {(float)(m_pScene->lighting.m_vAmbientColor.c[0]*2.0), (float)(m_pScene->lighting.m_vAmbientColor.c[1]*2.0), (float)(m_pScene->lighting.m_vAmbientColor.c[2]*2.0),1.0};
		if(m_nCurrentActiveTexture!=2){glActiveTexture(GL_TEXTURE0_ARB+2);m_nCurrentActiveTexture=2;}
		m_ShadowTexture.m_piTexture->PrepareTexture(2);
		//Set up texture coordinate generation.
		
		glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, pfAmbienColor);
		
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE_EXT);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE0);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PREVIOUS);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_CONSTANT);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_COLOR);
		
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_PRIMARY_COLOR);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
	}
	
	
	if(m_sHardwareSupport.nMaxTextureUnits>3)
	{
		if(m_nCurrentActiveTexture!=3){glActiveTexture(GL_TEXTURE0_ARB+3);m_nCurrentActiveTexture=3;}
		m_ShadowTexture.m_piTexture->PrepareTexture(3);
		
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PRIMARY_COLOR);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
		
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_PRIMARY_COLOR);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
	}
}

void COpenGLRenderForwardFixed::UnprepareSunShadows()
{
	if(m_nCurrentActiveTexture!=1){glActiveTexture(GL_TEXTURE0_ARB+1);m_nCurrentActiveTexture=1;}
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);
	
	if(m_sHardwareSupport.nMaxTextureUnits>2)
	{
		if(m_nCurrentActiveTexture!=2){glActiveTexture(GL_TEXTURE0_ARB+2);m_nCurrentActiveTexture=2;}
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);
	}
	if(m_sHardwareSupport.nMaxTextureUnits>3)
	{
		if(m_nCurrentActiveTexture!=3){glActiveTexture(GL_TEXTURE0_ARB+3);m_nCurrentActiveTexture=3;}
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);
	}
}
#endif

void COpenGLRenderForwardFixed::AnalyzeStages(bool *pbShadowsPresent,bool *pbLightingPresent)
{
	bool bShadowsPresent=false;
	bool bLightingPresent=false;
	std::map<SPointStageKey,SPointStage>::iterator iPointStage;
	for(iPointStage=m_pScene->objects.mPointStages.begin();iPointStage!=m_pScene->objects.mPointStages.end();iPointStage++)
	{
		const SPointStageKey *pKey=&iPointStage->first;
		bShadowsPresent|=pKey->sRenderState.bActiveShadowEmission;
		bLightingPresent|=pKey->sRenderState.bActiveLighting;
		if(bShadowsPresent && bLightingPresent){break;}
	}
	std::map<SLineStageKey,SLineStage>::iterator iLineStage;
	for(iLineStage=m_pScene->objects.mLineStages.begin();iLineStage!=m_pScene->objects.mLineStages.end();iLineStage++)
	{
		const SLineStageKey *pKey=&iLineStage->first;
		bShadowsPresent|=pKey->sRenderState.bActiveShadowEmission;
		bLightingPresent|=pKey->sRenderState.bActiveLighting;
		if(bShadowsPresent && bLightingPresent){break;}
	}
	std::map<STriangleStageKey,STriangleStage>::iterator iTriangleStage;
	for(iTriangleStage=m_pScene->objects.mTriangleStages.begin();iTriangleStage!=m_pScene->objects.mTriangleStages.end();iTriangleStage++)
	{
		const STriangleStageKey *pKey=&iTriangleStage->first;
		bShadowsPresent|=pKey->sRenderState.bActiveShadowEmission;
		bLightingPresent|=pKey->sRenderState.bActiveLighting;
		if(bShadowsPresent && bLightingPresent){break;}
	}
	std::multimap<double,std::pair<const SModelStageKey*,const SModelStage*> >::iterator iModelStage;
	for(iModelStage=m_pScene->objects.mModelStages.begin();iModelStage!=m_pScene->objects.mModelStages.end();iModelStage++)
	{
		const SModelStageKey *pKey=iModelStage->second.first;
		bShadowsPresent|=pKey->sRenderState.bActiveShadowEmission;
		bLightingPresent|=pKey->sRenderState.bActiveLighting;
		if(bShadowsPresent && bLightingPresent){break;}
	}
	*pbShadowsPresent=bShadowsPresent;
	*pbLightingPresent=bLightingPresent;
}

void COpenGLRenderForwardFixed::RenderScene(SSceneData &sScene)
{
	if(sScene.camera.bProjectionModified)
	{
		float fMatrix[16];
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		ToOpenGLMatrix(&sScene.camera.m_ProjectionMatrix,fMatrix);
		glLoadMatrixf(fMatrix);	
		glMatrixMode(GL_MODELVIEW);
	}

	if(sScene.camera.bViewModified)
	{
		glLoadIdentity();
		glRotatef((0-sScene.camera.m_vCameraAngles.c[ROLL])	,AxisPosZ.c[0],AxisPosZ.c[1],AxisPosZ.c[2]);
		glRotatef((0-sScene.camera.m_vCameraAngles.c[PITCH]),AxisPosX.c[0],AxisPosX.c[1],AxisPosX.c[2]);
		glRotatef((0-(sScene.camera.m_vCameraAngles.c[YAW]-90))	,AxisPosY.c[0],AxisPosY.c[1],AxisPosY.c[2]);
		glTranslatef(-sScene.camera.m_vCameraPos.c[0],-sScene.camera.m_vCameraPos.c[1],-sScene.camera.m_vCameraPos.c[2]);
	}
		
	if(sScene.camera.bViewportModified)
	{
		glViewport(sScene.camera.m_nViewportX,sScene.camera.m_nViewportY,sScene.camera.m_nViewportW,sScene.camera.m_nViewportH);
	}
	
	if(sScene.clipping.bEnabled && !m_bClippingActive){m_bClippingActive=true;glEnable(GL_SCISSOR_TEST);}
	else if(!sScene.clipping.bEnabled && m_bClippingActive){m_bClippingActive=false;glDisable(GL_SCISSOR_TEST);}
	if(m_rClipRect!=sScene.clipping.rRect){m_rClipRect=sScene.clipping.rRect;glScissor(m_rClipRect.x,m_rClipRect.y,m_rClipRect.w,m_rClipRect.h);}
	if(sScene.bClear)
	{
		if(m_vClearColor!=sScene.vClearColor)
		{
			m_vClearColor=sScene.vClearColor;
			glClearColor(sScene.vClearColor.c[0],sScene.vClearColor.c[1],sScene.vClearColor.c[2],1.0);
		}
		
		SGameRect rClearRect(sScene.camera.m_nViewportX,sScene.camera.m_nViewportY,sScene.camera.m_nViewportW,sScene.camera.m_nViewportH);
		if(sScene.clipping.bEnabled){rClearRect.ClipToRect(&m_rClipRect);}
		glScissor(rClearRect.x,rClearRect.y,rClearRect.w,rClearRect.h);
		if(!m_bClippingActive){glEnable(GL_SCISSOR_TEST);}
		glClear(GL_COLOR_BUFFER_BIT);
		if(!m_bClippingActive){glDisable(GL_SCISSOR_TEST);}
		glScissor(m_rClipRect.x,m_rClipRect.h,m_rClipRect.w,m_rClipRect.h);
	}
	
	
	m_pScene=&sScene;
	
	bool bShadowsPresent=false,bLightingPresent=false;
	AnalyzeStages(&bShadowsPresent,&bLightingPresent);
	
	if(!m_bSelecting && bShadowsPresent){PrepareSunShadows();}
	RenderAllStages(false);
	if(!m_bSelecting && bShadowsPresent){UnprepareSunShadows();}
	
	m_pScene=NULL;
}

void COpenGLRenderForwardFixed::StartFrame()
{
}

void COpenGLRenderForwardFixed::EndFrame()
{	
	for(unsigned int x=0;x<MAX_TEXTURE_LEVELS;x++)
	{
		REL(m_ppiEffectiveTextureLevels[x]);
	}
}

void COpenGLRenderForwardFixed::SetRenderState( const SRenderState &sNewState)
{
	m_sRenderState.bActiveShadowEmission=sNewState.bActiveShadowEmission;
	m_sRenderState.bActiveWater=sNewState.bActiveWater;
	m_sRenderState.eShadingModel=sNewState.eShadingModel;
	
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
		if(sNewState.bActiveLighting)
		{
			PrepareLighting();
		}
		else
		{
			UnPrepareLighting();
		}
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
	if(m_sRenderState.bActiveShadowReception!=sNewState.bActiveShadowReception)
	{
#ifndef ANDROID_GLES1
		if(sNewState.bActiveShadowReception)
		{
			for(int x=1;x<4;x++)
			{
				if(m_nCurrentActiveTexture!=x){glActiveTexture(GL_TEXTURE0_ARB+x);m_nCurrentActiveTexture=x;}
				glEnable(GL_TEXTURE_2D);
			}
		}
		else
		{
			for(int x=1;x<4;x++)
			{
				if(m_nCurrentActiveTexture!=x){glActiveTexture(GL_TEXTURE0_ARB+x);m_nCurrentActiveTexture=x;}
				glDisable(GL_TEXTURE_2D);
			}
		}
#endif
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
		if(sNewState.bActiveHeightFog)
		{
			if(m_pScene->camera.m_vCameraForward==AxisNegY)
			{
				glEnable(GL_FOG);
				glFogf(GL_FOG_START,m_pScene->camera.m_vCameraPos.c[1]-sNewState.vHeightFogMaxs.c[1]);
				glFogf(GL_FOG_END,m_pScene->camera.m_vCameraPos.c[1]-sNewState.vHeightFogMins.c[1]);
			}
			float vHeightFogColor[3]={(float)sNewState.vHeightFogColor.c[0],(float)sNewState.vHeightFogColor.c[1],(float)sNewState.vHeightFogColor.c[2]};
			glFogfv(GL_FOG_COLOR,vHeightFogColor);
			glFogf(GL_FOG_MODE,GL_LINEAR);
		}
		else
		{
			glDisable(GL_FOG);
		}
		
		bStateChange=true;
	}
	if(bStateChange)
	{
		m_sRenderState=sNewState;
	}   
}

void COpenGLRenderForwardFixed::RenderModelStages(bool bRenderingShadow)
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
					if(!bRenderingShadow){SetRenderState(pKey->sRenderState);}
					pStage->piGLModel->PrepareRenderBuffer(this,pKey->nAnimation,pKey->nFrame,nBuffer,bRenderingShadow,&m_RenderMappings);
					if(!bRenderingShadow)
					{
						CVector vAmbientColor,vDiffuseColor,vSpecularColor;
						float fShininess,fOpacity;
						float *pColors=NULL;
						pStage->piModel->GetRenderBufferMaterial(pKey->nAnimation,pKey->nFrame,nBuffer,&vAmbientColor,&vDiffuseColor,&vSpecularColor, &fShininess, &fOpacity);
						pStage->piModel->GetRenderBufferColors(pKey->nAnimation,pKey->nFrame,nBuffer,&pColors);
						
						if(pColors)
						{
							glEnable(GL_COLOR_MATERIAL);
							glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
						}
						else
						{
							GLfloat  pAmbient[]={(float)vAmbientColor.c[0],(float)vAmbientColor.c[1],(float)vAmbientColor.c[2],fOpacity};
							GLfloat  pDiffuse[]={(float)vDiffuseColor.c[0],(float)vDiffuseColor.c[1],(float)vDiffuseColor.c[2],fOpacity};
							glMaterialfv(GL_FRONT,GL_AMBIENT,pAmbient);
							glMaterialfv(GL_FRONT,GL_DIFFUSE,pDiffuse);
						}
						GLfloat  pSpecular[]={(float)vSpecularColor.c[0],(float)vSpecularColor.c[1],(float)vSpecularColor.c[2],fOpacity};
						glMaterialfv(GL_FRONT,GL_SPECULAR,pSpecular);
						glMaterialf(GL_FRONT,GL_SHININESS,fShininess);
						
						unsigned long nLevels=0;
						pStage->piModel->GetRenderBufferTextureLevels(pKey->nAnimation,pKey->nFrame,nBuffer,&nLevels);
						for(unsigned long x=0;x<nLevels && x<MAX_TEXTURE_LEVELS;x++)
						{
							bool bIdentity=false;
							pStage->piModel->GetRenderBufferTextureMatrix(pKey->nAnimation,pKey->nFrame,nBuffer,x,NULL,&bIdentity);
							if(!bIdentity)
							{
								CMatrix matrix;
								pStage->piModel->GetRenderBufferTextureMatrix(pKey->nAnimation,pKey->nFrame,nBuffer,x,&matrix,&bIdentity);

								float pfMatrix[16];
								ToOpenGLMatrix(&matrix,pfMatrix);
								if(m_nCurrentActiveTexture!=x){glActiveTexture(GL_TEXTURE0_ARB+x);m_nCurrentActiveTexture=x;}
								glMatrixMode(GL_TEXTURE);
								glLoadMatrixf(pfMatrix);
								glMatrixMode(GL_MODELVIEW);
								
								pMatrixesToRestore[nMatrixesToRestore]=x;
								nMatrixesToRestore++;
							}
						}
					}
				}
				
				glPushMatrix();
				glTranslatef(vPos.c[0],vPos.c[1],vPos.c[2]);
				if(vAngles.c[YAW]){glRotatef(vAngles.c[YAW]	,0,1,0);}
				if(vAngles.c[PITCH]){glRotatef(vAngles.c[PITCH]	,0,0,1);}
				if(vAngles.c[ROLL]){glRotatef(vAngles.c[ROLL]	,1,0,0);}

				pStage->piGLModel->CallRenderBuffer(this,pKey->nAnimation,pKey->nFrame,nBuffer,1);
				glPopMatrix();
			}
			if(bRenderBufferPrepared)
			{
				for(unsigned int x=0;x<nMatrixesToRestore;x++)
				{
					CMatrix identity;
					if(m_nCurrentActiveTexture!=x){glActiveTexture(GL_TEXTURE0_ARB+x);m_nCurrentActiveTexture=x;}
					glMatrixMode(GL_TEXTURE);
					glLoadIdentity();
					glMatrixMode(GL_MODELVIEW);
				}
				nMatrixesToRestore=0;
				pStage->piGLModel->UnPrepareRenderBuffer(this,pKey->nAnimation,pKey->nFrame,nBuffer,bRenderingShadow,&m_RenderMappings);
			}
		}
	}
}

void COpenGLRenderForwardFixed::SetEffectiveTexture(IGenericTexture *pTexture,int nTextureLevel)
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
	
	glEnable(GL_TEXTURE_2D);
}

void COpenGLRenderForwardFixed::PrepareTexture(IGenericTexture *piTexture,unsigned int nTextureLevel)
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

void COpenGLRenderForwardFixed::UnprepareTexture(unsigned int nTextureLevel)
{
	if(nTextureLevel>MAX_TEXTURE_LEVELS){return;}
	if(m_ppiTextureLevels[nTextureLevel]==NULL){return;}
	
	REL(m_ppiTextureLevels[nTextureLevel]);
	m_nTextureLevels--;
	
	if(m_nCurrentActiveTexture!=nTextureLevel){glActiveTexture(GL_TEXTURE0_ARB+nTextureLevel);m_nCurrentActiveTexture=nTextureLevel;}
	glDisable(GL_TEXTURE_2D);
	REL(m_ppiEffectiveTextureLevels[nTextureLevel]);
}

void COpenGLRenderForwardFixed::PrepareNormalMap(IGenericTexture *piNormalMap)
{
}

void COpenGLRenderForwardFixed::UnprepareNormalMap()
{
}

void COpenGLRenderForwardFixed::RenderTriangleStages(bool bRenderingShadow)
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
		}
		if(piTexture){UnprepareTexture(0);}
	}
}

void COpenGLRenderForwardFixed::RenderLineStages(bool bRenderingShadow)
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

void COpenGLRenderForwardFixed::RenderPointStages(bool bRenderingShadow)
{
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
}

void COpenGLRenderForwardFixed::SetVertexBufferObject(SVertexBufferObject *pVBO)
{
	if(pVBO==NULL)
	{
		return;
	}
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
	
	if(!m_sBufferState.bVertexBufferEnabled && pVBO->nVertexOffset!=-1)     {glEnableClientState(GL_VERTEX_ARRAY);m_sBufferState.bVertexBufferEnabled=true;}
	else if(m_sBufferState.bVertexBufferEnabled && pVBO->nVertexOffset==-1){glDisableClientState(GL_VERTEX_ARRAY);m_sBufferState.bVertexBufferEnabled=false;}
	
	if(!m_sBufferState.bColorBufferEnabled && pVBO->nColorOffset!=-1)     {glEnableClientState(GL_COLOR_ARRAY);m_sBufferState.bColorBufferEnabled=true;}
	else if(m_sBufferState.bColorBufferEnabled && pVBO->nColorOffset==-1){glDisableClientState(GL_COLOR_ARRAY);m_sBufferState.bColorBufferEnabled=false;}
	
	if(!m_sBufferState.bNormalBufferEnabled && pVBO->nNormalOffset!=-1)     {glEnableClientState(GL_NORMAL_ARRAY);m_sBufferState.bNormalBufferEnabled=true;}
	else if(m_sBufferState.bNormalBufferEnabled && pVBO->nNormalOffset==-1){glDisableClientState(GL_NORMAL_ARRAY);m_sBufferState.bNormalBufferEnabled=false;}
	
	if(pVBO->nVertexOffset!=-1){glVertexPointer(3,GL_FLOAT,pVBO->nVertexStride,(void*)pVBO->nVertexOffset);}
	if(pVBO->nColorOffset!=-1){glColorPointer(4,GL_FLOAT,pVBO->nColorStride,(void*)pVBO->nColorOffset);}
	if(pVBO->nNormalOffset!=-1){glNormalPointer(GL_FLOAT,pVBO->nNormalStride,(void*)pVBO->nNormalOffset);}
	
	for(int x=0;x<MAX_OPENGL_TEXTURE_COORDS;x++)
	{
		bool bEnable=(x<pVBO->nTextures && pVBO->pTexOffsets[x]!=-1);
		if(bEnable!=m_sBufferState.pbTexCoordEnabled[x])        {glClientActiveTextureARB(GL_TEXTURE0_ARB+x);}
		if(!m_sBufferState.pbTexCoordEnabled[x] && bEnable)     {glEnableClientState(GL_TEXTURE_COORD_ARRAY);m_sBufferState.pbTexCoordEnabled[x]=true;}
		else if(m_sBufferState.pbTexCoordEnabled[x] && !bEnable){glDisableClientState(GL_TEXTURE_COORD_ARRAY);m_sBufferState.pbTexCoordEnabled[x]=false;}
		if(bEnable){glTexCoordPointer(2,GL_FLOAT,pVBO->pTexStrides[x],(void*)pVBO->pTexOffsets[x]);}
	}
}

void COpenGLRenderForwardFixed::SetVertexPointers(float *pVertex,float *pNormal,float *pColor,int nTex,float **pTex, float *pNormalMap,float *pTangent,float *pBiTangent)
{
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
	if(!m_sBufferState.bVertexBufferEnabled && pVertex)     {glEnableClientState(GL_VERTEX_ARRAY);m_sBufferState.bVertexBufferEnabled=true;}
	else if(m_sBufferState.bVertexBufferEnabled && !pVertex){glDisableClientState(GL_VERTEX_ARRAY);m_sBufferState.bVertexBufferEnabled=false;}

	if(!m_sBufferState.bColorBufferEnabled && pColor)     {glEnableClientState(GL_COLOR_ARRAY);m_sBufferState.bColorBufferEnabled=true;}
	else if(m_sBufferState.bColorBufferEnabled && !pColor){glDisableClientState(GL_COLOR_ARRAY);m_sBufferState.bColorBufferEnabled=false;}

	if(!m_sBufferState.bNormalBufferEnabled && pNormal)     {glEnableClientState(GL_NORMAL_ARRAY);m_sBufferState.bNormalBufferEnabled=true;}
	else if(m_sBufferState.bNormalBufferEnabled && !pNormal){glDisableClientState(GL_NORMAL_ARRAY);m_sBufferState.bNormalBufferEnabled=false;}

	if(pVertex){glVertexPointer(3,GL_FLOAT,0,pVertex);}
	if(pColor){glColorPointer(4,GL_FLOAT,0,pColor);}
	if(pNormal){glNormalPointer(GL_FLOAT,0,pNormal);}
	for(int x=0;x<nTex;x++)
	{
		bool bEnable=(x<nTex && pTex[x]!=NULL);
		
		if(bEnable!=m_sBufferState.pbTexCoordEnabled[x])        {glClientActiveTextureARB(GL_TEXTURE0_ARB+x);}
		if(!m_sBufferState.pbTexCoordEnabled[x] && bEnable)     {glEnableClientState(GL_TEXTURE_COORD_ARRAY);m_sBufferState.pbTexCoordEnabled[x]=true;}
		else if(m_sBufferState.pbTexCoordEnabled[x] && !bEnable){glDisableClientState(GL_TEXTURE_COORD_ARRAY);m_sBufferState.pbTexCoordEnabled[x]=false;}
		if(bEnable){glTexCoordPointer(2,GL_FLOAT,0,pTex[x]);}
	}
}

void COpenGLRenderForwardFixed::RenderAllStages(bool bRenderingShadow)
{
	RenderModelStages(bRenderingShadow);
	RenderTriangleStages(bRenderingShadow);
	RenderLineStages(bRenderingShadow);
	RenderPointStages(bRenderingShadow);
}

void COpenGLRenderForwardFixed::ReloadShaders()
{
}

#ifdef ANDROID_GLES1
void COpenGLRenderForwardFixed::StartSelection(SGameRect rWindowRect,IGenericCamera *piCamera,double dx,double dy,double dPrecision){}
void COpenGLRenderForwardFixed::SetSelectionId( unsigned int nId ){}
int COpenGLRenderForwardFixed::EndSelection(){return -1;}
#else
void COpenGLRenderForwardFixed::StartSelection(SGameRect rWindowRect,IGenericCamera *piCamera,double dx,double dy,double dPrecision)
{
	m_bSelecting=true;
	
	GLint  viewport[4]={0};
	double dNearPlane=0,dFarPlane=0;
	double dRoll=piCamera->GetAngles().c[ROLL];
	double dPitch=piCamera->GetAngles().c[PITCH];
	double dYaw=piCamera->GetAngles().c[YAW];
	CVector vPosition=piCamera->GetPosition();
	piCamera->GetClippingPlanes(dNearPlane,dFarPlane);
	
	glSelectBuffer(SELECTION_BUFFER_SIZE,m_pSelectionBuffer);
	glRenderMode(GL_SELECT);
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glGetIntegerv(GL_VIEWPORT,viewport);
	glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);
	gluPickMatrix(rWindowRect.x+dx,rWindowRect.y+dy,dPrecision*0.5,dPrecision*0.5,viewport);
	gluPerspective(piCamera->GetViewAngle(),rWindowRect.w/rWindowRect.h,1,dFarPlane);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	
	glLoadIdentity();
	glRotated((0-dRoll)	,AxisPosZ.c[0],AxisPosZ.c[1],AxisPosZ.c[2]);
	glRotated((0-dPitch),AxisPosX.c[0],AxisPosX.c[1],AxisPosX.c[2]);
	glRotated((0-(dYaw-90))	,AxisPosY.c[0],AxisPosY.c[1],AxisPosY.c[2]);
	glTranslated(-vPosition.c[0],-vPosition.c[1],-vPosition.c[2]);
	glInitNames();
	glPushName(0);
}

void COpenGLRenderForwardFixed::SetSelectionId( unsigned int nId )
{
	glPopName();
	glPushName(nId);
}

int COpenGLRenderForwardFixed::EndSelection()
{
	glPopName();
	glFlush();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	
	int nNewIndex=-1;
	double dCurrentMin=100000000.0;
	int nHits = glRenderMode(GL_RENDER);
	if(nHits)
	{
		GLuint *pSelectionBuffer=m_pSelectionBuffer;
		
		for(int x=0;x<nHits;x++)
		{
			int nIds=*pSelectionBuffer++;
			int nMin=*pSelectionBuffer++;
			pSelectionBuffer++;// nMax
			for(int y=0;y<nIds;y++,pSelectionBuffer++)
			{
				if(y==0 && nMin<dCurrentMin)
				{
					nNewIndex=*pSelectionBuffer;
					dCurrentMin=nMin;
				}
			}
		}
	}
	m_bSelecting=false;
	return nNewIndex;
}

#endif
