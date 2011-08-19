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
#include "OpenGLGraphics.h"
#include "OpenGLRender.h"

COpenGLRender::COpenGLRender(void)
{
	m_pCurrentShader=NULL;
	m_bRestoreTextureMatrix=false;
	m_bHardwareSupportRead=false;
	m_bIgnoreShaderSupport=false;
	m_bShadowVolumeFirstVertex=false;
	m_bRenderingWithShader=false;
	m_bRenderingShadowReception=false;
	m_dShadowAntiFlickeringMargin=10;
	
	m_vAmbientColor=CVector(0.5,0.5,0.5);
	m_bStagedRendering=false;
	m_dProyectionWidth=1;
	m_dProyectionHeight=1;
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

	m_bLightingPrepared=false;
	m_nActiveLights=0;
	m_bActiveHeightFog=false;

	m_vColor=CVector(0,0,0);
	m_dAlpha=1;

	m_dStagedRenderingMinZ=0;
	m_dStagedRenderingMaxZ=0;
	m_dMinDistanceToLight=10000;
	m_bPrecompileShaders=true;
	
	m_nFirstTimeStamp=GetTimeStamp();
	m_piNormalMap=NULL;
	m_piSkyShadow=NULL;
	m_nShadowTextureLevel=1;
	m_nSkyShadowTextureLevel=2;
	m_nNormalMapTextureLevel=3;
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

void COpenGLRender::StartFrame(IGenericViewport *piViewport)
{
	m_piCurrentViewport=piViewport;

	glDisable(GL_NORMALIZE);
	glDisable(GL_AUTO_NORMAL);

	SRenderState sDefault;
	SetRenderState(sDefault,true);
}

void COpenGLRender::EndFrame()
{
	EndStagedRendering();
	m_piCurrentViewport=NULL;
}

IGenericViewport *COpenGLRender::GetViewPort()
{
	return m_piCurrentViewport;
}

void COpenGLRender::SetOrthographicProjection(double cx,double cy)
{
	m_bPerspectiveProjection=false;
	m_dProyectionWidth=cx;
	m_dProyectionHeight=cy;

	UpdateProjectionMatrix();
}

void COpenGLRender::SetPerspectiveProjection(double dViewAngle,double dNearPlane,double dFarPlane)
{
	m_bPerspectiveProjection=true;
	m_dPerspectiveNearPlane=dNearPlane;
	m_dPerspectiveFarPlane=dFarPlane;
	m_dPerspectiveViewAngle=dViewAngle;

	UpdateProjectionMatrix();
}

void COpenGLRender::SetViewport(double x,double y,double cx, double cy)
{
	m_nViewportX=(int)x;
	m_nViewportY=(int)y;
	m_nViewportW=(int)cx;
	m_nViewportH=(int)cy;

	UpdateProjectionMatrix();
}

void COpenGLRender::UpdateProjectionMatrix()
{
	double dViewportW=m_nViewportW;
	double dViewportH=m_nViewportH;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(m_nViewportX,m_nViewportY,m_nViewportW,m_nViewportH);
	if(m_bPerspectiveProjection)
	{
		double dAspectRatio=dViewportW/dViewportH;
		gluPerspective(m_dPerspectiveViewAngle,dAspectRatio,m_dPerspectiveNearPlane,m_dPerspectiveFarPlane);
	}
	else
	{
		glOrtho(-m_dProyectionWidth*0.5,m_dProyectionWidth*0.5,-m_dProyectionHeight*0.5,m_dProyectionHeight*0.5,1,10000);
	}	
	glMatrixMode(GL_MODELVIEW);
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

		GLhandleARB hFakeShaderProgram=m_bIgnoreShaderSupport?0:glCreateProgramObjectARB();
		m_sHardwareSupport.bShaders=(hFakeShaderProgram!=0);
		if(hFakeShaderProgram){glDeleteObjectARB(hFakeShaderProgram);hFakeShaderProgram=0;}

		glGetIntegerv(GL_MAX_LIGHTS,&m_sHardwareSupport.nMaxLights);
		glGetIntegerv(GL_MAX_TEXTURE_UNITS,&m_sHardwareSupport.nMaxTextureUnits);

		if(m_sHardwareSupport.bShaders)
		{
			ReloadShaders();
		}
		m_sRenderOptions.bEnableShadows&=(m_nShadowTextureLevel<m_sHardwareSupport.nMaxTextureUnits);
		m_sRenderOptions.bEnableSkyShadow&=(m_nSkyShadowTextureLevel<m_sHardwareSupport.nMaxTextureUnits);
		m_sRenderOptions.bEnableNormalMaps&=(m_nNormalMapTextureLevel<m_sHardwareSupport.nMaxTextureUnits);
		m_sRenderOptions.bEnableShader&=m_sHardwareSupport.bShaders;
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotated((0-dRoll)	,AxisPosZ.c[0],AxisPosZ.c[1],AxisPosZ.c[2]);
	glRotated((0-dPitch),AxisPosX.c[0],AxisPosX.c[1],AxisPosX.c[2]);
	glRotated((0-(dYaw-90))	,AxisPosY.c[0],AxisPosY.c[1],AxisPosY.c[2]);
	glTranslated(-vPosition.c[0],-vPosition.c[1],-vPosition.c[2]);

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
	GLdouble pdScissorBox[4];
	glGetDoublev(GL_SCISSOR_BOX,pdScissorBox);
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


void COpenGLRender::Clear(const CVector &vColor,double dAlpha)
{
	CVector vOrigin(m_dProyectionWidth*0.5,m_dProyectionHeight*0.5,0);

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glPolygonMode(GL_FRONT,GL_FILL);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColor4d(vColor.c[0],vColor.c[1],vColor.c[2],dAlpha);

	glBegin(GL_QUADS);
	glVertex3d(m_dProyectionWidth,m_dProyectionHeight,0);
	glVertex3d(0,m_dProyectionHeight,0);
	glVertex3d(0,0,0);
	glVertex3d(m_dProyectionWidth,0,0);
	glEnd();

	glPopAttrib();
}

void COpenGLRender::SetColor(const CVector &vColor,double dAlpha)
{
	m_vColor=vColor;
	m_dAlpha=dAlpha;
	if(!m_bStagedRendering)
	{
		glColor4d(vColor.c[0],vColor.c[1],vColor.c[2],dAlpha);
	}
}

void COpenGLRender::SelectTexture(IGenericTexture *pTexture,int nTextureLevel)
{
	if(!m_sRenderOptions.bEnableTextures){return;}
	if(!m_sRenderState.bActiveTextures){return;}
	IGenericTexture *piOldTexture=m_mTextureLevels[nTextureLevel];
	REL(piOldTexture);

	m_mTextureLevels[nTextureLevel]=ADD(pTexture);
	if(!m_bStagedRendering){pTexture->PrepareTexture(this,nTextureLevel);}
	
	if(m_sRenderOptions.bEnableStagedRenderingStats){m_sStagedStats.nTextureChanges++;}
}

void COpenGLRender::SetTextureMatrix(CMatrix *pMatrix,int nTextureLevel)
{
	if(!m_sRenderOptions.bEnableTextures){return;}
	if(!m_sRenderState.bActiveTextures){return;}
	if(m_bStagedRendering){return;}
	
	GLdouble pdMatrix[16];
	ToOpenGLMatrix(pMatrix,pdMatrix);
	glActiveTexture(GL_TEXTURE0_ARB+nTextureLevel);
	glMatrixMode(GL_TEXTURE);
	glLoadMatrixd(pdMatrix);
	m_bRestoreTextureMatrix=true;
	glMatrixMode(GL_MODELVIEW);
}

void COpenGLRender::UnselectTexture(int nTextureLevel)
{
	if(!m_sRenderOptions.bEnableTextures){return;}
	IGenericTexture *piTexture=m_mTextureLevels[nTextureLevel];
	if(!m_bStagedRendering && m_bRestoreTextureMatrix)
	{
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		m_bRestoreTextureMatrix=false;
	}
	if(!m_bStagedRendering && piTexture){piTexture->UnprepareTexture(this,nTextureLevel);}
	REL(piTexture);
	m_mTextureLevels.erase(nTextureLevel);
}

void COpenGLRender::SelectNormalMap(IGenericTexture *pNormalMap)
{
	if(!m_sRenderOptions.bEnableNormalMaps){return;}
	REL(m_piNormalMap);
	m_piNormalMap=ADD(pNormalMap);
	if(!m_bStagedRendering){m_piNormalMap->PrepareTexture(this,m_nNormalMapTextureLevel);}
}

void COpenGLRender::UnselectNormalMap()
{
	if(!m_sRenderOptions.bEnableNormalMaps){return;}
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
		if(m_piSkyShadow){m_piSkyShadow->PrepareTexture(this,m_nSkyShadowTextureLevel);}
	}
}

void COpenGLRender::UnselectSkyShadow()
{
	if(!m_bStagedRendering && m_piSkyShadow)
	{
		if(m_piSkyShadow){m_piSkyShadow->UnprepareTexture(this,m_nSkyShadowTextureLevel);}
	}
	REL(m_piSkyShadow);
}

void COpenGLRender::RenderTexture(const CVector &vOrigin,double s1,double s2)
{
	CVector vAxis1=AxisPosX;
	CVector vAxis2=AxisNegY;

	glBegin(GL_QUADS);
	glTexCoord2f(1,1);
	glVertex3dv((vOrigin+vAxis1*(s1/2.0)-vAxis2*(s2/2.0)).c);

	glTexCoord2f(0,1);
	glVertex3dv((vOrigin-vAxis1*(s1/2.0)-vAxis2*(s2/2.0)).c);

	glTexCoord2f(0,0);
	glVertex3dv((vOrigin-vAxis1*(s1/2.0)+vAxis2*(s2/2.0)).c);

	glTexCoord2f(1,0);
	glVertex3dv((vOrigin+vAxis1*(s1/2.0)+vAxis2*(s2/2.0)).c);
	glEnd();
}

void COpenGLRender::RenderTextureRect(double dPosx,double dPosy,double dWidth,double dHeight,double dTexturex,double dTexturey,double dTextureWidth,double dTextureHeight)
{
	CVector vOrigin=CVector(dPosx,dPosy,0);

	glBegin(GL_QUADS);
	glTexCoord2d(dTexturex+dTextureWidth,dTexturey+dTextureHeight);
	glVertex3dv((vOrigin+AxisPosX*dWidth+AxisPosY*dHeight).c);

	glTexCoord2d(dTexturex,dTexturey+dTextureHeight);
	glVertex3dv((vOrigin+AxisPosY*dHeight).c);

	glTexCoord2d(dTexturex,dTexturey);
	glVertex3dv((vOrigin).c);

	glTexCoord2d(dTexturex+dTextureWidth,dTexturey);
	glVertex3dv((vOrigin+AxisPosX*dWidth).c);
	glEnd();
}

void COpenGLRender::RenderBBox(const CVector &vOrigin,const CVector &vOrientation,const CVector &vMins,const CVector &vMaxs,const CVector &vColor,unsigned long nStipple)
{
	
	CVector vPoints[8];
	CalcBBoxVolume(vOrigin,vOrientation,vMins,vMaxs,vPoints);

	//bottom
	RenderLine(vPoints[0],vPoints[1],vColor,nStipple);
	RenderLine(vPoints[1],vPoints[2],vColor,nStipple);
	RenderLine(vPoints[2],vPoints[3],vColor,nStipple);
	RenderLine(vPoints[3],vPoints[0],vColor,nStipple);
	// top
	RenderLine(vPoints[4],vPoints[5],vColor,nStipple);
	RenderLine(vPoints[5],vPoints[6],vColor,nStipple);
	RenderLine(vPoints[6],vPoints[7],vColor,nStipple);
	RenderLine(vPoints[7],vPoints[4],vColor,nStipple);
	// verticals
	RenderLine(vPoints[0],vPoints[4],vColor,nStipple);
	RenderLine(vPoints[1],vPoints[5],vColor,nStipple);
	RenderLine(vPoints[2],vPoints[6],vColor,nStipple);
	RenderLine(vPoints[3],vPoints[7],vColor,nStipple);
}

void COpenGLRender::RenderBBox(const CVector &vMins,const CVector &vMaxs,const CVector &vColor,unsigned long nStipple)
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
	RenderLine(vPoints[0],vPoints[1],vColor,nStipple);
	RenderLine(vPoints[1],vPoints[2],vColor,nStipple);
	RenderLine(vPoints[2],vPoints[3],vColor,nStipple);
	RenderLine(vPoints[3],vPoints[0],vColor,nStipple);
	// top
	RenderLine(vPoints[4],vPoints[5],vColor,nStipple);
	RenderLine(vPoints[5],vPoints[6],vColor,nStipple);
	RenderLine(vPoints[6],vPoints[7],vColor,nStipple);
	RenderLine(vPoints[7],vPoints[4],vColor,nStipple);
	// verticals
	RenderLine(vPoints[0],vPoints[4],vColor,nStipple);
	RenderLine(vPoints[1],vPoints[5],vColor,nStipple);
	RenderLine(vPoints[2],vPoints[6],vColor,nStipple);
	RenderLine(vPoints[3],vPoints[7],vColor,nStipple);
}

void COpenGLRender::RenderLineStrip(unsigned int nLines,const CVector *pPoints,const CVector &vColor,unsigned long nStipple)
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
			*pColorBuffer++=1;
			
			*pColorBuffer++=(float)vColor.c[0];
			*pColorBuffer++=(float)vColor.c[1];
			*pColorBuffer++=(float)vColor.c[2];
			*pColorBuffer++=1;
			
			pBuffer->nUsedElements++;
		}
	}
	else
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glColor3d(vColor.c[0],vColor.c[1],vColor.c[2]);
		glDisable(GL_TEXTURE_2D);
		
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1,(unsigned short)nStipple);
		
		glBegin(GL_LINE_LOOP);
		for(unsigned int x=0;x<nLines;x++)
		{
			glVertex3d(pPoints[x].c[0],pPoints[x].c[1],pPoints[x].c[2]);
		}
		glEnd();
		
		glPopAttrib();
	}
}


void COpenGLRender::RenderLines(unsigned int nLines,const CVector *pPoints,const CVector &vColor,unsigned long nStipple)
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
			*pColorBuffer++=1;
			
			*pColorBuffer++=(float)vColor.c[0];
			*pColorBuffer++=(float)vColor.c[1];
			*pColorBuffer++=(float)vColor.c[2];
			*pColorBuffer++=1;
			
			pBuffer->nUsedElements++;
		}
	}
	else
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glColor3d(vColor.c[0],vColor.c[1],vColor.c[2]);
		glDisable(GL_TEXTURE_2D);
		
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1,(unsigned short)nStipple);
		
		glBegin(GL_LINES);
		for(unsigned int x=0;x<nLines*2;x++)
		{
			glVertex3d(pPoints[x].c[0],pPoints[x].c[1],pPoints[x].c[2]);
		}
		glEnd();
		
		glPopAttrib();
	}
}
void COpenGLRender::RenderLine(const CVector &v1,const CVector &v2,const CVector &vColor,unsigned long nStipple)
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
		*pColorBuffer++=1;

		*pColorBuffer++=(float)vColor.c[0];
		*pColorBuffer++=(float)vColor.c[1];
		*pColorBuffer++=(float)vColor.c[2];
		*pColorBuffer++=1;

		pBuffer->nUsedElements++;
	}
	else
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glColor3d(vColor.c[0],vColor.c[1],vColor.c[2]);
		glDisable(GL_TEXTURE_2D);

		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1,(unsigned short)nStipple);

		glBegin(GL_LINES);
		glVertex3d(v1.c[0],v1.c[1],v1.c[2]);
		glVertex3d(v2.c[0],v2.c[1],v2.c[2]);
		glEnd();

		glPopAttrib();
	}
}


void COpenGLRender::RenderRect(const CVector &vCenter,const CVector &vAxisW,const CVector &vAxisH,double w,double h)
{
	CVector vTemp[4];
	vTemp[0]=vCenter+vAxisW*(w/2.0)+vAxisH*(h/2.0);
	vTemp[1]=vCenter+vAxisW*(w/2.0)-vAxisH*(h/2.0);
	vTemp[2]=vCenter-vAxisW*(w/2.0)-vAxisH*(h/2.0);
	vTemp[3]=vCenter-vAxisW*(w/2.0)+vAxisH*(h/2.0);
	glBegin(GL_QUADS);
	glVertex3dv(vTemp[0].c);
	glVertex3dv(vTemp[1].c);
	glVertex3dv(vTemp[2].c);
	glVertex3dv(vTemp[3].c);
	glEnd();
}

void COpenGLRender::RenderRect(double x,double y,double w,double h)
{
	glBegin(GL_QUADS);
	glVertex2d(x,y);
	glVertex2d(x+w,y);
	glVertex2d(x+w,y+h);
	glVertex2d(x,y+h);
	glEnd();
}

void COpenGLRender::RenderPolygon(unsigned int nVertexes,const CVector *pVertexes,const CVector *pColors)
{
	glBegin(GL_POLYGON);
	for(unsigned int x=0;x<nVertexes;x++)
	{
	  if(pColors){glColor4d(pColors[x].c[0],pColors[x].c[1],pColors[x].c[2],m_dAlpha);}
	  glVertex3dv(pVertexes[x].c);
	}
	glColor4d(m_vColor.c[0],m_vColor.c[1],m_vColor.c[2],m_dAlpha);
	glEnd();
}

void COpenGLRender::RenderArrowHead(const CVector &vPosition,const CVector &vDirection,CVector &vUp,double dForward,double dUp,double dRight)
{
	CVector pvBase[4];
	CVector vHead;
	CVector vRight=vDirection^vUp;
	
	pvBase[3]=vPosition+vUp*dUp-vRight*dRight;
	pvBase[2]=vPosition+vUp*dUp+vRight*dRight;
	pvBase[1]=vPosition-vUp*dUp+vRight*dRight;
	pvBase[0]=vPosition-vUp*dUp-vRight*dRight;
	vHead=vPosition+vDirection*dForward;
	
	glBegin(GL_QUADS);
	for(unsigned int x=0;x<4;x++){glVertex3dv(pvBase[x].c);}
	glEnd();
	
	glBegin(GL_TRIANGLES);
	for(unsigned int x=0;x<4;x++)
	{
		glVertex3dv(vHead.c);
		glVertex3dv(pvBase[(x+1)&0x3].c);
		glVertex3dv(pvBase[x].c);
	}
	glEnd();
}

void COpenGLRender::RenderPyramid(const CVector &vTopVertex,const CVector &vSizes,bool bSolid)
{
	CVector vPyramidBaseRect[4];
	vPyramidBaseRect[0]=vTopVertex+CVector(-vSizes.c[0],-vSizes.c[1],-vSizes.c[2]);
	vPyramidBaseRect[1]=vTopVertex+CVector(-vSizes.c[0],-vSizes.c[1],vSizes.c[2]);
	vPyramidBaseRect[2]=vTopVertex+CVector(vSizes.c[0],-vSizes.c[1],vSizes.c[2]);
	vPyramidBaseRect[3]=vTopVertex+CVector(vSizes.c[0],-vSizes.c[1],-vSizes.c[2]);

	if(!bSolid)
	{
		glBegin(GL_LINES);
		glVertex3dv(vTopVertex.c);
		glVertex3dv(vPyramidBaseRect[0].c);

		glVertex3dv(vTopVertex.c);
		glVertex3dv(vPyramidBaseRect[1].c);

		glVertex3dv(vTopVertex.c);
		glVertex3dv(vPyramidBaseRect[2].c);

		glVertex3dv(vTopVertex.c);
		glVertex3dv(vPyramidBaseRect[3].c);
		glEnd();
	}

	if(bSolid)
	{
		glBegin(GL_TRIANGLES);
		glVertex3dv(vPyramidBaseRect[0].c);
		glVertex3dv(vPyramidBaseRect[1].c);
		glVertex3dv(vTopVertex.c);

		glVertex3dv(vTopVertex.c);
		glVertex3dv(vPyramidBaseRect[2].c);
		glVertex3dv(vPyramidBaseRect[3].c);

		//Lateral
		glVertex3dv(vPyramidBaseRect[1].c);
		glVertex3dv(vPyramidBaseRect[2].c);
		glVertex3dv(vTopVertex.c);

		//Lateral
		glVertex3dv(vTopVertex.c);
		glVertex3dv(vPyramidBaseRect[3].c);
		glVertex3dv(vPyramidBaseRect[0].c);
		glEnd();
	}

	glBegin(GL_QUADS);
	// Base
	glVertex3dv(vPyramidBaseRect[3].c);
	glVertex3dv(vPyramidBaseRect[2].c);
	glVertex3dv(vPyramidBaseRect[1].c);
	glVertex3dv(vPyramidBaseRect[0].c);

	glEnd();
}

void COpenGLRender::RenderPyramid(const CVector &vTopVertex,double dUpperSizeX,double dUpperSizeZ,double dLowerSizeX,double dLowerSizeZ,double dHeight,bool bSolid)
{
	CVector vPyramidLowerRect[4],vPyramidUpperRect[4]; 

	vPyramidUpperRect[0]=vTopVertex+CVector(-dUpperSizeX,0,-dUpperSizeZ);
	vPyramidUpperRect[1]=vTopVertex+CVector(-dUpperSizeX,0,dUpperSizeZ);
	vPyramidUpperRect[2]=vTopVertex+CVector(dUpperSizeX,0,dUpperSizeZ);
	vPyramidUpperRect[3]=vTopVertex+CVector(dUpperSizeX,0,-dUpperSizeZ);

	vPyramidLowerRect[0]=vTopVertex+CVector(-dLowerSizeX,-dHeight,-dLowerSizeZ);
	vPyramidLowerRect[1]=vTopVertex+CVector(-dLowerSizeX,-dHeight,dLowerSizeZ);
	vPyramidLowerRect[2]=vTopVertex+CVector(dLowerSizeX,-dHeight,dLowerSizeZ);
	vPyramidLowerRect[3]=vTopVertex+CVector(dLowerSizeX,-dHeight,-dLowerSizeZ);

	if(!bSolid)
	{
		glBegin(GL_LINES);
		glVertex3dv(vPyramidUpperRect[0].c);
		glVertex3dv(vPyramidLowerRect[0].c);

		glVertex3dv(vPyramidUpperRect[1].c);
		glVertex3dv(vPyramidLowerRect[1].c);

		glVertex3dv(vPyramidUpperRect[2].c);
		glVertex3dv(vPyramidLowerRect[2].c);

		glVertex3dv(vPyramidUpperRect[3].c);
		glVertex3dv(vPyramidLowerRect[3].c);
		glEnd();
	}

	glBegin(GL_QUADS);
	glVertex3dv(vPyramidLowerRect[3].c);
	glVertex3dv(vPyramidLowerRect[2].c);
	glVertex3dv(vPyramidLowerRect[1].c);
	glVertex3dv(vPyramidLowerRect[0].c);

	glVertex3dv(vPyramidUpperRect[0].c);
	glVertex3dv(vPyramidUpperRect[1].c);
	glVertex3dv(vPyramidUpperRect[2].c);
	glVertex3dv(vPyramidUpperRect[3].c);

	glVertex3dv(vPyramidLowerRect[1].c);
	glVertex3dv(vPyramidLowerRect[2].c);
	glVertex3dv(vPyramidUpperRect[2].c);
	glVertex3dv(vPyramidUpperRect[1].c);

	glVertex3dv(vPyramidUpperRect[0].c);
	glVertex3dv(vPyramidUpperRect[3].c);
	glVertex3dv(vPyramidLowerRect[3].c);
	glVertex3dv(vPyramidLowerRect[0].c);

	glVertex3dv(vPyramidUpperRect[1].c);
	glVertex3dv(vPyramidUpperRect[0].c);
	glVertex3dv(vPyramidLowerRect[0].c);
	glVertex3dv(vPyramidLowerRect[1].c);

	glVertex3dv(vPyramidLowerRect[2].c);
	glVertex3dv(vPyramidLowerRect[3].c);
	glVertex3dv(vPyramidUpperRect[3].c);
	glVertex3dv(vPyramidUpperRect[2].c);

	glEnd();
}

/*
void CScenarioEditorMainWindow::RenderPlane(CVector vMins,CVector vMaxs,CVector vColor,double dAlpha)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glColor4d(vColor.c[0],vColor.c[1],vColor.c[2],dAlpha);
	glDisable(GL_TEXTURE_2D);

	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glBegin(GL_QUADS);
	// Front
	glVertex3f(vMaxs.c[0],vMins.c[1],vMins.c[2]);
	glVertex3f(vMaxs.c[0],vMins.c[1],vMaxs.c[2]);
	glVertex3f(vMins.c[0],vMins.c[1],vMaxs.c[2]);
	glVertex3f(vMins.c[0],vMins.c[1],vMins.c[2]);

	// Back
	glVertex3f(vMins.c[0],vMaxs.c[1],vMins.c[2]);
	glVertex3f(vMins.c[0],vMaxs.c[1],vMaxs.c[2]);
	glVertex3f(vMaxs.c[0],vMaxs.c[1],vMaxs.c[2]);
	glVertex3f(vMaxs.c[0],vMaxs.c[1],vMins.c[2]);

	glEnd();
	glPopAttrib();
	glEnable(GL_TEXTURE_2D);
}
*/
void COpenGLRender::ActivateHeightFog(const CVector &vMins,const CVector &vMaxs,const CVector &vColor)
{
	if(!m_sRenderOptions.bEnableHeightFog){return;}

	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveHeightFog=true;
	pState->vHeightFogMins=vMins;
	pState->vHeightFogMaxs=vMaxs;
	pState->vHeightFogColor=vColor;
	
	if(!m_bStagedRendering)
	{
		if(!m_bRenderingWithShader)
		{
			if(m_vCameraForward==AxisNegY)
			{
			glEnable(GL_FOG);
			glFogf(GL_FOG_START,m_vCameraPos.c[1]-vMaxs.c[1]);
			glFogf(GL_FOG_END,m_vCameraPos.c[1]-vMins.c[1]);
			}
		}
		else
		{
			float vHeightFogColor[3]={(float)vColor.c[0],(float)vColor.c[1],(float)vColor.c[2]};
			glFogfv(GL_FOG_COLOR,vHeightFogColor);
			glFogf(GL_FOG_MODE,GL_LINEAR);
		}
	}
}

void COpenGLRender::ActivateWater()
{
	//if(!m_sRenderOptions.bEnableShader){return;}
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveWater=true;
}

void COpenGLRender::SetWaterMappingSize(double dMaxU,double dMaxV)
{
	std::map<SShaderKey,CGenericShaderWrapper>::iterator iShader;
	for(iShader=m_mShaders.begin();iShader!=m_mShaders.end();iShader++)
	{
		if(iShader->first.bWater)
		{
			iShader->second.m_piShader->AddUniform("WaterMappingSize",dMaxU,dMaxV);
		}
	}
}

void COpenGLRender::SetWaterMappingOffset(double dMaxU,double dMaxV)
{
	std::map<SShaderKey,CGenericShaderWrapper>::iterator iShader;
	for(iShader=m_mShaders.begin();iShader!=m_mShaders.end();iShader++)
	{
		if(iShader->first.bWater)
		{
			iShader->second.m_piShader->AddUniform("WaterMappingOffset",dMaxU,dMaxV);
		}
	}
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
	if(!m_bRenderingWithShader && !m_bStagedRendering && m_vCameraForward==AxisNegY)
	{
		glDisable(GL_FOG);
	}
}
bool COpenGLRender::IsHeightFogActive(){return m_bActiveHeightFog=false;}


void COpenGLRender::ActivateTextures()
{
	if(!m_sRenderOptions.bEnableTextures){return;}

	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveTextures=true;
}
void COpenGLRender::DeactivateTextures()
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveTextures=false;
}
bool COpenGLRender::AreTexturesActive()
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	return pState->bActiveTextures;
}

void COpenGLRender::ActivateSolid()
{
	if(!m_sRenderOptions.bEnableSolid){return;}
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveSolid=true;
	if(!m_bStagedRendering){glPolygonMode(GL_FRONT,GL_FILL);}
}

void COpenGLRender::DeactivateSolid()
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveSolid=false;
	if(!m_bStagedRendering){glPolygonMode(GL_FRONT,GL_LINE);}
}
bool COpenGLRender::IsSolidActive()
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	return pState->bActiveSolid;
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
		if(m_piSkyShadow){m_piSkyShadow->PrepareTexture(this,m_nSkyShadowTextureLevel);}
	}
}

void COpenGLRender::DeactivateSkyShadow()
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveSkyShadow=false;

	if(!m_bStagedRendering && m_piSkyShadow && m_sRenderState.bActiveSkyShadow)
	{
		if(m_piSkyShadow){m_piSkyShadow->UnprepareTexture(this,m_nSkyShadowTextureLevel);}
	}
}

bool COpenGLRender::IsSkyShadowActive()
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	return pState->bActiveSkyShadow;
}
void COpenGLRender::ActivateDepth()
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveDepth=true;
	if(!m_bStagedRendering){glEnable(GL_DEPTH_TEST);glDepthFunc(m_sRenderState.nDepthFunction);}
}

void COpenGLRender::SetDepthFunction(unsigned int nDepthFunc)
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->nDepthFunction=nDepthFunc;
	if(!m_bStagedRendering){glDepthFunc(nDepthFunc);}
}

void COpenGLRender::DeactivateDepth()
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveDepth=false;
	if(!m_bStagedRendering){glDisable(GL_DEPTH_TEST);}
}

void COpenGLRender::ActivateBlending()
{
	if(!m_sRenderOptions.bEnableBlending){return;}
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveBlending=true;
	if(!m_bStagedRendering)
	{
		glEnable(GL_BLEND);
		glBlendFunc(m_sRenderState.nBlendOperator1, m_sRenderState.nBlendOperator2);
	}
}

void COpenGLRender::SetBlendingLayer(unsigned int nLayer)
{
	if(!m_sRenderOptions.bEnableBlending){return;}
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->nBlendingLayer=nLayer;
}

void COpenGLRender::SetBlendingFunction(unsigned int nOperator1,unsigned int nOperator2)
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->nBlendOperator1=nOperator1;
	pState->nBlendOperator2=nOperator2;
	if(!m_bStagedRendering)
	{
		glBlendFunc(nOperator1, nOperator2);
	}
}

void COpenGLRender::DeactivateBlending()
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveBlending=false;
	if(!m_bStagedRendering)
	{
		glDisable(GL_BLEND);
	}
}

bool COpenGLRender::IsBlendingActive()
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	return pState->bActiveBlending;
}

void COpenGLRender::ActivateLighting()
{
	if(!m_sRenderOptions.bEnableLighting){return;}
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveLighting=true;
	if(!m_bStagedRendering){PrepareLighting();}
}

void COpenGLRender::DeactivateLighting()
{
	if(!m_sRenderOptions.bEnableLighting){return;}
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->bActiveLighting=false;
	if(!m_bStagedRendering){UnPrepareLighting();}
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
	if(m_bLightingPrepared){return;}


	const GLfloat pfAmbienColor[]  = {(float)m_vAmbientColor.c[0], (float)m_vAmbientColor.c[1], (float)m_vAmbientColor.c[2], 1.0};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,pfAmbienColor);
	if(!m_bRenderingWithShader)
	{
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE ,0);
		glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER ,1);
		glEnable(GL_LIGHTING);
	}

	std::vector<IGenericLight *>::iterator i;
	int nlIndex=GL_LIGHT0;
	for(i=m_vLights.begin(); nlIndex<(GL_LIGHT0+m_sHardwareSupport.nMaxLights) && i!=m_vLights.end();i++)
	{
		IGenericLight *piLight=*i;
		eGenericLightType eType=piLight->GetType();
		if(!m_bRenderingWithShader){glDisable(nlIndex);}

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
		if(!m_bRenderingWithShader){glEnable(nlIndex);}
		nlIndex++;
		m_nActiveLights++;
	}
	m_bLightingPrepared=true;
}

void COpenGLRender::UnPrepareLighting()
{
	if(!m_bLightingPrepared){return;}
	std::set<IGenericLight *>::iterator i;
	for(int nlIndex=GL_LIGHT0;nlIndex<(GL_LIGHT0+m_sHardwareSupport.nMaxLights) && nlIndex<(GL_LIGHT0+(int)m_nActiveLights);nlIndex++)
	{
		float vColor[4]={0};
		glLightfv(nlIndex,GL_AMBIENT,vColor);
		glLightfv(nlIndex,GL_DIFFUSE ,vColor);
		glLightfv(nlIndex,GL_SPECULAR,vColor);
		if(!m_bRenderingWithShader){glDisable(nlIndex);}
	}
	m_nActiveLights=0;
	if(!m_bRenderingWithShader)
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_COLOR_MATERIAL);
	}
	m_bLightingPrepared=false;
}

bool COpenGLRender::IsLightingActive()
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	return pState->bActiveLighting;
}

void COpenGLRender::RenderPoint( const CVector &vPosition,double dSize,const CVector &vColor,double dAlpha )
{

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
		glColor4d(vColor.c[0],vColor.c[1],vColor.c[2],dAlpha);
		glPointSize((GLfloat)dSize);
		glBegin(GL_POINTS);
		glVertex3dv(vPosition.c);
		glEnd();
	}
}

void COpenGLRender::RenderModel(const CVector &vOrigin,const CVector &vOrientation,IGenericModel *piModel,unsigned int nAnimation,unsigned int nFrame)
{
	if(m_bStagedRendering)
	{
		bool bSkip=false;

		if(m_sRenderOptions.bEnableShadows)
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
		sInstance.bSkipRender=bSkip;
		pStage->vInstances.push_back(sInstance);
	}
	else
	{
		glPushMatrix();
		glTranslated(vOrigin.c[0],vOrigin.c[1],vOrigin.c[2]);
		glRotated(vOrientation.c[YAW]	,0,1,0);
		glRotated(vOrientation.c[PITCH]	,0,0,1);
		glRotated(vOrientation.c[ROLL]	,1,0,0);

		IOpenGLModel *piGLModel=QI(IOpenGLModel,piModel);
		if(piGLModel)
		{
			for(unsigned long x=0;x<piModel->GetFrameRenderBuffers(nAnimation,nFrame);x++)
			{			
				piGLModel->PrepareRenderBuffer(this,nAnimation,nFrame,x,false,m_nNormalMapTextureLevel);
				piGLModel->CallRenderBuffer(this,nAnimation,nFrame,x);
				piGLModel->UnPrepareRenderBuffer(this,nAnimation,nFrame,x,false,m_nNormalMapTextureLevel);
			}
		}
		REL(piGLModel);
		glPopMatrix();
	}
}

void COpenGLRender::RenderParticle(IGenericTexture *piTexture,const CVector &vOrigin,double dAngle,double s1,double s2,const CVector &vColor,double dAlpha,double dTextX,double dTextY,double dTextW,double dTextH)
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
		float *pVertexBuffer=pBuffer->pVertexBuffer+(pBuffer->nUsedElements*4*3);
		float *pColorBuffer=pBuffer->pColorBuffer+(pBuffer->nUsedElements*4*4);
		float *pTexBuffer=pBuffer->pTexBuffers+(pBuffer->nUsedElements*4*2);

		CMatrix m;
		m.R(m_vCameraForward,DegreesToRadians(dAngle));

		CVector vAxis1=m_vCameraRight*(s1/2.0);
		CVector vAxis2=m_vCameraUp*(s2/2.0);
		CVector vVertex[4];
		vVertex[0]=(vAxis1+vAxis2);
		vVertex[1]=(Origin-vAxis1+vAxis2);
		vVertex[2]=(Origin-vAxis1-vAxis2);
		vVertex[3]=(vAxis1-vAxis2);

		for(int x=0;x<4;x++)
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
		*pTexBuffer++=dTextX+dTextW;*pTexBuffer++=dTextY;
		pBuffer->nUsedElements++;
	}
	else
	{
		SelectTexture(piTexture,0);
		glColor4d(vColor.c[0],vColor.c[1],vColor.c[2],dAlpha);

		glDepthMask(false);
		glPushMatrix();
		glTranslated(vOrigin.c[0],vOrigin.c[1],vOrigin.c[2]);
		glRotated(dAngle,m_vCameraForward.c[0],m_vCameraForward.c[1],m_vCameraForward.c[2]);

		CVector vAxis1=m_vCameraRight;
		CVector vAxis2=m_vCameraUp;
		glBegin(GL_QUADS);
		glTexCoord2f(dTextX+dTextW,dTextY+dTextH);
		glVertex3dv((Origin+vAxis1*(s1/2.0)+vAxis2*(s2/2.0)).c);
		glTexCoord2f(dTextX,dTextY+dTextH);
		glVertex3dv((Origin-vAxis1*(s1/2.0)+vAxis2*(s2/2.0)).c);
		glTexCoord2f(dTextX,dTextY);
		glVertex3dv((Origin-vAxis1*(s1/2.0)-vAxis2*(s2/2.0)).c);
		glTexCoord2f(dTextX+dTextW,dTextY);
		glVertex3dv((Origin+vAxis1*(s1/2.0)-vAxis2*(s2/2.0)).c);
		glEnd();
		glPopMatrix();
		glDepthMask(true);
		UnselectTexture(0);
	}
}
void COpenGLRender::StartStagedRendering()
{
	m_sPreStagedRenderingState=m_sRenderState;
	m_sStagedRenderingState=m_sRenderState;
	SetRenderState(m_sRenderState,true);
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

void COpenGLRender::EndStagedRendering()
{
	if(!m_bStagedRendering){return;}

	unsigned int nRenderStart=0;
	if(m_sRenderOptions.bEnableStagedRenderingStats){nRenderStart=GetTimeStamp();m_sStagedStats=SRenderStats();}
	
	m_bStagedRendering=false;
	
	if(m_sRenderOptions.bEnableShadows)
	{
		if(m_ShadowTexture.m_piTexture==NULL)
		{
			if(m_ShadowTexture.Create(m_piSystem,"Texture",""))
			{
				if(!m_ShadowTexture.m_piTexture->CreateDepth(1024,1024,m_piCurrentViewport))
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
					m_ShadowShader.m_piShader->Load("Shaders/ShadowBufferShader-Vertex.c","Shaders/ShadowBufferShader-Fragment.c","");
					if(m_bPrecompileShaders){m_ShadowShader.m_piShader->Compile();}
				}
			}
		}
	}
	CVector vLightAngles;
	CVector pvCameraVolume[8];
	CVector pvLightVolume[8];
	CVector pvShadowVolume[8];
	CVector vLightPosition;
	CVector vVolumeMidPoint;
	CVector vLightForward;
	double dLightNear;
	double dLightFar;

	m_bRenderingWithShader=(m_sRenderOptions.bEnableShader && m_sHardwareSupport.bShaders);

	double pCameraModelViewMatrix[16];
	double pCameraProjectionMatrix[16];
	CMatrix cameraModelViewMatrix;
	CMatrix cameraProjectionMatrix;
	glGetDoublev(GL_MODELVIEW_MATRIX, pCameraModelViewMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, pCameraProjectionMatrix);
	FromOpenGLMatrix(pCameraModelViewMatrix,&cameraModelViewMatrix);
	FromOpenGLMatrix(pCameraProjectionMatrix,&cameraProjectionMatrix);
	CMatrix invertedCameraMatrix=cameraModelViewMatrix;
	invertedCameraMatrix.Inverse();

	float fCurrentTime=((double)(GetTimeStamp()-m_nFirstTimeStamp))*0.001;
	std::map<SShaderKey,CGenericShaderWrapper>::iterator iShader;
	for(iShader=m_mShaders.begin();iShader!=m_mShaders.end();iShader++)
	{
		if(iShader->first.bHeightFog || iShader->first.bSkyShadow)
		{
			iShader->second.m_piShader->AddUniform("CameraModelViewInverse",(double*)invertedCameraMatrix.e);
		}
		if(iShader->first.bSkyShadow)
		{
			iShader->second.m_piShader->AddUniform("SkyData",CVector(fCurrentTime*m_dSKyShadowSpeed,m_dSKyShadowXResolution,m_dSKyShadowZResolution),m_dSKyShadowOpacity);
		}
		if(iShader->first.bWater)
		{
			iShader->second.m_piShader->AddUniform("CurrentRealTime",fCurrentTime);
		}
	}

	double dPreviousNear=m_dPerspectiveNearPlane;
	double dPreviousFar=m_dPerspectiveFarPlane;
	double dPreviousViewAngle=m_dPerspectiveViewAngle;
	
	if(m_sRenderOptions.bEnableShadows && m_ShadowTexture.m_piTexture && m_sHardwareSupport.nMaxTextureUnits>1)
	{
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
		double pLightProjectionMatrix[16];

		PushOptions();
		m_sRenderOptions.bEnableHeightFog=false;

		m_ShadowTexture.m_piTexture->StartRenderingToTexture();

		glPushAttrib(GL_ALL_ATTRIB_BITS);
		
		// Render Shadows
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		unsigned dwWidth=0,dwHeight=0;
		m_ShadowTexture.m_piTexture->GetSize(&dwWidth,&dwHeight);
		SetViewport(0, 0, dwWidth, dwHeight);
		SetPerspectiveProjection(dLightViewAngle,dLightNear,dLightFar);
		SetCamera(vLightPosition,vLightAngles.c[YAW],vLightAngles.c[PITCH],vLightAngles.c[ROLL]);

		CalcCameraVolume(vLightPosition,vLightAngles,dLightViewAngle,1,dLightNear,dLightFar,pvLightVolume);

		glGetDoublev(GL_PROJECTION_MATRIX, pLightProjectionMatrix);
		glGetDoublev(GL_MODELVIEW_MATRIX, pLightModelViewMatrix);

		glClear(GL_DEPTH_BUFFER_BIT);

		if(m_sRenderOptions.bEnableShader && m_ShadowShader.m_piShader){m_ShadowShader.m_piShader->Activate();}
		RenderModelStages(true,false);
		RenderModelStages(true,true);
		if(m_sRenderOptions.bEnableShader && m_ShadowShader.m_piShader){m_ShadowShader.m_piShader->Deactivate();}

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		glPopAttrib();
		m_ShadowTexture.m_piTexture->StopRenderingToTexture();
		PopOptions();

		SetViewport(nPreviousViewportX, nPreviousViewportY,nPreviousViewportW, nPreviousViewportH);
		SetPerspectiveProjection(dPreviousViewAngle,dPreviousNear,dPreviousFar);//m_dStagedRenderingMinZ>1.0?m_dStagedRenderingMinZ-1.0:m_dStagedRenderingMinZ,m_dStagedRenderingMaxZ+1.0);
		SetCamera(vPreviousCameraPosition,vPreviousCameraAngles.c[YAW],vPreviousCameraAngles.c[PITCH],vPreviousCameraAngles.c[ROLL]);

		// Se hace esto porque el inicio y fin del pintado a textura hace un push attrib y pop attrib
		// ya que pueden estar en otro contexto de render dependiendo de la tecnica usada.
		SetRenderState(m_sStagedRenderingState,true);

		//glPushAttrib(GL_ALL_ATTRIB_BITS);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		//Calculate texture matrix for projection
		//This matrix takes us from eye space to the light's clip space
		//It is postmultiplied by the inverse of the current view matrix when specifying texgen
		CMatrix biasMatrix;
		biasMatrix.e[0][0]=0.5f;
		biasMatrix.e[0][1]=0.0f;
		biasMatrix.e[0][2]=0.0f;
		biasMatrix.e[0][3]=0.5f;

		biasMatrix.e[1][0]=0.0f;
		biasMatrix.e[1][1]=0.5f;
		biasMatrix.e[1][2]=0.0f;
		biasMatrix.e[1][3]=0.5f;

		biasMatrix.e[2][0]=0.0f;
		biasMatrix.e[2][1]=0.0f;
		biasMatrix.e[2][2]=0.5f;
		biasMatrix.e[2][3]=0.5f;

		biasMatrix.e[3][0]=0.0f;
		biasMatrix.e[3][1]=0.0f;
		biasMatrix.e[3][2]=0.0f;
		biasMatrix.e[3][3]=1.0f;

		CMatrix lightModelViewMatrix;
		CMatrix lightProjectionMatrix;

		FromOpenGLMatrix(pLightModelViewMatrix,&lightModelViewMatrix);
		FromOpenGLMatrix(pLightProjectionMatrix,&lightProjectionMatrix);

		CMatrix textureMatrix;
		CMatrix polyoffsetmatrix;
		polyoffsetmatrix.T(Origin-m_vCameraForward);
		textureMatrix*=polyoffsetmatrix;
		textureMatrix*=lightModelViewMatrix;
		textureMatrix*=lightProjectionMatrix;
		textureMatrix*=biasMatrix;
		
		double dTextureMatrix[16];
		ToOpenGLMatrix(&textureMatrix,dTextureMatrix);

		m_ShadowTexture.m_piTexture->PrepareTexture(this,m_bRenderingWithShader?m_nShadowTextureLevel:1);
		//Set up texture coordinate generation.

		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		glTexGendv(GL_S, GL_EYE_PLANE,textureMatrix.e[0]); 
		glEnable(GL_TEXTURE_GEN_S);

		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		glTexGendv(GL_T, GL_EYE_PLANE,textureMatrix.e[1]);
		glEnable(GL_TEXTURE_GEN_T);

		glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		glTexGendv(GL_R, GL_EYE_PLANE,textureMatrix.e[2]);
		glEnable(GL_TEXTURE_GEN_R);

		glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		glTexGendv(GL_Q, GL_EYE_PLANE,textureMatrix.e[3]);
		glEnable(GL_TEXTURE_GEN_Q);

		//Enable shadow comparison
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);
		glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY);

		if(!m_bRenderingWithShader)
		{
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE0);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);

			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_PRIMARY_COLOR);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
		}

		if(!m_bRenderingWithShader && m_sHardwareSupport.nMaxTextureUnits>2)
		{
			const GLfloat pfAmbienColor[]  = {(float)(m_vAmbientColor.c[0]*2.0), (float)(m_vAmbientColor.c[1]*2.0), (float)(m_vAmbientColor.c[2]*2.0),1.0};
			m_ShadowTexture.m_piTexture->PrepareTexture(this,2);
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


		if(!m_bRenderingWithShader && m_sHardwareSupport.nMaxTextureUnits>3)
		{
			m_ShadowTexture.m_piTexture->PrepareTexture(this,3);

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
		if(m_sRenderOptions.bEnableStagedRenderingStats)
		{
			glFinish();
			m_sStagedStats.nShadowTime=GetTimeStamp()-nShadowStart;
		}
		
		if(m_bRenderingWithShader && m_piSkyShadow && m_sRenderOptions.bEnableSkyShadow){m_piSkyShadow->PrepareTexture(this,m_nSkyShadowTextureLevel);}
		RenderAllStages(false,true);
		
		m_ShadowTexture.m_piTexture->UnprepareTexture(this,m_bRenderingWithShader?m_nShadowTextureLevel:1);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);
		if(!m_bRenderingWithShader && m_sHardwareSupport.nMaxTextureUnits>2)
		{
			m_ShadowTexture.m_piTexture->UnprepareTexture(this,2);
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
			glDisable(GL_TEXTURE_GEN_R);
			glDisable(GL_TEXTURE_GEN_Q);
		}
		if(!m_bRenderingWithShader && m_sHardwareSupport.nMaxTextureUnits>3)
		{
			m_ShadowTexture.m_piTexture->UnprepareTexture(this,3);
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
			glDisable(GL_TEXTURE_GEN_R);
			glDisable(GL_TEXTURE_GEN_Q);
		}
	}
	else
	{
		//SetPerspectiveProjection(m_dPerspectiveViewAngle,m_dStagedRenderingMinZ>1.0?m_dStagedRenderingMinZ-1.0:m_dStagedRenderingMinZ,m_dStagedRenderingMaxZ+1.0);
		if(m_bRenderingWithShader && m_piSkyShadow && m_sRenderOptions.bEnableSkyShadow){m_piSkyShadow->PrepareTexture(this,m_nSkyShadowTextureLevel);}
		RenderAllStages(false,true);
	}

	RenderAllStages(false,false);
	
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
	if(m_bRenderingWithShader && m_piSkyShadow && m_sRenderOptions.bEnableSkyShadow){m_piSkyShadow->UnprepareTexture(this,m_nSkyShadowTextureLevel);}
	
	m_mModelStages.clear();
	m_mTextureParticleStages.clear();
	m_mLineStages.clear();
	m_mPointStages.clear();

	if(m_sRenderOptions.bEnableStagedRenderingStats)
	{
		glFinish();
		m_sStagedStats.nRenderTime=GetTimeStamp()-nRenderStart;
	}
	
	
	//SetPerspectiveProjection(dPreviousViewAngle,dPreviousNear,dPreviousFar);
	SetRenderState(m_sPreStagedRenderingState,true);
	if(m_pCurrentShader){m_pCurrentShader->m_piShader->Deactivate();m_pCurrentShader=NULL;}
	m_bRenderingWithShader=false;
}
void 			COpenGLRender::SetShadingModel(EShadingModel eModel)
{
	SRenderState *pState=m_bStagedRendering?&m_sStagedRenderingState:&m_sRenderState;
	pState->eShadingModel=eModel;
}

EShadingModel 	COpenGLRender::GetShadingModel(){return m_sRenderState.eShadingModel;}

SRenderStats COpenGLRender::GetStagedRenderingStats()
{
	return m_sStagedStats;
}

void COpenGLRender::SetRenderState( const SRenderState &sNewState,bool bForce)
{
	m_sRenderState.bActiveShadowEmission=sNewState.bActiveShadowEmission;
	m_sRenderState.bActiveShadowReception=sNewState.bActiveShadowReception;
	m_sRenderState.bActiveWater=sNewState.bActiveWater;
	m_sRenderState.eShadingModel=sNewState.eShadingModel;
	
	bool bStateChange=false;
	if(bForce || m_sRenderState.bActiveTextures!=sNewState.bActiveTextures)
	{
		sNewState.bActiveTextures?ActivateTextures():DeactivateTextures();
		bStateChange=true;
	}
	if(bForce || m_sRenderState.bActiveSolid!=sNewState.bActiveSolid)
	{
		sNewState.bActiveSolid?ActivateSolid():DeactivateSolid();
		bStateChange=true;
	}
	if(bForce || m_sRenderState.bActiveLighting!=sNewState.bActiveLighting)
	{
		sNewState.bActiveLighting?ActivateLighting():DeactivateLighting();
		bStateChange=true;
	}
	if(bForce || m_sRenderState.bActiveBlending!=sNewState.bActiveBlending)
	{
		sNewState.bActiveBlending?ActivateBlending():DeactivateBlending();
		bStateChange=true;
	}
	if(bForce || m_sRenderState.nBlendOperator1!=sNewState.nBlendOperator1 ||
		m_sRenderState.nBlendOperator2!=sNewState.nBlendOperator2)
	{
		SetBlendingFunction(sNewState.nBlendOperator1,sNewState.nBlendOperator2);
		bStateChange=true;
	}
	if(bForce || m_sRenderState.bActiveDepth!=sNewState.bActiveDepth)
	{
		sNewState.bActiveDepth?ActivateDepth():DeactivateDepth();
		bStateChange=true;
	}
	if(bForce || m_sRenderState.bActiveSkyShadow!=sNewState.bActiveSkyShadow)
	{
		sNewState.bActiveSkyShadow?ActivateSkyShadow():DeactivateSkyShadow();
	}
	if(bForce || m_sRenderState.nDepthFunction!=sNewState.nDepthFunction)
	{
		SetDepthFunction(sNewState.nDepthFunction);
		bStateChange=true;
	}
	if(bForce || m_sRenderState.bActiveHeightFog!=sNewState.bActiveHeightFog)
	{
		sNewState.bActiveHeightFog?ActivateHeightFog(sNewState.vHeightFogMins,sNewState.vHeightFogMaxs,sNewState.vHeightFogColor):DeactivateHeightFog();
	}
	if(bStateChange && m_sRenderOptions.bEnableStagedRenderingStats){m_sStagedStats.nStateChanges++;}
	   
	
	if(m_bRenderingWithShader)
	{
		bool bTempSkyShadow=m_piSkyShadow && m_piSkyShadow && m_sRenderOptions.bEnableSkyShadow && sNewState.bActiveSkyShadow && sNewState.bActiveShadowReception;
		SShaderKey key(sNewState.eShadingModel,sNewState.bActiveHeightFog,m_sRenderOptions.bEnableShadows && m_bRenderingShadowReception,sNewState.bActiveTextures && m_sRenderOptions.bEnableTextures?m_mTextureLevels.size():0,m_sRenderOptions.bEnableLighting && sNewState.bActiveLighting,sNewState.bActiveWater,m_sRenderOptions.bEnableNormalMaps && m_piNormalMap,bTempSkyShadow);
		std::map<SShaderKey,CGenericShaderWrapper>::iterator iShader=m_mShaders.find(key);
		CGenericShaderWrapper *pNewShader=(iShader==m_mShaders.end())?NULL:&iShader->second;
		if(m_pCurrentShader!=pNewShader)
		{
			if(m_sRenderOptions.bEnableStagedRenderingStats){m_sStagedStats.nShaderChanges++;}
			if(m_pCurrentShader){m_pCurrentShader->m_piShader->Deactivate();}
			m_pCurrentShader=pNewShader;

			if(m_pCurrentShader)
			{
				m_pCurrentShader->m_piShader->Activate();
				if(key.bLighting){m_pCurrentShader->m_piShader->AddUniform("g_ActiveLights",(int)m_nActiveLights);}
				if(key.bHeightFog)
				{
					m_pCurrentShader->m_piShader->AddUniform("g_vHeightFogMins",m_sRenderState.vHeightFogMins);
					m_pCurrentShader->m_piShader->AddUniform("g_vHeightFogMaxs",m_sRenderState.vHeightFogMaxs);
				}
			}
		}
	}
}

void COpenGLRender::EnableShaders()
{
	if(!m_sHardwareSupport.bShaders){return;}
	m_sRenderOptions.bEnableShader=true;
}
void COpenGLRender::DisableShaders(){m_sRenderOptions.bEnableShader=false;}
bool COpenGLRender::AreShadersEnabled(){return m_sRenderOptions.bEnableShader;}

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
	RTTRACE("                      %d ms Render",m_sStagedStats.nRenderTime);
	RTTRACE("                      %d ms Shadow",m_sStagedStats.nShadowTime);
}

void COpenGLRender::RenderModelStages(bool bRenderingShadow,bool bShadowReceptionState)
{
	// Flush Model Stages

	std::map<SModelStageKey,SModelStage>::iterator iModelStage;
	for(iModelStage=m_mModelStages.begin();iModelStage!=m_mModelStages.end();iModelStage++)
	{
		const SModelStageKey *pKey=&iModelStage->first;
		SModelStage *pStage=&iModelStage->second;
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
			
			// For each instance
			for(unsigned int nInstance=0;nInstance<pStage->vInstances.size();nInstance++)
			{
				CVector vPos=pStage->vInstances[nInstance].vPos;
				CVector vAngles=pStage->vInstances[nInstance].vAngles;
				bool bSkipRender=pStage->vInstances[nInstance].bSkipRender;
				if(!bRenderingShadow && bSkipRender){continue;}
				if(!bRenderBufferPrepared)
				{
					bRenderBufferPrepared=true;
					pStage->piGLModel->PrepareRenderBuffer(this,pKey->nAnimation,pKey->nFrame,nBuffer,bRenderingShadow,m_nNormalMapTextureLevel);
					SetRenderState(pKey->sRenderState,false);
				}
				
				glPushMatrix();
				glTranslated(vPos.c[0],vPos.c[1],vPos.c[2]);
				if(vAngles.c[YAW]){glRotated(vAngles.c[YAW]	,0,1,0);}
				if(vAngles.c[PITCH]){glRotated(vAngles.c[PITCH]	,0,0,1);}
				if(vAngles.c[ROLL]){glRotated(vAngles.c[ROLL]	,1,0,0);}
				pStage->piGLModel->CallRenderBuffer(this,pKey->nAnimation,pKey->nFrame,nBuffer);
				glPopMatrix();
			}
			
			if(bRenderBufferPrepared)
			{
				pStage->piGLModel->UnPrepareRenderBuffer(this,pKey->nAnimation,pKey->nFrame,nBuffer,bRenderingShadow,m_nNormalMapTextureLevel);
			}
		}
	}
}

void COpenGLRender::RenderParticleStages(bool bRenderingShadow,bool bShadowReceptionState)
{
	// Flush Texture Particles Stages.

	if(!bRenderingShadow){glDepthMask(false);}
	glClientActiveTextureARB(GL_TEXTURE0_ARB+0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	std::map<STextureParticleStageKey,STextureParticleStage>::iterator iParticleStage;
	for(iParticleStage=m_mTextureParticleStages.begin();iParticleStage!=m_mTextureParticleStages.end();iParticleStage++)
	{
		const STextureParticleStageKey *pKey=&iParticleStage->first;
		STextureParticleStage *pStage=&iParticleStage->second;
		if(bRenderingShadow && !pKey->sRenderState.bActiveShadowEmission){continue;}
		if(bShadowReceptionState!=pKey->sRenderState.bActiveShadowReception){continue;}

		IGenericTexture *piTexture=pKey->piTexture;
		if(piTexture){SelectTexture(piTexture,0);}

		SetRenderState(pKey->sRenderState,false);

		for(unsigned int x=0;x<pStage->vBuffers.size();x++)
		{
			
			STextureParticleBuffer *pBuffer=pStage->vBuffers[x];
			glVertexPointer(3,GL_FLOAT,0,pBuffer->pVertexBuffer);
			glColorPointer(4,GL_FLOAT,0,pBuffer->pColorBuffer);
			glTexCoordPointer(2,GL_FLOAT,0,pBuffer->pTexBuffers);
			glDrawArrays(GL_QUADS,0,pBuffer->nUsedElements*4);
			if(!bRenderingShadow && m_sRenderOptions.bEnableStagedRenderingStats)
			{
				m_sStagedStats.nParticles+=pBuffer->nUsedElements;
				m_sStagedStats.nTotalFaces+=pBuffer->nUsedElements;
				m_sStagedStats.nTotalVertexes+=pBuffer->nUsedElements*4;
				m_sStagedStats.nInmediateFaces+=pBuffer->nUsedElements;
				m_sStagedStats.nInmediateVertexes+=pBuffer->nUsedElements*4;
			}
		}
		if(piTexture){UnselectTexture(0);}
	}
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	if(!bRenderingShadow){glDepthMask(true);}
}

void COpenGLRender::RenderLineStages(bool bRenderingShadow,bool bShadowReceptionState)
{
	glEnable(GL_LINE_STIPPLE);
	//glEnable(GL_LINE_SMOOTH);
	//glLineWidth(1);

	std::map<SLineStageKey,SLineStage>::iterator iLineStage;
	for(iLineStage=m_mLineStages.begin();iLineStage!=m_mLineStages.end();iLineStage++)
	{
		const SLineStageKey *pKey=&iLineStage->first;
		SLineStage *pStage=&iLineStage->second;

		if(bRenderingShadow && !pKey->sRenderState.bActiveShadowEmission){continue;}
		if(bShadowReceptionState!=pKey->sRenderState.bActiveShadowReception){continue;}


		SetRenderState(pKey->sRenderState,false);
		glLineStipple(1,(unsigned short)pKey->nStipple);

		for(unsigned int x=0;x<pStage->vBuffers.size();x++)
		{
			SLineBuffer *pBuffer=pStage->vBuffers[x];
			glVertexPointer(3,GL_FLOAT,0,pBuffer->pVertexBuffer);
			glColorPointer(4,GL_FLOAT,0,pBuffer->pColorBuffer);
			glDrawArrays(GL_LINES,0,pBuffer->nUsedElements*2);
			if(!bRenderingShadow && m_sRenderOptions.bEnableStagedRenderingStats)
			{
				m_sStagedStats.nLines+=pBuffer->nUsedElements;
				m_sStagedStats.nTotalVertexes+=pBuffer->nUsedElements*2;
				m_sStagedStats.nInmediateVertexes+=pBuffer->nUsedElements*2;				
			}
		}
	}
	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_LINE_STIPPLE);
}

void COpenGLRender::RenderPointStages(bool bRenderingShadow,bool bShadowReceptionState)
{
	std::map<SPointStageKey,SPointStage>::iterator iPointStage;
	for(iPointStage=m_mPointStages.begin();iPointStage!=m_mPointStages.end();iPointStage++)
	{
		const SPointStageKey *pKey=&iPointStage->first;
		SPointStage *pStage=&iPointStage->second;

		if(bRenderingShadow && !pKey->sRenderState.bActiveShadowEmission){continue;}
		if(bShadowReceptionState!=pKey->sRenderState.bActiveShadowReception){continue;}

		SetRenderState(pKey->sRenderState,false);
		glPointSize((float)pKey->dSize);

		for(unsigned int x=0;x<pStage->vBuffers.size();x++)
		{
			SPointBuffer *pBuffer=pStage->vBuffers[x];
			glVertexPointer(3,GL_FLOAT,0,pBuffer->pVertexBuffer);
			glColorPointer(4,GL_FLOAT,0,pBuffer->pColorBuffer);
			glDrawArrays(GL_POINTS,0,pBuffer->nUsedElements);
			if(!bRenderingShadow && m_sRenderOptions.bEnableStagedRenderingStats)
			{
				m_sStagedStats.nPoints+=pBuffer->nUsedElements;
				m_sStagedStats.nTotalVertexes+=pBuffer->nUsedElements;
				m_sStagedStats.nInmediateVertexes+=pBuffer->nUsedElements;				
			}
		}
	}
}

void COpenGLRender::RenderAllStages(bool bRenderingShadow,bool bShadowReceptionState)
{
	m_bRenderingShadowReception=bShadowReceptionState;
	RenderModelStages(bRenderingShadow,bShadowReceptionState);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	RenderParticleStages(bRenderingShadow,bShadowReceptionState);
	RenderLineStages(bRenderingShadow,bShadowReceptionState);
	RenderPointStages(bRenderingShadow,bShadowReceptionState);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
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
			SetRenderState(m_sRenderStateStack.top(),false);
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

bool COpenGLRender::IsRenderingWithShader(){return m_bRenderingWithShader;}

void COpenGLRender::AddShader( const SShaderKey &key )
{
	char sTemp[128];
	std::string sPreprocessor;
	if(key.bHeightFog){sPreprocessor+="#define ENABLE_FOG\n";}
	if(key.bWater){sPreprocessor+="#define ENABLE_WATER\n";}
	if(key.bShadows)
	{
		sPreprocessor+="#define ENABLE_SHADOWS\n";
		sPreprocessor+="#define ENABLE_SOFT_SHADOWS\n";
		sprintf(sTemp,"#define SHADOW_TEXTURE_LEVEL %d\n",m_nShadowTextureLevel);
		sPreprocessor+=sTemp;
	}
	if(key.bNormalMap)
	{
		sPreprocessor+="#define ENABLE_NORMAL_MAP\n";
		sprintf(sTemp,"#define NORMAL_MAP_TEXTURE_LEVEL %d\n",m_nNormalMapTextureLevel);
		sPreprocessor+=sTemp;
	}
	if(key.bSkyShadow)
	{
		sPreprocessor+="#define ENABLE_SKY_SHADOW\n";
		sprintf(sTemp,"#define SKY_TEXTURE_LEVEL %d\n",m_nSkyShadowTextureLevel);
		sPreprocessor+=sTemp;
	}
	if(key.nTextureUnits)
	{
		sPreprocessor+="#define ENABLE_TEXTURES\n";
		sprintf(sTemp,"#define TEXTURE_UNITS %d\n",key.nTextureUnits);
		sPreprocessor+=sTemp;
	}
	if(key.bLighting)
	{
		sPreprocessor+="#define ENABLE_LIGHTING\n";
	}
	CGenericShaderWrapper wrapper;
	if(wrapper.Create(m_piSystem,"Shader",""))
	{
		if(key.eShadingModel==eShadingModel_Gouraud)
		{
			wrapper.m_piShader->Load("Shaders/RenderShader-Gouraud-Vertex.c","Shaders/RenderShader-Gouraud-Fragment.c",sPreprocessor);
		}
		else if(key.eShadingModel==eShadingModel_Balanced)
		{
			wrapper.m_piShader->Load("Shaders/RenderShader-Balanced-Vertex.c","Shaders/RenderShader-Balanced-Fragment.c",sPreprocessor);
		}
		else if(key.eShadingModel==eShadingModel_Phong)
		{
			wrapper.m_piShader->Load("Shaders/RenderShader-Phong-Vertex.c","Shaders/RenderShader-Phong-Fragment.c",sPreprocessor);
		}
		if(m_bPrecompileShaders){wrapper.m_piShader->Compile();}
		if(key.nTextureUnits>=1){wrapper.m_piShader->AddUniform("Texture0",(int)0);}
		if(key.nTextureUnits>=2){wrapper.m_piShader->AddUniform("Texture1",(int)1);}
		if(key.bSkyShadow){wrapper.m_piShader->AddUniform("SkyShadowMap",(int)m_nSkyShadowTextureLevel);}
		if(key.bNormalMap){wrapper.m_piShader->AddUniform("NormalMap",(int)m_nNormalMapTextureLevel);}
		if(key.bShadows){wrapper.m_piShader->AddUniform("ShadowMap",(int)m_nShadowTextureLevel);}
		m_mShaders[key]=wrapper;
	}
}

void COpenGLRender::StartSelection(SGameRect rWindowRect,IGenericCamera *piCamera,double dx,double dy,double dPrecision)
{
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
	gluPickMatrix(rWindowRect.x+dx,rWindowRect.y+dy,dPrecision,dPrecision,viewport);
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

void COpenGLRender::SetSelectionId( unsigned int nId )
{
	glPopName();
	glPushName(nId);
}

int COpenGLRender::EndSelection()
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
	return nNewIndex;
}

void COpenGLRender::ReloadShaders()
{
	std::map<SShaderKey,CGenericShaderWrapper>::iterator iShader;
	for(iShader=m_mShaders.begin();iShader!=m_mShaders.end();iShader++)
	{
		iShader->second.Destroy();
	}
	m_mShaders.clear();
	
	if(m_sHardwareSupport.bShaders)
	{
		for(unsigned int eShading=eShadingModel_Gouraud;eShading<=eShadingModel_Phong;eShading++)
		{
			for(int bHeightFog=0;bHeightFog<=1;bHeightFog++)
			{
				for(int bShadows=0;bShadows<=1;bShadows++)
				{
					for(int bSkyShadow=0;bSkyShadow<=1;bSkyShadow++)
					{
						for(int bNormalMap=0;bNormalMap<=1;bNormalMap++)
						{
							for(int nTextures=0;nTextures<=m_sHardwareSupport.nMaxTextureUnits;nTextures++)
							{
								AddShader(SShaderKey((EShadingModel)eShading,bHeightFog!=0,bShadows!=0,nTextures,true,true,bNormalMap!=0,bSkyShadow!=0));
								AddShader(SShaderKey((EShadingModel)eShading,bHeightFog!=0,bShadows!=0,nTextures,true,false,bNormalMap!=0,bSkyShadow!=0));
								AddShader(SShaderKey((EShadingModel)eShading,bHeightFog!=0,bShadows!=0,nTextures,false,true,bNormalMap!=0,bSkyShadow!=0));
								AddShader(SShaderKey((EShadingModel)eShading,bHeightFog!=0,bShadows!=0,nTextures,false,false,bNormalMap!=0,bSkyShadow!=0));
							}
						}
					}
				}
			}
		}
	}
}
