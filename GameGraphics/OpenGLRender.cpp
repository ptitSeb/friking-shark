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

const unsigned int SPointBuffer::buffer_size=POINT_BUFFER_SIZE;
const unsigned int SLineBuffer::buffer_size=LINE_BUFFER_SIZE;
const unsigned int STriangleBuffer::buffer_size=TRIANGLE_BUFFER_SIZE;

COpenGLRender::COpenGLRender()
{
	SetShadowQuality(eShadowQuality_High);
	m_bIgnoreShaderSupport=false;
	m_bIgnoreInstancingSupport=false;
	m_piCurrentRenderPath=NULL;
	
	m_nStagedTextureLevels=0;
	
	m_bShadowVolumeFirstVertex=false;
	m_dShadowAntiFlickeringMargin=10;
	
	m_sScene.lighting.m_vAmbientColor=CVector(0.5,0.5,0.5);
	m_bStagedRendering=false;
	m_sScene.camera.m_dProjectionWidth=1;
	m_sScene.camera.m_dProjectionHeight=1;
	m_piCurrentViewport=NULL;
	m_sScene.camera.m_nViewportX=0;
	m_sScene.camera.m_nViewportY=0;
	m_sScene.camera.m_nViewportW=1;
	m_sScene.camera.m_nViewportH=1;
	m_sScene.sky.m_dSkyShadowSpeed=0.0;
	m_sScene.sky.m_dSkyShadowXResolution=1.0;
	m_sScene.sky.m_dSkyShadowZResolution=1.0;
	m_sScene.sky.m_dSkyShadowOpacity=1.0;

	m_sScene.camera.m_bPerspectiveProjection=false;
	m_sScene.camera.m_dPerspectiveNearPlane=1.0;
	m_sScene.camera.m_dPerspectiveFarPlane=10000.0;

	m_dStagedRenderingMinZ=0;
	m_dStagedRenderingMaxZ=0;
	m_dMinDistanceToLight=10000;
	
	m_nCreationTime=GetTimeStamp();
	m_sScene.sky.m_piSkyShadow=NULL;
	m_piCurrentRenderPath=NULL;
	
	memset(m_ppiStagedTextureLevels,0,sizeof(m_ppiStagedTextureLevels));
}

COpenGLRender::~COpenGLRender()
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
			m_sScene.lighting.m_piSunLight=ADD(m_SunLight.m_piLight);
			m_sScene.lighting.m_piSunLight->SetDirectional(CVector(0,-1,0));
			m_sScene.lighting.m_piSunLight->SetAmbientColor(CVector(0,0,0));
			m_sScene.lighting.m_piSunLight->SetDiffuseColor(CVector(0.5,0.5,0.5));
			m_sScene.lighting.m_piSunLight->SetSpecularColor(CVector(0.5,0.5,0.5));
			AddLight(m_sScene.lighting.m_piSunLight);
		}
	}
	return true;
}

void COpenGLRender::Destroy()
{
	if(m_piCurrentRenderPath){m_piCurrentRenderPath->Cleanup();}
	REL(m_piCurrentRenderPath);
	m_vRenderPaths.clear();
	
	REL(m_piCurrentViewport);
	REL(m_sScene.lighting.m_piSunLight);
	REL(m_sScene.sky.m_piSkyShadow);
	
	RemoveLight(m_SunLight.m_piLight);
	m_SunLight.Destroy();
	
	CSystemObjectBase::Destroy();
}

bool COpenGLRender::Setup(IGenericViewport *piViewport)
{
	REL(m_piCurrentViewport);
	m_piCurrentViewport=ADD(piViewport);

#ifdef ANDROID_GLES1
	m_sHardwareSupport.bShaders=false;
#elif defined ANDROID_GLES2
	m_sHardwareSupport.bShaders=true;
#else
	m_sHardwareSupport.bShaders=GLEE_VERSION_3_0 && !m_bIgnoreShaderSupport;
#endif

#ifdef ANDROID
	m_sHardwareSupport.bObjectInstancing=false;
#else
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,&m_sHardwareSupport.nMaxTextureUnits);
	m_sHardwareSupport.bObjectInstancing=GLEE_ARB_draw_instanced && !m_bIgnoreInstancingSupport;
#endif		

	bool bOk=false;

	for(unsigned int x=0;m_piCurrentRenderPath==NULL && x<m_vRenderPaths.size();x++)
	{
		std::string sName=m_vRenderPaths[x].m_piOpenGLRender->GetFriendlyName();
		if(!m_vRenderPaths[x].m_piOpenGLRender->Setup(this,m_piCurrentViewport,m_sHardwareSupport))
		{
			RTTRACE("COpenGLRender::Setup -> Failed to setup %s render path",sName.c_str());
		}
		else
		{
			bOk=true;
			m_piCurrentRenderPath=ADD(m_vRenderPaths[x].m_piOpenGLRender);
			RTTRACE("COpenGLRender::Setup -> Render path %s initialized",sName.c_str());
		}
	}
	if(!bOk)
	{
		RTTRACE("COpenGLRender::Setup -> ERROR: Failed to setup a render path!");
	}
	return bOk;
}

IGenericViewport *COpenGLRender::GetViewPort()
{
	return ADD(m_piCurrentViewport);
}

void COpenGLRender::SetOrthographicProjection(double cx,double cy)
{
	Flush();
	InternalSetOrthographicProjection(cx,cy);
}

void COpenGLRender::InternalSetOrthographicProjection(double cx,double cy)
{
	m_sScene.camera.m_bPerspectiveProjection=false;
	m_sScene.camera.m_dProjectionWidth=cx;
	m_sScene.camera.m_dProjectionHeight=cy;

	double dViewportW=m_sScene.camera.m_nViewportW;
	double dViewportH=m_sScene.camera.m_nViewportH;
	
	ComputeOrthographicMatrix(&m_sScene.camera.m_ProjectionMatrix,-m_sScene.camera.m_dProjectionWidth*0.5,m_sScene.camera.m_dProjectionWidth*0.5,-m_sScene.camera.m_dProjectionHeight*0.5,m_sScene.camera.m_dProjectionHeight*0.5,1,10000);
	m_sScene.camera.bProjectionModified=true;
}

void COpenGLRender::SetPerspectiveProjection(double dViewAngle,double dNearPlane,double dFarPlane)
{
	Flush();
	InternalSetPerspectiveProjection(dViewAngle,dNearPlane,dFarPlane);
}

void COpenGLRender::InternalSetPerspectiveProjection(double dViewAngle,double dNearPlane,double dFarPlane)
{
	m_sScene.camera.m_bPerspectiveProjection=true;
	m_sScene.camera.m_dPerspectiveNearPlane=dNearPlane;
	m_sScene.camera.m_dPerspectiveFarPlane=dFarPlane;
	m_sScene.camera.m_dPerspectiveViewAngle=dViewAngle;

	double dAspectRatio=((double)m_sScene.camera.m_nViewportW)/((double)m_sScene.camera.m_nViewportH);
	ComputePerspectiveMatrix(&m_sScene.camera.m_ProjectionMatrix,m_sScene.camera.m_dPerspectiveViewAngle,dAspectRatio,m_sScene.camera.m_dPerspectiveNearPlane,m_sScene.camera.m_dPerspectiveFarPlane);
	m_sScene.camera.bProjectionModified=true;
}

void COpenGLRender::SetViewport(double x,double y,double cx, double cy)
{
	Flush();
	InternalSetViewport(x,y,cx,cy);
}

void COpenGLRender::InternalSetViewport(double x,double y,double cx, double cy)
{
	if(m_sScene.camera.m_nViewportX!=(int)x || m_sScene.camera.m_nViewportY!=(int)y || m_sScene.camera.m_nViewportW!=(int)cx || m_sScene.camera.m_nViewportH!=(int)cy)
	{
		m_sScene.camera.m_nViewportX=(int)x;
		m_sScene.camera.m_nViewportY=(int)y;
		m_sScene.camera.m_nViewportW=(int)cx;
		m_sScene.camera.m_nViewportH=(int)cy;
		m_sScene.camera.bViewportModified=true;
	}
}

CVector COpenGLRender::GetCameraPosition(){return m_sScene.camera.m_vCameraPos;}
CVector COpenGLRender::GetCameraForward(){return m_sScene.camera.m_vCameraForward;}
CVector COpenGLRender::GetCameraRight(){return m_sScene.camera.m_vCameraRight;}
CVector COpenGLRender::GetCameraUp(){return m_sScene.camera.m_vCameraUp;}

void COpenGLRender::SetCamera(const CVector &vPosition,double dYaw, double dPitch, double dRoll)
{
	Flush();
	InternalSetCamera(vPosition,dYaw,dPitch,dRoll);
}

void COpenGLRender::InternalSetCamera(const CVector &vPosition,double dYaw, double dPitch, double dRoll)
{
	m_sScene.camera.bViewModified=true;
	
	ComputeCameraMatrix(&m_sScene.camera.m_ViewMatrix,vPosition,dYaw,dPitch,dRoll);
	
	VectorsFromAngles(dYaw,dPitch,dRoll,m_sScene.camera.m_vCameraForward,m_sScene.camera.m_vCameraRight,m_sScene.camera.m_vCameraUp);
	m_sScene.camera.m_vCameraPos=vPosition;
	m_sScene.camera.m_vCameraAngles.c[YAW]=dYaw;
	m_sScene.camera.m_vCameraAngles.c[PITCH]=dPitch;
	m_sScene.camera.m_vCameraAngles.c[ROLL]=dRoll;
}


void COpenGLRender::ActivateClipping()
{
	m_sScene.clipping.bEnabled=true;
}

void COpenGLRender::DeactivateClipping()
{
	m_sScene.clipping.bEnabled=false;
}

void COpenGLRender::SetClipRect(double x,double y,double cx, double cy)
{
	m_sScene.clipping.rRect.x=x;
	m_sScene.clipping.rRect.y=y;
	m_sScene.clipping.rRect.w=cx;
	m_sScene.clipping.rRect.h=cy;
}

void COpenGLRender::GetClipRect(double *px,double *py,double *pcx, double *pcy)
{
	*px=m_sScene.clipping.rRect.x;
	*py=m_sScene.clipping.rRect.y;
	*pcx=m_sScene.clipping.rRect.w;
	*pcy=m_sScene.clipping.rRect.h;
}

bool COpenGLRender::IsClippingActive()
{
	return m_sScene.clipping.bEnabled;
}

void COpenGLRender::SelectTexture(IGenericTexture *pTexture,int nTextureLevel)
{
	if(nTextureLevel>MAX_TEXTURE_LEVELS){return;}
	if(m_ppiStagedTextureLevels[nTextureLevel])
	{
		m_nStagedTextureLevels--;
		REL(m_ppiStagedTextureLevels[nTextureLevel]);
	}
	m_ppiStagedTextureLevels[nTextureLevel]=ADD(pTexture);
	m_nStagedTextureLevels++;
}

void COpenGLRender::UnselectTexture(int nTextureLevel)
{
	if(nTextureLevel>MAX_TEXTURE_LEVELS){return;}
	if(m_ppiStagedTextureLevels[nTextureLevel])
	{
		REL(m_ppiStagedTextureLevels[nTextureLevel]);
		m_nStagedTextureLevels--;
	}
}

void COpenGLRender::SetSkyShadowParameters(double dSpeed, double dXResolution, double dZResolution, double dOpacity)
{
	m_sScene.sky.m_dSkyShadowSpeed=dSpeed;
	m_sScene.sky.m_dSkyShadowXResolution=dXResolution;
	m_sScene.sky.m_dSkyShadowZResolution=dZResolution;
	m_sScene.sky.m_dSkyShadowOpacity=dOpacity;
}

void COpenGLRender::SelectSkyShadow(IGenericTexture *pSkyShadow)
{
	if(!m_sRenderOptions.bEnableSkyShadow){return;}
	REL(m_sScene.sky.m_piSkyShadow);
	m_sScene.sky.m_piSkyShadow=ADD(pSkyShadow);
}

void COpenGLRender::UnselectSkyShadow()
{
	REL(m_sScene.sky.m_piSkyShadow);
}

void COpenGLRender::ActivateWater(){m_sStagedRenderingState.bActiveWater=true;}
void COpenGLRender::DeactivateWater(){m_sStagedRenderingState.bActiveWater=false;}
void COpenGLRender::SetWaterMappingSize(double dMaxU,double dMaxV){m_sScene.water.dSizeU=dMaxU;m_sScene.water.dSizeV=dMaxV;}
void COpenGLRender::SetWaterMappingOffset(double dMaxU,double dMaxV){m_sScene.water.dOffsetU=dMaxU;m_sScene.water.dOffsetV=dMaxV;}
bool COpenGLRender::IsHeightFogActive(){return m_sStagedRenderingState.bActiveHeightFog;}
bool COpenGLRender::AreTexturesActive(){return m_sStagedRenderingState.bActiveTextures;}
void COpenGLRender::ActivateTextures(){m_sStagedRenderingState.bActiveTextures=m_sRenderOptions.bEnableTextures;}
void COpenGLRender::DeactivateTextures(){m_sStagedRenderingState.bActiveTextures=false;}
bool COpenGLRender::IsSolidActive(){return m_sStagedRenderingState.bActiveSolid;}
void COpenGLRender::ActivateSolid(){m_sStagedRenderingState.bActiveSolid=m_sRenderOptions.bEnableSolid;}
void COpenGLRender::DeactivateSolid(){m_sStagedRenderingState.bActiveSolid=false;}
void COpenGLRender::ActivateShadowEmission(){m_sStagedRenderingState.bActiveShadowEmission=m_sRenderOptions.bEnableShadows;}
void COpenGLRender::DeactivateShadowEmission(){m_sStagedRenderingState.bActiveShadowEmission=false;}
bool COpenGLRender::IsShadowEmissionActive(){return m_sStagedRenderingState.bActiveShadowEmission;}
void COpenGLRender::ActivateShadowReception(){m_sStagedRenderingState.bActiveShadowReception=m_sRenderOptions.bEnableShadows;}
void COpenGLRender::DeactivateShadowReception(){m_sStagedRenderingState.bActiveShadowReception=false;}
bool COpenGLRender::IsShadowReceptionActive(){return m_sStagedRenderingState.bActiveShadowReception;}
void COpenGLRender::ActivateSkyShadow(){m_sStagedRenderingState.bActiveSkyShadow=m_sRenderOptions.bEnableSkyShadow;}
void COpenGLRender::DeactivateSkyShadow(){m_sStagedRenderingState.bActiveSkyShadow=false;}
bool COpenGLRender::IsSkyShadowActive(){return m_sStagedRenderingState.bActiveSkyShadow;}
void COpenGLRender::ActivateDepth(){m_sStagedRenderingState.bActiveDepth=true;}
void COpenGLRender::DeactivateDepth(){m_sStagedRenderingState.bActiveDepth=false;}
void COpenGLRender::ActivateDepthWrite(){m_sStagedRenderingState.bActiveDepthWrite=true;}
void COpenGLRender::DeactivateDepthWrite(){m_sStagedRenderingState.bActiveDepthWrite=false;}
void COpenGLRender::ActivateHeightFog(const CVector &vMins,const CVector &vMaxs,const CVector &vColor)
{
	m_sStagedRenderingState.bActiveHeightFog=m_sRenderOptions.bEnableHeightFog;
	m_sStagedRenderingState.vHeightFogMins=vMins;
	m_sStagedRenderingState.vHeightFogMaxs=vMaxs;
	m_sStagedRenderingState.vHeightFogColor=vColor;
}
void COpenGLRender::DeactivateHeightFog(){m_sStagedRenderingState.bActiveHeightFog=false;}


void COpenGLRender::SetDepthFunction(EDepthFunction eFunction)
{
	m_sStagedRenderingState.nDepthFunction=GL_LESS;
	if(eFunction==eDepthFunction_Less){m_sStagedRenderingState.nDepthFunction=GL_LESS;}
	else if(eFunction==eDepthFunction_LessOrEqual){m_sStagedRenderingState.nDepthFunction=GL_LEQUAL;}
	else if(eFunction==eDepthFunction_Equal){m_sStagedRenderingState.nDepthFunction=GL_EQUAL;}
	else if(eFunction==eDepthFunction_Greater){m_sStagedRenderingState.nDepthFunction=GL_GREATER;}
	else if(eFunction==eDepthFunction_GreaterOrEqual){m_sStagedRenderingState.nDepthFunction=GL_GEQUAL;}
	else if(eFunction==eDepthFunction_Always){m_sStagedRenderingState.nDepthFunction=GL_ALWAYS;}
	else if(eFunction==eDepthFunction_Never){m_sStagedRenderingState.nDepthFunction=GL_NEVER;}
	else if(eFunction==eDepthFunction_NotEqual){m_sStagedRenderingState.nDepthFunction=GL_NOTEQUAL;}
}

void COpenGLRender::ActivateBlending(){m_sStagedRenderingState.bActiveBlending=m_sRenderOptions.bEnableBlending;}

void COpenGLRender::SetBlendingFunction(unsigned int nOperator1,unsigned int nOperator2)
{
	m_sStagedRenderingState.nBlendOperator1=nOperator1;
	m_sStagedRenderingState.nBlendOperator2=nOperator2;
}
void COpenGLRender::SetBlendingLayer(unsigned int nLayer){m_sStagedRenderingState.nBlendingLayer=nLayer;}
void COpenGLRender::DeactivateBlending(){m_sStagedRenderingState.bActiveBlending=false;}
bool COpenGLRender::IsBlendingActive(){return m_sStagedRenderingState.bActiveBlending;}
void COpenGLRender::ActivateLighting(){m_sStagedRenderingState.bActiveLighting=m_sRenderOptions.bEnableLighting;}
void COpenGLRender::DeactivateLighting(){m_sStagedRenderingState.bActiveLighting=false;}
bool COpenGLRender::IsLightingActive(){return m_sStagedRenderingState.bActiveLighting;}

EShadingModel 	COpenGLRender::GetShadingModel(){return m_piCurrentRenderPath?m_piCurrentRenderPath->GetShadingModel():eShadingModel_UNKNOWN;}
EShadowQuality  COpenGLRender::GetShadowQuality(){return m_eShadowQuality;}
void            COpenGLRender::SetShadowQuality(EShadowQuality eQuality)
{
	m_eShadowQuality=eQuality;
	m_sScene.lighting.m_nDesiredSunShadowWidth=512;
	m_sScene.lighting.m_nDesiredSunShadowHeight=512;
	switch(m_eShadowQuality)
	{
	case eShadowQuality_High:
		m_sScene.lighting.m_nDesiredSunShadowWidth=1024;
		m_sScene.lighting.m_nDesiredSunShadowHeight=1024;
	break;
	case eShadowQuality_Medium:
		m_sScene.lighting.m_nDesiredSunShadowWidth=768;
		m_sScene.lighting.m_nDesiredSunShadowHeight=768;
	break;
	case eShadowQuality_Low:
		m_sScene.lighting.m_nDesiredSunShadowWidth=512;
		m_sScene.lighting.m_nDesiredSunShadowHeight=512;
	break;
	}
}

void COpenGLRender::SetCurrentRenderPath(std::string sRenderPath)
{
	for(unsigned int x=0;x<m_vRenderPaths.size();x++)
	{
		if(m_vRenderPaths[x].m_piOpenGLRender->GetFriendlyName()==sRenderPath)
		{
			IOpenGLRender *piNewPath=m_vRenderPaths[x].m_piOpenGLRender;
			if(m_piCurrentRenderPath==piNewPath){return;}
			m_piCurrentRenderPath->Cleanup();
			REL(m_piCurrentRenderPath);
			m_piCurrentRenderPath=ADD(piNewPath);
			if(!m_piCurrentRenderPath->Setup(this,m_piCurrentViewport,m_sHardwareSupport))
			{
				RTTRACE("COpenGLRender::SetCurrentRenderPath -> Failed to setup %s render path",sRenderPath.c_str());
			}
			else
			{
				RTTRACE("COpenGLRender::Setup -> Render path %s initialized",sRenderPath.c_str());
			}
		}
	}
}

std::string	COpenGLRender::GetCurrentRenderPath()
{
	return m_piCurrentRenderPath?m_piCurrentRenderPath->GetFriendlyName():"Unknown";
}

void COpenGLRender::GetRenderPaths(std::vector<std::string> *pvRenderPaths)
{
	for(unsigned int x=0;x<m_vRenderPaths.size();x++)
	{
		pvRenderPaths->push_back(m_vRenderPaths[x].m_piOpenGLRender->GetFriendlyName());
	}
}

SRenderStats 	COpenGLRender::GetStagedRenderingStats(){return m_sStagedStats;}

void COpenGLRender::EnableNormalMaps(){m_sRenderOptions.bEnableNormalMaps=true;}
void COpenGLRender::DisableNormalMaps(){m_sRenderOptions.bEnableNormalMaps=false;}
bool COpenGLRender::AreNormalMapsEnabled(){return m_sRenderOptions.bEnableNormalMaps;}

void COpenGLRender::EnableSkyShadow(){m_sRenderOptions.bEnableSkyShadow=true;}
void COpenGLRender::DisableSkyShadow(){m_sRenderOptions.bEnableSkyShadow=false;DeactivateSkyShadow();}
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

void COpenGLRender::EnableShadows(){m_sRenderOptions.bEnableShadows=true;}
void COpenGLRender::DisableShadows(){m_sRenderOptions.bEnableShadows=false;DeactivateShadowEmission();DeactivateShadowReception();}
bool COpenGLRender::AreShadowsEnabled(){return m_sRenderOptions.bEnableShadows;}

void COpenGLRender::EnableAutoShadowVolume(){m_sRenderOptions.bEnableAutoShadowVolume=true;}
void COpenGLRender::DisableAutoShadowVolume(){m_sRenderOptions.bEnableAutoShadowVolume=false;}
bool COpenGLRender::IsAutoShadowVolumeEnabled(){return m_sRenderOptions.bEnableAutoShadowVolume;}

void COpenGLRender::EnableStagedRenderingStats(){m_sRenderOptions.bEnableStagedRenderingStats=true;}
void COpenGLRender::DisableStagedRenderingStats(){m_sRenderOptions.bEnableStagedRenderingStats=false;}
bool COpenGLRender::AreStagedRenderingStatsEnabled(){return m_sRenderOptions.bEnableStagedRenderingStats;}

void COpenGLRender::SetAmbientLight( const CVector &vColor ){m_sScene.lighting.m_vAmbientColor=vColor;}
void COpenGLRender::GetAmbientLight( CVector *pvColor ){if(pvColor){*pvColor=m_sScene.lighting.m_vAmbientColor;}}

void COpenGLRender::SetSunLight( const CVector &vPosition,const CVector &vDirection,const CVector &vAmbientColor,const CVector &vDiffuseColor,const CVector &vSpecularColor)
{
	if(m_sScene.lighting.m_piSunLight)
	{
		m_sScene.lighting.m_piSunLight->SetPosition(vPosition);
		m_sScene.lighting.m_piSunLight->SetAmbientColor(vAmbientColor);
		m_sScene.lighting.m_piSunLight->SetDiffuseColor(vDiffuseColor);
		m_sScene.lighting.m_piSunLight->SetSpecularColor(vSpecularColor);
		m_sScene.lighting.m_piSunLight->SetDirectional(vDirection);
	}
}

void COpenGLRender::GetSunLight( CVector *pvPosition,CVector *pvDirection,CVector *pvAmbientColor,CVector *pvDiffuseColor,CVector *pvSpecularColor )
{
	if(m_sScene.lighting.m_piSunLight)
	{
		if(pvPosition){*pvPosition=m_sScene.lighting.m_piSunLight->GetPosition();}
		if(pvDirection){*pvDirection=m_sScene.lighting.m_piSunLight->GetDirectionalDirection();}
		if(pvAmbientColor){*pvAmbientColor=m_sScene.lighting.m_piSunLight->GetAmbientColor();}
		if(pvDiffuseColor){*pvDiffuseColor=m_sScene.lighting.m_piSunLight->GetDiffuseColor();}
		if(pvSpecularColor){*pvSpecularColor=m_sScene.lighting.m_piSunLight->GetSpecularColor();}
	}
}

void COpenGLRender::AddLight( IGenericLight *piLight)
{
	std::vector<IGenericLight *>::iterator i;
	for(i=m_sScene.lighting.m_vLights.begin();i!=m_sScene.lighting.m_vLights.end();i++)
	{
		if(*i==piLight){return;}
	}
	m_sScene.lighting.m_vLights.push_back(ADD(piLight));
}

void COpenGLRender::RemoveLight( IGenericLight *piLight)
{
	std::vector<IGenericLight *>::iterator i;
	for(i=m_sScene.lighting.m_vLights.begin();i!=m_sScene.lighting.m_vLights.end();i++)
	{
		if(*i==piLight)
		{
			m_sScene.lighting.m_vLights.erase(i);
			REL(piLight);
			return;
		}
	}
}
void COpenGLRender::PushState()
{
	m_sStagedRenderingStateStack.push(m_sStagedRenderingState);
}

void COpenGLRender::PopState()
{
	if(m_sStagedRenderingStateStack.size())
	{
		m_sStagedRenderingState=m_sStagedRenderingStateStack.top();
		m_sStagedRenderingStateStack.pop();		
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


template<typename stagekeytype,typename stagetype>
stagetype *COpenGLRender::GetStage(stagekeytype &key,std::map<stagekeytype,stagetype> &stageMap)
{
	stagetype *pStage=NULL;
	typename std::map<stagekeytype,stagetype>::iterator iStage;
	iStage=stageMap.find(key);
	if(iStage==stageMap.end())
	{
		if(!m_bStagedRendering)
		{
			Flush();
		}
		pStage=&stageMap[key];
		pStage->Init(key);
	}
	else
	{
		pStage=&iStage->second;
	}
	
	return pStage;
}

template<typename stagetype>
typename stagetype::buffer_type *COpenGLRender::GetNextStageBufferElement(stagetype *pStage,TBufferPool<typename stagetype::buffer_type> &stagePool)
{
	int nBuffers=pStage->vBuffers.size();
	typename stagetype::buffer_type *pBuffer=nBuffers?pStage->vBuffers[nBuffers-1]:NULL;
	if(pBuffer==NULL || pBuffer->nUsedElements>=stagetype::buffer_type::buffer_size)
	{
		pBuffer=stagePool.Allocate();
		pStage->vBuffers.push_back(pBuffer);
		pBuffer->nUsedElements=1;
	}
	else
	{
		pBuffer->Next();
	}
	return pBuffer;
}

template<typename stagetype>
typename stagetype::buffer_type *COpenGLRender::GetNextStageBufferElement(stagetype *pStage,TBufferPool<typename stagetype::buffer_type> &stagePool,typename stagetype::buffer_type *pBuffer)
{
	if(pBuffer->nUsedElements>=stagetype::buffer_type::buffer_size)
	{
		pBuffer=stagePool.Allocate();
		pStage->vBuffers.push_back(pBuffer);
		pBuffer->nUsedElements=1;
	}
	else
	{
		pBuffer->Next();
	}
	return pBuffer;
}

void COpenGLRender::RenderTexture(const CVector &vOrigin,double s1,double s2,const CVector &vColor,double dAlpha)
{
	RenderTexture(vOrigin,s1,s2,0,0,1,1,vColor,dAlpha);
}

void COpenGLRender::RenderTexture(const CVector &vOrigin,double s1,double s2,double dTexX,double dTexY,double dTexW,double dTexH,const CVector &vColor,double dAlpha)
{
	STriangleStageKey key(m_ppiStagedTextureLevels[0],m_sStagedRenderingState);
	STriangleStage *pStage=GetStage(key,m_sScene.objects.mTriangleStages);
	STriangleBuffer *pBuffer=GetNextStageBufferElement(pStage,m_TriangleBuffers);
	
	CVector vAxis1=m_sScene.camera.m_vCameraRight*(s1/2.0);
	CVector vAxis2=m_sScene.camera.m_vCameraUp*(s2/2.0);
	
	CVector v[4];
	v[0]=vOrigin+vAxis1+vAxis2;
	v[1]=vOrigin-vAxis1+vAxis2;
	v[2]=vOrigin-vAxis1-vAxis2;
	v[3]=vOrigin+vAxis1-vAxis2;
	
	CVector vt[4];
	vt[0].c[0]=dTexX+dTexW;
	vt[0].c[1]=dTexY+dTexH;
	
	vt[1].c[0]=dTexX;
	vt[1].c[1]=dTexY+dTexH;
	
	vt[2].c[0]=dTexX;
	vt[2].c[1]=dTexY;
	
	vt[3].c[0]=dTexX+dTexW;
	vt[3].c[1]=dTexY;
	
	pBuffer->pVertexBufferCursor[0]=v[0].c[0];
	pBuffer->pVertexBufferCursor[1]=v[0].c[1];
	pBuffer->pVertexBufferCursor[2]=v[0].c[2];
	
	pBuffer->pVertexBufferCursor[3]=v[1].c[0];
	pBuffer->pVertexBufferCursor[4]=v[1].c[1];
	pBuffer->pVertexBufferCursor[5]=v[1].c[2];
	
	pBuffer->pVertexBufferCursor[6]=v[2].c[0];
	pBuffer->pVertexBufferCursor[7]=v[2].c[1];
	pBuffer->pVertexBufferCursor[8]=v[2].c[2];
	
	pBuffer->pTexBufferCursor[0]=vt[0].c[0];
	pBuffer->pTexBufferCursor[1]=vt[0].c[1];
	
	pBuffer->pTexBufferCursor[2]=vt[1].c[0];
	pBuffer->pTexBufferCursor[3]=vt[1].c[1];
	
	pBuffer->pTexBufferCursor[4]=vt[2].c[0];
	pBuffer->pTexBufferCursor[5]=vt[2].c[1];
	
	pBuffer->pColorBufferCursor[0]=vColor.c[0];
	pBuffer->pColorBufferCursor[1]=vColor.c[1];
	pBuffer->pColorBufferCursor[2]=vColor.c[2];
	pBuffer->pColorBufferCursor[3]=dAlpha;
	
	pBuffer->pColorBufferCursor[4]=vColor.c[0];
	pBuffer->pColorBufferCursor[5]=vColor.c[1];
	pBuffer->pColorBufferCursor[6]=vColor.c[2];
	pBuffer->pColorBufferCursor[7]=dAlpha;
	
	pBuffer->pColorBufferCursor[8]=vColor.c[0];
	pBuffer->pColorBufferCursor[9]=vColor.c[1];
	pBuffer->pColorBufferCursor[10]=vColor.c[2];
	pBuffer->pColorBufferCursor[11]=dAlpha;
	
	pBuffer=GetNextStageBufferElement(pStage,m_TriangleBuffers,pBuffer);
	
	pBuffer->pVertexBufferCursor[0]=v[0].c[0];
	pBuffer->pVertexBufferCursor[1]=v[0].c[1];
	pBuffer->pVertexBufferCursor[2]=v[0].c[2];
	
	pBuffer->pVertexBufferCursor[3]=v[2].c[0];
	pBuffer->pVertexBufferCursor[4]=v[2].c[1];
	pBuffer->pVertexBufferCursor[5]=v[2].c[2];
	
	pBuffer->pVertexBufferCursor[6]=v[3].c[0];
	pBuffer->pVertexBufferCursor[7]=v[3].c[1];
	pBuffer->pVertexBufferCursor[8]=v[3].c[2];
	
	pBuffer->pTexBufferCursor[0]=vt[0].c[0];
	pBuffer->pTexBufferCursor[1]=vt[0].c[1];
	
	pBuffer->pTexBufferCursor[2]=vt[2].c[0];
	pBuffer->pTexBufferCursor[3]=vt[2].c[1];
	
	pBuffer->pTexBufferCursor[4]=vt[3].c[0];
	pBuffer->pTexBufferCursor[5]=vt[3].c[1];
	
	pBuffer->pColorBufferCursor[0]=vColor.c[0];
	pBuffer->pColorBufferCursor[1]=vColor.c[1];
	pBuffer->pColorBufferCursor[2]=vColor.c[2];
	pBuffer->pColorBufferCursor[3]=dAlpha;
	
	pBuffer->pColorBufferCursor[4]=vColor.c[0];
	pBuffer->pColorBufferCursor[5]=vColor.c[1];
	pBuffer->pColorBufferCursor[6]=vColor.c[2];
	pBuffer->pColorBufferCursor[7]=dAlpha;
	
	pBuffer->pColorBufferCursor[8]=vColor.c[0];
	pBuffer->pColorBufferCursor[9]=vColor.c[1];
	pBuffer->pColorBufferCursor[10]=vColor.c[2];
	pBuffer->pColorBufferCursor[11]=dAlpha;
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

void COpenGLRender::RenderLineLoop(unsigned int nPoints,const CVector *pPoints,unsigned long nStipple,const CVector &vColor,double dAlpha)
{
	SLineStageKey key(m_sStagedRenderingState,nStipple);
	SLineStage *pStage=GetStage(key,m_sScene.objects.mLineStages);
	for(unsigned int x=0;x<nPoints;x++)
	{
		SLineBuffer *pBuffer=GetNextStageBufferElement(pStage,m_LineBuffers);
		int nNext=(x+1)%nPoints;
		
		pBuffer->pVertexBufferCursor[0]=(float)pPoints[x].c[0];
		pBuffer->pVertexBufferCursor[1]=(float)pPoints[x].c[1];
		pBuffer->pVertexBufferCursor[2]=(float)pPoints[x].c[2];
		
		pBuffer->pVertexBufferCursor[3]=(float)pPoints[nNext].c[0];
		pBuffer->pVertexBufferCursor[4]=(float)pPoints[nNext].c[1];
		pBuffer->pVertexBufferCursor[5]=(float)pPoints[nNext].c[2];
		
		pBuffer->pColorBufferCursor[0]=(float)vColor.c[0];
		pBuffer->pColorBufferCursor[1]=(float)vColor.c[1];
		pBuffer->pColorBufferCursor[2]=(float)vColor.c[2];
		pBuffer->pColorBufferCursor[3]=(float)dAlpha;
		
		pBuffer->pColorBufferCursor[4]=(float)vColor.c[0];
		pBuffer->pColorBufferCursor[5]=(float)vColor.c[1];
		pBuffer->pColorBufferCursor[6]=(float)vColor.c[2];
		pBuffer->pColorBufferCursor[7]=(float)dAlpha;
	}
}


void COpenGLRender::RenderLines(unsigned int nLines,const CVector *pPoints,unsigned long nStipple,const CVector &vColor,double dAlpha)
{
	SLineStageKey key(m_sStagedRenderingState,nStipple);
	SLineStage *pStage=GetStage(key,m_sScene.objects.mLineStages);
	for(unsigned int x=0;x<nLines*2;x+=2)
	{
		SLineBuffer *pBuffer=GetNextStageBufferElement(pStage,m_LineBuffers);
		
		pBuffer->pVertexBufferCursor[0]=(float)pPoints[x].c[0];
		pBuffer->pVertexBufferCursor[1]=(float)pPoints[x].c[1];
		pBuffer->pVertexBufferCursor[2]=(float)pPoints[x].c[2];
		
		pBuffer->pVertexBufferCursor[3]=(float)pPoints[x+1].c[0];
		pBuffer->pVertexBufferCursor[4]=(float)pPoints[x+1].c[1];
		pBuffer->pVertexBufferCursor[5]=(float)pPoints[x+1].c[2];
		
		pBuffer->pColorBufferCursor[0]=(float)vColor.c[0];
		pBuffer->pColorBufferCursor[1]=(float)vColor.c[1];
		pBuffer->pColorBufferCursor[2]=(float)vColor.c[2];
		pBuffer->pColorBufferCursor[3]=(float)dAlpha;
		
		pBuffer->pColorBufferCursor[4]=(float)vColor.c[0];
		pBuffer->pColorBufferCursor[5]=(float)vColor.c[1];
		pBuffer->pColorBufferCursor[6]=(float)vColor.c[2];
		pBuffer->pColorBufferCursor[7]=(float)dAlpha;
	}
}


void COpenGLRender::RenderLine(const CVector &v1,const CVector &v2,unsigned long nStipple,const CVector &vColor,double dAlpha)
{
	SLineStageKey key(m_sStagedRenderingState,nStipple);
	SLineStage *pStage=GetStage(key,m_sScene.objects.mLineStages);
	SLineBuffer *pBuffer=GetNextStageBufferElement(pStage,m_LineBuffers);
	
	pBuffer->pVertexBufferCursor[0]=(float)v1.c[0];
	pBuffer->pVertexBufferCursor[1]=(float)v1.c[1];
	pBuffer->pVertexBufferCursor[2]=(float)v1.c[2];
	
	pBuffer->pVertexBufferCursor[3]=(float)v2.c[0];
	pBuffer->pVertexBufferCursor[4]=(float)v2.c[1];
	pBuffer->pVertexBufferCursor[5]=(float)v2.c[2];
	
	pBuffer->pColorBufferCursor[0]=(float)vColor.c[0];
	pBuffer->pColorBufferCursor[1]=(float)vColor.c[1];
	pBuffer->pColorBufferCursor[2]=(float)vColor.c[2];
	pBuffer->pColorBufferCursor[3]=(float)dAlpha;
	
	pBuffer->pColorBufferCursor[4]=(float)vColor.c[0];
	pBuffer->pColorBufferCursor[5]=(float)vColor.c[1];
	pBuffer->pColorBufferCursor[6]=(float)vColor.c[2];
	pBuffer->pColorBufferCursor[7]=(float)dAlpha;
}

void COpenGLRender::RenderRect(const CVector &vCenter,const CVector &vAxisW,const CVector &vAxisH,double w,double h,CVector &vColor,double dAlpha)
{	
	CVector v[4];
	v[0]=vCenter+vAxisW*(w/2.0)-vAxisH*(h/2.0);
	v[1]=vCenter-vAxisW*(w/2.0)-vAxisH*(h/2.0);
	v[2]=vCenter-vAxisW*(w/2.0)+vAxisH*(h/2.0);
	v[3]=vCenter+vAxisW*(w/2.0)+vAxisH*(h/2.0);	
	
	if(m_sStagedRenderingState.bActiveSolid)
	{
		STriangleStageKey key(NULL,m_sStagedRenderingState);
		STriangleStage *pStage=GetStage(key,m_sScene.objects.mTriangleStages);
		STriangleBuffer *pBuffer=GetNextStageBufferElement(pStage,m_TriangleBuffers);
		
		pBuffer->pVertexBufferCursor[0]=v[0].c[0];
		pBuffer->pVertexBufferCursor[1]=v[0].c[1];
		pBuffer->pVertexBufferCursor[2]=v[0].c[2];
		
		pBuffer->pVertexBufferCursor[3]=v[1].c[0];
		pBuffer->pVertexBufferCursor[4]=v[1].c[1];
		pBuffer->pVertexBufferCursor[5]=v[1].c[2];
		
		pBuffer->pVertexBufferCursor[6]=v[2].c[0];
		pBuffer->pVertexBufferCursor[7]=v[2].c[1];
		pBuffer->pVertexBufferCursor[8]=v[2].c[2];
		
		pBuffer->pColorBufferCursor[0]=vColor.c[0];
		pBuffer->pColorBufferCursor[1]=vColor.c[1];
		pBuffer->pColorBufferCursor[2]=vColor.c[2];
		pBuffer->pColorBufferCursor[3]=dAlpha;
		
		pBuffer->pColorBufferCursor[4]=vColor.c[0];
		pBuffer->pColorBufferCursor[5]=vColor.c[1];
		pBuffer->pColorBufferCursor[6]=vColor.c[2];
		pBuffer->pColorBufferCursor[7]=dAlpha;
		
		pBuffer->pColorBufferCursor[8]=vColor.c[0];
		pBuffer->pColorBufferCursor[9]=vColor.c[1];
		pBuffer->pColorBufferCursor[10]=vColor.c[2];
		pBuffer->pColorBufferCursor[11]=dAlpha;
		
		pBuffer=GetNextStageBufferElement(pStage,m_TriangleBuffers,pBuffer);
		
		pBuffer->pVertexBufferCursor[0]=v[0].c[0];
		pBuffer->pVertexBufferCursor[1]=v[0].c[1];
		pBuffer->pVertexBufferCursor[2]=v[0].c[2];
		
		pBuffer->pVertexBufferCursor[3]=v[2].c[0];
		pBuffer->pVertexBufferCursor[4]=v[2].c[1];
		pBuffer->pVertexBufferCursor[5]=v[2].c[2];
		
		pBuffer->pVertexBufferCursor[6]=v[3].c[0];
		pBuffer->pVertexBufferCursor[7]=v[3].c[1];
		pBuffer->pVertexBufferCursor[8]=v[3].c[2];
		
		pBuffer->pColorBufferCursor[0]=vColor.c[0];
		pBuffer->pColorBufferCursor[1]=vColor.c[1];
		pBuffer->pColorBufferCursor[2]=vColor.c[2];
		pBuffer->pColorBufferCursor[3]=dAlpha;
		
		pBuffer->pColorBufferCursor[4]=vColor.c[0];
		pBuffer->pColorBufferCursor[5]=vColor.c[1];
		pBuffer->pColorBufferCursor[6]=vColor.c[2];
		pBuffer->pColorBufferCursor[7]=dAlpha;
		
		pBuffer->pColorBufferCursor[8]=vColor.c[0];
		pBuffer->pColorBufferCursor[9]=vColor.c[1];
		pBuffer->pColorBufferCursor[10]=vColor.c[2];
		pBuffer->pColorBufferCursor[11]=dAlpha;
	}
	else
	{
		RenderLineLoop(4,v,0xFFFF,vColor,dAlpha);
	}
}

void COpenGLRender::RenderRect(double x,double y,double w,double h,CVector &vColor,double dAlpha)
{
	CVector v[4];
	v[0]=CVector(x,y,0);
	v[1]=CVector(x+w,y,0);
	v[2]=CVector(x+w,y+h,0);
	v[3]=CVector(x,y+h,0);
	
	if(m_sStagedRenderingState.bActiveSolid)
	{
		STriangleStageKey key(NULL,m_sStagedRenderingState);
		STriangleStage *pStage=GetStage(key,m_sScene.objects.mTriangleStages);
		STriangleBuffer *pBuffer=GetNextStageBufferElement(pStage,m_TriangleBuffers);
		
		pBuffer->pVertexBufferCursor[0]=v[0].c[0];
		pBuffer->pVertexBufferCursor[1]=v[0].c[1];
		pBuffer->pVertexBufferCursor[2]=v[0].c[2];
		
		pBuffer->pVertexBufferCursor[3]=v[1].c[0];
		pBuffer->pVertexBufferCursor[4]=v[1].c[1];
		pBuffer->pVertexBufferCursor[5]=v[1].c[2];
		
		pBuffer->pVertexBufferCursor[6]=v[2].c[0];
		pBuffer->pVertexBufferCursor[7]=v[2].c[1];
		pBuffer->pVertexBufferCursor[8]=v[2].c[2];
		
		pBuffer->pColorBufferCursor[0]=vColor.c[0];
		pBuffer->pColorBufferCursor[1]=vColor.c[1];
		pBuffer->pColorBufferCursor[2]=vColor.c[2];
		pBuffer->pColorBufferCursor[3]=dAlpha;
		
		pBuffer->pColorBufferCursor[4]=vColor.c[0];
		pBuffer->pColorBufferCursor[5]=vColor.c[1];
		pBuffer->pColorBufferCursor[6]=vColor.c[2];
		pBuffer->pColorBufferCursor[7]=dAlpha;
		
		pBuffer->pColorBufferCursor[8]=vColor.c[0];
		pBuffer->pColorBufferCursor[9]=vColor.c[1];
		pBuffer->pColorBufferCursor[10]=vColor.c[2];
		pBuffer->pColorBufferCursor[11]=dAlpha;
		
		pBuffer=GetNextStageBufferElement(pStage,m_TriangleBuffers,pBuffer);
		
		pBuffer->pVertexBufferCursor[0]=v[0].c[0];
		pBuffer->pVertexBufferCursor[1]=v[0].c[1];
		pBuffer->pVertexBufferCursor[2]=v[0].c[2];
		
		pBuffer->pVertexBufferCursor[3]=v[2].c[0];
		pBuffer->pVertexBufferCursor[4]=v[2].c[1];
		pBuffer->pVertexBufferCursor[5]=v[2].c[2];
		
		pBuffer->pVertexBufferCursor[6]=v[3].c[0];
		pBuffer->pVertexBufferCursor[7]=v[3].c[1];
		pBuffer->pVertexBufferCursor[8]=v[3].c[2];
		
		pBuffer->pColorBufferCursor[0]=vColor.c[0];
		pBuffer->pColorBufferCursor[1]=vColor.c[1];
		pBuffer->pColorBufferCursor[2]=vColor.c[2];
		pBuffer->pColorBufferCursor[3]=dAlpha;
		
		pBuffer->pColorBufferCursor[4]=vColor.c[0];
		pBuffer->pColorBufferCursor[5]=vColor.c[1];
		pBuffer->pColorBufferCursor[6]=vColor.c[2];
		pBuffer->pColorBufferCursor[7]=dAlpha;
		
		pBuffer->pColorBufferCursor[8]=vColor.c[0];
		pBuffer->pColorBufferCursor[9]=vColor.c[1];
		pBuffer->pColorBufferCursor[10]=vColor.c[2];
		pBuffer->pColorBufferCursor[11]=dAlpha;
	}
	else
	{
		RenderLineLoop(4,v,0xFFFF,vColor,dAlpha);
	}
	
}

void COpenGLRender::RenderPolygon(unsigned int nVertexes,const CVector *pVertexes,const CVector &vColor,double dAlpha)
{
	if(nVertexes<3){return;}
	
	if(m_sStagedRenderingState.bActiveSolid)
	{
		STriangleStageKey key(NULL,m_sStagedRenderingState);
		STriangleStage *pStage=GetStage(key,m_sScene.objects.mTriangleStages);
		
		for(unsigned int x=0;x<nVertexes-2;x++)
		{
			STriangleBuffer *pBuffer=GetNextStageBufferElement(pStage,m_TriangleBuffers);
			pBuffer->pVertexBufferCursor[0]=pVertexes[0].c[0];
			pBuffer->pVertexBufferCursor[1]=pVertexes[0].c[1];
			pBuffer->pVertexBufferCursor[2]=pVertexes[0].c[2];
			
			pBuffer->pVertexBufferCursor[3]=pVertexes[x+1].c[0];
			pBuffer->pVertexBufferCursor[4]=pVertexes[x+1].c[1];
			pBuffer->pVertexBufferCursor[5]=pVertexes[x+1].c[2];
			
			pBuffer->pVertexBufferCursor[6]=pVertexes[x+2].c[0];
			pBuffer->pVertexBufferCursor[7]=pVertexes[x+2].c[1];
			pBuffer->pVertexBufferCursor[8]=pVertexes[x+2].c[2];
			
			pBuffer->pColorBufferCursor[0]=vColor.c[0];
			pBuffer->pColorBufferCursor[1]=vColor.c[1];
			pBuffer->pColorBufferCursor[2]=vColor.c[2];
			pBuffer->pColorBufferCursor[3]=dAlpha;
			
			pBuffer->pColorBufferCursor[4]=vColor.c[0];
			pBuffer->pColorBufferCursor[5]=vColor.c[1];
			pBuffer->pColorBufferCursor[6]=vColor.c[2];
			pBuffer->pColorBufferCursor[7]=dAlpha;
			
			pBuffer->pColorBufferCursor[8]=vColor.c[0];
			pBuffer->pColorBufferCursor[9]=vColor.c[1];
			pBuffer->pColorBufferCursor[10]=vColor.c[2];
			pBuffer->pColorBufferCursor[11]=dAlpha;
		}
	}
	else
	{
		RenderLineLoop(nVertexes,pVertexes,0xFFFF,vColor,dAlpha);
	}
}

void COpenGLRender::RenderPolygon(unsigned int nVertexes,const CVector *pVertexes,const CVector *pColors,const double *pdAlphas)
{
	if(nVertexes<3){return;}
	
	if(m_sStagedRenderingState.bActiveSolid)
	{
		STriangleStageKey key(NULL,m_sStagedRenderingState);
		STriangleStage *pStage=GetStage(key,m_sScene.objects.mTriangleStages);
		
		for(unsigned int x=0;x<nVertexes-2;x++)
		{
			STriangleBuffer *pBuffer=GetNextStageBufferElement(pStage,m_TriangleBuffers);
			
			pBuffer->pVertexBufferCursor[0]=pVertexes[0].c[0];
			pBuffer->pVertexBufferCursor[1]=pVertexes[0].c[1];
			pBuffer->pVertexBufferCursor[2]=pVertexes[0].c[2];
			
			pBuffer->pVertexBufferCursor[3]=pVertexes[x+1].c[0];
			pBuffer->pVertexBufferCursor[4]=pVertexes[x+1].c[1];
			pBuffer->pVertexBufferCursor[5]=pVertexes[x+1].c[2];
			
			pBuffer->pVertexBufferCursor[6]=pVertexes[x+2].c[0];
			pBuffer->pVertexBufferCursor[7]=pVertexes[x+2].c[1];
			pBuffer->pVertexBufferCursor[8]=pVertexes[x+2].c[2];
			
			pBuffer->pColorBufferCursor[0]=pColors[0].c[0];
			pBuffer->pColorBufferCursor[1]=pColors[0].c[1];
			pBuffer->pColorBufferCursor[2]=pColors[0].c[2];
			pBuffer->pColorBufferCursor[3]=pdAlphas[0];
			
			pBuffer->pColorBufferCursor[4]=pColors[x+1].c[0];
			pBuffer->pColorBufferCursor[5]=pColors[x+1].c[1];
			pBuffer->pColorBufferCursor[6]=pColors[x+1].c[2];
			pBuffer->pColorBufferCursor[7]=pdAlphas[x+1];
			
			pBuffer->pColorBufferCursor[8]=pColors[x+2].c[0];
			pBuffer->pColorBufferCursor[9]=pColors[x+2].c[1];
			pBuffer->pColorBufferCursor[10]=pColors[x+2].c[2];
			pBuffer->pColorBufferCursor[11]=pdAlphas[x+2];
		}
	}
	else
	{
		RenderLineLoop(nVertexes,pVertexes,0xFFFF,pColors[0],pdAlphas[0]);
	}
}

void COpenGLRender::RenderArrowHead(const CVector &vPosition,const CVector &vDirection,CVector &vUp,double dForward,double dUp,double dRight,const CVector &vColor,double dAlpha)
{
	CVector pvVertexes[5];
	CVector vRight=vDirection^vUp;
	
	pvVertexes[0]=vPosition-vUp*dUp-vRight*dRight;
	pvVertexes[1]=vPosition-vUp*dUp+vRight*dRight;
	pvVertexes[2]=vPosition+vUp*dUp+vRight*dRight;
	pvVertexes[3]=vPosition+vUp*dUp-vRight*dRight;
	pvVertexes[4]=vPosition+vDirection*dForward;
	
	STriangleStageKey key(NULL,m_sStagedRenderingState);
	STriangleStage *pStage=GetStage(key,m_sScene.objects.mTriangleStages);
	
	unsigned short pFaces[18]={0,1,2,  0,2,3,  4,1,0,   4,2,1   ,4,3,2,  4,0,3};
	unsigned short *pFaceCursor=pFaces;
	
	for(int x=0;x<6;x++)
	{
		STriangleBuffer *pBuffer=GetNextStageBufferElement(pStage,m_TriangleBuffers);
		for(int y=0;y<3;y++)
		{
			unsigned short nVertex=*pFaceCursor++;
			pBuffer->pVertexBufferCursor[y*3+0]=pvVertexes[nVertex].c[0];
			pBuffer->pVertexBufferCursor[y*3+1]=pvVertexes[nVertex].c[1];
			pBuffer->pVertexBufferCursor[y*3+2]=pvVertexes[nVertex].c[2];
			pBuffer->pColorBufferCursor[y*4+0]=vColor.c[0];
			pBuffer->pColorBufferCursor[y*4+1]=vColor.c[1];
			pBuffer->pColorBufferCursor[y*4+2]=vColor.c[2];
			pBuffer->pColorBufferCursor[y*4+3]=dAlpha;
		}
	}
}

void COpenGLRender::Clear(const CVector &vColor)
{
	if(!m_bStagedRendering)
	{
		Flush();
	}
	m_sScene.bClear=true;
	m_sScene.vClearColor=vColor;
}

void COpenGLRender::ClearDepth()
{
	if(!m_bStagedRendering)
	{
		Flush();
	}
	m_sScene.bClearDepth=true;
}

void COpenGLRender::SetModelTinting(CVector vTinting)
{
	m_sStagedRenderingState.vModelTinting=vTinting;
}

void COpenGLRender::RenderPoint( const CVector &vPosition,double dSize,const CVector &vColor,double dAlpha )
{
	SPointStageKey key(m_sStagedRenderingState,(unsigned long)dSize);
	SPointStage *pStage=GetStage(key,m_sScene.objects.mPointStages);
	SPointBuffer *pBuffer=GetNextStageBufferElement(pStage,m_PointBuffers);
	
	pBuffer->pVertexBufferCursor[0]=(float)vPosition.c[0];
	pBuffer->pVertexBufferCursor[1]=(float)vPosition.c[1];
	pBuffer->pVertexBufferCursor[2]=(float)vPosition.c[2];
	
	pBuffer->pColorBufferCursor[0]=(float)vColor.c[0];
	pBuffer->pColorBufferCursor[1]=(float)vColor.c[1];
	pBuffer->pColorBufferCursor[2]=(float)vColor.c[2];
	pBuffer->pColorBufferCursor[3]=(float)dAlpha;
}

void COpenGLRender::RenderModel(const CVector &vOrigin,const CVector &vOrientation,IGenericModel *piModel,unsigned int nAnimation,unsigned int nFrame)
{
	bool bSkip=false;
	
	if(m_bStagedRendering && m_sRenderOptions.bEnableShadows && m_sRenderOptions.bEnableAutoShadowVolume)
	{
		bool    pbBBoxVolumePointOutSide[8];
		CVector vBBoxVolume[8];
		CVector vMins,vMaxs;
		double dRadius=piModel->GetFrameRadius(nAnimation,nFrame);
		piModel->GetFrameBBox(nAnimation,nFrame,&vMins,&vMaxs);
		
		if(m_sStagedRenderingState.bActiveShadowEmission)
		{
			double dDistanceToLight=(m_sScene.lighting.m_piSunLight->GetPosition()-vOrigin);
			dDistanceToLight-=dRadius;
			if(dDistanceToLight<m_dMinDistanceToLight){m_dMinDistanceToLight=dDistanceToLight;}
		}
		
		if(!bSkip)
		{
			bSkip=(m_sScene.camera.m_CameraForwardPlane.GetSide(vOrigin)<(m_sScene.camera.m_dPerspectiveNearPlane-dRadius));
		}
		bool bInsideByRadius=false;
		int nInsideByRadius=0;
		if(!bSkip)
		{
			for(int x=0;!bSkip && x<4;x++)
			{
				double dRadiusSide=m_sScene.camera.m_CameraFustrumPlanes[x].GetSide(vOrigin);
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
					if(m_sScene.camera.m_CameraForwardPlane.GetSide(vBBoxVolume[y])<m_sScene.camera.m_dPerspectiveNearPlane)
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
						if(m_sScene.camera.m_CameraFustrumPlanes[x].GetSide(vBBoxVolume[y])>0)
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
							if(CPolygon::Divide(m_sScene.camera.m_CameraFustrumPlanes[y],&polygons[x],&fake1,&fake2,NULL,&lCutSegment)>1)
							{
								int nLineCutPlanes[2]={0};
								if(y==0 || y==2){nLineCutPlanes[0]=1;nLineCutPlanes[1]=3;}
								if(y==1 || y==3){nLineCutPlanes[0]=0;nLineCutPlanes[1]=2;}
								
								bool bContinue=true;
								CLine lCuttedLine;
								lCuttedLine=lCutSegment;
								for(int z=0;z<2;z++)
								{
									CPlane *pPlane=&m_sScene.camera.m_CameraFustrumPlanes[nLineCutPlanes[z]];
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
	sInstance.dDistanceToCamera=m_sScene.camera.m_CameraForwardPlane.GetSide(vOrigin);
	sInstance.bSkipRender=bSkip;
	sInstance.nId=pStage->vInstances.size();
	sInstance.vTinting=m_sStagedRenderingState.vModelTinting;
	if(pStage->vInstances.size()==0 || sInstance.dDistanceToCamera<pStage->dNearestModel)
	{
		pStage->dNearestModel=sInstance.dDistanceToCamera;
	}
	pStage->vInstances.push_back(sInstance);
}

void COpenGLRender::RenderParticle(IGenericTexture *piTexture,const CVector &vOrigin,double dAngle,double s1,double s2,double dTexX,double dTexY,double dTexW,double dTexH,const CVector &vColor,double dAlpha)
{
	STriangleStageKey key(piTexture,m_sStagedRenderingState);
	STriangleStage *pStage=GetStage(key,m_sScene.objects.mTriangleStages);
	STriangleBuffer *pBuffer=GetNextStageBufferElement(pStage,m_TriangleBuffers);
	
	CMatrix m;
	m.R(m_sScene.camera.m_vCameraForward,DegreesToRadians(dAngle));
	
	CVector vAxis1=m_sScene.camera.m_vCameraRight*(s1/2.0);
	CVector vAxis2=m_sScene.camera.m_vCameraUp*(s2/2.0);
	vAxis1*=m;
	vAxis2*=m;
	
	CVector v[4];
	v[0]=(vOrigin+vAxis1+vAxis2);
	v[1]=(vOrigin-vAxis1+vAxis2);
	v[2]=(vOrigin-vAxis1-vAxis2);
	v[3]=(vOrigin+vAxis1-vAxis2);
	
	CVector vt[4];
	vt[0].c[0]=dTexX+dTexW;
	vt[0].c[1]=dTexY+dTexH;
	
	vt[1].c[0]=dTexX;
	vt[1].c[1]=dTexY+dTexH;
	
	vt[2].c[0]=dTexX;
	vt[2].c[1]=dTexY;
	
	vt[3].c[0]=dTexX+dTexW;
	vt[3].c[1]=dTexY;
	
	pBuffer->pVertexBufferCursor[0]=v[0].c[0];
	pBuffer->pVertexBufferCursor[1]=v[0].c[1];
	pBuffer->pVertexBufferCursor[2]=v[0].c[2];
	
	pBuffer->pVertexBufferCursor[3]=v[1].c[0];
	pBuffer->pVertexBufferCursor[4]=v[1].c[1];
	pBuffer->pVertexBufferCursor[5]=v[1].c[2];
	
	pBuffer->pVertexBufferCursor[6]=v[2].c[0];
	pBuffer->pVertexBufferCursor[7]=v[2].c[1];
	pBuffer->pVertexBufferCursor[8]=v[2].c[2];
	
	pBuffer->pTexBufferCursor[0]=vt[0].c[0];
	pBuffer->pTexBufferCursor[1]=vt[0].c[1];
	
	pBuffer->pTexBufferCursor[2]=vt[1].c[0];
	pBuffer->pTexBufferCursor[3]=vt[1].c[1];
	
	pBuffer->pTexBufferCursor[4]=vt[2].c[0];
	pBuffer->pTexBufferCursor[5]=vt[2].c[1];
	
	pBuffer->pColorBufferCursor[0]=vColor.c[0];
	pBuffer->pColorBufferCursor[1]=vColor.c[1];
	pBuffer->pColorBufferCursor[2]=vColor.c[2];
	pBuffer->pColorBufferCursor[3]=dAlpha;
	
	pBuffer->pColorBufferCursor[4]=vColor.c[0];
	pBuffer->pColorBufferCursor[5]=vColor.c[1];
	pBuffer->pColorBufferCursor[6]=vColor.c[2];
	pBuffer->pColorBufferCursor[7]=dAlpha;
	
	pBuffer->pColorBufferCursor[8]=vColor.c[0];
	pBuffer->pColorBufferCursor[9]=vColor.c[1];
	pBuffer->pColorBufferCursor[10]=vColor.c[2];
	pBuffer->pColorBufferCursor[11]=dAlpha;
	
	pBuffer=GetNextStageBufferElement(pStage,m_TriangleBuffers,pBuffer);
	
	pBuffer->pVertexBufferCursor[0]=v[0].c[0];
	pBuffer->pVertexBufferCursor[1]=v[0].c[1];
	pBuffer->pVertexBufferCursor[2]=v[0].c[2];
	
	pBuffer->pVertexBufferCursor[3]=v[2].c[0];
	pBuffer->pVertexBufferCursor[4]=v[2].c[1];
	pBuffer->pVertexBufferCursor[5]=v[2].c[2];
	
	pBuffer->pVertexBufferCursor[6]=v[3].c[0];
	pBuffer->pVertexBufferCursor[7]=v[3].c[1];
	pBuffer->pVertexBufferCursor[8]=v[3].c[2];
	
	pBuffer->pTexBufferCursor[0]=vt[0].c[0];
	pBuffer->pTexBufferCursor[1]=vt[0].c[1];
	
	pBuffer->pTexBufferCursor[2]=vt[2].c[0];
	pBuffer->pTexBufferCursor[3]=vt[2].c[1];
	
	pBuffer->pTexBufferCursor[4]=vt[3].c[0];
	pBuffer->pTexBufferCursor[5]=vt[3].c[1];
	
	pBuffer->pColorBufferCursor[0]=vColor.c[0];
	pBuffer->pColorBufferCursor[1]=vColor.c[1];
	pBuffer->pColorBufferCursor[2]=vColor.c[2];
	pBuffer->pColorBufferCursor[3]=dAlpha;
	
	pBuffer->pColorBufferCursor[4]=vColor.c[0];
	pBuffer->pColorBufferCursor[5]=vColor.c[1];
	pBuffer->pColorBufferCursor[6]=vColor.c[2];
	pBuffer->pColorBufferCursor[7]=dAlpha;
	
	pBuffer->pColorBufferCursor[8]=vColor.c[0];
	pBuffer->pColorBufferCursor[9]=vColor.c[1];
	pBuffer->pColorBufferCursor[10]=vColor.c[2];
	pBuffer->pColorBufferCursor[11]=dAlpha;
}

void COpenGLRender::StartStagedRendering()
{
	Flush();
	
	m_bStagedRendering=true;
	m_dStagedRenderingMinZ=m_sScene.camera.m_dPerspectiveFarPlane;
	m_dStagedRenderingMaxZ=m_sScene.camera.m_dPerspectiveNearPlane;

	double dCameraAspect=((m_sScene.camera.m_nViewportH)==0)?1:(double)(m_sScene.camera.m_nViewportW)/(double)(m_sScene.camera.m_nViewportH);
	CalcCameraPlanes(m_sScene.camera.m_vCameraPos,m_sScene.camera.m_vCameraAngles,m_sScene.camera.m_dPerspectiveViewAngle,dCameraAspect,m_sScene.camera.m_dPerspectiveNearPlane,m_sScene.camera.m_dPerspectiveFarPlane,m_sScene.camera.m_CameraFustrumPlanes);

	m_sScene.camera.m_CameraForwardPlane=CPlane(m_sScene.camera.m_vCameraForward,m_sScene.camera.m_vCameraPos);
	m_sScene.camera.m_CameraRightPlane=CPlane(m_sScene.camera.m_vCameraRight,m_sScene.camera.m_vCameraPos);
	m_sScene.camera.m_CameraUpPlane=CPlane(m_sScene.camera.m_vCameraUp,m_sScene.camera.m_vCameraPos);
	
	m_dMinDistanceToLight=10000;

	m_bShadowVolumeFirstVertex=true;
	
	
	if(!m_sRenderOptions.bEnableAutoShadowVolume)
	{		
		CVector pVolume[8];
		CalcCameraVolume(m_sScene.camera.m_vCameraPos,m_sScene.camera.m_vCameraAngles, m_sScene.camera.m_dPerspectiveViewAngle,dCameraAspect,m_sScene.camera.m_dPerspectiveNearPlane, m_sScene.camera.m_dPerspectiveFarPlane,pVolume);
		for(int x=0;x<8;x++)
		{
			ProcessCameraVertex(pVolume[x]);
		}
	}
}

void COpenGLRender::SortModels()
{
	// Sort models in each stage and stages by nearest model
	m_sScene.objects.mModelStages.clear();
	std::map<SModelStageKey,SModelStage>::iterator iModelStage;
	for(iModelStage=m_mModelStages.begin();iModelStage!=m_mModelStages.end();iModelStage++)
	{
		const SModelStageKey *pKey=&iModelStage->first;
		SModelStage *pStage=&iModelStage->second;
		std::sort(iModelStage->second.vInstances.begin(), iModelStage->second.vInstances.end());
		m_sScene.objects.mModelStages.insert(std::pair<double,std::pair<const SModelStageKey*,const SModelStage*> >(pStage->dNearestModel,std::pair<const SModelStageKey*,const SModelStage*>(pKey,pStage)));
	}	
}

void COpenGLRender::Flush()
{
	if(m_mModelStages.size()){SortModels();}
	
	int nStagesToRender=m_sScene.objects.mModelStages.size()+
	                    m_sScene.objects.mLineStages.size()+
						m_sScene.objects.mPointStages.size()+
						m_sScene.objects.mTriangleStages.size();

	if(nStagesToRender==0 && m_sScene.bClear==false && m_sScene.bClearDepth==false){return;}
	
	
	if(m_piCurrentRenderPath)
	{
		m_piCurrentRenderPath->RenderScene(m_sScene);
		m_sScene.camera.bViewModified=false;
		m_sScene.camera.bProjectionModified=false;
		m_sScene.camera.bViewportModified=false;
	}

	std::map<SModelStageKey,SModelStage>::iterator iModelStage;
	for(iModelStage=m_mModelStages.begin();iModelStage!=m_mModelStages.end();iModelStage++)
	{
		SModelStage *pStage=&iModelStage->second;
		REL(pStage->piModel);
		REL(pStage->piGLModel);
	}
	std::map<STriangleStageKey,STriangleStage>::iterator iTriangleStage;
	for(iTriangleStage=m_sScene.objects.mTriangleStages.begin();iTriangleStage!=m_sScene.objects.mTriangleStages.end();iTriangleStage++)
	{
		IGenericTexture *piTexture=iTriangleStage->first.piTexture;
		STriangleStage *pStage=&iTriangleStage->second;

		for(unsigned int x=0;x<pStage->vBuffers.size();x++)
		{
			STriangleBuffer *pBuffer=pStage->vBuffers[x];
			m_TriangleBuffers.Free(pBuffer);
		}
		REL(piTexture);
	}
	std::map<SLineStageKey,SLineStage>::iterator iLineStage;
	for(iLineStage=m_sScene.objects.mLineStages.begin();iLineStage!=m_sScene.objects.mLineStages.end();iLineStage++)
	{
		SLineStage *pStage=&iLineStage->second;
		for(unsigned int x=0;x<pStage->vBuffers.size();x++)
		{
			SLineBuffer *pBuffer=pStage->vBuffers[x];
			m_LineBuffers.Free(pBuffer);
		}
	}
	std::map<SPointStageKey,SPointStage>::iterator iPointStage;
	for(iPointStage=m_sScene.objects.mPointStages.begin();iPointStage!=m_sScene.objects.mPointStages.end();iPointStage++)
	{
		SPointStage *pStage=&iPointStage->second;
		for(unsigned int x=0;x<pStage->vBuffers.size();x++)
		{
			SPointBuffer *pBuffer=pStage->vBuffers[x];
			m_PointBuffers.Free(pBuffer);
		}
	}

	m_mModelStages.clear();
	m_sScene.objects.mModelStages.clear();
	m_sScene.objects.mTriangleStages.clear();
	m_sScene.objects.mLineStages.clear();
	m_sScene.objects.mPointStages.clear();
	m_sScene.bClear=false;
	m_sScene.bClearDepth=false;
}

void COpenGLRender::EndStagedRendering()
{
	if(!m_bStagedRendering){return;}
	m_bStagedRendering=false;
	
	// Sort models by its distance to the camera.
	if(m_mModelStages.size()){SortModels();}
	
	int nStagesToRender=m_sScene.objects.mModelStages.size()+
	m_sScene.objects.mLineStages.size()+
	m_sScene.objects.mPointStages.size()+
	m_sScene.objects.mTriangleStages.size();
	
	if(nStagesToRender==0 && m_sScene.bClear==false && m_sScene.bClearDepth==false){return;}
	
	unsigned int nRenderStart=0;
	if(m_sRenderOptions.bEnableStagedRenderingStats){nRenderStart=GetTimeStamp();m_sStagedStats=SRenderStats();}

	if(m_piCurrentRenderPath)
	{
		ComputeSunShadowCamera();

		m_piCurrentRenderPath->RenderScene(m_sScene);
		m_sScene.camera.bViewModified=false;
		m_sScene.camera.bProjectionModified=false;
		m_sScene.camera.bViewportModified=false;
	}

	std::map<SModelStageKey,SModelStage>::iterator iModelStage;
	for(iModelStage=m_mModelStages.begin();iModelStage!=m_mModelStages.end();iModelStage++)
	{
		SModelStage *pStage=&iModelStage->second;
		REL(pStage->piModel);
		REL(pStage->piGLModel);
	}
	std::map<STriangleStageKey,STriangleStage>::iterator iTriangleStage;
	for(iTriangleStage=m_sScene.objects.mTriangleStages.begin();iTriangleStage!=m_sScene.objects.mTriangleStages.end();iTriangleStage++)
	{
		IGenericTexture *piTexture=iTriangleStage->first.piTexture;
		STriangleStage *pStage=&iTriangleStage->second;
		
		for(unsigned int x=0;x<pStage->vBuffers.size();x++)
		{
			STriangleBuffer *pBuffer=pStage->vBuffers[x];
			m_TriangleBuffers.Free(pBuffer);
		}
		REL(piTexture);
	}
	std::map<SLineStageKey,SLineStage>::iterator iLineStage;
	for(iLineStage=m_sScene.objects.mLineStages.begin();iLineStage!=m_sScene.objects.mLineStages.end();iLineStage++)
	{
		SLineStage *pStage=&iLineStage->second;
		for(unsigned int x=0;x<pStage->vBuffers.size();x++)
		{
			SLineBuffer *pBuffer=pStage->vBuffers[x];
			m_LineBuffers.Free(pBuffer);
		}
	}
	std::map<SPointStageKey,SPointStage>::iterator iPointStage;
	for(iPointStage=m_sScene.objects.mPointStages.begin();iPointStage!=m_sScene.objects.mPointStages.end();iPointStage++)
	{
		SPointStage *pStage=&iPointStage->second;
		for(unsigned int x=0;x<pStage->vBuffers.size();x++)
		{
			SPointBuffer *pBuffer=pStage->vBuffers[x];
			m_PointBuffers.Free(pBuffer);
		}
	}

	m_mModelStages.clear();
	m_sScene.objects.mModelStages.clear();
	m_sScene.objects.mTriangleStages.clear();
	m_sScene.objects.mLineStages.clear();
	m_sScene.objects.mPointStages.clear();
	m_sScene.bClear=false;
	m_sScene.bClearDepth=false;
	
	if(m_sRenderOptions.bEnableStagedRenderingStats)
	{
		glFinish();
		m_sStagedStats.nRenderTime=GetTimeStamp()-nRenderStart;
	}
}
void COpenGLRender::StartFrame()
{
	float fCurrentTime=((double)(GetTimeStamp()-m_nCreationTime))*0.001;
	m_sScene.fTime=fCurrentTime;

	if(m_piCurrentRenderPath){m_piCurrentRenderPath->StartFrame();}
}

void COpenGLRender::EndFrame()
{
	m_sStagedStats.nShaderChanges=0;
	if(m_bStagedRendering)
	{
		EndStagedRendering();
	}
	else
	{
		Flush();
	}
	if(m_piCurrentRenderPath){m_piCurrentRenderPath->EndFrame();}
}

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

void COpenGLRender::ComputeSunShadowCamera()
{
	CVector vLightAngles;
	CVector pvCameraVolume[8];
	CVector pvLightVolume[8];
	CVector pvShadowVolume[8];
	CVector vLightPosition;
	CVector vVolumeMidPoint;
	CVector vLightForward;
	double dLightNear;
	double dLightFar;

	CMatrix cameraViewMatrix=m_sScene.camera.m_ViewMatrix;
	CMatrix cameraProjectionMatrix=m_sScene.camera.m_ProjectionMatrix;
	CMatrix identity;
	double dPreviousNear=m_sScene.camera.m_dPerspectiveNearPlane;
	double dPreviousFar=m_sScene.camera.m_dPerspectiveFarPlane;
	double dPreviousViewAngle=m_sScene.camera.m_dPerspectiveViewAngle;

	int nPreviousViewportX=m_sScene.camera.m_nViewportX;
	int nPreviousViewportY=m_sScene.camera.m_nViewportY;
	int nPreviousViewportW=m_sScene.camera.m_nViewportW;
	int nPreviousViewportH=m_sScene.camera.m_nViewportH;
	double dPreviousViewAspect=((m_sScene.camera.m_nViewportH)==0)?1:(double)(m_sScene.camera.m_nViewportW)/(double)(m_sScene.camera.m_nViewportH);
	CVector vPreviousCameraPosition=m_sScene.camera.m_vCameraPos;
	CVector vPreviousCameraAngles=m_sScene.camera.m_vCameraAngles;

	CalcCameraVolume(vPreviousCameraPosition,vPreviousCameraAngles,dPreviousViewAngle,dPreviousViewAspect,m_dStagedRenderingMinZ,m_dStagedRenderingMaxZ,pvCameraVolume);
	CalcBBoxVolume(Origin,Origin,m_vShadowVolumeMins,m_vShadowVolumeMaxs,pvShadowVolume);

	for(int x=0;x<8;x++){vVolumeMidPoint+=pvShadowVolume[x];}
	vVolumeMidPoint/=8.0;
	CVector vLightRight,vLightUp;
	vLightPosition=m_SunLight.m_piLight->GetPosition()+m_sScene.camera.m_vCameraPos;//vVolumeMidPoint-vLightForward*500.0;

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
	/*
	Disabled until properly implemented
	vLightRight=AxisPosX;
	vLightUp=vLightRight^vLightForward;
	vLightRight=vLightUp^vLightForward;
	vLightAngles=AnglesFromVectors(vLightForward,vLightRight,vLightUp);
	*/
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

	m_sScene.lighting.m_vSunShadowPos=vLightPosition;
	m_sScene.lighting.m_vSunShadowAngles=vLightAngles;
	m_sScene.lighting.m_dSunShadowNearPlane=dLightNear;
	m_sScene.lighting.m_dSunShadowFarPlane=dLightFar;
	m_sScene.lighting.m_dSunShadowViewAngle=std::max(dViewAngleRight,dViewAngleUp)*2.0;//dPreviousViewAngle*0.5;
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

	ToCameraRef(&m_sScene.camera.m_CameraForwardPlane,&m_sScene.camera.m_CameraRightPlane,&m_sScene.camera.m_CameraUpPlane,1,&vTemp);

	double dDistance=vTemp.c[2];
	if(dDistance<m_dStagedRenderingMinZ){m_dStagedRenderingMinZ=dDistance;}
	if(dDistance>m_dStagedRenderingMaxZ){m_dStagedRenderingMaxZ=dDistance;}
	if(m_dStagedRenderingMinZ<m_sScene.camera.m_dPerspectiveNearPlane){m_dStagedRenderingMinZ=m_sScene.camera.m_dPerspectiveNearPlane;}
	if(m_dStagedRenderingMaxZ>m_sScene.camera.m_dPerspectiveFarPlane){m_dStagedRenderingMaxZ=m_sScene.camera.m_dPerspectiveFarPlane;}
}

void COpenGLRender::StartSelection(SGameRect rWindowRect,IGenericCamera *piCamera,double dx,double dy,double dPrecision)
{
	Flush();
	ActivateDepth();
	if(m_piCurrentRenderPath){m_piCurrentRenderPath->StartSelection(rWindowRect,piCamera,dx,dy,dPrecision);}
}

void COpenGLRender::SetSelectionId( unsigned int nId )
{
	Flush();
	if(m_piCurrentRenderPath){m_piCurrentRenderPath->SetSelectionId(nId);}
}

int COpenGLRender::EndSelection()
{
	Flush();
	if(m_piCurrentRenderPath){return m_piCurrentRenderPath->EndSelection();}
	return -1;
}

void COpenGLRender::ReloadShaders()
{
	if(m_piCurrentRenderPath){m_piCurrentRenderPath->ReloadShaders();}
}
