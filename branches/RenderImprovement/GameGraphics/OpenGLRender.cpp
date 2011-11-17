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
#include "OpenGLRender.h"

#define MAX_OBJECT_INSTANCES 64
#define MAX_DYNAMIC_LIGHTS 7

#ifdef ANDROID 
	#define DEFAULT_SHADOW_SIZE 512
#else
	#define DEFAULT_SHADOW_SIZE 1024
#endif


void glhFrustumd2(CMatrix *pMatrix, double left, double right, double bottom, double top,double znear, double zfar)
{
	double temp, temp2, temp3, temp4;
	temp = 2.0 * znear;
	temp2 = right - left;
	temp3 = top - bottom;
	temp4 = zfar - znear;
	pMatrix->e[0][0] = temp / temp2;
	pMatrix->e[1][0] = 0.0;
	pMatrix->e[2][0] = 0.0;
	pMatrix->e[3][0] = 0.0;
	pMatrix->e[0][1] = 0.0;
	pMatrix->e[1][1] = temp / temp3;
	pMatrix->e[2][1] = 0.0;
	pMatrix->e[3][1] = 0.0;
	pMatrix->e[0][2] = (right + left) / temp2;
	pMatrix->e[1][2] = (top + bottom) / temp3;
	pMatrix->e[2][2] = (-zfar - znear) / temp4;
	pMatrix->e[3][2] = -1.0;
	pMatrix->e[0][3] = 0.0;
	pMatrix->e[1][3] = 0.0;
	pMatrix->e[2][3] = (-temp * zfar) / temp4;
	pMatrix->e[3][3] = 0.0;
}

void glhPerspectived2(CMatrix *pMatrix, double fovyInDegrees, double aspectRatio,double znear, double zfar)
{
	double ymax, xmax;
	double temp, temp2, temp3, temp4;
	ymax = znear * tanf(fovyInDegrees * M_PI / 360.0);
	xmax = ymax * aspectRatio;
	glhFrustumd2(pMatrix, -xmax, xmax, -ymax, ymax, znear, zfar);
}

void glhOrtho(CMatrix *pMatrix, double dLeft, double dRight, double dBottom, double dTop,double dNear, double dFar)
{
	pMatrix->e[0][0]=2.0/(dRight-dLeft);
	pMatrix->e[1][0]=0;
	pMatrix->e[2][0]=0;
	pMatrix->e[3][0]=0;
	pMatrix->e[0][1]=0;
	pMatrix->e[1][1]=2.0/(dTop-dBottom);
	pMatrix->e[2][1]=0;
	pMatrix->e[3][1]=0;
	pMatrix->e[0][2]=0;
	pMatrix->e[1][2]=0;
	pMatrix->e[2][2]=-2.0/(dFar-dNear);
	pMatrix->e[3][2]=0;
	pMatrix->e[0][3]=-(dRight+dLeft)/(dRight-dLeft);
	pMatrix->e[1][3]=-(dTop+dBottom)/(dTop-dBottom);
	pMatrix->e[2][3]=-(dFar+dNear)/(dFar-dNear);
	pMatrix->e[3][3]=1.0;
}

COpenGLRender::COpenGLRender(void)
{
	m_bLightingPrepared=false;
	m_bRestoreTextureMatrix=false;
	m_bHardwareSupportRead=false;
	m_bShadowVolumeFirstVertex=false;
	m_bRenderingShadowReception=false;
	m_dShadowAntiFlickeringMargin=10;
	
	m_nCurrentBlendOperator1=GL_SRC_ALPHA;
	m_nCurrentBlendOperator2=GL_ONE_MINUS_SRC_ALPHA;
	m_nCurrentDepthFunc=GL_LESS;
	
	m_bRenderingPoints=false;
	
	m_bSelecting=false;
	m_nSelectionPrecision=0;
	m_pShaderBeforeSelection=NULL;
	
#ifndef ANDROID
	m_nCurrentPolygonMode=GL_FILL;
#endif
	
	m_nCurrentActiveTexture=0;
	
	m_vAmbientColor=CVector(0.5,0.5,0.5);
	m_bStagedRendering=false;
	m_dProjectionWidth=1;
	m_dProjectionHeight=1;
	m_piCurrentViewport=NULL;
	m_nViewportX=0;
	m_nViewportY=0;
	m_nViewportW=1;
	m_nViewportH=1;
	m_dSKyShadowSpeed=0.0;
	m_dSKyShadowXResolution=1.0;
	m_dSKyShadowZResolution=1.0;
	m_dSKyShadowOpacity=1.0;
	
	m_bPerspectiveProjection=false;
	m_dPerspectiveNearPlane=1.0;
	m_dPerspectiveFarPlane=10000.0;

	m_nActiveLights=0;
	m_bActiveHeightFog=false;

	m_dStagedRenderingMinZ=0;
	m_dStagedRenderingMaxZ=0;
	m_dMinDistanceToLight=10000;
	
	m_nFirstTimeStamp=GetTimeStamp();
	m_piNormalMap=NULL;
	m_piSkyShadow=NULL;
	
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
}

COpenGLRender::~COpenGLRender(void)
{
}
bool COpenGLRender::Init(std::string sClass,std::string sName,ISystem *piSystem)
{
	bool bOk=CSystemObjectBase::Init(sClass,sName,piSystem);
	if(bOk)
	{
		m_SunLight.Create(m_piSystem,"Light","SunLight");
		if(m_SunLight.m_piLight)
		{
			m_SunLight.m_piLight->SetDirectional(CVector(0,-1,0));
			m_SunLight.m_piLight->SetAmbientColor(CVector(0,0,0));
			m_SunLight.m_piLight->SetDiffuseColor(CVector(0.5,0.5,0.5));
			m_SunLight.m_piLight->SetSpecularColor(CVector(0.5,0.5,0.5));
		}

		AddLight(m_SunLight.m_piLight);
	}
	return bOk;
}

void COpenGLRender::Destroy()
{
	REL(m_piCurrentViewport);
	REL(m_piSkyShadow);
	REL(m_piNormalMap);
	
	RemoveLight(m_SunLight.m_piLight);
	m_SunLight.Destroy();
	m_ShadowTexture.Destroy();
	std::map<SShaderKey,CGenericShaderWrapper>::iterator iShader;
	for(iShader=m_mShaders.begin();iShader!=m_mShaders.end();iShader++)
	{
		iShader->second.Destroy();
	}
	m_mShaders.clear();

	CSystemObjectBase::Destroy();
}

void COpenGLRender::SetViewport(IGenericViewport *piViewPort)
{
	REL(m_piCurrentViewport);
	m_piCurrentViewport=ADD(piViewPort);
}

IGenericViewport *COpenGLRender::GetViewPort()
{
	return ADD(m_piCurrentViewport);
}


void COpenGLRender::UpdateProjectionMatrix()
{
	#ifdef ANDROID_GLES1
	float fMatrix[16];
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ToOpenGLMatrix(&m_ProjectionMatrix,fMatrix);
	glLoadMatrixf(fMatrix);	
	glMatrixMode(GL_MODELVIEW);
	#else
	std::map<SShaderKey,CGenericShaderWrapper>::iterator iShader;
	for(iShader=m_mShaders.begin();iShader!=m_mShaders.end();iShader++)
	{
		iShader->second.m_piShader->AddUniform("uProjection",m_ProjectionMatrix,false);
	}
	if(m_ShadowShader.m_piShader){m_ShadowShader.m_piShader->AddUniform("uProjection",m_ProjectionMatrix,false);}
#endif
}

void COpenGLRender::SetOrthographicProjection(double cx,double cy)
{
	m_bPerspectiveProjection=false;
	m_dProjectionWidth=cx;
	m_dProjectionHeight=cy;

	double dViewportW=m_nViewportW;
	double dViewportH=m_nViewportH;
	
	glhOrtho(&m_ProjectionMatrix,-m_dProjectionWidth*0.5,m_dProjectionWidth*0.5,-m_dProjectionHeight*0.5,m_dProjectionHeight*0.5,1,10000);
	UpdateProjectionMatrix();
}

void COpenGLRender::SetPerspectiveProjection(double dViewAngle,double dNearPlane,double dFarPlane)
{
	m_bPerspectiveProjection=true;
	m_dPerspectiveNearPlane=dNearPlane;
	m_dPerspectiveFarPlane=dFarPlane;
	m_dPerspectiveViewAngle=dViewAngle;

	double dAspectRatio=((double)m_nViewportW)/((double)m_nViewportH);
	glhPerspectived2(&m_ProjectionMatrix,m_dPerspectiveViewAngle,dAspectRatio,m_dPerspectiveNearPlane,m_dPerspectiveFarPlane);
	UpdateProjectionMatrix();
}

void COpenGLRender::SetViewport(double x,double y,double cx, double cy)
{
	if(m_nViewportX!=x || m_nViewportY!=y || m_nViewportW!=cx || m_nViewportH!=cy)
	{
		m_nViewportX=(int)x;
		m_nViewportY=(int)y;
		m_nViewportW=(int)cx;
		m_nViewportH=(int)cy;
		glViewport(m_nViewportX,m_nViewportY,m_nViewportW,m_nViewportH);
	}
}

CVector COpenGLRender::GetCameraPosition(){return m_vCameraPos;}
CVector COpenGLRender::GetCameraForward(){return m_vCameraForward;}
CVector COpenGLRender::GetCameraRight(){return m_vCameraRight;}
CVector COpenGLRender::GetCameraUp(){return m_vCameraUp;}

void COpenGLRender::SetCamera(const CVector &vPosition,double dYaw, double dPitch, double dRoll)
{
	if(!m_bHardwareSupportRead)
	{
		m_bHardwareSupportRead=true;

	//	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,&m_sHardwareSupport.nMaxTextureUnits);
		
		#ifndef ANDROID_GLES1
		GLhandleARB hFakeShaderProgram=glCreateProgramObjectARB();
		m_sHardwareSupport.bShaders=(hFakeShaderProgram!=0);
		if(hFakeShaderProgram){glDeleteProgram(hFakeShaderProgram);hFakeShaderProgram=0;}
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,&m_sHardwareSupport.nMaxTextureUnits);
		#endif
		
		#ifdef ANDROID
		m_sHardwareSupport.bObjectInstancing=false;
		#else
		m_sHardwareSupport.bObjectInstancing=true;
		#endif		
		
		if(m_sHardwareSupport.bShaders){ReloadShaders();}
		
		m_sRenderOptions.bEnableShadows=m_sRenderOptions.bEnableShadows && (m_nShadowTextureLevel<m_sHardwareSupport.nMaxTextureUnits);
		m_sRenderOptions.bEnableSkyShadow=m_sRenderOptions.bEnableSkyShadow && (m_nSkyShadowTextureLevel<m_sHardwareSupport.nMaxTextureUnits);
		m_sRenderOptions.bEnableNormalMaps=m_sRenderOptions.bEnableNormalMaps && (m_nNormalMapTextureLevel<m_sHardwareSupport.nMaxTextureUnits);
	}
	
	CMatrix tmp,identity;
	m_ModelViewMatrix.I();
	if(vPosition.c[0] || vPosition.c[1] || vPosition.c[2]){tmp.T(Origin-vPosition);m_ModelViewMatrix*=tmp;}
	if(dYaw!=90.0){tmp.R(AxisPosY,DegreesToRadians(0-(dYaw-90)));m_ModelViewMatrix*=tmp;}
	if(dPitch){tmp.R(AxisPosX,DegreesToRadians(0-dPitch));m_ModelViewMatrix*=tmp;}
	if(dRoll){tmp.R(AxisPosZ,DegreesToRadians(0-dRoll));m_ModelViewMatrix*=tmp;}
	
#ifdef ANDROID_GLES1
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef((0-dRoll)	,AxisPosZ.c[0],AxisPosZ.c[1],AxisPosZ.c[2]);
	glRotatef((0-dPitch),AxisPosX.c[0],AxisPosX.c[1],AxisPosX.c[2]);
	glRotatef((0-(dYaw-90))	,AxisPosY.c[0],AxisPosY.c[1],AxisPosY.c[2]);
	glTranslatef(-vPosition.c[0],-vPosition.c[1],-vPosition.c[2]);
#else
	std::map<SShaderKey,CGenericShaderWrapper>::iterator iShader;
	for(iShader=m_mShaders.begin();iShader!=m_mShaders.end();iShader++)
	{
		iShader->second.m_piShader->AddUniform("uModel",identity,false);
		iShader->second.m_piShader->AddUniform("uView",m_ModelViewMatrix,false);
	}
	if(m_SelectionShader.m_piShader)
	{
		m_SelectionShader.m_piShader->AddUniform("uModel",identity,false);
		m_SelectionShader.m_piShader->AddUniform("uView",m_ModelViewMatrix,false);
	}
	if(m_ShadowShader.m_piShader)
	{
		m_ShadowShader.m_piShader->AddUniform("uModel",identity,false);
		m_ShadowShader.m_piShader->AddUniform("uView",m_ModelViewMatrix,false);
	}
#endif

	VectorsFromAngles(dYaw,dPitch,dRoll,m_vCameraForward,m_vCameraRight,m_vCameraUp);
	m_vCameraPos=vPosition;
	m_vCameraAngles.c[YAW]=dYaw;
	m_vCameraAngles.c[PITCH]=dPitch;
	m_vCameraAngles.c[ROLL]=dRoll;
}


void COpenGLRender::ActivateClipping(bool bActive)
{
	if(bActive)
	{
		glEnable(GL_SCISSOR_TEST);
	}
	else
	{
		glDisable(GL_SCISSOR_TEST);
	}
}

void COpenGLRender::SetClipRect(double x,double y,double cx, double cy)
{
	glScissor((int)x,(int)y,(int)cx,(int)cy);
}

void COpenGLRender::GetClipRect(double *px,double *py,double *pcx, double *pcy)
{
	float pdScissorBox[4];
	glGetFloatv(GL_SCISSOR_BOX,pdScissorBox);
	*px=pdScissorBox[0];
	*py=pdScissorBox[1];
	*pcx=pdScissorBox[2];
	*pcy=pdScissorBox[3];
}

bool COpenGLRender::IsClippingActive()
{
	GLboolean bActive=0;
	glGetBooleanv(GL_SCISSOR_TEST,&bActive);
	return bActive!=0;
}

void COpenGLRender::SelectTexture(IGenericTexture *pTexture,int nTextureLevel)
{
	if(!m_sRenderOptions.bEnableTextures){return;}
	if(!m_sRenderState.bActiveTextures){return;}
	IGenericTexture *piOldTexture=m_mTextureLevels[nTextureLevel];
	REL(piOldTexture);

	m_mTextureLevels[nTextureLevel]=ADD(pTexture);
	if(!m_bStagedRendering)
	{
		if(m_nCurrentActiveTexture!=nTextureLevel){glActiveTexture(GL_TEXTURE0_ARB+nTextureLevel);m_nCurrentActiveTexture=nTextureLevel;}
		pTexture->PrepareTexture(this,nTextureLevel);
	}
	if(m_sRenderOptions.bEnableStagedRenderingStats){m_sStagedStats.nTextureChanges++;}
}

void COpenGLRender::SetTextureMatrix(CMatrix *pMatrix,int nTextureLevel)
{
	if(!m_sRenderOptions.bEnableTextures){return;}
	if(!m_sRenderState.bActiveTextures){return;}
	if(m_bStagedRendering){return;}

	if(m_pCurrentShader){m_pCurrentShader->m_piShader->AddUniform(nTextureLevel==0?"uTexMatrix0":"uTexMatrix1",*pMatrix,false);}
	
#ifdef ANDROID_GLES1
	float pfMatrix[16];
	ToOpenGLMatrix(pMatrix,pfMatrix);
	if(m_nCurrentActiveTexture!=nTextureLevel){glActiveTexture(GL_TEXTURE0_ARB+nTextureLevel);m_nCurrentActiveTexture=nTextureLevel;}
	glMatrixMode(GL_TEXTURE);
	glLoadMatrixf(pfMatrix);
	glMatrixMode(GL_MODELVIEW);
#endif
	
	m_bRestoreTextureMatrix=true;
}

void COpenGLRender::UnselectTexture(int nTextureLevel)
{
	if(!m_sRenderOptions.bEnableTextures){return;}
	IGenericTexture *piTexture=m_mTextureLevels[nTextureLevel];
	if(!m_bStagedRendering && m_bRestoreTextureMatrix)
	{
		CMatrix identity;
		if(m_pCurrentShader){m_pCurrentShader->m_piShader->AddUniform(nTextureLevel==0?"uTexMatrix0":"uTexMatrix1",identity,false);}
#ifdef ANDROID_GLES1
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
#endif
		m_bRestoreTextureMatrix=false;
	}
	if(!m_bStagedRendering && piTexture)
	{
		if(m_nCurrentActiveTexture!=nTextureLevel){glActiveTexture(GL_TEXTURE0_ARB+nTextureLevel);m_nCurrentActiveTexture=nTextureLevel;}
		piTexture->UnprepareTexture(this,nTextureLevel);
	}
	REL(piTexture);
	m_mTextureLevels.erase(nTextureLevel);
}

void COpenGLRender::SelectNormalMap(IGenericTexture *pNormalMap)
{
	if(!m_sRenderOptions.bEnableNormalMaps){return;}
	REL(m_piNormalMap);
	m_piNormalMap=ADD(pNormalMap);
	
	if(m_nCurrentActiveTexture!=m_nNormalMapTextureLevel){glActiveTexture(GL_TEXTURE0_ARB+m_nNormalMapTextureLevel);m_nCurrentActiveTexture=m_nNormalMapTextureLevel;}
	if(!m_bStagedRendering){m_piNormalMap->PrepareTexture(this,m_nNormalMapTextureLevel);}
}

void COpenGLRender::UnselectNormalMap()
{
	if(!m_sRenderOptions.bEnableNormalMaps){return;}
	if(m_nCurrentActiveTexture!=m_nNormalMapTextureLevel){glActiveTexture(GL_TEXTURE0_ARB+m_nNormalMapTextureLevel);m_nCurrentActiveTexture=m_nNormalMapTextureLevel;}
	if(!m_bStagedRendering && m_piNormalMap){m_piNormalMap->UnprepareTexture(this,m_nNormalMapTextureLevel);}
	REL(m_piNormalMap);
}

void COpenGLRender::SetSkyShadowParameters(double dSpeed, double dXResolution, double dZResolution, double dOpacity)
{
	m_dSKyShadowSpeed=dSpeed;
	m_dSKyShadowXResolution=dXResolution;
	m_dSKyShadowZResolution=dZResolution;
	m_dSKyShadowOpacity=dOpacity;
}

void COpenGLRender::SelectSkyShadow(IGenericTexture *pSkyShadow)
{
	if(!m_sRenderOptions.bEnableSkyShadow){return;}
	REL(m_piSkyShadow);
	m_piSkyShadow=ADD(pSkyShadow);
	if(!m_bStagedRendering && m_piSkyShadow && m_sRenderState.bActiveSkyShadow)
	{
		if(m_nCurrentActiveTexture!=m_nSkyShadowTextureLevel){glActiveTexture(GL_TEXTURE0_ARB+m_nSkyShadowTextureLevel);m_nCurrentActiveTexture=m_nSkyShadowTextureLevel;}
		if(m_piSkyShadow){m_piSkyShadow->PrepareTexture(this,m_nSkyShadowTextureLevel);}
	}
}

void COpenGLRender::UnselectSkyShadow()
{
	if(!m_bStagedRendering && m_piSkyShadow)
	{
		if(m_nCurrentActiveTexture!=m_nSkyShadowTextureLevel){glActiveTexture(GL_TEXTURE0_ARB+m_nSkyShadowTextureLevel);m_nCurrentActiveTexture=m_nSkyShadowTextureLevel;}
		if(m_piSkyShadow){m_piSkyShadow->UnprepareTexture(this,m_nSkyShadowTextureLevel);}
	}
	REL(m_piSkyShadow);
}

void COpenGLRender::RenderTexture(const CVector &vOrigin,double s1,double s2,const CVector &vColor,double dAlpha)
{
	RenderTexture(vOrigin,s1,s2,0,0,1,1,vColor,dAlpha);
}

void COpenGLRender::RenderTexture(const CVector &vOrigin,double s1,double s2,double dTexX,double dTexY,double dTexW,double dTexH,const CVector &vColor,double dAlpha)
{
	if(m_bStagedRendering)
	{
		std::map<unsigned long,IGenericTexture*>::iterator iTexture=m_mTextureLevels.find(0);
		if(iTexture==m_mTextureLevels.end())
		{
			return;
		}
		STextureParticleStageKey key(iTexture->second,m_sStagedRenderingState);
		std::map<STextureParticleStageKey,STextureParticleStage>::iterator iParticleStage;
		STextureParticleStage *pStage=NULL;
		iParticleStage=m_mTextureParticleStages.find(key);
		if(iParticleStage==m_mTextureParticleStages.end())
		{
			ADD(key.piTexture);
			pStage=&m_mTextureParticleStages[key];
		}
		else
		{
			pStage=&iParticleStage->second;
		}
		
		int nBuffers=pStage->vBuffers.size();
		STextureParticleBuffer *pBuffer=nBuffers?pStage->vBuffers[nBuffers-1]:NULL;
		if(pBuffer==NULL || pBuffer->nUsedElements>=TEXTURE_PARTICLE_BUFFER_SIZE)
		{
			pBuffer=new STextureParticleBuffer;
			pStage->vBuffers.push_back(pBuffer);
		}
		float *pVertexBuffer=pBuffer->pVertexBuffer+(pBuffer->nUsedElements*6*3);
		float *pColorBuffer=pBuffer->pColorBuffer+(pBuffer->nUsedElements*6*4);
		float *pTexBuffer=pBuffer->pTexBuffers+(pBuffer->nUsedElements*6*2);
		
		CVector vAxis1=m_vCameraRight*(s1/2.0);
		CVector vAxis2=m_vCameraUp*(s2/2.0);
		CVector vVertex[6];
		vVertex[0]=(vAxis1+vAxis2);
		vVertex[1]=(Origin-vAxis1+vAxis2);
		vVertex[2]=(Origin-vAxis1-vAxis2);
		vVertex[3]=vVertex[0];
		vVertex[4]=vVertex[2];
		vVertex[5]=(vAxis1-vAxis2);
		
		for(int x=0;x<6;x++)
		{
			*pVertexBuffer++=(float)(vOrigin.c[0]+vVertex[x].c[0]);
			*pVertexBuffer++=(float)(vOrigin.c[1]+vVertex[x].c[1]);
			*pVertexBuffer++=(float)(vOrigin.c[2]+vVertex[x].c[2]);
			*pColorBuffer++=(float)vColor.c[0];
			*pColorBuffer++=(float)vColor.c[1];
			*pColorBuffer++=(float)vColor.c[2];
			*pColorBuffer++=(float)dAlpha;
		}
		*pTexBuffer++=dTexX+dTexW;*pTexBuffer++=dTexY+dTexH;
		*pTexBuffer++=dTexX;*pTexBuffer++=dTexY+dTexH;
		*pTexBuffer++=dTexX;*pTexBuffer++=dTexY;
		*pTexBuffer++=dTexX+dTexW;*pTexBuffer++=dTexY+dTexH;
		*pTexBuffer++=dTexX;*pTexBuffer++=dTexY;
		*pTexBuffer++=dTexX+dTexW;*pTexBuffer++=dTexY;
		pBuffer->nUsedElements++;
	}
	else
	{
		SetCurrentRenderStateShader();
		
		CVector vAxis1=m_vCameraRight*(s1*0.5);
		CVector vAxis2=m_vCameraUp*(s2*0.5);
		
		CVector v[4];
		v[0]=vOrigin-vAxis1-vAxis2;
		v[1]=vOrigin+vAxis1-vAxis2;
		v[2]=vOrigin+vAxis1+vAxis2;
		v[3]=vOrigin-vAxis1+vAxis2;	
		
		float pVertexBuffer[12];
		float pTexVertexBuffer[8];
		float pColorBuffer[16];
		GLushort pFaceIndexes[12];
		
		pTexVertexBuffer[0]=dTexX;
		pTexVertexBuffer[1]=dTexY;
		
		pTexVertexBuffer[2]=dTexX+dTexW;
		pTexVertexBuffer[3]=dTexY;
		
		pTexVertexBuffer[4]=dTexX+dTexW;
		pTexVertexBuffer[5]=dTexY+dTexH;
		
		pTexVertexBuffer[6]=dTexX;
		pTexVertexBuffer[7]=dTexY+dTexH;
		
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
		
		pFaceIndexes[0]=0;
		pFaceIndexes[1]=1;
		pFaceIndexes[2]=2;
		pFaceIndexes[3]=0;
		pFaceIndexes[4]=2;
		pFaceIndexes[5]=3;
		
		pColorBuffer[0]=vColor.c[0];
		pColorBuffer[1]=vColor.c[1];
		pColorBuffer[2]=vColor.c[2];
		pColorBuffer[3]=dAlpha;
		pColorBuffer[4]=vColor.c[0];
		pColorBuffer[5]=vColor.c[1];
		pColorBuffer[6]=vColor.c[2];
		pColorBuffer[7]=dAlpha;
		pColorBuffer[8]=vColor.c[0];
		pColorBuffer[9]=vColor.c[1];
		pColorBuffer[10]=vColor.c[2];
		pColorBuffer[11]=dAlpha;
		pColorBuffer[12]=vColor.c[0];
		pColorBuffer[13]=vColor.c[1];
		pColorBuffer[14]=vColor.c[2];
		pColorBuffer[15]=dAlpha;
		
		float *pTexVertexBufferTemp=pTexVertexBuffer;
		SetVertexPointers(pVertexBuffer,NULL,pColorBuffer,1,&pTexVertexBufferTemp);
		glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_SHORT,pFaceIndexes);
	}
}

void COpenGLRender::RenderBBox(const CVector &vOrigin,const CVector &vOrientation,const CVector &vMins,const CVector &vMaxs,unsigned long nStipple,const CVector &vColor,double dAlpha)
{
	CVector vPoints[8];
	CalcBBoxVolume(vOrigin,vOrientation,vMins,vMaxs,vPoints);

	//bottom
	RenderLine(vPoints[0],vPoints[1],nStipple,vColor,dAlpha);
	RenderLine(vPoints[1],vPoints[2],nStipple,vColor,dAlpha);
	RenderLine(vPoints[2],vPoints[3],nStipple,vColor,dAlpha);
	RenderLine(vPoints[3],vPoints[0],nStipple,vColor,dAlpha);
	// top
	RenderLine(vPoints[4],vPoints[5],nStipple,vColor,dAlpha);
	RenderLine(vPoints[5],vPoints[6],nStipple,vColor,dAlpha);
	RenderLine(vPoints[6],vPoints[7],nStipple,vColor,dAlpha);
	RenderLine(vPoints[7],vPoints[4],nStipple,vColor,dAlpha);
	// verticals
	RenderLine(vPoints[0],vPoints[4],nStipple,vColor,dAlpha);
	RenderLine(vPoints[1],vPoints[5],nStipple,vColor,dAlpha);
	RenderLine(vPoints[2],vPoints[6],nStipple,vColor,dAlpha);
	RenderLine(vPoints[3],vPoints[7],nStipple,vColor,dAlpha);
}

void COpenGLRender::RenderBBox(const CVector &vMins,const CVector &vMaxs,unsigned long nStipple,const CVector &vColor,double dAlpha)
{
	CVector vPoints[8];
	// bottom
	vPoints[0]=CVector(vMaxs.c[0],vMins.c[1],vMins.c[2]);
	vPoints[1]=CVector(vMaxs.c[0],vMins.c[1],vMaxs.c[2]);
	vPoints[2]=CVector(vMins.c[0],vMins.c[1],vMaxs.c[2]);
	vPoints[3]=CVector(vMins.c[0],vMins.c[1],vMins.c[2]);
	vPoints[4]=CVector(vMaxs.c[0],vMaxs.c[1],vMins.c[2]);
	vPoints[5]=CVector(vMaxs.c[0],vMaxs.c[1],vMaxs.c[2]);
	vPoints[6]=CVector(vMins.c[0],vMaxs.c[1],vMaxs.c[2]);
	vPoints[7]=CVector(vMins.c[0],vMaxs.c[1],vMins.c[2]);
	//bottom
	RenderLine(vPoints[0],vPoints[1],nStipple,vColor,dAlpha);
	RenderLine(vPoints[1],vPoints[2],nStipple,vColor,dAlpha);
	RenderLine(vPoints[2],vPoints[3],nStipple,vColor,dAlpha);
	RenderLine(vPoints[3],vPoints[0],nStipple,vColor,dAlpha);
	// top
	RenderLine(vPoints[4],vPoints[5],nStipple,vColor,dAlpha);
	RenderLine(vPoints[5],vPoints[6],nStipple,vColor,dAlpha);
	RenderLine(vPoints[6],vPoints[7],nStipple,vColor,dAlpha);
	RenderLine(vPoints[7],vPoints[4],nStipple,vColor,dAlpha);
	// verticals
	RenderLine(vPoints[0],vPoints[4],nStipple,vColor,dAlpha);
	RenderLine(vPoints[1],vPoints[5],nStipple,vColor,dAlpha);
	RenderLine(vPoints[2],vPoints[6],nStipple,vColor,dAlpha);
	RenderLine(vPoints[3],vPoints[7],nStipple,vColor,dAlpha);
}

void COpenGLRender::RenderLineLoop(unsigned int nLines,const CVector *pPoints,unsigned long nStipple,const CVector &vColor,double dAlpha)
{
	
	if(m_bStagedRendering)
	{
		SLineStageKey key(m_sStagedRenderingState,nStipple);
		std::map<SLineStageKey,SLineStage>::iterator iParticleStage;
		SLineStage *pStage=NULL;
		iParticleStage=m_mLineStages.find(key);
		if(iParticleStage==m_mLineStages.end())
		{
			pStage=&m_mLineStages[key];
		}
		else
		{
			pStage=&iParticleStage->second;
		}
		for(unsigned int x=0;x<nLines;x++)
		{
			int nBuffers=pStage->vBuffers.size();
			SLineBuffer *pBuffer=nBuffers?pStage->vBuffers[nBuffers-1]:NULL;
			if(pBuffer==NULL || pBuffer->nUsedElements>=LINE_BUFFER_SIZE)
			{
				pBuffer=new SLineBuffer;
				pStage->vBuffers.push_back(pBuffer);
			}
			float *pVertexBuffer=pBuffer->pVertexBuffer+(pBuffer->nUsedElements*2*3);
			float *pColorBuffer=pBuffer->pColorBuffer+(pBuffer->nUsedElements*2*4);
			
			*pVertexBuffer++=(float)pPoints[x].c[0];
			*pVertexBuffer++=(float)pPoints[x].c[1];
			*pVertexBuffer++=(float)pPoints[x].c[2];
			
			*pVertexBuffer++=(float)pPoints[(x+1)%nLines].c[0];
			*pVertexBuffer++=(float)pPoints[(x+1)%nLines].c[1];
			*pVertexBuffer++=(float)pPoints[(x+1)%nLines].c[2];
			
			*pColorBuffer++=(float)vColor.c[0];
			*pColorBuffer++=(float)vColor.c[1];
			*pColorBuffer++=(float)vColor.c[2];
			*pColorBuffer++=(float)dAlpha;
			
			*pColorBuffer++=(float)vColor.c[0];
			*pColorBuffer++=(float)vColor.c[1];
			*pColorBuffer++=(float)vColor.c[2];
			*pColorBuffer++=(float)dAlpha;
			
			pBuffer->nUsedElements++;
		}
	}
	else
	{	
		SLineBuffer buffer;
		float *pTempVertexBuffer=buffer.pVertexBuffer;
		float *pTempColorBuffer=buffer.pColorBuffer;
		
		for(int x=0;x<LINE_BUFFER_SIZE && x<nLines;x++)
		{
			buffer.nUsedElements++;
			*pTempVertexBuffer++=pPoints[x].c[0];
			*pTempVertexBuffer++=pPoints[x].c[1];
			*pTempVertexBuffer++=pPoints[x].c[2];
			
			*pTempVertexBuffer++=pPoints[(x+1)%nLines].c[0];
			*pTempVertexBuffer++=pPoints[(x+1)%nLines].c[1];
			*pTempVertexBuffer++=pPoints[(x+1)%nLines].c[2];

			*pTempColorBuffer++=vColor.c[0];
			*pTempColorBuffer++=vColor.c[1];
			*pTempColorBuffer++=vColor.c[2];
			*pTempColorBuffer++=(float)dAlpha;
			
			*pTempColorBuffer++=vColor.c[0];
			*pTempColorBuffer++=vColor.c[1];
			*pTempColorBuffer++=vColor.c[2];
			*pTempColorBuffer++=(float)dAlpha;
		}
		SetVertexPointers(buffer.pVertexBuffer,NULL,buffer.pColorBuffer,0,NULL);
		glDrawArrays(GL_LINES,0,2*buffer.nUsedElements);
	}
}


void COpenGLRender::RenderLines(unsigned int nLines,const CVector *pPoints,unsigned long nStipple,const CVector &vColor,double dAlpha)
{
	if(m_bStagedRendering)
	{
		SLineStageKey key(m_sStagedRenderingState,nStipple);
		std::map<SLineStageKey,SLineStage>::iterator iParticleStage;
		SLineStage *pStage=NULL;
		iParticleStage=m_mLineStages.find(key);
		if(iParticleStage==m_mLineStages.end())
		{
			pStage=&m_mLineStages[key];
		}
		else
		{
			pStage=&iParticleStage->second;
		}
		for(unsigned int x=0;x<nLines*2;x+=2)
		{
			int nBuffers=pStage->vBuffers.size();
			SLineBuffer *pBuffer=nBuffers?pStage->vBuffers[nBuffers-1]:NULL;
			if(pBuffer==NULL || pBuffer->nUsedElements>=LINE_BUFFER_SIZE)
			{
				pBuffer=new SLineBuffer;
				pStage->vBuffers.push_back(pBuffer);
			}
			float *pVertexBuffer=pBuffer->pVertexBuffer+(pBuffer->nUsedElements*2*3);
			float *pColorBuffer=pBuffer->pColorBuffer+(pBuffer->nUsedElements*2*4);
			
			*pVertexBuffer++=(float)pPoints[x].c[0];
			*pVertexBuffer++=(float)pPoints[x].c[1];
			*pVertexBuffer++=(float)pPoints[x].c[2];
			
			*pVertexBuffer++=(float)pPoints[x+1].c[0];
			*pVertexBuffer++=(float)pPoints[x+1].c[1];
			*pVertexBuffer++=(float)pPoints[x+1].c[2];
			
			*pColorBuffer++=(float)vColor.c[0];
			*pColorBuffer++=(float)vColor.c[1];
			*pColorBuffer++=(float)vColor.c[2];
			*pColorBuffer++=(float)dAlpha;
			
			*pColorBuffer++=(float)vColor.c[0];
			*pColorBuffer++=(float)vColor.c[1];
			*pColorBuffer++=(float)vColor.c[2];
			*pColorBuffer++=(float)dAlpha;
			
			pBuffer->nUsedElements++;
		}
	}	
	else
	{	
#ifndef ANDROID
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1,(unsigned short)nStipple);
#endif
		
		SLineBuffer buffer;
		float *pVertexBuffer=buffer.pVertexBuffer;
		float *pColorBuffer=buffer.pColorBuffer;
		for(unsigned int x=0;x<nLines*2;x+=2)
		{
			*pVertexBuffer++=(float)pPoints[x].c[0];
			*pVertexBuffer++=(float)pPoints[x].c[1];
			*pVertexBuffer++=(float)pPoints[x].c[2];
			
			*pVertexBuffer++=(float)pPoints[x+1].c[0];
			*pVertexBuffer++=(float)pPoints[x+1].c[1];
			*pVertexBuffer++=(float)pPoints[x+1].c[2];
			
			*pColorBuffer++=(float)vColor.c[0];
			*pColorBuffer++=(float)vColor.c[1];
			*pColorBuffer++=(float)vColor.c[2];
			*pColorBuffer++=(float)dAlpha;
			
			*pColorBuffer++=(float)vColor.c[0];
			*pColorBuffer++=(float)vColor.c[1];
			*pColorBuffer++=(float)vColor.c[2];
			*pColorBuffer++=(float)dAlpha;
			
			buffer.nUsedElements++;
		}
		SetVertexPointers(buffer.pVertexBuffer,NULL,buffer.pColorBuffer,0,NULL);
		glDrawArrays(GL_LINES,0,2*buffer.nUsedElements);
		
#ifndef ANDROID
		glDisable(GL_LINE_STIPPLE);
#endif
	}
}
void COpenGLRender::RenderLine(const CVector &v1,const CVector &v2,unsigned long nStipple,const CVector &vColor,double dAlpha)
{
	if(m_bStagedRendering)
	{
		SLineStageKey key(m_sStagedRenderingState,nStipple);
		std::map<SLineStageKey,SLineStage>::iterator iParticleStage;
		SLineStage *pStage=NULL;
		iParticleStage=m_mLineStages.find(key);
		if(iParticleStage==m_mLineStages.end())
		{
			pStage=&m_mLineStages[key];
		}
		else
		{
			pStage=&iParticleStage->second;
		}

		int nBuffers=pStage->vBuffers.size();
		SLineBuffer *pBuffer=nBuffers?pStage->vBuffers[nBuffers-1]:NULL;
		if(pBuffer==NULL || pBuffer->nUsedElements>=LINE_BUFFER_SIZE)
		{
			pBuffer=new SLineBuffer;
			pStage->vBuffers.push_back(pBuffer);
		}
		float *pVertexBuffer=pBuffer->pVertexBuffer+(pBuffer->nUsedElements*2*3);
		float *pColorBuffer=pBuffer->pColorBuffer+(pBuffer->nUsedElements*2*4);

		*pVertexBuffer++=(float)v1.c[0];
		*pVertexBuffer++=(float)v1.c[1];
		*pVertexBuffer++=(float)v1.c[2];

		*pVertexBuffer++=(float)v2.c[0];
		*pVertexBuffer++=(float)v2.c[1];
		*pVertexBuffer++=(float)v2.c[2];

		*pColorBuffer++=(float)vColor.c[0];
		*pColorBuffer++=(float)vColor.c[1];
		*pColorBuffer++=(float)vColor.c[2];
		*pColorBuffer++=(float)dAlpha;

		*pColorBuffer++=(float)vColor.c[0];
		*pColorBuffer++=(float)vColor.c[1];
		*pColorBuffer++=(float)vColor.c[2];
		*pColorBuffer++=(float)dAlpha;

		pBuffer->nUsedElements++;
	}
	else
	{
#ifndef ANDROID
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1,(unsigned short)nStipple);
#endif
		SetCurrentRenderStateShader();
		
		float pVertexBuffer[6];
		float pColorBuffer[8];
		
		pVertexBuffer[0]=v1.c[0];
		pVertexBuffer[1]=v1.c[1];
		pVertexBuffer[2]=v1.c[2];
		
		pVertexBuffer[3]=v2.c[0];
		pVertexBuffer[4]=v2.c[1];
		pVertexBuffer[5]=v2.c[2];		
		
		pColorBuffer[0]=vColor.c[0];
		pColorBuffer[1]=vColor.c[1];
		pColorBuffer[2]=vColor.c[2];
		pColorBuffer[3]=dAlpha;
		pColorBuffer[4]=vColor.c[0];
		pColorBuffer[5]=vColor.c[1];
		pColorBuffer[6]=vColor.c[2];
		pColorBuffer[7]=dAlpha;
		
		SetVertexPointers(pVertexBuffer,NULL,pColorBuffer,0,NULL);
		glDrawArrays(GL_LINES,0,2);
		
#ifndef ANDROID
		glDisable(GL_LINE_STIPPLE);
#endif
	}
}

void COpenGLRender::RenderRect(const CVector &vCenter,const CVector &vAxisW,const CVector &vAxisH,double w,double h,CVector &vColor,double dAlpha)
{	
	SetCurrentRenderStateShader();
	
	CVector v[4];
	v[0]=vCenter+vAxisW*(w/2.0)-vAxisH*(h/2.0);
	v[1]=vCenter-vAxisW*(w/2.0)-vAxisH*(h/2.0);
	v[2]=vCenter-vAxisW*(w/2.0)+vAxisH*(h/2.0);
	v[3]=vCenter+vAxisW*(w/2.0)+vAxisH*(h/2.0);	
	
	float pVertexBuffer[12];
	float pColorBuffer[16];
	
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
	
	pColorBuffer[0]=vColor.c[0];
	pColorBuffer[1]=vColor.c[1];
	pColorBuffer[2]=vColor.c[2];
	pColorBuffer[3]=dAlpha;
	pColorBuffer[4]=vColor.c[0];
	pColorBuffer[5]=vColor.c[1];
	pColorBuffer[6]=vColor.c[2];
	pColorBuffer[7]=dAlpha;
	pColorBuffer[8]=vColor.c[0];
	pColorBuffer[9]=vColor.c[1];
	pColorBuffer[10]=vColor.c[2];
	pColorBuffer[11]=dAlpha;
	pColorBuffer[12]=vColor.c[0];
	pColorBuffer[13]=vColor.c[1];
	pColorBuffer[14]=vColor.c[2];
	pColorBuffer[15]=dAlpha;
	
	SetVertexPointers(pVertexBuffer,NULL,pColorBuffer,0,NULL);
	if(m_sRenderState.bActiveSolid)
	{
		GLushort pFaceIndexes[6];
		pFaceIndexes[0]=0;
		pFaceIndexes[1]=1;
		pFaceIndexes[2]=2;
		pFaceIndexes[3]=0;
		pFaceIndexes[4]=2;
		pFaceIndexes[5]=3;
		
		glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_SHORT,pFaceIndexes);
	}
	else
	{	
		GLushort pFaceIndexes[4];
		pFaceIndexes[0]=0;
		pFaceIndexes[1]=1;
		pFaceIndexes[2]=2;
		pFaceIndexes[3]=3;
		
		glDrawElements(GL_LINE_LOOP,4,GL_UNSIGNED_SHORT,pFaceIndexes);
	}
}

void COpenGLRender::RenderRect(double x,double y,double w,double h,CVector &vColor,double dAlpha)
{
	SetCurrentRenderStateShader();
	
	CVector v[4];
	v[0]=CVector(x,y,0);
	v[1]=CVector(x+w,y,0);
	v[2]=CVector(x+w,y+h,0);
	v[3]=CVector(x,y+h,0);
	
	float pVertexBuffer[12];
	float pColorBuffer[16];
	
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
	
	pColorBuffer[0]=vColor.c[0];
	pColorBuffer[1]=vColor.c[1];
	pColorBuffer[2]=vColor.c[2];
	pColorBuffer[3]=dAlpha;
	pColorBuffer[4]=vColor.c[0];
	pColorBuffer[5]=vColor.c[1];
	pColorBuffer[6]=vColor.c[2];
	pColorBuffer[7]=dAlpha;
	pColorBuffer[8]=vColor.c[0];
	pColorBuffer[9]=vColor.c[1];
	pColorBuffer[10]=vColor.c[2];
	pColorBuffer[11]=dAlpha;
	pColorBuffer[12]=vColor.c[0];
	pColorBuffer[13]=vColor.c[1];
	pColorBuffer[14]=vColor.c[2];
	pColorBuffer[15]=dAlpha;
	
	SetVertexPointers(pVertexBuffer,NULL,pColorBuffer,0,NULL);
	if(m_sRenderState.bActiveSolid)
	{
		GLushort pFaceIndexes[6];
		pFaceIndexes[0]=0;
		pFaceIndexes[1]=1;
		pFaceIndexes[2]=2;
		pFaceIndexes[3]=0;
		pFaceIndexes[4]=2;
		pFaceIndexes[5]=3;
		
		glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_SHORT,pFaceIndexes);
	}
	else
	{	
		GLushort pFaceIndexes[4];
		pFaceIndexes[0]=0;
		pFaceIndexes[1]=1;
		pFaceIndexes[2]=2;
		pFaceIndexes[3]=3;
		
		glDrawElements(GL_LINE_LOOP,4,GL_UNSIGNED_SHORT,pFaceIndexes);
	}
}
void COpenGLRender::RenderPolygon(unsigned int nVertexes,const CVector *pVertexes,const CVector &vColor,double dAlpha)
{
	if(nVertexes<3){return;}
	
	STextureParticleBuffer buffer;
	float *pTempVertexBuffer=buffer.pVertexBuffer;
	float *pTempColorBuffer=buffer.pColorBuffer;
	
	for(int x=0;x<TEXTURE_PARTICLE_BUFFER_SIZE && x<nVertexes-2;x++)
	{
		buffer.nUsedElements++;
		*pTempVertexBuffer++=pVertexes[0].c[0];
		*pTempVertexBuffer++=pVertexes[0].c[1];
		*pTempVertexBuffer++=pVertexes[0].c[2];
		
		*pTempVertexBuffer++=pVertexes[x+1].c[0];
		*pTempVertexBuffer++=pVertexes[x+1].c[1];
		*pTempVertexBuffer++=pVertexes[x+1].c[2];
		
		*pTempVertexBuffer++=pVertexes[x+2].c[0];
		*pTempVertexBuffer++=pVertexes[x+2].c[1];
		*pTempVertexBuffer++=pVertexes[x+2].c[2];
		
		*pTempColorBuffer++=vColor.c[0];
		*pTempColorBuffer++=vColor.c[1];
		*pTempColorBuffer++=vColor.c[2];
		*pTempColorBuffer++=dAlpha;
		
		*pTempColorBuffer++=vColor.c[0];
		*pTempColorBuffer++=vColor.c[1];
		*pTempColorBuffer++=vColor.c[2];
		*pTempColorBuffer++=dAlpha;
		
		*pTempColorBuffer++=vColor.c[0];
		*pTempColorBuffer++=vColor.c[1];
		*pTempColorBuffer++=vColor.c[2];
		*pTempColorBuffer++=dAlpha;
	}
	SetVertexPointers(buffer.pVertexBuffer,NULL,buffer.pColorBuffer,0,NULL);
	glDrawArrays(GL_TRIANGLES,0,3*buffer.nUsedElements);
}

void COpenGLRender::RenderPolygon(unsigned int nVertexes,const CVector *pVertexes,const CVector *pColors,const double *pdAlphas)
{
	if(nVertexes<3){return;}
	
	STextureParticleBuffer buffer;
	float *pTempVertexBuffer=buffer.pVertexBuffer;
	float *pTempColorBuffer=buffer.pColorBuffer;
	
	for(int x=0;x<TEXTURE_PARTICLE_BUFFER_SIZE && x<nVertexes-2;x++)
	{
		buffer.nUsedElements++;
		*pTempVertexBuffer++=pVertexes[0].c[0];
		*pTempVertexBuffer++=pVertexes[0].c[1];
		*pTempVertexBuffer++=pVertexes[0].c[2];
		
		*pTempVertexBuffer++=pVertexes[x+1].c[0];
		*pTempVertexBuffer++=pVertexes[x+1].c[1];
		*pTempVertexBuffer++=pVertexes[x+1].c[2];
		
		*pTempVertexBuffer++=pVertexes[x+2].c[0];
		*pTempVertexBuffer++=pVertexes[x+2].c[1];
		*pTempVertexBuffer++=pVertexes[x+2].c[2];

		*pTempColorBuffer++=pColors[0].c[0];
		*pTempColorBuffer++=pColors[0].c[1];
		*pTempColorBuffer++=pColors[0].c[2];
		*pTempColorBuffer++=pdAlphas[0];
		
		*pTempColorBuffer++=pColors[x+1].c[0];
		*pTempColorBuffer++=pColors[x+1].c[1];
		*pTempColorBuffer++=pColors[x+1].c[2];
		*pTempColorBuffer++=pdAlphas[x+1];
		
		*pTempColorBuffer++=pColors[x+2].c[0];
		*pTempColorBuffer++=pColors[x+2].c[1];
		*pTempColorBuffer++=pColors[x+2].c[2];
		*pTempColorBuffer++=pdAlphas[x+2];
	}
	SetVertexPointers(buffer.pVertexBuffer,NULL,buffer.pColorBuffer,0,NULL);
	glDrawArrays(GL_TRIANGLES,0,3*buffer.nUsedElements);
}

void COpenGLRender::RenderArrowHead(const CVector &vPosition,const CVector &vDirection,CVector &vUp,double dForward,double dUp,double dRight,const CVector &vColor,double dAlpha)
{
	CVector pvBase[4];
	CVector vHead;
	CVector vRight=vDirection^vUp;
	
	pvBase[3]=vPosition+vUp*dUp-vRight*dRight;
	pvBase[2]=vPosition+vUp*dUp+vRight*dRight;
	pvBase[1]=vPosition-vUp*dUp+vRight*dRight;
	pvBase[0]=vPosition-vUp*dUp-vRight*dRight;
	vHead=vPosition+vDirection*dForward;
	
	STextureParticleBuffer buffer;
	float *pTempVertexBuffer=buffer.pVertexBuffer;	
	float *pTempColorBuffer=buffer.pColorBuffer;	
	*pTempVertexBuffer++=pvBase[0].c[0];
	*pTempVertexBuffer++=pvBase[0].c[1];
	*pTempVertexBuffer++=pvBase[0].c[2];
	*pTempVertexBuffer++=pvBase[1].c[0];
	*pTempVertexBuffer++=pvBase[1].c[1];
	*pTempVertexBuffer++=pvBase[1].c[2];
	*pTempVertexBuffer++=pvBase[2].c[0];
	*pTempVertexBuffer++=pvBase[2].c[1];
	*pTempVertexBuffer++=pvBase[2].c[2];
	*pTempVertexBuffer++=pvBase[3].c[0];
	*pTempVertexBuffer++=pvBase[3].c[1];
	*pTempVertexBuffer++=pvBase[3].c[2];
	*pTempVertexBuffer++=vHead.c[0];
	*pTempVertexBuffer++=vHead.c[1];
	*pTempVertexBuffer++=vHead.c[2];
	
	
	*pTempColorBuffer++=vColor.c[0];
	*pTempColorBuffer++=vColor.c[1];
	*pTempColorBuffer++=vColor.c[2];
	*pTempColorBuffer++=dAlpha;

	*pTempColorBuffer++=vColor.c[0];
	*pTempColorBuffer++=vColor.c[1];
	*pTempColorBuffer++=vColor.c[2];
	*pTempColorBuffer++=dAlpha;
	
	*pTempColorBuffer++=vColor.c[0];
	*pTempColorBuffer++=vColor.c[1];
	*pTempColorBuffer++=vColor.c[2];
	*pTempColorBuffer++=dAlpha;
	
	*pTempColorBuffer++=vColor.c[0];
	*pTempColorBuffer++=vColor.c[1];
	*pTempColorBuffer++=vColor.c[2];
	*pTempColorBuffer++=dAlpha;
	
	*pTempColorBuffer++=vColor.c[0];
	*pTempColorBuffer++=vColor.c[1];
	*pTempColorBuffer++=vColor.c[2];
	*pTempColorBuffer++=dAlpha;
	
	unsigned short pFaces[18]={0,1,2,  0,2,3,  4,1,0,   4,2,1   ,4,3,2,  4,0,3};

	SetVertexPointers(buffer.pVertexBuffer,NULL,buffer.pColorBuffer,0,NULL);
	glDrawElements(GL_TRIANGLES,18,GL_UNSIGNED_SHORT,pFaces);
}

void COpenGLRender::ActivateHeightFog(const CVector &vMins,const CVector &vMaxs,const CVector &vColor)
{
	if(!m_sRenderOptions.bEnableHeightFog){return;}

	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveHeightFog=true;
	pState->vHeightFogMins=vMins;
	pState->vHeightFogMaxs=vMaxs;
	pState->vHeightFogColor=vColor;
#ifdef ANDROID_GLES1
	if(!m_bStagedRendering)
	{
		if(m_vCameraForward==AxisNegY)
		{
			glEnable(GL_FOG);
			glFogf(GL_FOG_START,m_vCameraPos.c[1]-vMaxs.c[1]);
			glFogf(GL_FOG_END,m_vCameraPos.c[1]-vMins.c[1]);
		}
		float vHeightFogColor[3]={(float)vColor.c[0],(float)vColor.c[1],(float)vColor.c[2]};
		glFogfv(GL_FOG_COLOR,vHeightFogColor);
		glFogf(GL_FOG_MODE,GL_LINEAR);
	}
#endif
}

void COpenGLRender::ActivateWater()
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveWater=true;
}

void COpenGLRender::SetWaterMappingSize(double dMaxU,double dMaxV)
{
#ifndef ANDROID_GLES1
	std::map<SShaderKey,CGenericShaderWrapper>::iterator iShader;
	for(iShader=m_mShaders.begin();iShader!=m_mShaders.end();iShader++)
	{
		if(iShader->first.bWater)
		{
			iShader->second.m_piShader->AddUniform("WaterMappingSize",dMaxU,dMaxV,false);
		}
	}
#endif
}

void COpenGLRender::SetWaterMappingOffset(double dMaxU,double dMaxV)
{
#ifndef ANDROID_GLES1
	std::map<SShaderKey,CGenericShaderWrapper>::iterator iShader;
	for(iShader=m_mShaders.begin();iShader!=m_mShaders.end();iShader++)
	{
		if(iShader->first.bWater)
		{
			iShader->second.m_piShader->AddUniform("WaterMappingOffset",dMaxU,dMaxV,false);
		}
	}
#endif
}

void COpenGLRender::DeactivateWater()
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveWater=false;
}

void COpenGLRender::DeactivateHeightFog()
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveHeightFog=false;
	
#ifdef ANDROID_GLES1
	if(!m_bStagedRendering && m_vCameraForward==AxisNegY)
	{
		glDisable(GL_FOG);
	}
#endif
}
bool COpenGLRender::IsHeightFogActive(){return m_bActiveHeightFog=false;}


void COpenGLRender::ActivateTextures(){InternalActivateTextures();if(!m_bStagedRendering){SetCurrentRenderStateShader();}}
void COpenGLRender::DeactivateTextures(){InternalDeactivateTextures();if(!m_bStagedRendering){SetCurrentRenderStateShader();}}
bool COpenGLRender::AreTexturesActive(){SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;return pState->bActiveTextures;}

void COpenGLRender::InternalActivateTextures()
{
	if(!m_sRenderOptions.bEnableTextures){return;}
	
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveTextures=true;
}

void COpenGLRender::InternalDeactivateTextures()
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveTextures=false;
}

void COpenGLRender::ActivateSolid(){InternalActivateSolid();if(!m_bStagedRendering){SetCurrentRenderStateShader();}}
void COpenGLRender::DeactivateSolid(){InternalDeactivateSolid();if(!m_bStagedRendering){SetCurrentRenderStateShader();}}
bool COpenGLRender::IsSolidActive(){SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;return pState->bActiveSolid;}

void COpenGLRender::InternalActivateSolid()
{
	if(!m_sRenderOptions.bEnableSolid){return;}
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveSolid=true;
#ifndef ANDROID
	if(!m_bStagedRendering)
	{
		if(m_nCurrentPolygonMode!=GL_FILL){glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);m_nCurrentPolygonMode=GL_FILL;}
	}
#endif
}

void COpenGLRender::InternalDeactivateSolid()
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveSolid=false;
#ifndef ANDROID
	if(!m_bStagedRendering)
	{
		if(m_nCurrentPolygonMode!=GL_LINE){glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);m_nCurrentPolygonMode=GL_LINE;}
	}
#endif
}

void COpenGLRender::ActivateShadowEmission()
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveShadowEmission=true;
}

void COpenGLRender::DeactivateShadowEmission()
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveShadowEmission=false;
}
bool COpenGLRender::IsShadowEmissionActive()
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	return pState->bActiveShadowEmission;
}

void COpenGLRender::ActivateShadowReception()
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveShadowReception=true;
}

void COpenGLRender::DeactivateShadowReception()
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveShadowReception=false;
}
bool COpenGLRender::IsShadowReceptionActive()
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	return pState->bActiveShadowReception;
}

void COpenGLRender::ActivateSkyShadow()
{
	if(!m_sRenderOptions.bEnableSkyShadow){return;}
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveSkyShadow=true;
	
	if(!m_bStagedRendering && m_piSkyShadow)
	{
		if(m_nCurrentActiveTexture!=m_nSkyShadowTextureLevel){glActiveTexture(GL_TEXTURE0_ARB+m_nSkyShadowTextureLevel);m_nCurrentActiveTexture=m_nSkyShadowTextureLevel;}
		if(m_piSkyShadow){m_piSkyShadow->PrepareTexture(this,m_nSkyShadowTextureLevel);}
	}
}

void COpenGLRender::DeactivateSkyShadow()
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveSkyShadow=false;

	if(!m_bStagedRendering && m_piSkyShadow && m_sRenderState.bActiveSkyShadow)
	{
		if(m_nCurrentActiveTexture!=m_nSkyShadowTextureLevel){glActiveTexture(GL_TEXTURE0_ARB+m_nSkyShadowTextureLevel);m_nCurrentActiveTexture=m_nSkyShadowTextureLevel;}
		if(m_piSkyShadow){m_piSkyShadow->UnprepareTexture(this,m_nSkyShadowTextureLevel);}
	}
}

bool COpenGLRender::IsSkyShadowActive()
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	return pState->bActiveSkyShadow;
}

void COpenGLRender::InternalActivateDepth()
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveDepth=true;
	if(!m_bStagedRendering){glEnable(GL_DEPTH_TEST);}
}

void COpenGLRender::InternalSetDepthFunction(EDepthFunction eFunction)
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->nDepthFunction=GL_LESS;
	
	if(eFunction==eDepthFunction_Less){pState->nDepthFunction=GL_LESS;}
	else if(eFunction==eDepthFunction_LessOrEqual){pState->nDepthFunction=GL_LEQUAL;}
	else if(eFunction==eDepthFunction_Equal){pState->nDepthFunction=GL_EQUAL;}
	else if(eFunction==eDepthFunction_Greater){pState->nDepthFunction=GL_GREATER;}
	else if(eFunction==eDepthFunction_GreaterOrEqual){pState->nDepthFunction=GL_GEQUAL;}
	else if(eFunction==eDepthFunction_Always){pState->nDepthFunction=GL_ALWAYS;}
	else if(eFunction==eDepthFunction_Never){pState->nDepthFunction=GL_NEVER;}
	else if(eFunction==eDepthFunction_NotEqual){pState->nDepthFunction=GL_NOTEQUAL;}
	
	if(!m_bStagedRendering)
	{
		if(m_nCurrentDepthFunc!=pState->nDepthFunction)
		{
			glDepthFunc(pState->nDepthFunction);
			m_nCurrentDepthFunc=pState->nDepthFunction;
		}
	}
}

void COpenGLRender::InternalDeactivateDepth()
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveDepth=false;
	if(!m_bStagedRendering){glDisable(GL_DEPTH_TEST);}
}

void COpenGLRender::ActivateDepth()
{
	InternalActivateDepth();
	if(!m_bStagedRendering){SetCurrentRenderStateShader();}
}

void COpenGLRender::SetDepthFunction(EDepthFunction eFunction)
{
	InternalSetDepthFunction(eFunction);
	if(!m_bStagedRendering){SetCurrentRenderStateShader();}
}

void COpenGLRender::DeactivateDepth()
{
	InternalDeactivateDepth();
	if(!m_bStagedRendering){SetCurrentRenderStateShader();}
}

void COpenGLRender::InternalActivateBlending()
{
	if(!m_sRenderOptions.bEnableBlending){return;}
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveBlending=true;
	if(!m_bStagedRendering){glEnable(GL_BLEND);}
}

void COpenGLRender::InternalSetBlendingLayer(unsigned int nLayer)
{
	if(!m_sRenderOptions.bEnableBlending){return;}
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->nBlendingLayer=nLayer;
}

void COpenGLRender::InternalSetBlendingFunction(unsigned int nOperator1,unsigned int nOperator2)
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->nBlendOperator1=nOperator1;
	pState->nBlendOperator2=nOperator2;
	if(!m_bStagedRendering)
	{
		if(m_nCurrentBlendOperator1!=nOperator1 ||
			m_nCurrentBlendOperator2!=nOperator2)
		{
			glBlendFunc(nOperator1, nOperator2);
			m_nCurrentBlendOperator1=nOperator1;
			m_nCurrentBlendOperator2=nOperator2;
		}
	}
}

void COpenGLRender::InternalDeactivateBlending()
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveBlending=false;
	if(!m_bStagedRendering)
	{
		glDisable(GL_BLEND);
	}
}

void COpenGLRender::ActivateBlending()
{
	InternalActivateBlending();
	if(!m_bStagedRendering){SetCurrentRenderStateShader();}
}

void COpenGLRender::SetBlendingLayer(unsigned int nLayer)
{
	InternalSetBlendingLayer(nLayer);
	if(!m_bStagedRendering){SetCurrentRenderStateShader();}
}

void COpenGLRender::SetBlendingFunction(unsigned int nOperator1,unsigned int nOperator2)
{
	InternalSetBlendingFunction(nOperator1,nOperator2);
	if(!m_bStagedRendering){SetCurrentRenderStateShader();}
}

void COpenGLRender::DeactivateBlending()
{
	InternalDeactivateBlending();
	if(!m_bStagedRendering){SetCurrentRenderStateShader();}
}

bool COpenGLRender::IsBlendingActive()
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	return pState->bActiveBlending;
}


void COpenGLRender::InternalActivateLighting()
{
	if(!m_sRenderOptions.bEnableLighting){return;}
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveLighting=true;
	if(!m_bStagedRendering){PrepareLighting();}
}

void COpenGLRender::InternalDeactivateLighting()
{
	if(!m_sRenderOptions.bEnableLighting){return;}
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveLighting=false;
	if(!m_bStagedRendering){UnPrepareLighting();}
}

void COpenGLRender::ActivateLighting()
{
	InternalActivateLighting();
	if(!m_bStagedRendering){SetCurrentRenderStateShader();}
}

void COpenGLRender::DeactivateLighting()
{
	InternalDeactivateLighting();
	if(!m_bStagedRendering){SetCurrentRenderStateShader();}
}

void COpenGLRender::AddLight( IGenericLight *piLight)
{
	std::vector<IGenericLight *>::iterator i;
	for(i=m_vLights.begin();i!=m_vLights.end();i++)
	{
		if(*i==piLight){return;}
	}
	m_vLights.push_back(ADD(piLight));
}

void COpenGLRender::RemoveLight( IGenericLight *piLight)
{
	std::vector<IGenericLight *>::iterator i;
	for(i=m_vLights.begin();i!=m_vLights.end();i++)
	{
		if(*i==piLight)
		{
			m_vLights.erase(i);
			REL(piLight);
			return;
		}
	}
}



void COpenGLRender::PrepareLighting()
{
#ifdef ANDROID_GLES1
	if(m_bLightingPrepared){return;}
	
	const GLfloat pfAmbienColor[]  = {(float)m_vAmbientColor.c[0], (float)m_vAmbientColor.c[1], (float)m_vAmbientColor.c[2], 1.0};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,pfAmbienColor);
//	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE ,0);
//	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER ,1);
	glEnable(GL_LIGHTING);
	
	std::vector<IGenericLight *>::iterator i;
	int nlIndex=GL_LIGHT0;
	for(i=m_vLights.begin(); nlIndex<(GL_LIGHT0+MAX_DYNAMIC_LIGHTS) && i!=m_vLights.end();i++)
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
#endif
}

void COpenGLRender::UnPrepareLighting()
{
#ifdef ANDROID_GLES1
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
#endif
}

void COpenGLRender::SetupLightsInShaders()
{
	if(!m_sRenderOptions.bEnableLighting){return;}
	
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
	for(i=m_vLights.begin(); i!=m_vLights.end();i++)
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
		
		pShader->m_piShader->AddUniform("uWorldEyeDir",m_vCameraForward,false);
		pShader->m_piShader->AddUniform("uAmbient",m_vAmbientColor,1.0,false);
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

bool COpenGLRender::IsLightingActive()
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	return pState->bActiveLighting;
}

void COpenGLRender::RenderPoint( const CVector &vPosition,double dSize,const CVector &vColor,double dAlpha )
{
	m_bRenderingPoints=true;

	if(m_bStagedRendering)
	{
		SPointStageKey key(m_sStagedRenderingState,(unsigned long)dSize);
		std::map<SPointStageKey,SPointStage>::iterator iParticleStage;
		SPointStage *pStage=NULL;
		iParticleStage=m_mPointStages.find(key);
		if(iParticleStage==m_mPointStages.end())
		{
			pStage=&m_mPointStages[key];
		}
		else
		{
			pStage=&iParticleStage->second;
		}

		int nBuffers=pStage->vBuffers.size();
		SPointBuffer *pBuffer=nBuffers?pStage->vBuffers[nBuffers-1]:NULL;
		if(pBuffer==NULL || pBuffer->nUsedElements>=POINT_BUFFER_SIZE)
		{
			pBuffer=new SPointBuffer;
			pStage->vBuffers.push_back(pBuffer);
		}
		float *pVertexBuffer=pBuffer->pVertexBuffer+(pBuffer->nUsedElements*3);
		float *pColorBuffer=pBuffer->pColorBuffer+(pBuffer->nUsedElements*4);

		*pVertexBuffer++=(float)vPosition.c[0];
		*pVertexBuffer++=(float)vPosition.c[1];
		*pVertexBuffer++=(float)vPosition.c[2];

		*pColorBuffer++=(float)vColor.c[0];
		*pColorBuffer++=(float)vColor.c[1];
		*pColorBuffer++=(float)vColor.c[2];
		*pColorBuffer++=(float)dAlpha;

		pBuffer->nUsedElements++;
	}
	else
	{
#ifndef ANDROID
		glPointSize((GLfloat)dSize);
#endif
		SetCurrentRenderStateShader();
				
		float pVertexBuffer[3];
		float pColorBuffer[4];
		
		pVertexBuffer[0]=vPosition.c[0];
		pVertexBuffer[1]=vPosition.c[1];
		pVertexBuffer[2]=vPosition.c[2];
		pColorBuffer[0]=vColor.c[0];
		pColorBuffer[1]=vColor.c[1];
		pColorBuffer[2]=vColor.c[2];
		pColorBuffer[3]=dAlpha;
		
		SetVertexPointers(pVertexBuffer,NULL,pColorBuffer,0,NULL);
		glDrawArrays(GL_POINTS,0,1);
	}
	
	m_bRenderingPoints=false;
}

void COpenGLRender::RenderModel(const CVector &vOrigin,const CVector &vOrientation,IGenericModel *piModel,unsigned int nAnimation,unsigned int nFrame)
{
	if(m_bStagedRendering)
	{
		bool bSkip=false;

		if(m_sRenderOptions.bEnableShadows && m_sRenderOptions.bEnableAutoShadowVolume)
		{
			bool    pbBBoxVolumePointOutSide[8];
			CVector vBBoxVolume[8];
			CVector vMins,vMaxs;
			double dRadius=piModel->GetFrameRadius(nAnimation,nFrame);
			piModel->GetFrameBBox(nAnimation,nFrame,&vMins,&vMaxs);

			if(m_sStagedRenderingState.bActiveShadowEmission)
			{
				double dDistanceToLight=(m_SunLight.m_piLight->GetPosition()-vOrigin);
				dDistanceToLight-=dRadius;
				if(dDistanceToLight<m_dMinDistanceToLight){m_dMinDistanceToLight=dDistanceToLight;}
			}
	
			if(!bSkip)
			{
				bSkip=(m_CameraForwardPlane.GetSide(vOrigin)<(m_dPerspectiveNearPlane-dRadius));
			}
			bool bInsideByRadius=false;
			int nInsideByRadius=0;
			if(!bSkip)
			{
				for(int x=0;!bSkip && x<4;x++)
				{
					double dRadiusSide=m_CameraFustrumPlanes[x].GetSide(vOrigin);
					if(dRadiusSide>dRadius)
					{
						bSkip=true;
					}
					else if(dRadiusSide<-dRadius)
					{
						nInsideByRadius++;
					}
				}
			}
			bInsideByRadius=(nInsideByRadius==4);
			if(bInsideByRadius && m_sRenderOptions.bEnableAutoShadowVolume)
			{
				CalcBBoxVolume(vOrigin,vOrientation,vMins,vMaxs,vBBoxVolume);
				for(int x=0;x<8;x++)
				{
					ProcessCameraVertex(vBBoxVolume[x]);
				}
			}
			if(!bInsideByRadius)
			{
				if(!bSkip)
				{
					CalcBBoxVolume(vOrigin,vOrientation,vMins,vMaxs,vBBoxVolume);

					// Se descartan los objetos con todos los puntos de su BBOX detras de la camara
					int nBackPoints=0;
					for(int y=0;y<8;y++)
					{
						if(m_CameraForwardPlane.GetSide(vBBoxVolume[y])<m_dPerspectiveNearPlane)
						{
							nBackPoints++;
						}
						else
						{
							break;
						}
					}
					if(nBackPoints==8)
					{
						bSkip=true;
					}
				}
				int nPassedInsidePointTests=0;
				if(!bSkip)
				{
					// Se contabilizan los puntos que estan dentro y fuera del area de la camara
					// (solo de los 4 planos laterales y superiores)
					int nInsideTestPerVertex[8]={0};
					for(int x=0;x<4;x++)
					{
						int nOutPoints=0;
						for(int y=0;y<8;y++)
						{
							if(m_CameraFustrumPlanes[x].GetSide(vBBoxVolume[y])>0)
							{
								nOutPoints++;
							}
							else
							{
								nInsideTestPerVertex[y]++;
								nPassedInsidePointTests++;
							}
						}
						// Si todos los puntos estan detras de uno de los planos, el objeto esta fuera
						if(nOutPoints==8)
						{
							bSkip=true;
							break;
						}
					}
					if(!bSkip)
					{
						for(int y=0;y<8;y++)
						{
							pbBBoxVolumePointOutSide[y]=(nInsideTestPerVertex[y]!=4);
						}
					}				
				}
				if(m_sRenderOptions.bEnableAutoShadowVolume && !bSkip)
				{
					// Si todos los puntos estan dentro de los 4 planos el objeto esta completamente dentro de la camara
					// por lo que se toman sus puntos para calcular e volumnen de las sombras y las zetas de camara optimas
					if(nPassedInsidePointTests==32)
					{
						for(int x=0;x<8;x++)
						{
							ProcessCameraVertex(vBBoxVolume[x]);
						}
					}
					else
					{
						// El objeto esta parcialmente dentro de la camara
						// Se intersecan los poligonos que forman el box con el volumen de la camara paraa
						// hallar el volumen de visualizacion optimo

						bool	pbVertexOutSide[6][4];
						CVector ppVertexes[6][4];
						CPolygon polygons[6];
						for(int x=0;x<6;x++)
						{
							polygons[x].m_pVertexes=ppVertexes[x];
							polygons[x].m_nVertexes=4;
						}
						polygons[0].m_pVertexes[0]=vBBoxVolume[0];
						polygons[0].m_pVertexes[1]=vBBoxVolume[1];
						polygons[0].m_pVertexes[2]=vBBoxVolume[2];
						polygons[0].m_pVertexes[3]=vBBoxVolume[3];

						polygons[1].m_pVertexes[0]=vBBoxVolume[4];
						polygons[1].m_pVertexes[1]=vBBoxVolume[5];
						polygons[1].m_pVertexes[2]=vBBoxVolume[6];
						polygons[1].m_pVertexes[3]=vBBoxVolume[7];

						polygons[2].m_pVertexes[0]=vBBoxVolume[0];
						polygons[2].m_pVertexes[1]=vBBoxVolume[1];
						polygons[2].m_pVertexes[2]=vBBoxVolume[5];
						polygons[2].m_pVertexes[3]=vBBoxVolume[4];

						polygons[3].m_pVertexes[0]=vBBoxVolume[1];
						polygons[3].m_pVertexes[1]=vBBoxVolume[2];
						polygons[3].m_pVertexes[2]=vBBoxVolume[6];
						polygons[3].m_pVertexes[3]=vBBoxVolume[5];

						polygons[4].m_pVertexes[0]=vBBoxVolume[2];
						polygons[4].m_pVertexes[1]=vBBoxVolume[3];
						polygons[4].m_pVertexes[2]=vBBoxVolume[7];
						polygons[4].m_pVertexes[3]=vBBoxVolume[6];

						polygons[5].m_pVertexes[0]=vBBoxVolume[3];
						polygons[5].m_pVertexes[1]=vBBoxVolume[0];
						polygons[5].m_pVertexes[2]=vBBoxVolume[4];
						polygons[5].m_pVertexes[3]=vBBoxVolume[7];

						pbVertexOutSide[0][0]=pbBBoxVolumePointOutSide[0];
						pbVertexOutSide[0][1]=pbBBoxVolumePointOutSide[1];
						pbVertexOutSide[0][2]=pbBBoxVolumePointOutSide[2];
						pbVertexOutSide[0][3]=pbBBoxVolumePointOutSide[3];

						pbVertexOutSide[1][0]=pbBBoxVolumePointOutSide[4];
						pbVertexOutSide[1][1]=pbBBoxVolumePointOutSide[5];
						pbVertexOutSide[1][2]=pbBBoxVolumePointOutSide[6];
						pbVertexOutSide[1][3]=pbBBoxVolumePointOutSide[7];

						pbVertexOutSide[2][0]=pbBBoxVolumePointOutSide[0];
						pbVertexOutSide[2][1]=pbBBoxVolumePointOutSide[1];
						pbVertexOutSide[2][2]=pbBBoxVolumePointOutSide[5];
						pbVertexOutSide[2][3]=pbBBoxVolumePointOutSide[4];

						pbVertexOutSide[3][0]=pbBBoxVolumePointOutSide[1];
						pbVertexOutSide[3][1]=pbBBoxVolumePointOutSide[2];
						pbVertexOutSide[3][2]=pbBBoxVolumePointOutSide[6];
						pbVertexOutSide[3][3]=pbBBoxVolumePointOutSide[5];

						pbVertexOutSide[4][0]=pbBBoxVolumePointOutSide[2];
						pbVertexOutSide[4][1]=pbBBoxVolumePointOutSide[3];
						pbVertexOutSide[4][2]=pbBBoxVolumePointOutSide[7];
						pbVertexOutSide[4][3]=pbBBoxVolumePointOutSide[6];

						pbVertexOutSide[5][0]=pbBBoxVolumePointOutSide[3];
						pbVertexOutSide[5][1]=pbBBoxVolumePointOutSide[0];
						pbVertexOutSide[5][2]=pbBBoxVolumePointOutSide[4];
						pbVertexOutSide[5][3]=pbBBoxVolumePointOutSide[7];

						for(int x=0;x<6;x++)
						{
							for(int y=0;y<4;y++)
							{
								CPolygon fake1,fake2;
								CLine lCutSegment;
								if(CPolygon::Divide(m_CameraFustrumPlanes[y],&polygons[x],&fake1,&fake2,NULL,&lCutSegment)>1)
								{
									int nLineCutPlanes[2]={0};
									if(y==0 || y==2){nLineCutPlanes[0]=1;nLineCutPlanes[1]=3;}
									if(y==1 || y==3){nLineCutPlanes[0]=0;nLineCutPlanes[1]=2;}

									bool bContinue=true;
									CLine lCuttedLine;
									lCuttedLine=lCutSegment;
									for(int z=0;z<2;z++)
									{
										CPlane *pPlane=&m_CameraFustrumPlanes[nLineCutPlanes[z]];
										double d1=pPlane->GetSide(lCutSegment.m_Points[0]);
										double d2=pPlane->GetSide(lCutSegment.m_Points[1]);
										double length=d1-d2;
										if(d1>0 && d2>0)
										{
											bContinue=false;
											break;
										}
										if(d1<0 && d2<0)
										{
											continue;
										}
										if(d1>0)
										{
											double dFraction=length?d1/length:0;
											lCuttedLine.m_Points[0]=lCutSegment.m_Points[0]+(lCutSegment.m_Points[1]-lCutSegment.m_Points[0])*dFraction;
										}
										else if(d2>0)
										{
											double dFraction=length?d1/length:0;
											lCuttedLine.m_Points[1]=lCutSegment.m_Points[0]+(lCutSegment.m_Points[1]-lCutSegment.m_Points[0])*dFraction;
										}
									}
									if(bContinue)
									{
									
										for(int z=0;z<2;z++)
										{
											ProcessCameraVertex(lCuttedLine.m_Points[z]);
										}
									}
								}
								else 
								{
									for(int z=0;z<4;z++)
									{
										if(!pbVertexOutSide[x][z])
										{
											ProcessCameraVertex(polygons[x].m_pVertexes[z]);
										}
									}
								}
							}

						}
						for(int x=0;x<6;x++){polygons[x].m_pVertexes=NULL;polygons[x].m_nVertexes=0;}
					}
				}
			}
		}

		SModelStageKey key(piModel,nAnimation,nFrame,m_sStagedRenderingState);
		std::map<SModelStageKey,SModelStage>::iterator iModelStage;
		SModelStage *pStage=NULL;
		iModelStage=m_mModelStages.find(key);
		if(iModelStage==m_mModelStages.end())
		{
			pStage=&m_mModelStages[key];
			pStage->piGLModel=QI(IOpenGLModel,piModel);
			pStage->piModel=ADD(piModel);
		}
		else
		{
			pStage=&iModelStage->second;
		}

		SModelInstance sInstance;
		sInstance.vAngles=vOrientation;
		sInstance.vPos=vOrigin;
		sInstance.dDistanceToCamera=m_CameraForwardPlane.GetSide(vOrigin);
		sInstance.bSkipRender=bSkip;
		sInstance.nId=pStage->vInstances.size();
		if(pStage->vInstances.size()==0 || sInstance.dDistanceToCamera<pStage->dNearestModel)
		{
			pStage->dNearestModel=sInstance.dDistanceToCamera;
		}
		pStage->vInstances.push_back(sInstance);
	}
	else
	{
#ifdef ANDROID_GLES1
		glPushMatrix();
		glTranslatef(vOrigin.c[0],vOrigin.c[1],vOrigin.c[2]);
		glRotatef(vOrientation.c[YAW]	,0,1,0);
		glRotatef(vOrientation.c[PITCH]	,0,0,1);
		glRotatef(vOrientation.c[ROLL]	,1,0,0);
#else
		CMatrix m,tmp;
		if(vOrientation.c[ROLL]){tmp.R(AxisPosX,DegreesToRadians(vOrientation.c[ROLL]));m*=tmp;}
		if(vOrientation.c[PITCH]){tmp.R(AxisPosZ,DegreesToRadians(vOrientation.c[PITCH]));m*=tmp;}
		if(vOrientation.c[YAW]){tmp.R(AxisPosY,DegreesToRadians(vOrientation.c[YAW]));m*=tmp;}
		if(vOrigin.c[0] || vOrigin.c[1] || vOrigin.c[2]){tmp.T(vOrigin);m*=tmp;}
#endif
		IOpenGLModel *piGLModel=QI(IOpenGLModel,piModel);
		if(piGLModel)
		{
			for(unsigned long x=0;x<piModel->GetFrameRenderBuffers(nAnimation,nFrame);x++)
			{
				piGLModel->PrepareRenderBuffer(this,nAnimation,nFrame,x,false,&m_RenderMappings);
				SetCurrentRenderStateShader();
#ifndef ANDROID_GLES1
				if(m_pCurrentShader){m_pCurrentShader->m_piShader->AddUniform("uModel",m,true);}
#endif
				piGLModel->CallRenderBuffer(this,nAnimation,nFrame,x,1);
				piGLModel->UnPrepareRenderBuffer(this,nAnimation,nFrame,x,false,&m_RenderMappings);
			}
		}
		REL(piGLModel);
#ifdef ANDROID_GLES1
		glPopMatrix();
#endif
	}
}

void COpenGLRender::RenderParticle(IGenericTexture *piTexture,const CVector &vOrigin,double dAngle,double s1,double s2,double dTextX,double dTextY,double dTextW,double dTextH,const CVector &vColor,double dAlpha)
{
	if(m_bStagedRendering)
	{
		STextureParticleStageKey key(piTexture,m_sStagedRenderingState);
		std::map<STextureParticleStageKey,STextureParticleStage>::iterator iParticleStage;
		STextureParticleStage *pStage=NULL;
		iParticleStage=m_mTextureParticleStages.find(key);
		if(iParticleStage==m_mTextureParticleStages.end())
		{
			ADD(key.piTexture);
			pStage=&m_mTextureParticleStages[key];
		}
		else
		{
			pStage=&iParticleStage->second;
		}
		
		int nBuffers=pStage->vBuffers.size();
		STextureParticleBuffer *pBuffer=nBuffers?pStage->vBuffers[nBuffers-1]:NULL;
		if(pBuffer==NULL || pBuffer->nUsedElements>=TEXTURE_PARTICLE_BUFFER_SIZE)
		{
			pBuffer=new STextureParticleBuffer;
			pStage->vBuffers.push_back(pBuffer);
		}
		float *pVertexBuffer=pBuffer->pVertexBuffer+(pBuffer->nUsedElements*6*3);
		float *pColorBuffer=pBuffer->pColorBuffer+(pBuffer->nUsedElements*6*4);
		float *pTexBuffer=pBuffer->pTexBuffers+(pBuffer->nUsedElements*6*2);

		CMatrix m;
		m.R(m_vCameraForward,DegreesToRadians(dAngle));

		CVector vAxis1=m_vCameraRight*(s1/2.0);
		CVector vAxis2=m_vCameraUp*(s2/2.0);
		CVector vVertex[6];
		vVertex[0]=(vAxis1+vAxis2);
		vVertex[1]=(Origin-vAxis1+vAxis2);
		vVertex[2]=(Origin-vAxis1-vAxis2);
		vVertex[3]=vVertex[0];
		vVertex[4]=vVertex[2];
		vVertex[5]=(vAxis1-vAxis2);
		
		for(int x=0;x<6;x++)
		{
			vVertex[x]*=m;
			*pVertexBuffer++=(float)(vOrigin.c[0]+vVertex[x].c[0]);
			*pVertexBuffer++=(float)(vOrigin.c[1]+vVertex[x].c[1]);
			*pVertexBuffer++=(float)(vOrigin.c[2]+vVertex[x].c[2]);
			*pColorBuffer++=(float)vColor.c[0];
			*pColorBuffer++=(float)vColor.c[1];
			*pColorBuffer++=(float)vColor.c[2];
			*pColorBuffer++=(float)dAlpha;
		}
		*pTexBuffer++=dTextX+dTextW;*pTexBuffer++=dTextY+dTextH;
		*pTexBuffer++=dTextX;*pTexBuffer++=dTextY+dTextH;
		*pTexBuffer++=dTextX;*pTexBuffer++=dTextY;
		*pTexBuffer++=dTextX+dTextW;*pTexBuffer++=dTextY+dTextH;
		*pTexBuffer++=dTextX;*pTexBuffer++=dTextY;
		*pTexBuffer++=dTextX+dTextW;*pTexBuffer++=dTextY;
		pBuffer->nUsedElements++;
	}
}

void COpenGLRender::StartStagedRendering()
{
	m_sPreStagedRenderingState=m_sRenderState;
	m_sStagedRenderingState=m_sRenderState;
	SetRenderState(m_sRenderState,eStateChange_Force);
	m_bStagedRendering=true;
	m_dStagedRenderingMinZ=m_dPerspectiveFarPlane;
	m_dStagedRenderingMaxZ=m_dPerspectiveNearPlane;

	double dCameraAspect=((m_nViewportH)==0)?1:(double)(m_nViewportW)/(double)(m_nViewportH);
	CalcCameraPlanes(m_vCameraPos,m_vCameraAngles,m_dPerspectiveViewAngle,dCameraAspect,m_dPerspectiveNearPlane,m_dPerspectiveFarPlane,m_CameraFustrumPlanes);

	m_CameraForwardPlane=CPlane(m_vCameraForward,m_vCameraPos);
	m_CameraRightPlane=CPlane(m_vCameraRight,m_vCameraPos);
	m_CameraUpPlane=CPlane(m_vCameraUp,m_vCameraPos);
	
	m_dMinDistanceToLight=10000;

	m_bShadowVolumeFirstVertex=true;
	
	
	if(!m_sRenderOptions.bEnableAutoShadowVolume)
	{		
		CVector pVolume[8];
		CalcCameraVolume(m_vCameraPos,m_vCameraAngles, m_dPerspectiveViewAngle,dCameraAspect,m_dPerspectiveNearPlane, m_dPerspectiveFarPlane,pVolume);
		for(int x=0;x<8;x++)
		{
			ProcessCameraVertex(pVolume[x]);
		}
	}
}
#ifdef ANDROID_GLES1
void COpenGLRender::PrepareSunShadows(){}
void COpenGLRender::UnprepareSunShadows(){}
#else
void COpenGLRender::PrepareSunShadows()
{
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
	if(m_sHardwareSupport.bShaders)
	{
		if(m_ShadowShader.m_piShader==NULL)
		{
			if(m_ShadowShader.Create(m_piSystem,"Shader",""))
			{
#ifdef ANDROID
				m_ShadowShader.m_piShader->Load("Shaders/Android-ShadowBufferShader-Vertex.c","Shaders/Android-ShadowBufferShader-Fragment.c","");
#else
				m_ShadowShader.m_piShader->Load("Shaders/ShadowBufferShader-Vertex.c","Shaders/ShadowBufferShader-Fragment.c","");
#endif
			}
		}
	}
	
	if(m_ShadowTexture.m_piTexture==NULL){return;}
		
	CVector vLightAngles;
	CVector pvCameraVolume[8];
	CVector pvLightVolume[8];
	CVector pvShadowVolume[8];
	CVector vLightPosition;
	CVector vVolumeMidPoint;
	CVector vLightForward;
	double dLightNear;
	double dLightFar;
	
	double pCameraViewMatrix[16];
	double pCameraProjectionMatrix[16];
	CMatrix cameraViewMatrix=m_ModelViewMatrix;
	CMatrix cameraProjectionMatrix=m_ProjectionMatrix;
	CMatrix identity;
	double dPreviousNear=m_dPerspectiveNearPlane;
	double dPreviousFar=m_dPerspectiveFarPlane;
	double dPreviousViewAngle=m_dPerspectiveViewAngle;

	unsigned int nShadowStart=0;
	if(m_sRenderOptions.bEnableStagedRenderingStats){nShadowStart=GetTimeStamp();}
	
	int nPreviousViewportX=m_nViewportX;
	int nPreviousViewportY=m_nViewportY;
	int nPreviousViewportW=m_nViewportW;
	int nPreviousViewportH=m_nViewportH;
	double dPreviousViewAspect=((m_nViewportH)==0)?1:(double)(m_nViewportW)/(double)(m_nViewportH);
	CVector vPreviousCameraPosition=m_vCameraPos;
	CVector vPreviousCameraAngles=m_vCameraAngles;
	
	CalcCameraVolume(vPreviousCameraPosition,vPreviousCameraAngles,dPreviousViewAngle,dPreviousViewAspect,m_dStagedRenderingMinZ,m_dStagedRenderingMaxZ,pvCameraVolume);
	CalcBBoxVolume(Origin,Origin,m_vShadowVolumeMins,m_vShadowVolumeMaxs,pvShadowVolume);
	
	for(int x=0;x<8;x++){vVolumeMidPoint+=pvShadowVolume[x];}
	vVolumeMidPoint/=8.0;
	CVector vLightRight,vLightUp;
	vLightPosition=m_SunLight.m_piLight->GetPosition()+m_vCameraPos;//vVolumeMidPoint-vLightForward*500.0;
	
	CVector *pVolumeToUse=pvShadowVolume;
	if(fabs(m_vLastShadowCameraTarget.c[0]-vVolumeMidPoint.c[0])>m_dShadowAntiFlickeringMargin ||
		fabs(m_vLastShadowCameraTarget.c[1]-vVolumeMidPoint.c[1])>m_dShadowAntiFlickeringMargin ||
		fabs(m_vLastShadowCameraTarget.c[2]-vVolumeMidPoint.c[2])>m_dShadowAntiFlickeringMargin ||
		fabs(m_vLastShadowCameraPosition.c[0]-vLightPosition.c[0])>m_dShadowAntiFlickeringMargin ||
		fabs(m_vLastShadowCameraPosition.c[1]-vLightPosition.c[1])>m_dShadowAntiFlickeringMargin ||
		fabs(m_vLastShadowCameraPosition.c[2]-vLightPosition.c[2])>m_dShadowAntiFlickeringMargin)
	{
		m_vLastShadowCameraTarget=vVolumeMidPoint;
		m_vLastShadowCameraPosition=vLightPosition;
		for(int x=0;x<8;x++){m_pvLastShadowVolume[x]=pvShadowVolume[x];}
	}
	else
	{
		vVolumeMidPoint=m_vLastShadowCameraTarget;
		vLightPosition=m_vLastShadowCameraPosition;
		for(int x=0;x<8;x++){pvShadowVolume[x]=m_pvLastShadowVolume[x];}
	}
	vLightForward=m_vLastShadowCameraTarget-vLightPosition;
	vLightForward.N();
	vLightAngles=AnglesFromVector(vLightForward);
	VectorsFromAngles(vLightAngles,NULL,&vLightRight,&vLightUp);
	
	double dLightMinRigthAngle=0,dLightMaxRigthAngle=0;
	double dLightMinUpAngle=0,dLightMaxUpAngle=0;
	dLightNear=100000;
	dLightFar=0;
	CPlane forwardPlane(vLightForward,vLightPosition);
	CPlane rightPlane(vLightRight,vLightPosition);
	CPlane upPlane(vLightUp,vLightPosition);
	for(int x=0;x<8;x++)
	{
		double dDistance=forwardPlane.GetSide(pVolumeToUse[x]);
		double dRight=rightPlane.GetSide(pVolumeToUse[x])+m_dShadowAntiFlickeringMargin;
		double dUp=upPlane.GetSide(pVolumeToUse[x])+m_dShadowAntiFlickeringMargin;
		dLightNear=std::min(dLightNear,dDistance);
		dLightFar=std::max(dLightFar,dDistance);
		
		if(dDistance)
		{
			double dRightAngle=RadiansToDegrees(atan2(dRight,dDistance));
			double dUpAngle=RadiansToDegrees(atan2(dUp,dDistance));
			dLightMinRigthAngle=std::min(dLightMinRigthAngle,dRightAngle);
			dLightMaxRigthAngle=std::max(dLightMaxRigthAngle,dRightAngle);
			dLightMinUpAngle=std::min(dLightMinUpAngle,dUpAngle);
			dLightMaxUpAngle=std::max(dLightMaxUpAngle,dUpAngle);
		}
	}
	double dViewAngleRight=std::max(fabs(dLightMinRigthAngle),fabs(dLightMaxRigthAngle));
	double dViewAngleUp=std::max(fabs(dLightMinUpAngle),fabs(dLightMaxUpAngle));
	
	dLightNear=std::min(dLightNear,m_dMinDistanceToLight);
	if(dLightNear<1){dLightNear=1;}
	if(dLightFar<1){dLightFar=1;}
	
	double dLightViewAngle=std::max(dViewAngleRight,dViewAngleUp)*2.0;//dPreviousViewAngle*0.5;
	double pLightModelViewMatrix[16];
	
	m_ShadowTexture.m_piTexture->StartRenderingToTexture();
	
	unsigned dwWidth=0,dwHeight=0;
	m_ShadowTexture.m_piTexture->GetSize(&dwWidth,&dwHeight);
	SetViewport(0, 0, dwWidth, dwHeight);
	SetPerspectiveProjection(dLightViewAngle,dLightNear,dLightFar);
	SetCamera(vLightPosition,vLightAngles.c[YAW],vLightAngles.c[PITCH],vLightAngles.c[ROLL]);
	
	CMatrix lightModelViewMatrix=m_ModelViewMatrix;
	CMatrix lightProjectionMatrix=m_ProjectionMatrix;
	
	CalcCameraVolume(vLightPosition,vLightAngles,dLightViewAngle,1,dLightNear,dLightFar,pvLightVolume);
	
	glClear(GL_DEPTH_BUFFER_BIT);
	
	CGenericShaderWrapper *pPreviousShader=NULL;
	if(m_ShadowShader.m_piShader)
	{
		pPreviousShader=m_pCurrentShader;
		if(pPreviousShader){pPreviousShader->m_piShader->Deactivate();}
		m_pCurrentShader=&m_ShadowShader;
		m_ShadowShader.m_piShader->Activate();
	}
	RenderModelStages(true,false);
	RenderModelStages(true,true);
	if(m_ShadowShader.m_piShader){m_ShadowShader.m_piShader->Deactivate();}
	if(pPreviousShader){pPreviousShader->m_piShader->Activate();m_pCurrentShader=pPreviousShader;}
	
	m_ShadowTexture.m_piTexture->StopRenderingToTexture();
	
	SetViewport(nPreviousViewportX, nPreviousViewportY,nPreviousViewportW, nPreviousViewportH);
	SetPerspectiveProjection(dPreviousViewAngle,dPreviousNear,dPreviousFar);//m_dStagedRenderingMinZ>1.0?m_dStagedRenderingMinZ-1.0:m_dStagedRenderingMinZ,m_dStagedRenderingMaxZ+1.0);
	SetCamera(vPreviousCameraPosition,vPreviousCameraAngles.c[YAW],vPreviousCameraAngles.c[PITCH],vPreviousCameraAngles.c[ROLL]);
	
	// Se hace esto porque el inicio y fin del pintado a textura hace un push attrib y pop attrib
	// ya que pueden estar en otro contexto de render dependiendo de la tecnica usada.
	SetRenderState(m_sStagedRenderingState,eStateChange_Force);
	
	CMatrix biasMatrix;
	biasMatrix.e[0][0]=0.5f;
	biasMatrix.e[0][3]=0.5f;
	biasMatrix.e[1][1]=0.5f;
	biasMatrix.e[1][3]=0.5f;
	biasMatrix.e[2][2]=0.5f;
	biasMatrix.e[2][3]=0.5f;
	biasMatrix.e[3][3]=1.0f;
	
	CMatrix shadowMatrix;
	shadowMatrix.T(Origin-m_vCameraForward);
	shadowMatrix*=lightModelViewMatrix;
	shadowMatrix*=lightProjectionMatrix;
	shadowMatrix*=biasMatrix;
	
	
	if(m_nCurrentActiveTexture!=m_nShadowTextureLevel){glActiveTexture(GL_TEXTURE0_ARB+m_nShadowTextureLevel);m_nCurrentActiveTexture=m_nShadowTextureLevel;}
	m_ShadowTexture.m_piTexture->PrepareTexture(this,m_nShadowTextureLevel);
	
	std::map<SShaderKey,CGenericShaderWrapper>::iterator iShader;
	for(iShader=m_mShaders.begin();iShader!=m_mShaders.end();iShader++)
	{
		if(iShader->first.bShadows)
		{
			iShader->second.m_piShader->AddUniform("uShadowMatrix",shadowMatrix,false);
		}
	}
	if(m_sRenderOptions.bEnableStagedRenderingStats)
	{
		glFinish();
		m_sStagedStats.nShadowTime=GetTimeStamp()-nShadowStart;
	}
}

void COpenGLRender::UnprepareSunShadows()
{
	if(m_nCurrentActiveTexture!=m_nShadowTextureLevel){glActiveTexture(GL_TEXTURE0_ARB+m_nShadowTextureLevel);m_nCurrentActiveTexture=m_nShadowTextureLevel;}
	if(m_ShadowTexture.m_piTexture){m_ShadowTexture.m_piTexture->UnprepareTexture(this,m_nShadowTextureLevel);}
}
#endif

void COpenGLRender::SortModels()
{
	// Sort models in each stage and stages by nearest model
	m_mSortedModelStages.clear();
	std::map<SModelStageKey,SModelStage>::iterator iModelStage;
	for(iModelStage=m_mModelStages.begin();iModelStage!=m_mModelStages.end();iModelStage++)
	{
		const SModelStageKey *pKey=&iModelStage->first;
		SModelStage *pStage=&iModelStage->second;
		std::sort(iModelStage->second.vInstances.begin(), iModelStage->second.vInstances.end());
		m_mSortedModelStages.insert(std::pair<double,std::pair<const SModelStageKey*,const SModelStage*> >(pStage->dNearestModel,std::pair<const SModelStageKey*,const SModelStage*>(pKey,pStage)));
	}	
}

void COpenGLRender::EndStagedRendering()
{
	if(!m_bStagedRendering){return;}
	
	// Sort models by its distance to the camera.
	SortModels();	
	
	unsigned int nRenderStart=0;
	float fCurrentTime=((double)(GetTimeStamp()-m_nFirstTimeStamp))*0.001;
	
	m_bStagedRendering=false;
	
	if(m_sRenderOptions.bEnableStagedRenderingStats){nRenderStart=GetTimeStamp();m_sStagedStats=SRenderStats();}

	SetupLightsInShaders();
	
	std::map<SShaderKey,CGenericShaderWrapper>::iterator iShader;
	for(iShader=m_mShaders.begin();iShader!=m_mShaders.end();iShader++)
	{
		if(iShader->first.bSkyShadow){iShader->second.m_piShader->AddUniform("SkyData",CVector(fCurrentTime*m_dSKyShadowSpeed,m_dSKyShadowXResolution,m_dSKyShadowZResolution),m_dSKyShadowOpacity,false);}
		if(iShader->first.bWater)    {iShader->second.m_piShader->AddUniform("CurrentRealTime",fCurrentTime,false);}
	}

	if(m_nCurrentActiveTexture!=m_nSkyShadowTextureLevel){glActiveTexture(GL_TEXTURE0_ARB+m_nSkyShadowTextureLevel);m_nCurrentActiveTexture=m_nSkyShadowTextureLevel;}
	if(m_piSkyShadow && m_sRenderOptions.bEnableSkyShadow){m_piSkyShadow->PrepareTexture(this,m_nSkyShadowTextureLevel);}
	
	if(m_sRenderOptions.bEnableShadows){PrepareSunShadows();}
	RenderAllStages(false,true);
	if(m_sRenderOptions.bEnableShadows){UnprepareSunShadows();}
	RenderAllStages(false,false);
	
	
	if(m_nCurrentActiveTexture!=m_nSkyShadowTextureLevel){glActiveTexture(GL_TEXTURE0_ARB+m_nSkyShadowTextureLevel);m_nCurrentActiveTexture=m_nSkyShadowTextureLevel;}
	if(m_piSkyShadow && m_sRenderOptions.bEnableSkyShadow){m_piSkyShadow->UnprepareTexture(this,m_nSkyShadowTextureLevel);}
	
	std::map<SModelStageKey,SModelStage>::iterator iModelStage;
	for(iModelStage=m_mModelStages.begin();iModelStage!=m_mModelStages.end();iModelStage++)
	{
		SModelStage *pStage=&iModelStage->second;
		REL(pStage->piModel);
		REL(pStage->piGLModel);
	}
	std::map<STextureParticleStageKey,STextureParticleStage>::iterator iParticleStage;
	for(iParticleStage=m_mTextureParticleStages.begin();iParticleStage!=m_mTextureParticleStages.end();iParticleStage++)
	{
		IGenericTexture *piTexture=iParticleStage->first.piTexture;
		STextureParticleStage *pStage=&iParticleStage->second;
		
		for(unsigned int x=0;x<pStage->vBuffers.size();x++)
		{
			STextureParticleBuffer *pBuffer=pStage->vBuffers[x];
			delete pBuffer;
		}
		REL(piTexture);
	}
	std::map<SLineStageKey,SLineStage>::iterator iLineStage;
	for(iLineStage=m_mLineStages.begin();iLineStage!=m_mLineStages.end();iLineStage++)
	{
		SLineStage *pStage=&iLineStage->second;
		for(unsigned int x=0;x<pStage->vBuffers.size();x++)
		{
			SLineBuffer *pBuffer=pStage->vBuffers[x];
			delete pBuffer;
		}
	}
	std::map<SPointStageKey,SPointStage>::iterator iPointStage;
	for(iPointStage=m_mPointStages.begin();iPointStage!=m_mPointStages.end();iPointStage++)
	{
		SPointStage *pStage=&iPointStage->second;
		for(unsigned int x=0;x<pStage->vBuffers.size();x++)
		{
			SPointBuffer *pBuffer=pStage->vBuffers[x];
			delete pBuffer;
		}
	}

	m_mModelStages.clear();
	m_mSortedModelStages.clear();
	m_mTextureParticleStages.clear();
	m_mLineStages.clear();
	m_mPointStages.clear();
	
	if(m_sRenderOptions.bEnableStagedRenderingStats)
	{
		glFinish();
		m_sStagedStats.nRenderTime=GetTimeStamp()-nRenderStart;
	}
	
	SetRenderState(m_sPreStagedRenderingState,eStateChange_Force);
}

void COpenGLRender::SetShadingModel(EShadingModel eModel)
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->eShadingModel=eModel;
}

EShadingModel 	COpenGLRender::GetShadingModel()
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	return pState->eShadingModel;
}

SRenderStats COpenGLRender::GetStagedRenderingStats()
{
	return m_sStagedStats;
}

void COpenGLRender::SetRenderState( const SRenderState &sNewState,EStateChangePolicy ePolicy,EStateChangeShader eShader)
{
	m_sRenderState.bActiveShadowEmission=sNewState.bActiveShadowEmission;
	m_sRenderState.bActiveShadowReception=sNewState.bActiveShadowReception;
	m_sRenderState.bActiveWater=sNewState.bActiveWater;
	m_sRenderState.eShadingModel=sNewState.eShadingModel;
	
	bool bStateChange=false;
	if(ePolicy==eStateChange_Force || m_sRenderState.bActiveTextures!=sNewState.bActiveTextures)
	{
		sNewState.bActiveTextures?InternalActivateTextures():InternalDeactivateTextures();
		bStateChange=true;
	}
	if(ePolicy==eStateChange_Force || m_sRenderState.bActiveSolid!=sNewState.bActiveSolid)
	{
		sNewState.bActiveSolid?InternalActivateSolid():InternalDeactivateSolid();
		bStateChange=true;
	}
	if(ePolicy==eStateChange_Force || m_sRenderState.bActiveLighting!=sNewState.bActiveLighting)
	{
		sNewState.bActiveLighting?InternalActivateLighting():InternalDeactivateLighting();
		bStateChange=true;
	}
	if(ePolicy==eStateChange_Force || m_sRenderState.bActiveBlending!=sNewState.bActiveBlending)
	{
		sNewState.bActiveBlending?InternalActivateBlending():InternalDeactivateBlending();
		bStateChange=true;
	}
	if(ePolicy==eStateChange_Force || m_sRenderState.nBlendOperator1!=sNewState.nBlendOperator1 ||
		m_sRenderState.nBlendOperator2!=sNewState.nBlendOperator2)
	{
		InternalSetBlendingFunction(sNewState.nBlendOperator1,sNewState.nBlendOperator2);
		bStateChange=true;
	}
	if(ePolicy==eStateChange_Force || m_sRenderState.bActiveDepth!=sNewState.bActiveDepth)
	{
		sNewState.bActiveDepth?InternalActivateDepth():InternalDeactivateDepth();
		bStateChange=true;
	}
	if(ePolicy==eStateChange_Force || m_sRenderState.bActiveSkyShadow!=sNewState.bActiveSkyShadow)
	{
		sNewState.bActiveSkyShadow?ActivateSkyShadow():DeactivateSkyShadow();
	}
	if(ePolicy==eStateChange_Force || m_sRenderState.nDepthFunction!=sNewState.nDepthFunction)
	{
		SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
		pState->nDepthFunction=sNewState.nDepthFunction;
		if(!m_bStagedRendering)
		{
			if(m_nCurrentDepthFunc!=sNewState.nDepthFunction)
			{
				glDepthFunc(sNewState.nDepthFunction);
				m_nCurrentDepthFunc=sNewState.nDepthFunction;
			}
		}
		bStateChange=true;
	}
	if(ePolicy==eStateChange_Force || m_sRenderState.bActiveHeightFog!=sNewState.bActiveHeightFog)
	{
		sNewState.bActiveHeightFog?ActivateHeightFog(sNewState.vHeightFogMins,sNewState.vHeightFogMaxs,sNewState.vHeightFogColor):DeactivateHeightFog();
	}
	if(bStateChange && m_sRenderOptions.bEnableStagedRenderingStats){m_sStagedStats.nStateChanges++;}
	   
	   
	if(eShader==eStateChange_UpdateShader)
	{
		SetCurrentRenderStateShader();
	}
}

void COpenGLRender::SetCurrentRenderStateShader()
{
	if(m_bSelecting){return;}
	
#ifndef ANDROID_GLES1
	bool bTempSkyShadow=m_piSkyShadow && m_piSkyShadow && m_sRenderOptions.bEnableSkyShadow && m_sRenderState.bActiveSkyShadow && m_sRenderState.bActiveShadowReception;
	SShaderKey key(m_sRenderState.eShadingModel,m_sRenderState.bActiveHeightFog,m_sRenderOptions.bEnableShadows && m_bRenderingShadowReception,m_sRenderState.bActiveTextures && m_sRenderOptions.bEnableTextures?m_mTextureLevels.size():0,m_sRenderOptions.bEnableLighting && m_sRenderState.bActiveLighting,m_sRenderState.bActiveWater,m_sRenderOptions.bEnableNormalMaps && m_piNormalMap,bTempSkyShadow,m_bRenderingPoints);
	std::map<SShaderKey,CGenericShaderWrapper>::iterator iShader=m_mShaders.find(key);
	CGenericShaderWrapper *pNewShader=(iShader==m_mShaders.end())?NULL:&iShader->second;
	if(!pNewShader)
	{
		AddShader(key);
		iShader=m_mShaders.find(key);
		pNewShader=(iShader==m_mShaders.end())?NULL:&iShader->second;
		if(pNewShader==NULL)
		{
			RTTRACE("COpenGLRender::SetRenderState -> Failed to create on-demand shader");
		}
	}
	if(m_pCurrentShader!=pNewShader)
	{
		if(m_pCurrentShader){m_pCurrentShader->m_piShader->Deactivate();}
		m_pCurrentShader=pNewShader;
		
		if(m_pCurrentShader)
		{
			if(m_sRenderOptions.bEnableStagedRenderingStats){m_sStagedStats.nShaderChanges++;}
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
#endif
}

void COpenGLRender::EnableNormalMaps()
{
	if(m_nNormalMapTextureLevel>=m_sHardwareSupport.nMaxTextureUnits){return;}
	m_sRenderOptions.bEnableNormalMaps=true;
}
void COpenGLRender::DisableNormalMaps(){m_sRenderOptions.bEnableNormalMaps=false;}
bool COpenGLRender::AreNormalMapsEnabled(){return m_sRenderOptions.bEnableNormalMaps;}

void COpenGLRender::EnableSkyShadow()
{
	if(m_nSkyShadowTextureLevel>=m_sHardwareSupport.nMaxTextureUnits){return;}
	m_sRenderOptions.bEnableSkyShadow=true;
}
void COpenGLRender::DisableSkyShadow(){m_sRenderOptions.bEnableSkyShadow=false;}
bool COpenGLRender::IsSkyShadowEnabled(){return m_sRenderOptions.bEnableSkyShadow;}

void COpenGLRender::EnableTextures(){m_sRenderOptions.bEnableTextures=true;}
void COpenGLRender::DisableTextures(){m_sRenderOptions.bEnableTextures=false;DeactivateTextures();}
bool COpenGLRender::AreTexturesEnabled(){return m_sRenderOptions.bEnableTextures;}

void COpenGLRender::EnableHeightFog(){m_sRenderOptions.bEnableHeightFog=true;}
void COpenGLRender::DisableHeightFog(){m_sRenderOptions.bEnableHeightFog=false;DeactivateHeightFog();}
bool COpenGLRender::IsHeightFogEnabled(){return m_sRenderOptions.bEnableHeightFog;}

void COpenGLRender::EnableLighting(){m_sRenderOptions.bEnableLighting=true;}
void COpenGLRender::DisableLighting(){m_sRenderOptions.bEnableLighting=false;DeactivateLighting();}
bool COpenGLRender::IsLightingEnabled(){return m_sRenderOptions.bEnableLighting;}

void COpenGLRender::EnableSolid(){m_sRenderOptions.bEnableSolid=true;}
void COpenGLRender::DisableSolid(){m_sRenderOptions.bEnableSolid=false;DeactivateSolid();}
bool COpenGLRender::IsSolidEnabled(){return m_sRenderOptions.bEnableSolid;}

void COpenGLRender::EnableBlending(){m_sRenderOptions.bEnableBlending=true;}
void COpenGLRender::DisableBlending(){m_sRenderOptions.bEnableBlending=false;DeactivateBlending();}
bool COpenGLRender::IsBlendingEnabled(){return m_sRenderOptions.bEnableBlending;}

void COpenGLRender::EnableShadows()
{
	if(m_nSkyShadowTextureLevel>=m_sHardwareSupport.nMaxTextureUnits){return;}
	m_sRenderOptions.bEnableShadows=true;
}
void COpenGLRender::DisableShadows(){m_sRenderOptions.bEnableShadows=false;}
bool COpenGLRender::AreShadowsEnabled(){return m_sRenderOptions.bEnableShadows;}

void COpenGLRender::EnableAutoShadowVolume(){m_sRenderOptions.bEnableAutoShadowVolume=true;}
void COpenGLRender::DisableAutoShadowVolume(){m_sRenderOptions.bEnableAutoShadowVolume=false;}
bool COpenGLRender::IsAutoShadowVolumeEnabled(){return m_sRenderOptions.bEnableAutoShadowVolume;}

void COpenGLRender::EnableStagedRenderingStats(){m_sRenderOptions.bEnableStagedRenderingStats=true;}
void COpenGLRender::DisableStagedRenderingStats(){m_sRenderOptions.bEnableStagedRenderingStats=false;}
bool COpenGLRender::AreStagedRenderingStatsEnabled(){return m_sRenderOptions.bEnableStagedRenderingStats;}
void COpenGLRender::DumpStagedRenderingStats()
{
	RTTRACE("--- Render Stats ---");
	RTTRACE("Vertexes  : %d ",m_sStagedStats.nTotalVertexes);
	RTTRACE("                      %d Buffered",m_sStagedStats.nBufferedVertexes);
	RTTRACE("                      %d Inmediate",m_sStagedStats.nInmediateVertexes);
	RTTRACE("Faces     : %d ",m_sStagedStats.nTotalFaces);
	RTTRACE("                      %d Buffered",m_sStagedStats.nBufferedFaces);
	RTTRACE("                      %d Inmediate",m_sStagedStats.nInmediateFaces);
	RTTRACE("Objects");
	RTTRACE("                      %d Models",m_sStagedStats.nModels);
	RTTRACE("                      %d Particles",m_sStagedStats.nParticles);
	RTTRACE("                      %d Lines",m_sStagedStats.nLines);
	RTTRACE("                      %d Points",m_sStagedStats.nPoints);
	RTTRACE("Changes");
	RTTRACE("                      %d State",m_sStagedStats.nStateChanges);
	RTTRACE("                      %d Shader",m_sStagedStats.nShaderChanges);
	RTTRACE("                      %d Texture",m_sStagedStats.nTextureChanges);
	RTTRACE("Time");
	RTTRACE("                      %d ms Render, %fps",m_sStagedStats.nRenderTime,1000.0/(double)m_sStagedStats.nRenderTime);
	RTTRACE("                      %d ms Shadow",m_sStagedStats.nShadowTime);
}

void COpenGLRender::RenderModelStages(bool bRenderingShadow,bool bShadowReceptionState)
{
	// Flush Model Stages
	std::map<double,std::pair<const SModelStageKey*,const SModelStage*> >::iterator iSortedModelStage;
	for(iSortedModelStage=m_mSortedModelStages.begin();iSortedModelStage!=m_mSortedModelStages.end();iSortedModelStage++)
	{
		const SModelStageKey *pKey=iSortedModelStage->second.first;
		const SModelStage *pStage=iSortedModelStage->second.second;
		if(bRenderingShadow && !pKey->sRenderState.bActiveShadowEmission){continue;}
		if(bShadowReceptionState!=pKey->sRenderState.bActiveShadowReception){continue;}

		if(!bRenderingShadow && m_sRenderOptions.bEnableStagedRenderingStats){m_sStagedStats.nModels+=pStage->vInstances.size();}
		
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
				if(m_sRenderOptions.bEnableStagedRenderingStats)
				{
					m_sStagedStats.nTotalFaces+=nFaces;
					m_sStagedStats.nTotalVertexes+=nVertexes;
					m_sStagedStats.nBufferedFaces+=nFaces;
					m_sStagedStats.nBufferedVertexes+=nVertexes;
				}
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
					if(!bRenderingShadow){SetRenderState(pKey->sRenderState,eStateChange_Incremental,eStateChange_DoNotUpdateShader);}
					pStage->piGLModel->PrepareRenderBuffer(this,pKey->nAnimation,pKey->nFrame,nBuffer,bRenderingShadow,&m_RenderMappings);
					if(!bRenderingShadow)
					{
#ifdef ANDROID_GLES1
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
#else
						SetCurrentRenderStateShader();
						
						if(m_pCurrentShader)
						{
							CVector vAmbientColor,vDiffuseColor,vSpecularColor;
							float fShininess,fOpacity;
							float *pColors=NULL;
							pStage->piModel->GetRenderBufferMaterial(pKey->nAnimation,pKey->nFrame,nBuffer,&vAmbientColor,&vDiffuseColor,&vSpecularColor, &fShininess, &fOpacity);
							pStage->piModel->GetRenderBufferColors(pKey->nAnimation,pKey->nFrame,nBuffer,&pColors);
							
							m_pCurrentShader->m_piShader->AddUniform("uDisableVertexColor",(float)(pColors?0.0:1.0),true);
							m_pCurrentShader->m_piShader->AddUniform("uMaterialSpecular",vSpecularColor,fOpacity,true);
							m_pCurrentShader->m_piShader->AddUniform("uMaterialDiffuse",vDiffuseColor,fOpacity,true);
							m_pCurrentShader->m_piShader->AddUniform("uMaterialShininess",fShininess,true);
						}
#endif
					}
				}
				
		#ifdef ANDROID_GLES1

				glPushMatrix();
				glTranslatef(vPos.c[0],vPos.c[1],vPos.c[2]);
				if(vAngles.c[YAW]){glRotatef(vAngles.c[YAW]	,0,1,0);}
				if(vAngles.c[PITCH]){glRotatef(vAngles.c[PITCH]	,0,0,1);}
				if(vAngles.c[ROLL]){glRotatef(vAngles.c[ROLL]	,1,0,0);}

				pStage->piGLModel->CallRenderBuffer(this,pKey->nAnimation,pKey->nFrame,nBuffer,1);
				glPopMatrix();
				
		#else
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
					if(m_pCurrentShader){m_pCurrentShader->m_piShader->AddUniformMatrixes("uModel",nInstances,fMatrixes,true);}
					pStage->piGLModel->CallRenderBuffer(this,pKey->nAnimation,pKey->nFrame,nBuffer,nInstances);
					
					nInstances=0;
					pfMatrixes=fMatrixes;
				}
		#endif
			}
			
		#ifndef ANDROID_GLES1
			if(nInstances)
			{
				if(m_pCurrentShader){m_pCurrentShader->m_piShader->AddUniformMatrixes("uModel",nInstances,fMatrixes,true);}
				pStage->piGLModel->CallRenderBuffer(this,pKey->nAnimation,pKey->nFrame,nBuffer,nInstances);
			}
		#endif

			if(bRenderBufferPrepared)
			{
				pStage->piGLModel->UnPrepareRenderBuffer(this,pKey->nAnimation,pKey->nFrame,nBuffer,bRenderingShadow,&m_RenderMappings);
			}
		}
	}
}

void COpenGLRender::RenderParticleStages(bool bRenderingShadow,bool bShadowReceptionState)
{
	// Flush Texture Particles Stages.
	if(!bRenderingShadow){glDepthMask(false);}

	std::map<STextureParticleStageKey,STextureParticleStage>::iterator iParticleStage;
	for(iParticleStage=m_mTextureParticleStages.begin();iParticleStage!=m_mTextureParticleStages.end();iParticleStage++)
	{
		const STextureParticleStageKey *pKey=&iParticleStage->first;
		STextureParticleStage *pStage=&iParticleStage->second;
		if(bRenderingShadow && !pKey->sRenderState.bActiveShadowEmission){continue;}
		if(bShadowReceptionState!=pKey->sRenderState.bActiveShadowReception){continue;}

		IGenericTexture *piTexture=pKey->piTexture;
		if(piTexture){SelectTexture(piTexture,0);}

		SetRenderState(pKey->sRenderState,eStateChange_Incremental);

		for(unsigned int x=0;x<pStage->vBuffers.size();x++)
		{
			STextureParticleBuffer *pBuffer=pStage->vBuffers[x];
			float *pTexVertexBufferTemp=pBuffer->pTexBuffers;
			SetVertexPointers(pBuffer->pVertexBuffer,NULL,pBuffer->pColorBuffer,1,&pTexVertexBufferTemp);
			glDrawArrays(GL_TRIANGLES,0,pBuffer->nUsedElements*6);
			if(!bRenderingShadow && m_sRenderOptions.bEnableStagedRenderingStats)
			{
				m_sStagedStats.nParticles+=pBuffer->nUsedElements;
				m_sStagedStats.nTotalFaces+=pBuffer->nUsedElements*2;
				m_sStagedStats.nTotalVertexes+=pBuffer->nUsedElements*6;
				m_sStagedStats.nInmediateFaces+=pBuffer->nUsedElements*2;
				m_sStagedStats.nInmediateVertexes+=pBuffer->nUsedElements*6;
			}
		}
		if(piTexture){UnselectTexture(0);}
	}
	if(!bRenderingShadow){glDepthMask(true);}
}

void COpenGLRender::RenderLineStages(bool bRenderingShadow,bool bShadowReceptionState)
{
#ifndef ANDROID
	glEnable(GL_LINE_STIPPLE);
#endif
	
	std::map<SLineStageKey,SLineStage>::iterator iLineStage;
	for(iLineStage=m_mLineStages.begin();iLineStage!=m_mLineStages.end();iLineStage++)
	{
		const SLineStageKey *pKey=&iLineStage->first;
		SLineStage *pStage=&iLineStage->second;

		if(bRenderingShadow && !pKey->sRenderState.bActiveShadowEmission){continue;}
		if(bShadowReceptionState!=pKey->sRenderState.bActiveShadowReception){continue;}

		SetRenderState(pKey->sRenderState,eStateChange_Incremental);
#ifndef ANDROID
		glLineStipple(1,(unsigned short)pKey->nStipple);
#endif

		for(unsigned int x=0;x<pStage->vBuffers.size();x++)
		{
			SLineBuffer *pBuffer=pStage->vBuffers[x];
			SetVertexPointers(pBuffer->pVertexBuffer,NULL,pBuffer->pColorBuffer,0,NULL);
			glDrawArrays(GL_LINES,0,pBuffer->nUsedElements*2);
			if(!bRenderingShadow && m_sRenderOptions.bEnableStagedRenderingStats)
			{
				m_sStagedStats.nLines+=pBuffer->nUsedElements;
				m_sStagedStats.nTotalVertexes+=pBuffer->nUsedElements*2;
				m_sStagedStats.nInmediateVertexes+=pBuffer->nUsedElements*2;				
			}
		}
	}
#ifndef ANDROID
	glDisable(GL_LINE_STIPPLE);
#endif
}

void COpenGLRender::RenderPointStages(bool bRenderingShadow,bool bShadowReceptionState)
{
	m_bRenderingPoints=true;
	SetCurrentRenderStateShader();
	
	std::map<SPointStageKey,SPointStage>::iterator iPointStage;
	for(iPointStage=m_mPointStages.begin();iPointStage!=m_mPointStages.end();iPointStage++)
	{
		const SPointStageKey *pKey=&iPointStage->first;
		SPointStage *pStage=&iPointStage->second;

		if(bRenderingShadow && !pKey->sRenderState.bActiveShadowEmission){continue;}
		if(bShadowReceptionState!=pKey->sRenderState.bActiveShadowReception){continue;}

		SetRenderState(pKey->sRenderState,eStateChange_Incremental);
#ifndef ANDROID
		glPointSize((float)pKey->dSize);
#endif
		for(unsigned int x=0;x<pStage->vBuffers.size();x++)
		{
			SPointBuffer *pBuffer=pStage->vBuffers[x];
			SetVertexPointers(pBuffer->pVertexBuffer,NULL,pBuffer->pColorBuffer,0,NULL);

			glDrawArrays(GL_POINTS,0,pBuffer->nUsedElements);
			if(!bRenderingShadow && m_sRenderOptions.bEnableStagedRenderingStats)
			{
				m_sStagedStats.nPoints+=pBuffer->nUsedElements;
				m_sStagedStats.nTotalVertexes+=pBuffer->nUsedElements;
				m_sStagedStats.nInmediateVertexes+=pBuffer->nUsedElements;				
			}
		}
	}
	
	m_bRenderingPoints=false;
	SetCurrentRenderStateShader();
}

void COpenGLRender::SetVertexBufferObject(SVertexBufferObject *pVBO)
{
	if(pVBO==NULL)
	{
		glBindBuffer(GL_ARRAY_BUFFER,0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
		return;
	}

	glBindBuffer(GL_ARRAY_BUFFER,pVBO->nBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,pVBO->nIndexesBufferObject);
	
#ifdef ANDROID_GLES1 

	if(!m_sBufferState.bVertexBufferEnabled && pVBO->nVertexOffset!=-1)     {glEnableClientState(GL_VERTEX_ARRAY);m_sBufferState.bVertexBufferEnabled=true;}
	else if(m_sBufferState.bVertexBufferEnabled && !pVBO->nVertexOffset==-1){glDisableClientState(GL_VERTEX_ARRAY);m_sBufferState.bVertexBufferEnabled=false;}
	
	if(!m_sBufferState.bColorBufferEnabled && pVBO->nColorOffset!=-1)     {glEnableClientState(GL_COLOR_ARRAY);m_sBufferState.bColorBufferEnabled=true;}
	else if(m_sBufferState.bColorBufferEnabled && !pVBO->nColorOffset==-1){glDisableClientState(GL_COLOR_ARRAY);m_sBufferState.bColorBufferEnabled=false;}
	
	if(!m_sBufferState.bNormalBufferEnabled && pVBO->nNormalOffset!=-1)     {glEnableClientState(GL_NORMAL_ARRAY);m_sBufferState.bNormalBufferEnabled=true;}
	else if(m_sBufferState.bNormalBufferEnabled && !pVBO->nNormalOffset==-1){glDisableClientState(GL_NORMAL_ARRAY);m_sBufferState.bNormalBufferEnabled=false;}
	
	if(pVBO->nVertexOffset!=-1){glVertexPointer(3,GL_FLOAT,0,(void*)pVBO->nVertexOffset);}
	if(pVBO->nColorOffset!=-1){glColorPointer(4,GL_FLOAT,0,(void*)pVBO->nColorOffset);}
	if(pVBO->nNormalOffset!=-1){glNormalPointer(GL_FLOAT,0,(void*)pVBO->nNormalOffset);}
	
	for(int x=0;x<MAX_OPENGL_TEXTURE_COORDS;x++)
	{
		bool bEnable=(x<pVBO->nTextures && pVBO->pTexOffsets[x]!=-1);
		if(bEnable!=m_sBufferState.pbTexCoordEnabled[x])        {glClientActiveTextureARB(GL_TEXTURE0_ARB+x);}
		if(!m_sBufferState.pbTexCoordEnabled[x] && bEnable)     {glEnableClientState(GL_TEXTURE_COORD_ARRAY);m_sBufferState.pbTexCoordEnabled[x]=true;}
		else if(m_sBufferState.pbTexCoordEnabled[x] && !bEnable){glDisableClientState(GL_TEXTURE_COORD_ARRAY);m_sBufferState.pbTexCoordEnabled[x]=false;}
		if(bEnable){glTexCoordPointer(2,GL_FLOAT,0,(void*)pVBO->pTexOffsets[x]);}
	}
#else
	
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
	
	if(pVBO->nVertexOffset!=-1){glVertexAttribPointer(m_RenderMappings.nVertexAttribIndex,3,GL_FLOAT,GL_FALSE,0,(void*)pVBO->nVertexOffset);}
	if(pVBO->nColorOffset!=-1){glVertexAttribPointer(m_RenderMappings.nColorAttribIndex,4,GL_FLOAT,GL_FALSE,0,(void*)pVBO->nColorOffset);}
	if(pVBO->nNormalOffset!=-1){glVertexAttribPointer(m_RenderMappings.nNormalAttribIndex,3,GL_FLOAT,GL_FALSE,0,(void*)pVBO->nNormalOffset);}
	if(pVBO->nNormalMapOffset!=-1){glVertexAttribPointer(m_RenderMappings.nNormalMapCoordAttribIndex,2,GL_FLOAT,GL_FALSE,0,(void*)pVBO->nNormalMapOffset);}
	if(pVBO->nTangentOffset!=-1){glVertexAttribPointer(m_RenderMappings.nTangentAttribIndex,3,GL_FLOAT,GL_FALSE,0,(void*)pVBO->nTangentOffset);}
	if(pVBO->nBitangentOffset!=-1){glVertexAttribPointer(m_RenderMappings.nBitangentAttribIndex,3,GL_FLOAT,GL_FALSE,0,(void*)pVBO->nBitangentOffset);}
	
	for(int x=0;x<MAX_OPENGL_TEXTURE_COORDS;x++)
	{
		bool bEnable=(x<pVBO->nTextures && pVBO->pTexOffsets[x]!=-1);
		if(!m_sBufferState.pbTexCoordEnabled[x] && bEnable)    {glEnableVertexAttribArray(m_RenderMappings.pTextureAttribIndex[x]);m_sBufferState.pbTexCoordEnabled[x]=true;}
		else if(m_sBufferState.pbTexCoordEnabled[x] && !bEnable){glDisableVertexAttribArray(m_RenderMappings.pTextureAttribIndex[x]);m_sBufferState.pbTexCoordEnabled[x]=false;}
		
		if(bEnable){glVertexAttribPointer(m_RenderMappings.pTextureAttribIndex[x],2,GL_FLOAT,GL_FALSE,0,(void*)pVBO->pTexOffsets[x]);}
	}
#endif
}

void COpenGLRender::SetVertexPointers(float *pVertex,float *pNormal,float *pColor,int nTex,float **pTex, float *pNormalMap,float *pTangent,float *pBiTangent)
{
#ifdef ANDROID_GLES1 
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
#else

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
	
	if(pVertex){glVertexAttribPointer(m_RenderMappings.nVertexAttribIndex,3,GL_FLOAT,GL_FALSE,0,pVertex);}
	if(pColor){glVertexAttribPointer(m_RenderMappings.nColorAttribIndex,4,GL_FLOAT,GL_FALSE,0,pColor);}
	if(pNormal){glVertexAttribPointer(m_RenderMappings.nNormalAttribIndex,3,GL_FLOAT,GL_FALSE,0,pNormal);}
	if(pNormalMap){glVertexAttribPointer(m_RenderMappings.nNormalMapCoordAttribIndex,2,GL_FLOAT,GL_FALSE,0,pNormalMap);}
	if(pTangent){glVertexAttribPointer(m_RenderMappings.nTangentAttribIndex,3,GL_FLOAT,GL_FALSE,0,pTangent);}
	if(pBiTangent){glVertexAttribPointer(m_RenderMappings.nBitangentAttribIndex,3,GL_FLOAT,GL_FALSE,0,pBiTangent);}

	for(int x=0;x<MAX_OPENGL_TEXTURE_COORDS;x++)
	{
		bool bEnable=(x<nTex && pTex[x]!=NULL);
		if(!m_sBufferState.pbTexCoordEnabled[x] && bEnable)    {glEnableVertexAttribArray(m_RenderMappings.pTextureAttribIndex[x]);m_sBufferState.pbTexCoordEnabled[x]=true;}
		else if(m_sBufferState.pbTexCoordEnabled[x] && !bEnable){glDisableVertexAttribArray(m_RenderMappings.pTextureAttribIndex[x]);m_sBufferState.pbTexCoordEnabled[x]=false;}
		
		if(bEnable){glVertexAttribPointer(m_RenderMappings.pTextureAttribIndex[x],2,GL_FLOAT,GL_FALSE,0,pTex[x]);}
	}
#endif
}

void COpenGLRender::RenderAllStages(bool bRenderingShadow,bool bShadowReceptionState)
{
	m_bRenderingShadowReception=bShadowReceptionState;
	RenderModelStages(bRenderingShadow,bShadowReceptionState);
	CMatrix identity;
	std::map<SShaderKey,CGenericShaderWrapper>::iterator iShader;
	for(iShader=m_mShaders.begin();iShader!=m_mShaders.end();iShader++)
	{
		iShader->second.m_piShader->AddUniform("uModel",identity,false);
	}
	RenderParticleStages(bRenderingShadow,bShadowReceptionState);
	RenderLineStages(bRenderingShadow,bShadowReceptionState);
	RenderPointStages(bRenderingShadow,bShadowReceptionState);
	m_bRenderingShadowReception=false;
}


void COpenGLRender::SetAmbientLight( const CVector &vColor ){m_vAmbientColor=vColor;}
void COpenGLRender::GetAmbientLight( CVector *pvColor ){if(pvColor){*pvColor=m_vAmbientColor;}}

void COpenGLRender::SetSunLight( const CVector &vPosition,const CVector &vDirection,const CVector &vAmbientColor,const CVector &vDiffuseColor,const CVector &vSpecularColor)
{
	if(m_SunLight.m_piLight)
	{
		m_SunLight.m_piLight->SetPosition(vPosition);
		m_SunLight.m_piLight->SetAmbientColor(vAmbientColor);
		m_SunLight.m_piLight->SetDiffuseColor(vDiffuseColor);
		m_SunLight.m_piLight->SetSpecularColor(vSpecularColor);
		m_SunLight.m_piLight->SetDirectional(vDirection);
	}
}

void COpenGLRender::GetSunLight( CVector *pvPosition,CVector *pvDirection,CVector *pvAmbientColor,CVector *pvDiffuseColor,CVector *pvSpecularColor )
{
	if(m_SunLight.m_piLight)
	{
		if(pvPosition){*pvPosition=m_SunLight.m_piLight->GetPosition();}
		if(pvDirection){*pvDirection=m_SunLight.m_piLight->GetDirectionalDirection();}
		if(pvAmbientColor){*pvAmbientColor=m_SunLight.m_piLight->GetAmbientColor();}
		if(pvDiffuseColor){*pvDiffuseColor=m_SunLight.m_piLight->GetDiffuseColor();}
		if(pvSpecularColor){*pvSpecularColor=m_SunLight.m_piLight->GetSpecularColor();}
	}

}

void COpenGLRender::PushState()
{
	if(m_bStagedRendering)
	{
		m_sStagedRenderingStateStack.push(m_sStagedRenderingState);
	}
	else
	{
		m_sRenderStateStack.push(m_sRenderState);
	}
}

void COpenGLRender::PopState()
{
	if(m_bStagedRendering)
	{
		if(m_sStagedRenderingStateStack.size())
		{
			m_sStagedRenderingState=m_sStagedRenderingStateStack.top();
			m_sStagedRenderingStateStack.pop();		
		}
	}
	else
	{
		if(m_sRenderStateStack.size())
		{
			SetRenderState(m_sRenderStateStack.top(),eStateChange_Incremental);
			m_sRenderStateStack.pop();
		}
	}
}

void COpenGLRender::PushOptions()
{
	m_sRenderOptionsStack.push(m_sRenderOptions);
}

void COpenGLRender::PopOptions()
{
	if(m_sRenderOptionsStack.size())
	{
		m_sRenderOptions=m_sRenderOptionsStack.top();
		m_sRenderOptionsStack.pop();		
	}
}

void COpenGLRender::ProcessCameraVertex( const CVector &vVertex )
{
	CVector vTemp=vVertex;
	if(m_bShadowVolumeFirstVertex)
	{
		m_bShadowVolumeFirstVertex=false;
		m_vShadowVolumeMins=vVertex;
		m_vShadowVolumeMaxs=vVertex;
	}
	else
	{
		CVector vTempMins;
		CVector vTempMaxs;
		vTempMins=CVector().Mins(m_vShadowVolumeMins,vTemp);
		vTempMaxs=CVector().Maxs(m_vShadowVolumeMaxs,vTemp);
		m_vShadowVolumeMins=vTempMins;
		m_vShadowVolumeMaxs=vTempMaxs;
	}

	ToCameraRef(&m_CameraForwardPlane,&m_CameraRightPlane,&m_CameraUpPlane,1,&vTemp);

	double dDistance=vTemp.c[2];
	if(dDistance<m_dStagedRenderingMinZ){m_dStagedRenderingMinZ=dDistance;}
	if(dDistance>m_dStagedRenderingMaxZ){m_dStagedRenderingMaxZ=dDistance;}
	if(m_dStagedRenderingMinZ<m_dPerspectiveNearPlane){m_dStagedRenderingMinZ=m_dPerspectiveNearPlane;}
	if(m_dStagedRenderingMaxZ>m_dPerspectiveFarPlane){m_dStagedRenderingMaxZ=m_dPerspectiveFarPlane;}
}

void COpenGLRender::AddShader( SShaderKey &key )
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
		sprintf(sTemp,"#define SHADOW_TEXTURE_LEVEL %d\n",m_nShadowTextureLevel);
		sPreprocessor+=sTemp;
		sDescription+="S";
	}
	if(key.bNormalMap)
	{
		sPreprocessor+="#define ENABLE_NORMAL_MAP\n";
		sprintf(sTemp,"#define NORMAL_MAP_TEXTURE_LEVEL %d\n",m_nNormalMapTextureLevel);
		sPreprocessor+=sTemp;
		sDescription+="N";
	}
	if(key.bSkyShadow)
	{
		sPreprocessor+="#define ENABLE_SKY_SHADOW\n";
		sprintf(sTemp,"#define SKY_TEXTURE_LEVEL %d\n",m_nSkyShadowTextureLevel);
		sPreprocessor+=sTemp;
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
		/*		if(key.eShadingModel==eShadingModel_Gouraud)
		{
			wrapper.m_piShader->Load("Shaders/RenderShader-Gouraud-Vertex.c","Shaders/RenderShader-Gouraud-Fragment.c",sPreprocessor);
		 }
		 */
#ifdef ANDROID
		wrapper.m_piShader->Load("Shaders/Android-RenderShader-Vertex.c","Shaders/Android-RenderShader-Fragment.c",sPreprocessor);
#else
		if(key.eShadingModel==eShadingModel_Balanced)
		{
			wrapper.m_piShader->Load("Shaders/RenderShader-Balanced-Vertex.c","Shaders/RenderShader-Balanced-Fragment.c",sPreprocessor);
		}
		else if(key.eShadingModel==eShadingModel_Phong)
		{
			wrapper.m_piShader->Load("Shaders/RenderShader-Phong-Vertex.c","Shaders/RenderShader-Phong-Fragment.c",sPreprocessor);
		}
#endif
		
		if(key.nTextureUnits>=1){wrapper.m_piShader->AddUniform("Texture0",(int)0,false);wrapper.m_piShader->AddUniform("uTexMatrix0",identity,false);}
		if(key.nTextureUnits>=2){wrapper.m_piShader->AddUniform("Texture1",(int)1,false);wrapper.m_piShader->AddUniform("uTexMatrix1",identity,false);}
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
		wrapper.m_piShader->AddAttribute("aTexCoord0",(int)m_RenderMappings.pTextureAttribIndex[0]);
		wrapper.m_piShader->AddAttribute("aTexCoord1",(int)m_RenderMappings.pTextureAttribIndex[1]);
		wrapper.m_piShader->AddAttribute("aVertexColor",(int)m_RenderMappings.nColorAttribIndex);
		m_mShaders[key]=wrapper;
		m_mShaderNames[key]=sDescription;
	}
}
	
void COpenGLRender::StartSelection(SGameRect rWindowRect,IGenericCamera *piCamera,double dx,double dy,double dPrecision)
{
	double dNearPlane=0,dFarPlane=0;
	double dRoll=piCamera->GetAngles().c[ROLL];
	double dPitch=piCamera->GetAngles().c[PITCH];
	double dYaw=piCamera->GetAngles().c[YAW];
	CVector vPosition=piCamera->GetPosition();
	piCamera->GetClippingPlanes(dNearPlane,dFarPlane);
	
	SetPerspectiveProjection(piCamera->GetViewAngle(),dNearPlane,dFarPlane);
	SetCamera(vPosition,dYaw,dPitch,dRoll);
	ActivateDepth();
	
	if(m_sHardwareSupport.bShaders)
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
	}
	
	if(m_SelectionShader.m_piShader){m_SelectionShader.m_piShader->AddUniform("uProjection",m_ProjectionMatrix,false);}
	if(m_SelectionShader.m_piShader){m_SelectionShader.m_piShader->AddUniform("uView",m_ModelViewMatrix,false);}
	
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	m_bSelecting=true;
	m_SelectionPos.x=dx-rWindowRect.x;
	m_SelectionPos.y=dy-rWindowRect.y;
	m_nSelectionPrecision=dPrecision;
	if(m_SelectionShader.m_piShader)
	{
		m_pShaderBeforeSelection=m_pCurrentShader;
		if(m_pShaderBeforeSelection){m_pShaderBeforeSelection->m_piShader->Deactivate();}
		m_SelectionShader.m_piShader->Activate();
		m_pCurrentShader=&m_SelectionShader;
		
		SetSelectionId(0);
	}
}

void COpenGLRender::SetSelectionId( unsigned int nId )
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

int COpenGLRender::EndSelection()
{
	unsigned int nSelectionSize=m_nSelectionPrecision+1;
	unsigned int nPrecisionHalf=m_nSelectionPrecision/2;
	int nSelectedId=-1;
	double dSelectionDistanceSquared=nSelectionSize+1.0;
	
	unsigned char *pColorBuffer=new unsigned char [nSelectionSize*nSelectionSize*4];
	memset(pColorBuffer,0,nSelectionSize*nSelectionSize*4);
	glFinish();
	glReadPixels(m_SelectionPos.x-nPrecisionHalf,m_SelectionPos.y-nPrecisionHalf,nSelectionSize,nSelectionSize,GL_RGBA,GL_UNSIGNED_BYTE,pColorBuffer);

	CVector vCenter(nPrecisionHalf,nPrecisionHalf,0);
	unsigned char *pColor=pColorBuffer;
	for(int y=0;y<nSelectionSize;y++)
	{
		for(int x=0;x<nSelectionSize;x++)
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

void COpenGLRender::ReloadShaders()
{
#ifndef ANDROID_GLES1
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
	
#endif


}
