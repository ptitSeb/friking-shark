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


#ifndef __GAME_GRAPHICS__
#define __GAME_GRAPHICS__

#include "GameRunTimeLib.h"
#include "GameKeys.h"

struct SVideoMode
{
  	unsigned int w;
	unsigned int h;
	unsigned int bpp;
	unsigned int rate;
	
	// Fullscreen size and position should not match the resolution if there are various monitors
	int fullscreenX;
	int fullscreenY;
	int fullscreenW;
	int fullscreenH;
	
	SVideoMode(){w=h=bpp=rate=0;fullscreenX=fullscreenY=0;fullscreenW=fullscreenH=0;}
};

BEGIN_STRUCT_PROPS(SGameRect)
	PROP_VALUE_FLAGS(x,"x",0,MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(y,"y",0,MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(w,"w",0,MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(h,"h",0,MRPF_NORMAL|MRPF_OPTIONAL)
END_STRUCT_PROPS()

BEGIN_STRUCT_PROPS(SGameSize)
	PROP_VALUE_FLAGS(w,"w",0,MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(h,"h",0,MRPF_NORMAL|MRPF_OPTIONAL)
END_STRUCT_PROPS()

BEGIN_STRUCT_PROPS(SGamePos)
	PROP_VALUE_FLAGS(x,"x",0,MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(y,"y",0,MRPF_NORMAL|MRPF_OPTIONAL)
END_STRUCT_PROPS()

enum eTextAlignment
{
	eTextAlignment_None		=0x0000,
	eTextAlignment_Left		=0x0001,
	eTextAlignment_Top		=0x0001,
	eTextAlignment_Center	=0x0002,
	eTextAlignment_Right	=0x0003,
	eTextAlignment_Bottom	=0x0003
};

DECLARE_SERIALIZABLE_ENUMERATION(eTextAlignment)

class IGenericCamera: virtual public ISystemUnknown
{
public:
	virtual CVector &GetPosition()=0;
	virtual CVector &GetAngles()=0;

	virtual void	SetPosition(CVector &vPosition)=0;
	virtual void	SetAngles(CVector &vAngles)=0;

	virtual void	SetClippingPlanes(double dNear,double dFar)=0;
	virtual void	GetClippingPlanes(double &dNear,double &dFar)=0;

	virtual void	SetViewAngle(double dAngle)=0;
	virtual double	GetViewAngle()=0;

	virtual CVector	&GetUpVector()=0;
	virtual CVector	&GetRightVector()=0;
	virtual CVector	&GetForwardVector()=0;

	virtual void	 SetAspectRatio(double dRatio)=0;
	virtual double	 GetAspectRatio()=0;
};

class IGenericViewportCallBack
{
public:

	virtual void OnRender()=0;

	virtual void OnLButtonDown(unsigned x,unsigned y)=0;
	virtual void OnLButtonDoubleClick(unsigned x,unsigned y)=0;
	virtual void OnLButtonUp(unsigned x,unsigned y)=0;
	virtual void OnRButtonDown(unsigned x,unsigned y)=0;
	virtual void OnRButtonDoubleClick(unsigned x,unsigned y)=0;
	virtual void OnRButtonUp(unsigned x,unsigned y)=0;
	virtual void OnMouseMove(unsigned x,unsigned y)=0;
	virtual void OnMouseWheelUp(unsigned x,unsigned y)=0;
	virtual void OnMouseWheelDown(unsigned x,unsigned y)=0;
		
	virtual void OnCharacter(unsigned short wCharacter)=0;
	virtual void OnKeyDown(unsigned short wKey)=0;
	virtual void OnKeyUp(unsigned short wKey)=0;

	virtual void OnSize(unsigned cx,unsigned cy)=0;
	virtual void OnMove(unsigned x,unsigned y)=0;
};

class IGenericViewport:virtual public ISystemUnknown
{
public:

	virtual bool CreateFullScreen(unsigned int w,unsigned int h,unsigned int bpp,unsigned int rate)=0;
	virtual bool CreateWindowed(unsigned x, unsigned y, unsigned w, unsigned h)=0;
	
	virtual void GetSize(unsigned *pdwWidth,unsigned *pdwHeight)=0;

	virtual void SetVSync(bool bVSync)=0;
	virtual bool GetVSync()=0;

	virtual void EnterLoop()=0;
	virtual void ExitLoop()=0;

	virtual void		SetCaption(std::string sCaption)=0;
	virtual std::string GetCaption()=0;

	virtual void GetCursorPos(int *pX,int *pY)=0;
	virtual void SetCursorPos(int x,int y)=0;

	virtual bool HasMouseCapture()=0;
	virtual void SetMouseCapture()=0;
	virtual void ReleaseMouseCapture()=0;

	virtual bool IsKeyDown(unsigned int nKey)=0;
	virtual bool GetKeyName(int nKey,std::string *psKey)=0;
	
	virtual bool IsActiveWindow()=0;

	virtual bool IsMouseVisible()=0;
	virtual void ShowMouseCursor(bool bShow)=0;
	
	virtual void SetCallBack(IGenericViewportCallBack *pCallBack)=0;
	
	virtual bool DetectDrag(double dx,double dy)=0;

	virtual void GetCurrentVideoMode(SVideoMode *pMode)=0;

	virtual bool SetWindowed(unsigned int x,unsigned int y,unsigned int w,unsigned int h)=0;
	virtual bool SetFullScreen(unsigned int w,unsigned int h,unsigned int bpp,unsigned int rate)=0;

	virtual void 	SetJoystickDeadZone(double dDeadZone)=0;
	virtual double  GetJoystickDeadZone()=0;
};

class IGenericRender;

class IGenericTexture:virtual public ISystemUnknown
{	
public:

	virtual std::string	GetFileName()=0;
	virtual void		GetSize(unsigned *pdwWidth,unsigned *pdwHeight)=0;

	virtual bool		HasAlphaChannel()=0;

	virtual unsigned long GetByteBufferLength()=0;
	virtual void		  *GetByteBuffer()=0;

	virtual CVector		GetPixelColor(unsigned long x, unsigned long y)=0;
	virtual double		GetPixelAlpha(unsigned long x, unsigned long y)=0;
	
	virtual bool		Load(std::string sFileName,bool bGenerateMipmaps=true, bool bResident=false)=0;
	virtual bool		Create( unsigned nWidth,unsigned nHeight,IGenericViewport *piViewport)=0;
	virtual bool		CreateDepth( unsigned nWidth,unsigned nHeight,IGenericViewport *piViewport)=0;

	virtual bool		StartRenderingToTexture()=0;
	virtual void		StopRenderingToTexture()=0;

	// for the render only

	virtual bool PrepareTexture(int nTextureLevel)=0;
	virtual void UnprepareTexture(int nTextureLevel)=0;
	
	virtual void ReleaseResidentData()=0;
};

class IGenericModel:virtual public ISystemUnknown
{	
public:

	virtual void Create()=0;
	virtual bool Load(std::string sFileName)=0;

	virtual void		  SetBSPOptions(bool bLoad)=0;
	virtual void		  GetBSPOptions(bool *pbLoad)=0;
	virtual CBSPNode	 *GetBSP()=0;
	virtual CTraceInfo	  GetTrace(const CVector &vOrigin,const CVector &vAngles,const CVector &p1,const CVector &p2 )=0;

	virtual std::string	 GetFileName()=0;

	virtual unsigned long GetAnimations()=0;
	virtual unsigned long GetAnimationFrames(unsigned long nAnimation)=0;
	virtual unsigned long GetFrameRenderBuffers(unsigned long nAnimation,unsigned long nFrame)=0;
	virtual void		  GetFrameBBox(unsigned long nAnimation,unsigned long nFrame,CVector *pvMins,CVector *pvMaxs)=0;
	virtual CVector		  GetFrameSize(unsigned long nAnimation,unsigned long nFrame)=0;
	virtual double		  GetFrameRadius(unsigned long nAnimation,unsigned long nFrame)=0;

	virtual unsigned long AddAnimation()=0;
	virtual unsigned long AddAnimationFrame(unsigned long nAnimation)=0;
	virtual unsigned long AddRenderBuffer(unsigned long nAnimation,unsigned long nFrame)=0;
	
	virtual void		  RemoveRenderBuffers(unsigned long nAnimation,unsigned long nFrame)=0;
	virtual void		  RemoveFrames(unsigned long nAnimation)=0;
	virtual void		  RemoveAnimations()=0;

	virtual void		  SetRenderBufferMaterial(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,CVector vAmbientColor,CVector vDiffuseColor,CVector vSpecularColor, float fShininess, float fOpacity)=0;
	virtual void		  SetRenderBufferTexture(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,IGenericTexture *piTexture)=0;
	virtual void		  SetRenderBufferTextureMatrix(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,CMatrix *pMatrix)=0;
	virtual void		  SetRenderBufferVertexes(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nVertexes,float *pVertexes)=0;
	virtual void		  SetRenderBufferFaces(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nFaces,unsigned int *pFacesVertexes)=0;
	virtual void		  SetRenderBufferNormals(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,float *pNormals)=0;
	virtual void		  SetRenderBufferColors(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,float *pColors)=0;
	virtual void		  SetRenderBufferTextureCoords(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,float *pTexVertexes)=0;
	virtual void		  SetRenderBufferNormalMap(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,IGenericTexture *piTexture)=0;
	virtual void		  SetRenderBufferNormalMapCoords(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,float *pNormalMapVertexes)=0;
	
	virtual void		  GetRenderBufferMaterial(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,CVector *pvAmbientColor,CVector *pvDiffuseColor,CVector *pvSpecularColor, float *pfShininess, float *pfOpacity)=0;
	virtual void		  GetRenderBufferTexture(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,IGenericTexture **ppiTexture)=0;
	virtual void		  GetRenderBufferTextureLevels(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long *pnTextureLevels)=0;
	virtual void		  GetRenderBufferTextureMatrix(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,CMatrix *pMatrix,bool *pbIdentity)=0;
	virtual void		  GetRenderBufferVertexes(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long *pVertexes,float **ppVertexes)=0;
	virtual void		  GetRenderBufferFaces(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long *pFaces,unsigned int **ppFacesVertexes)=0;
	virtual void		  GetRenderBufferNormals(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,float **ppNormals)=0;
	virtual void		  GetRenderBufferColors(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,float **ppColors)=0;
	virtual void		  GetRenderBufferTextureCoords(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,float **ppTexVertexes)=0;
	virtual void		  GetRenderBufferNormalMap(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,IGenericTexture **ppiTexture)=0;
	virtual void		  GetRenderBufferNormalMapCoords(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,float **ppNormalMapVertexes)=0;
	
	// Method to ensure that the resource is ready for usage
	virtual bool Prepare()=0;
	
	virtual void UpdateBufferObjects()=0;

	// for the render only
	virtual void		  Render(IGenericRender *piRender,unsigned long nAnimation,unsigned long nFrame)=0;

	virtual void GetGeometry(std::vector<CPolygon*> *pPolygons)=0;
};

enum eGenericLightType
{
	eGenericLightType_UNKNOWN,
	eGenericLightType_Omni,
	eGenericLightType_Spot,
	eGenericLightType_Directional
};

DECLARE_SERIALIZABLE_ENUMERATION(eGenericLightType)

class IGenericLight:virtual public ISystemUnknown
{
public:

	virtual eGenericLightType GetType()=0;

	virtual CVector GetDiffuseColor()=0;
	virtual CVector GetAmbientColor()=0;
	virtual CVector GetSpecularColor()=0;
	virtual CVector GetPosition()=0;

	virtual void SetDiffuseColor(CVector cColor)=0;
	virtual void SetAmbientColor(CVector cColor)=0;
	virtual void SetSpecularColor(CVector cColor)=0;
	virtual void SetPosition(CVector vPosition)=0;

	virtual void SetOmni(double dRadius)=0;
	virtual void SetSpot(CVector vDirection,double dAngle,double dExponent,double dAttenuationConstant,double dAttenuationLinear,double dAttenuationQuadratic)=0;
	virtual void SetDirectional(CVector vDirection)=0;

	// only for omni sources	
	virtual double GetOmniRadius()=0;

	// only for spot sources
	virtual CVector GetSpotDirection()=0;
	virtual double	GetSpotAngle()=0;
	virtual void	GetSpotParameters(double *pdExponent,double *pdAttenuationConstant,double *pdAttenuationLinear,double *pdAttenuationQuadratic)=0;
	// only for directional sources
	virtual CVector GetDirectionalDirection()=0;

	~IGenericLight(){}
};

struct IGenericFont:virtual public ISystemUnknown
{	
public:

	virtual void CalcTextSize(double dFontHeight,const char *pText,double *pdWidth,double *pdHeight)=0;
	virtual void RenderText(IGenericRender *piRender,double dFontHeight,double x,double y,const char *pText,const CVector &vColor,double dAlpha)=0;
	virtual void RenderText(IGenericRender *piRender,double dFontHeight,CVector vPosition,const char *pText,const CVector &vColor,double dAlpha)=0;
	virtual void RenderTextEx(IGenericRender *piRender,double dFontHeight,double x,double y,double w,double h,const char *pText,eTextAlignment dwHorzAlign,eTextAlignment dwVertAlign,const CVector &vColor,double dAlpha)=0;
};

struct IGenericShader:virtual public ISystemUnknown
{
	virtual void Load(std::string sVertexShaderFile,std::string sFragmentShaderFile,std::string sPreprocessorDefinitions)=0;
	virtual void Create(std::string sVertexShaderCode,std::string sFragmentShaderCode,std::string sPreprocessorDefinitions)=0;
	
	virtual void AddUniform( std::string sUniformName,int bValue ,bool temporal)=0;
	virtual void AddUniform( std::string sUniformName,float fValue ,bool temporal)=0;
	virtual void AddUniform( std::string sUniformName,float fValue1,float fValue2,bool temporal)=0;
	virtual void AddUniform( std::string sUniformName,const CVector &vVector ,bool temporal)=0;
	virtual void AddUniform( std::string sUniformName,const CVector &vColor, float fAlpha ,bool temporal)=0;
	virtual void AddUniform( std::string sUniformName,CMatrix &matrix,bool temporal)=0;
	
	virtual void AddUniformIntegers( std::string sUniformName,unsigned int nValues,int *pValues,bool temporal)=0;
	virtual void AddUniformFloats( std::string sUniformName,unsigned int nValues,float *pValues,bool temporal)=0;
	virtual void AddUniformVectors( std::string sUniformName,unsigned int nValues,const CVector *pvVectors,bool temporal)=0;
	virtual void AddUniformColors( std::string sUniformName,unsigned int nValues,const CVector *pvColor, float *pvfAlphas,bool temporal)=0;
	virtual void AddUniformMatrixes( std::string sUniformName,unsigned int nValues,double *pMatrixes,bool temporal)=0;
	virtual void AddUniformMatrixes( std::string sUniformName,unsigned int nValues,float *pMatrixes,bool temporal)=0;
	
	virtual void AddAttribute( std::string sAttributeName,int nIndex)=0;
	
	virtual void AddData( std::string sDataName,int nValue)=0;
	
	virtual bool Activate()=0;
	virtual void Deactivate()=0;

	virtual bool Compile()=0;
};

enum EShadingModel
{
	eShadingModel_UNKNOWN=0,
	eShadingModel_Vertex,
	eShadingModel_Mixed,
	eShadingModel_Fragment
};
DECLARE_SERIALIZABLE_ENUMERATION(EShadingModel)

enum EShadowQuality
{
	eShadowQuality_Low=0,
	eShadowQuality_Medium,
	eShadowQuality_High,
	eShadowQuality_Count
};
DECLARE_SERIALIZABLE_ENUMERATION(EShadowQuality)

struct SRenderStats
{
	unsigned int nTotalVertexes;
	unsigned int nInmediateVertexes;
	unsigned int nBufferedVertexes;
	
	unsigned int nTotalFaces;
	unsigned int nInmediateFaces;
	unsigned int nBufferedFaces;
	
	unsigned int nPoints;
	unsigned int nLines;
	unsigned int nParticles;
	unsigned int nModels;
	
	unsigned int nStateChanges;
	unsigned int nShaderChanges;
	unsigned int nTextureChanges;
	
	unsigned int nRenderTime;
	unsigned int nShadowTime;
	
	SRenderStats()
	{
		nTotalVertexes=0;
		nInmediateVertexes=0;
		nBufferedVertexes=0;
		
		nTotalFaces=0;
		nInmediateFaces=0;
		nBufferedFaces=0;
		
		nPoints=0;
		nLines=0;
		nParticles=0;
		nModels=0;
		
		nStateChanges=0;
		nShaderChanges=0;
		nTextureChanges=0;
		
		nRenderTime=0;
		nShadowTime=0;
	}
};

enum EDepthFunction
{
	eDepthFunction_Never,
	eDepthFunction_Less,
	eDepthFunction_LessOrEqual,
	eDepthFunction_Equal,
	eDepthFunction_NotEqual,
	eDepthFunction_GreaterOrEqual,
	eDepthFunction_Greater,
	eDepthFunction_Always
};

class IGenericRender:virtual public ISystemUnknown
{
public:
	virtual bool Setup(IGenericViewport *piViewport)=0;

	virtual void StartStagedRendering()=0;
	virtual void EndStagedRendering()=0;
	virtual SRenderStats GetStagedRenderingStats()=0;
	
	virtual void StartSelection(SGameRect rWindowRect,IGenericCamera *piCamera,double dx,double dy,double dPrecision)=0;
	virtual void SetSelectionId(unsigned int nId)=0;
	virtual int  EndSelection()=0;

	virtual void ReloadShaders()=0;
	
	virtual void SetOrthographicProjection(double cx,double cy)=0;
	virtual void SetPerspectiveProjection(double dViewAngle,double dNearPlane,double dFarPlane)=0;
	virtual void SetViewport(double x,double y,double cx, double cy)=0;
	
	virtual bool    IsPerspectiveProjection()=0;
	virtual void    GetOrthographicProjection(double *pcx,double *pcy)=0;
	virtual void    GetPerspectiveProjection(double *pdViewAngle,double *pdNearPlane,double *pdFarPlane)=0;
	virtual void    GetViewport(double *px,double *py,double *pcx, double *pcy)=0;
	
	virtual void SetCamera(const CVector &vPosition,double dYaw, double dPitch, double dRoll)=0;
	virtual CVector GetCameraPosition()=0;
	virtual CVector GetCameraForward()=0;
	virtual CVector GetCameraRight()=0;
	virtual CVector GetCameraUp()=0;
	
	virtual void Project(CVector vPos,double *px,double *py)=0;
	
	virtual void ActivateClipping()=0;
	virtual void DeactivateClipping()=0;
	virtual bool IsClippingActive()=0;
	virtual void SetClipRect(double x,double y,double cx, double cy)=0;
	virtual void GetClipRect(double *px,double *py,double *pcx, double *pcy)=0;
	
	virtual void SelectTexture(IGenericTexture *pTexture,int nTextureLevel)=0;
	virtual void UnselectTexture(int nTextureLevel)=0;
		
	virtual void SetSkyShadowParameters(double dSpeed, double dXResolution, double dZResolution, double dOpacity)=0;
	virtual void SelectSkyShadow(IGenericTexture *pTexture)=0;
	virtual void UnselectSkyShadow()=0;
	
	virtual void RenderPoint(const CVector &vPosition,double dSize,const CVector &vColor,double dAlpha)=0;
	virtual void RenderTexture(const CVector &vOrigin,double s1,double s2,const CVector &vColor,double dAlpha)=0;
	virtual void RenderTexture(const CVector &vOrigin,double s1,double s2,double dTexX,double dTexY,double dTexW,double dTexH,const CVector &vColor,double dAlpha)=0;
	virtual void RenderParticle(IGenericTexture *piTexture,const CVector &vOrigin,double dAngle,double s1,double s2,double dTextX,double dTextY,double dTextW,double dTextH,const CVector &vColor,double dAlpha)=0;
	virtual void RenderModel(const CVector &vOrigin,const CVector &vOrientation,IGenericModel *piModel,unsigned int nAnimation=0,unsigned int nFrame=0)=0;
	virtual void RenderBBox(const CVector &vMins,const CVector &vMaxs,unsigned long nStipple,const CVector &vColor,double dAlpha)=0;
	virtual void RenderBBox(const CVector &vOrigin,const CVector &vOrientation,const CVector &vMins,const CVector &vMaxs,unsigned long nStipple,const CVector &vColor,double dAlpha)=0;
	virtual void RenderLine(const CVector &v1,const CVector &v2,unsigned long nStipple,const CVector &vColor,double dAlpha)=0;
	virtual void RenderLines(unsigned int nLines,const CVector *pPoints,unsigned long nStipple,const CVector &vColor,double dAlpha)=0;
	virtual void RenderLineLoop(unsigned int nLines,const CVector *pPoints,unsigned long nStipple,const CVector &vColor,double dAlpha)=0;
	virtual void RenderRect(const CVector &vCenter,const CVector &vAxisW,const CVector &vAxisH,double w,double h,CVector &vColor,double dAlpha)=0;
	virtual void RenderRect(double x,double y,double w,double h,CVector &vColor,double dAlpha)=0;
	virtual void RenderPolygon(unsigned int nVertexes,const CVector *pVertexes,const CVector &vColor,double dAlpha)=0;
	virtual void RenderPolygon(unsigned int nVertexes,const CVector *pVertexes,const CVector *pColors,const double *pdAlphas)=0;
	virtual void RenderArrowHead(const CVector &vPosition,const CVector &vDirection,CVector &vUp,double dForward,double dUp,double dRight,const CVector &vColor,double dAlpha)=0;
	
	virtual void SetModelTinting(CVector vTinting)=0;
	
	virtual void Clear(const CVector &vColor)=0;
	virtual void ClearDepth()=0;
	
	// Render state

	virtual void ActivateHeightFog(const CVector &vMins,const CVector &vMaxs,const CVector &vColor)=0;
	virtual void DeactivateHeightFog()=0;
	virtual bool IsHeightFogActive()=0;
	 
	virtual void ActivateTextures()=0;
	virtual void DeactivateTextures()=0;
	virtual bool AreTexturesActive()=0;

	virtual void ActivateLighting()=0;
	virtual void DeactivateLighting()=0;
	virtual bool IsLightingActive()=0;

	virtual void SetAmbientLight(const CVector &vColor)=0;
	virtual void GetAmbientLight(CVector *pvColor)=0;

	virtual void SetSunLight(const CVector &vPosition,const CVector &vDirection,const CVector &vAmbientColor,const CVector &vDiffuseColor,const CVector &vSpecularColor)=0;
	virtual void GetSunLight(CVector *pvPosition,CVector *pvDirection,CVector *pvAmbientColor,CVector *pvDiffuseColor,CVector *pvSpecularColor)=0;

	virtual void AddLight(IGenericLight *)=0;
	virtual void RemoveLight(IGenericLight *)=0;
 
	virtual void ActivateSolid()=0;
	virtual void DeactivateSolid()=0;
	virtual bool IsSolidActive()=0;

	virtual void ActivateBlending()=0;
	virtual void DeactivateBlending()=0;
	virtual bool IsBlendingActive()=0;
	virtual void SetBlendingFunction(unsigned int nOperator1,unsigned int nOperator2)=0;
	virtual void SetBlendingLayer(unsigned int nLayer)=0;
	
	virtual void ActivateDepth()=0;
	virtual void DeactivateDepth()=0;
	virtual void ActivateDepthWrite()=0;
	virtual void DeactivateDepthWrite()=0;
	virtual void SetDepthFunction(EDepthFunction eDepthFunction)=0;

	virtual void ActivateShadowEmission()=0;
	virtual void DeactivateShadowEmission()=0;
	virtual bool IsShadowEmissionActive()=0;

	virtual void ActivateShadowReception()=0;
	virtual void DeactivateShadowReception()=0;
	virtual bool IsShadowReceptionActive()=0;
	
	virtual void ActivateSkyShadow()=0;
	virtual void DeactivateSkyShadow()=0;
	virtual bool IsSkyShadowActive()=0;
	
	virtual void PushState()=0;
	virtual void PopState()=0;
	
	virtual void ActivateWater()=0;
	virtual void SetWaterMappingSize(double dMaxU,double dMaxV)=0;
	virtual void SetWaterMappingOffset(double dMaxU,double dMaxV)=0;
	virtual void DeactivateWater()=0;
	
	// Render options

	virtual void EnableNormalMaps()=0;
	virtual void DisableNormalMaps()=0;
	virtual bool AreNormalMapsEnabled()=0;
	
	virtual void EnableSkyShadow()=0;
	virtual void DisableSkyShadow()=0;
	virtual bool IsSkyShadowEnabled()=0;
	
	virtual void EnableTextures()=0;
	virtual void DisableTextures()=0;
	virtual bool AreTexturesEnabled()=0;

	virtual void EnableHeightFog()=0;
	virtual void DisableHeightFog()=0;
	virtual bool IsHeightFogEnabled()=0;

	virtual void EnableLighting()=0;
	virtual void DisableLighting()=0;
	virtual bool IsLightingEnabled()=0;

	virtual void EnableSolid()=0;
	virtual void DisableSolid()=0;
	virtual bool IsSolidEnabled()=0;

	virtual void EnableBlending()=0;
	virtual void DisableBlending()=0;
	virtual bool IsBlendingEnabled()=0;

	virtual void EnableShadows()=0;
	virtual void DisableShadows()=0;
	virtual bool AreShadowsEnabled()=0;
	
	virtual void EnableAutoShadowVolume()=0;
	virtual void DisableAutoShadowVolume()=0;
	virtual bool IsAutoShadowVolumeEnabled()=0;
	
	virtual void EnableStagedRenderingStats()=0;
	virtual void DisableStagedRenderingStats()=0;
	virtual bool AreStagedRenderingStatsEnabled()=0;
	
	virtual void DumpStagedRenderingStats()=0;
	
	virtual EShadingModel 	GetShadingModel()=0;
	virtual EShadowQuality  GetShadowQuality()=0;
	virtual void            SetShadowQuality(EShadowQuality eQuality)=0;

	virtual bool			SetCurrentRenderPath(std::string sRenderPath)=0;
	virtual std::string		GetCurrentRenderPath()=0;
	virtual void			GetRenderPaths(std::vector<std::string> *pvRenderPaths)=0;

	virtual void PushOptions()=0;
	virtual void PopOptions()=0;
	
	virtual void StartFrame()=0;
	virtual void EndFrame()=0;
};

#endif
