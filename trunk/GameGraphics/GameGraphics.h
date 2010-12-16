#pragma once

#include "GameRuntimeLib.h"

struct SGameRect;

struct SGamePos
{
	double x;
	double y;

	void ClipToUnit();
	void ClipToRect(SGameRect *pRect);
	void CenterOnRect(SGameRect *pRect);

	SGamePos(double _x,double _y);
	SGamePos();
};

struct SGameSize
{
	double w;
	double h;

	SGameSize(double _w,double _h);
	SGameSize();
};

struct SGameRect
{
	double x;
	double y;
	double w;
	double h;

	void ClipToUnit();
	void ClipToRect(SGameRect *pRect);
	void CenterOnRect(SGameRect *pRect);

	bool Contains(SGamePos point);

	SGameRect(double _x,double _y,double _w,double _h);
	SGameRect();
};

BEGIN_STRUCT_PROPS(SGameRect)
	PROP_VALUE(x,"x",0)
	PROP_VALUE(y,"y",0)
	PROP_VALUE(w,"w",0)
	PROP_VALUE(h,"h",0)
END_STRUCT_PROPS()

BEGIN_STRUCT_PROPS(SGameSize)
	PROP_VALUE(w,"w",0)
	PROP_VALUE(h,"h",0)
END_STRUCT_PROPS()

BEGIN_STRUCT_PROPS(SGamePos)
	PROP_VALUE(x,"x",0)
	PROP_VALUE(y,"y",0)
END_STRUCT_PROPS()

typedef enum eTextAlignment
{
	eTextAlignment_None		=0x0000,
	eTextAlignment_Left		=0x0001,
	eTextAlignment_Top		=0x0001,
	eTextAlignment_Center	=0x0002,
	eTextAlignment_Right	=0x0003,
	eTextAlignment_Bottom	=0x0003
};

DECLARE_SERIALIZABLE_ENUMERATION(eTextAlignment);

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

	virtual LRESULT OnProcessMessage(LRESULT dwPreviousResult,HWND hwnd,UINT  uMsg, WPARAM  wParam,LPARAM  lParam)=0;

	virtual void OnLButtonDown(WORD wKeyState,unsigned x,unsigned y)=0;
	virtual void OnLButtonUp(WORD wKeyState,unsigned x,unsigned y)=0;
	virtual void OnRButtonDown(WORD wKeyState,unsigned x,unsigned y)=0;
	virtual void OnRButtonUp(WORD wKeyState,unsigned x,unsigned y)=0;
	virtual void OnMouseMove(unsigned x,unsigned y)=0;
	virtual void OnSize(unsigned cx,unsigned cy)=0;
};

class IGenericViewport:virtual public ISystemUnknown
{
public:

	virtual HWND Create(HWND hParent,RECT *pRect,bool bMaximized)=0;

	virtual HWND GetWindowHandle()=0;
	virtual HWND GetParentWindowHandle()=0;

	virtual bool IsMaximized()=0;
	virtual void SetMaximized(bool bMaximized)=0;

	virtual void SetPos(unsigned dwX,unsigned dwY)=0;
	virtual void GetPos(unsigned *pdwX,unsigned *pdwY)=0;

	virtual void GetSize(unsigned *pdwWidth,unsigned *pdwHeight)=0;
	virtual void SetSize(unsigned dwWidth,unsigned dwHeight)=0;

	virtual void GetClientSize(unsigned *pdwWidth,unsigned *pdwHeight)=0;

	virtual void GetRect(unsigned *pdwX,unsigned *pdwY,unsigned *pdwWidth,unsigned *pdwHeight)=0;
	virtual void SetRect(unsigned dwX,unsigned dwY,unsigned dwWidth,unsigned dwHeight)=0;

	virtual void SetVSync(bool bVSync)=0;
	virtual bool GetVSync()=0;

	virtual void				SetCaption(std::string sCaption)=0;
	virtual std::string GetCaption()=0;

	virtual void				SetIcon(HICON hIcon)=0;
	virtual HICON				GetIcon()=0;

	virtual void SetCallBack(IGenericViewportCallBack *pCallBack)=0;
};

class IGenericTexture:virtual public ISystemUnknown
{	
public:

	virtual std::string	GetFileName()=0;

	virtual bool		HasAlphaFile()=0;
	virtual std::string	GetAlphaFileName()=0;

	virtual void		GetSize(unsigned *pdwWidth,unsigned *pdwHeight)=0;

	virtual bool		HasAlphaChannel()=0;

	virtual unsigned long GetByteBufferLength()=0;
	virtual void		  *GetByteBuffer()=0;

	virtual bool		HasColorKey()=0;
	virtual COLORREF	GetColorKey()=0;

	virtual CVector		GetPixelColor(unsigned long x, unsigned long y)=0;

	virtual bool		Load(string sFileName,COLORREF *pColorKey=NULL,string *pAlphaFile=NULL,float fOpacity=1.0)=0;
	virtual bool		Create( unsigned nWidth,unsigned nHeight )=0;
	virtual bool		CreateDepth( unsigned nWidth,unsigned nHeight )=0;

	virtual bool		StartRenderingToTexture()=0;
	virtual void		StopRenderingToTexture()=0;

	// for the render only

	virtual bool PrepareTexture(IGenericRender *piRender,int nTextureLevel)=0;
	virtual void UnprepareTexture(IGenericRender *piRender,int nTextureLevel)=0;
};

class IGenericModel:virtual public ISystemUnknown
{	
public:

	virtual void Create()=0;
	virtual bool Load(string sFileName)=0;

	virtual void		  SetBSPOptions(bool bLoad,bool bAutoGenerate,bool bAutoUpdate)=0;
	virtual void		  GetBSPOptions(bool *pbLoad,bool *pbAutoGenerate,bool *pbAutoUpdate)=0;
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
	virtual void		  SetRenderBufferVertexes(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nVertexes,float *pVertexes)=0;
	virtual void		  SetRenderBufferFaces(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nFaces,unsigned int *pFacesVertexes)=0;
	virtual void		  SetRenderBufferNormals(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,float *pNormals)=0;
	virtual void		  SetRenderBufferColors(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,float *pColors)=0;
	virtual void		  SetRenderBufferTextureCoords(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,float *pTexVertexes)=0;

	virtual void		  GetRenderBufferMaterial(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,CVector *pvAmbientColor,CVector *pvDiffuseColor,CVector *pvSpecularColor, float *pfShininess, float *pfOpacity)=0;
	virtual void		  GetRenderBufferTexture(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,IGenericTexture **ppiTexture)=0;
	virtual void		  GetRenderBufferVertexes(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long *pVertexes,float **ppVertexes)=0;
	virtual void		  GetRenderBufferFaces(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long *pFaces,unsigned int **ppFacesVertexes)=0;
	virtual void		  GetRenderBufferNormals(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,float **ppNormals)=0;
	virtual void		  GetRenderBufferColors(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,float **ppColors)=0;
	virtual void		  GetRenderBufferTextureCoords(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,float **ppTexVertexes)=0;

	virtual void UpdateFrameBuffers()=0;

	// for the render only
	virtual void		  Render(IGenericRender *piRender,unsigned long nAnimation,unsigned long nFrame)=0;

};

enum eGenericLightType
{
	eGenericLightType_UNKNOWN,
	eGenericLightType_Omni,
	eGenericLightType_Spot,
	eGenericLightType_Directional
};

DECLARE_SERIALIZABLE_ENUMERATION(eGenericLightType);

class IGenericLight:virtual public ISystemUnknown
{
public:

	virtual eGenericLightType GetType()=0;

	virtual CVector GetColor()=0;
	virtual CVector GetPosition()=0;

	virtual void SetColor(CVector cColor)=0;
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

enum eGenericFontType
{
	eGenericFontType_UNKNOWN=0,
	eGenericFontType_System,
	eGenericFontType_Texture
};

DECLARE_SERIALIZABLE_ENUMERATION(eGenericFontType);

class IGenericFont:virtual public ISystemUnknown
{	
public:

	virtual eGenericFontType GetType()=0;

	virtual void CalcTextSize(double dFontHeight,const char *pText,double *pdWidth,double *pdHeight)=0;
	virtual void RenderText(double dFontHeight,double x,double y,const char *pText)=0;
	virtual void RenderTextEx(double dFontHeight,double x,double y,double w,double h,const char *pText,eTextAlignment dwHorzAlign,eTextAlignment dwVertAlign)=0;
};

struct IGenericShader:virtual public ISystemUnknown
{
	virtual void Load(string sVertexShaderFile,string sFragmentShaderFile,string sPreprocessorDefinitions)=0;
	virtual void Create(string sVertexShaderCode,string sFragmentShaderCode,string sPreprocessorDefinitions)=0;
	
	virtual void AddUniform( string sUniformName,int bValue )=0;
	virtual void AddUniform( string sUniformName,float fValue )=0;
	virtual void AddUniform( string sUniformName,const CVector &vVector )=0;
	virtual void AddUniform( string sUniformName,const CVector &vColor, float fAlpha )=0;
	virtual void AddUniform( string sUniformName,double *pMatrix)=0;

	virtual bool Activate()=0;
	virtual void Deactivate()=0;
};

class IGenericRender:virtual public ISystemUnknown
{
public:

	virtual void StartFrame(IGenericViewport *piViewport)=0;
	virtual void EndFrame()=0;

	virtual void StartStagedRendering()=0;
	virtual void EndStagedRendering()=0;

	virtual void StartSelection(SGameRect rWindowRect,IGenericCamera *piCamera,double dx,double dy,double dPrecision)=0;
	virtual void SetSelectionId(unsigned int nId)=0;
	virtual int  EndSelection()=0;

	virtual bool IsRenderingWithShader()=0;

	virtual IGenericViewport *GetViewPort()=0; // solo valido entre StartFrame y EndFrame.

	virtual void SetOrthographicProjection(double cx,double cy)=0;
	virtual void SetPerspectiveProjection(double dViewAngle,double dNearPlane,double dFarPlane)=0;
	virtual void SetViewport(double x,double y,double cx, double cy)=0;
	virtual void SetCamera(const CVector &vPosition,double dYaw, double dPitch, double dRoll)=0;

	virtual void ActivateClipping(bool bActivate)=0;
	virtual void SetClipRect(double x,double y,double cx, double cy)=0;

	virtual void Clear(const CVector &vColor,double dAlpha)=0;
	virtual void SetColor(const CVector &vColor,double dAlpha)=0;

	virtual void SelectTexture(IGenericTexture *pTexture,int nTextureLevel)=0;
	virtual void UnselectTexture(int nTextureLevel)=0;

	virtual void RenderPoint(const CVector &vPosition,double dSize,const CVector &vColor,double dAlpha)=0;
	virtual void RenderTexture(const CVector &vOrigin,double s1,double s2)=0;
	virtual void RenderParticle(IGenericTexture *piTexture,const CVector &vOrigin,double dAngle,double s1,double s2,const CVector &vColor,double dAlpha)=0;
	virtual void RenderTextureRect(double dPosx,double dPosy,double dWidth,double dHeight,double dTexturex,double dTexturey,double dTextureWidth,double dTextureHeight)=0;
	virtual void RenderModel(const CVector &vOrigin,const CVector &vOrientation,IGenericModel *piModel,unsigned int nAnimation=0,unsigned int nFrame=0)=0;
	virtual void RenderBBox(const CVector &vMins,const CVector &vMaxs,const CVector &vColor,unsigned long nStipple=0x8888)=0;
	virtual void RenderBBox(const CVector &vOrigin,const CVector &vOrientation,const CVector &vMins,const CVector &vMaxs,const CVector &vColor,unsigned long nStipple=0x8888)=0;
	virtual void RenderLine(const CVector &v1,const CVector &v2,const CVector &vColor,unsigned long nStipple=0x8888)=0;
	virtual void RenderRect(const CVector &vCenter,const CVector &vAxisW,const CVector &vAxisH,double w,double h)=0;
	virtual void RenderPyramid(const CVector &vTopVertex,const CVector &vSizes,bool bSolid)=0;
	virtual void RenderPyramid(const CVector &vTopVertex,double dUpperSizeX,double dUpperSizeZ,double dLowerSizeX,double dLowerSizeZ,double dHeight,bool bSolid)=0;


	// Render state

	virtual void ActivateHeightFog(double dMinHeight,double dMaxHeight,const CVector &vColor)=0;
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

	virtual void SetSunLight(const CVector &vPosition,const CVector &vDirection,const CVector &vColor)=0;
	virtual void GetSunLight(CVector *pvPosition,CVector *pvDirection,CVector *pvColor)=0;

	virtual void AddLight(IGenericLight *)=0;
	virtual void RemoveLight(IGenericLight *)=0;
 
	virtual void ActivateSolid()=0;
	virtual void DeactivateSolid()=0;
	virtual bool IsSolidActive()=0;

	virtual void ActivateBlending()=0;
	virtual void DeactivateBlending()=0;
	virtual bool IsBlendingActive()=0;
	virtual void SetBlendingFunction(unsigned int nOperator1,unsigned int nOperator2)=0;

	virtual void ActivateDepth()=0;
	virtual void DeactivateDepth()=0;
	virtual void SetDepthFunction(unsigned int nDepthFunc)=0;

	virtual void ActivateShadowEmission()=0;
	virtual void DeactivateShadowEmission()=0;
	virtual bool IsShadowEmissionActive()=0;

	virtual void ActivateShadowReception()=0;
	virtual void DeactivateShadowReception()=0;
	virtual bool IsShadowReceptionActive()=0;

	virtual void PushState()=0;
	virtual void PopState()=0;

	// Render options

	virtual void EnableShaders()=0;
	virtual void DisableShaders()=0;
	virtual bool AreShadersEnabled()=0;

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

	virtual void PushOptions()=0;
	virtual void PopOptions()=0;
};