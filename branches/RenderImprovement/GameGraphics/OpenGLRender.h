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

template<typename T>
struct TBufferPool
{
	std::stack<T *> m_sFreeBuffers;
	T *Allocate()
	{
		if(m_sFreeBuffers.size())
		{
			T *pBuffer=m_sFreeBuffers.top();
			m_sFreeBuffers.pop();
			return pBuffer;
		}
		return new T;
	}
	void Free(T *pBuffer)
	{
		pBuffer->Reset();
		m_sFreeBuffers.push(pBuffer);		
	}
	TBufferPool(){};
	~TBufferPool()
	{
		while(m_sFreeBuffers.size())
		{
			T *pBuffer=m_sFreeBuffers.top();
			delete pBuffer;
			m_sFreeBuffers.pop();
		}
	}
};

class COpenGLRender: virtual public CSystemObjectBase,virtual public IGenericRender
{
	bool				m_bHardwareSupportRead;
	bool				m_bIgnoreShaderSupport;
	bool				m_bIgnoreInstancingSupport;
	
	unsigned int 		m_nCreationTime;

	SRenderState   		m_sStagedRenderingState;
	bool			 	m_bShadowVolumeFirstVertex;

	IGenericTexture *				m_ppiStagedTextureLevels[MAX_TEXTURE_LEVELS];
	unsigned int 					m_nStagedTextureLevels;
	std::stack<SRenderState>        m_sStagedRenderingStateStack;
	std::stack<SRenderOptions>      m_sRenderOptionsStack;

	TBufferPool<SPointBuffer> 		m_PointBuffers;
	TBufferPool<SLineBuffer>  		m_LineBuffers;
	TBufferPool<STriangleBuffer>  	m_TriangleBuffers;

	SSceneData m_sScene;

	COpenGLRenderWrapper m_Kernel;

	template<typename stagekeytype,typename stagetype>
	stagetype *GetStage(stagekeytype &key,std::map<stagekeytype,stagetype> &stageMap);

	template<typename stagetype>
	typename stagetype::buffer_type *GetNextStageBufferElement(stagetype *pStage,TBufferPool<typename stagetype::buffer_type> &stagePool);

	template<typename stagetype>
	typename stagetype::buffer_type *GetNextStageBufferElement(stagetype *pStage,TBufferPool<typename stagetype::buffer_type> &stagePool,typename stagetype::buffer_type *pBuffer);


	void ProcessCameraVertex( const CVector &vVertex );
	void SortModels();
	void ComputeSunShadowCamera();

	SRenderOptions m_sRenderOptions;
	SRenderStats   m_sStagedStats;
	IGenericViewport *m_piCurrentViewport;

	// Viewport / Camera

	
	bool		 m_bStagedRendering;
	double		 m_dStagedRenderingMinZ;
	double		 m_dStagedRenderingMaxZ;
	double		 m_dMinDistanceToLight;

	// Shadows

	CVector				m_vShadowVolumeMins;
	CVector				m_vShadowVolumeMaxs;

	CVector      m_vLastShadowCameraTarget;
	CVector      m_vLastShadowCameraPosition;
	CVector      m_pvLastShadowVolume[8];
	double 		 m_dShadowAntiFlickeringMargin;

	// Lighting

	CGenericLightWrapper          m_SunLight;

	// Stages

	SHardwareSupport	m_sHardwareSupport;

	std::map<SModelStageKey,SModelStage> m_mModelStages;

	virtual void InternalSetOrthographicProjection(double cx,double cy);
	virtual void InternalSetPerspectiveProjection(double dViewAngle,double dNearPlane,double dFarPlane);
	virtual void InternalSetViewport(double x,double y,double cx, double cy);
	virtual void InternalSetCamera(const CVector &vPosition,double dYaw, double dPitch, double dRoll);

	void Flush();
	
public:

	bool Init(std::string sClass,std::string sName,ISystem *piSystem);
	void Destroy();

	// IGenericRender
	void StartFrame();
	void EndFrame();
	
	void SetViewport(IGenericViewport *piViewport);

	IGenericViewport *GetViewPort(); // solo valido entre StartFrame y EndFrame.

	void SetOrthographicProjection(double cx,double cy);
	void SetPerspectiveProjection(double dViewAngle,double dNearPlane,double dFarPlane);
	void SetViewport(double x,double y,double cx, double cy);
	void SetCamera(const CVector &vPosition,double dYaw, double dPitch, double dRoll);

	CVector GetCameraPosition();
	CVector GetCameraForward();
	CVector GetCameraRight();
	CVector GetCameraUp();

	void ActivateClipping();
	void DeactivateClipping();
	bool IsClippingActive();
	
	void SetClipRect(double x,double y,double cx, double cy);
	void GetClipRect(double *px,double *py,double *pcx, double *pcy);
	
	void SelectFont(const char *pFamilyName,double dSize);
	void SelectTexture(IGenericTexture *pTexture,int nTextureLevel);
	void UnselectTexture(int nTextureLevel);
	
	void SetSkyShadowParameters(double dSpeed, double dXResolution, double dZResolution, double dOpacity);
	void SelectSkyShadow(IGenericTexture *pTexture);
	void UnselectSkyShadow();
	
	void CalcTextSize(const char *pText,double *pdWidth,double *pdHeight);
	void RenderPoint(const CVector &vPosition,double dSize,const CVector &vColor,double dAlpha);
	void RenderText(double x,double y,const char *pText,const CVector &vColor,double dAlpha);
	void RenderTextEx(double x,double y,double w,double h,const char *pText,eTextAlignment dwHorzAlign,eTextAlignment dwVertAlign,const CVector &vColor,double dAlpha);
	void RenderTexture(const CVector &vOrigin,double s1,double s2,const CVector &vColor,double dAlpha);
	void RenderTexture(const CVector &vOrigin,double s1,double s2,double dTexX,double dTexY,double dTexW,double dTexH,const CVector &vColor,double dAlpha);
	void RenderParticle(IGenericTexture *piTexture,const CVector &vOrigin,double dAngle,double s1,double s2,double dTextX,double dTextY,double dTextW,double dTextH,const CVector &vColor,double dAlpha);
	void RenderModel(const CVector &vOrigin,const CVector &vOrientation,IGenericModel *piModel,unsigned int nAnimation,unsigned int nFrame);
	
	void RenderBBox(const CVector &vMins,const CVector &vMaxs,unsigned long nStipple,const CVector &vColor,double dAlpha);
	void RenderBBox(const CVector &vOrigin,const CVector &vOrientation,const CVector &vMins,const CVector &vMaxs,unsigned long nStipple,const CVector &vColor,double dAlpha);
	void RenderLine(const CVector &v1,const CVector &v2,unsigned long nStipple,const CVector &vColor,double dAlpha);
	void RenderLines(unsigned int nLines,const CVector *pPoints,unsigned long nStipple,const CVector &vColor,double dAlpha);
	void RenderLineLoop(unsigned int nLines,const CVector *pPoints,unsigned long nStipple,const CVector &vColor,double dAlpha);
	void RenderRect(const CVector &vCenter,const CVector &vAxisW,const CVector &vAxisH,double w,double h,CVector &vColor,double dAlpha);
	void RenderRect(double x,double y,double w,double h,CVector &vColor,double dAlpha);
	void RenderPolygon(unsigned int nVertexes,const CVector *pVertexes,const CVector &vColor,double dAlpha);
	void RenderPolygon(unsigned int nVertexes,const CVector *pVertexes,const CVector *pColors,const double *pdAlphas);
	void RenderArrowHead(const CVector &vPosition,const CVector &vDirection,CVector &vUp,double dForward,double dUp,double dRight,const CVector &vColor,double dAlpha);
	
	void ActivateHeightFog(const CVector &vMins,const CVector &vMaxs,const CVector &vColor);
	void DeactivateHeightFog();
	bool IsHeightFogActive();

	void ActivateTextures();
	void DeactivateTextures();
	bool AreTexturesActive();

	void ActivateLighting();
	void DeactivateLighting();
	bool IsLightingActive();
	
	void ActivateWater();
	void SetWaterMappingSize(double dMaxU,double dMaxV);
	void SetWaterMappingOffset(double dU,double dV);
	void DeactivateWater();
	
	void SetAmbientLight(const CVector &vColor);
	void GetAmbientLight(CVector *pvColor);

	void SetSunLight(const CVector &vPosition,const CVector &vDirection,const CVector &vAmbientColor,const CVector &vDiffuseColor,const CVector &vSpecularColor);
	void GetSunLight(CVector *pvPosition,CVector *pvDirection,CVector *pvAmbientColor,CVector *pvDiffuseColor,CVector *pvSpecularColor);

	void AddLight(IGenericLight *);
	void RemoveLight(IGenericLight *);

	void ActivateSolid();
	void DeactivateSolid();
	bool IsSolidActive();

	void ActivateBlending();
	void DeactivateBlending();
	bool IsBlendingActive();
	void SetBlendingFunction(unsigned int nOperator1,unsigned int nOperator2);
	void SetBlendingLayer(unsigned int nLayer);
	
	void ActivateDepth();
	void DeactivateDepth();
	void SetDepthFunction(EDepthFunction eDepthFunction);
	
	void ActivateDepthWrite();
	void DeactivateDepthWrite();
	
	void ActivateSkyShadow();
	void DeactivateSkyShadow();
	bool IsSkyShadowActive();
	
	void ActivateShadowEmission();
	void DeactivateShadowEmission();
	bool IsShadowEmissionActive();

	void ActivateShadowReception();
	void DeactivateShadowReception();
	bool IsShadowReceptionActive();

	void PushState();
	void PopState();

	// Render Options

	void EnableNormalMaps();
	void DisableNormalMaps();
	bool AreNormalMapsEnabled();
	
	void EnableSkyShadow();
	void DisableSkyShadow();
	bool IsSkyShadowEnabled();
	
	void EnableTextures();
	void DisableTextures();
	bool AreTexturesEnabled();

	void EnableHeightFog();
	void DisableHeightFog();
	bool IsHeightFogEnabled();

	void EnableLighting();
	void DisableLighting();
	bool IsLightingEnabled();

	void EnableSolid();
	void DisableSolid();
	bool IsSolidEnabled();

	void EnableBlending();
	void DisableBlending();
	bool IsBlendingEnabled();

	void EnableShadows();
	void DisableShadows();
	bool AreShadowsEnabled();
	
	void EnableAutoShadowVolume();
	void DisableAutoShadowVolume();
	bool IsAutoShadowVolumeEnabled();
	
	void EnableStagedRenderingStats();
	void DisableStagedRenderingStats();
	bool AreStagedRenderingStatsEnabled();
	void DumpStagedRenderingStats();
	
	void 			SetShadingModel(EShadingModel eModel);
	EShadingModel 	GetShadingModel();
	
	void PushOptions();
	void PopOptions();

	// Staged Rendering

	void StartStagedRendering();
	void EndStagedRendering();
	SRenderStats GetStagedRenderingStats();
	
	// Selection

	void StartSelection(SGameRect rWindowRect,IGenericCamera *piCamera,double dx,double dy,double dPrecision);
	void SetSelectionId(unsigned int nId);
	int EndSelection();
	
	void ReloadShaders();
	
	BEGIN_PROP_MAP(COpenGLRender)
	PROP_VALUE_FLAGS(m_dShadowAntiFlickeringMargin,"ShadowAntiFlickeringMargin",10,MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(m_bIgnoreShaderSupport,"IgnoreShaderSupport",false,MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(m_bIgnoreInstancingSupport,"IgnoreInstancingSupport",false,MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP();
	
	COpenGLRender();
	~COpenGLRender();
};
