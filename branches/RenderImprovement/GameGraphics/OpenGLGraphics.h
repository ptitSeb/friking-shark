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

class IOpenGLModel;

#define MAX_OBJECT_INSTANCES 64
#define MAX_DYNAMIC_LIGHTS 7

#define TRIANGLE_BUFFER_SIZE 		1024
#define LINE_BUFFER_SIZE			1024
#define POINT_BUFFER_SIZE			1024

#define MAX_TEXTURE_LEVELS 16

struct SRenderState
{
	bool bActiveTextures;
	bool bActiveLighting;
	bool bActiveSolid;
	bool bActiveShadowEmission;
	bool bActiveWater;
	bool bActiveShadowReception;
	bool bActiveSkyShadow;

	bool bActiveDepth;
	bool bActiveDepthWrite;
	unsigned long nDepthFunction;

	bool bActiveBlending;
	unsigned int nBlendOperator1;
	unsigned int nBlendOperator2;
	unsigned int nBlendingLayer;

	bool bActiveHeightFog;
	CVector vHeightFogMins;
	CVector vHeightFogMaxs;
	CVector vHeightFogColor;
	CVector vModelTinting;

	unsigned long nTextureLevels;

	bool operator <(const SRenderState &otherState) const
	{
		return Compare(otherState)<0;
	}

	int Compare(const SRenderState &otherState) const
	{
		if(bActiveBlending<otherState.bActiveBlending){return -1;}
		if(bActiveBlending>otherState.bActiveBlending){return 1;}
		if(nBlendingLayer<otherState.nBlendingLayer){return -1;}
		if(nBlendingLayer>otherState.nBlendingLayer){return 1;}

		if(bActiveLighting<otherState.bActiveLighting){return -1;}
		if(bActiveLighting>otherState.bActiveLighting){return 1;}

		if(nTextureLevels<otherState.nTextureLevels){return -1;}
		if(nTextureLevels>otherState.nTextureLevels){return 1;}

		if(bActiveDepthWrite<otherState.bActiveDepthWrite){return -1;}
		if(bActiveDepthWrite>otherState.bActiveDepthWrite){return 1;}
		if(bActiveDepth<otherState.bActiveDepth){return -1;}
		if(bActiveDepth>otherState.bActiveDepth){return 1;}
		if(nDepthFunction<otherState.nDepthFunction){return -1;}
		if(nDepthFunction>otherState.nDepthFunction){return 1;}

		if(bActiveHeightFog<otherState.bActiveHeightFog){return -1;}
		if(bActiveHeightFog>otherState.bActiveHeightFog){return 1;}

		if(nBlendOperator1<otherState.nBlendOperator1){return 1;}
		if(nBlendOperator1>otherState.nBlendOperator1){return -1;}
		if(nBlendOperator2<otherState.nBlendOperator2){return 1;}
		if(nBlendOperator2>otherState.nBlendOperator2){return -1;}

		if(bActiveTextures<otherState.bActiveTextures){return -1;}
		if(bActiveTextures>otherState.bActiveTextures){return 1;}

		if(bActiveSolid<otherState.bActiveSolid){return -1;}
		if(bActiveSolid>otherState.bActiveSolid){return 1;}

		if(bActiveShadowEmission<otherState.bActiveShadowEmission){return -1;}
		if(bActiveShadowEmission>otherState.bActiveShadowEmission){return 1;}
		if(bActiveShadowReception<otherState.bActiveShadowReception){return -1;}
		if(bActiveShadowReception>otherState.bActiveShadowReception){return 1;}
		if(bActiveWater<otherState.bActiveWater){return -1;}
		if(bActiveWater>otherState.bActiveWater){return 1;}
		if(bActiveSkyShadow<otherState.bActiveSkyShadow){return -1;}
		if(bActiveSkyShadow>otherState.bActiveSkyShadow){return 1;}
		return 0;
	}

	SRenderState()
	{
		bActiveTextures=true;
		bActiveLighting=false;
		bActiveSolid=true;
		bActiveShadowEmission=false;
		bActiveShadowReception=false;
		bActiveSkyShadow=false;
		nTextureLevels=0;

		bActiveDepth=true;
		bActiveDepthWrite=true;
		nDepthFunction=GL_LESS;

		bActiveBlending=false;
		nBlendOperator1=GL_SRC_ALPHA;
		nBlendOperator2=GL_ONE_MINUS_SRC_ALPHA;
		nBlendingLayer=0;

		bActiveHeightFog=false;
		bActiveWater=false;
		vModelTinting=ColorWhite;
	}
};

struct SRenderOptions
{
	bool bEnableTextures;
	bool bEnableLighting;
	bool bEnableSolid;
	bool bEnableShadows;
	bool bEnableAutoShadowVolume;
	bool bEnableBlending;
	bool bEnableHeightFog;
	bool bEnableNormalMaps;
	bool bEnableSkyShadow;
	bool bEnableStagedRenderingStats;

	SRenderOptions()
	{
		bEnableHeightFog=false;
		bEnableTextures=true;
		bEnableLighting=false;
		bEnableShadows=false;
		bEnableSolid=true;
		bEnableBlending=true;
		bEnableAutoShadowVolume=false;
		bEnableNormalMaps=false;
		bEnableSkyShadow=false;
		bEnableStagedRenderingStats=false;
	}
};

struct SHardwareSupport
{
	bool bShaders;
	bool bObjectInstancing;
	int  nMaxTextureUnits;

	SHardwareSupport(){bShaders=false;nMaxTextureUnits=2;bObjectInstancing=false;}
};

struct STriangleBuffer
{
	int nUsedElements;
	float pVertexBuffer[3*3*TRIANGLE_BUFFER_SIZE];
	float pColorBuffer[4*3*TRIANGLE_BUFFER_SIZE];
	float pTexBuffer[2*3*TRIANGLE_BUFFER_SIZE];

	float *pVertexBufferCursor;
	float *pColorBufferCursor;
	float *pTexBufferCursor;

	static const unsigned int buffer_size;

	void Reset()
	{
		pVertexBufferCursor=pVertexBuffer;
		pColorBufferCursor=pColorBuffer;
		pTexBufferCursor=pTexBuffer;
		nUsedElements=0;
	}
	void Next()
	{
		pVertexBufferCursor+=9;
		pColorBufferCursor+=12;
		pTexBufferCursor+=6;
		nUsedElements++;
	}
	STriangleBuffer(){Reset();}
};

struct STriangleStageKey
{
	SRenderState  sRenderState;
	IGenericTexture *piTexture;

	bool operator <(const STriangleStageKey &otherStage) const
	{
		int nStateComp=sRenderState.Compare(otherStage.sRenderState);
		if(nStateComp<0){return true;}
		if(nStateComp>0){return false;}
		if(piTexture<otherStage.piTexture){return true;}
		if(piTexture>otherStage.piTexture){return false;}
		return false;
	}

	STriangleStageKey(){piTexture=NULL;}
	STriangleStageKey(IGenericTexture *texture,SRenderState renderState){piTexture=texture;sRenderState=renderState;}
};


struct STriangleStage
{
	typedef STriangleBuffer buffer_type;
	std::vector<STriangleBuffer*> vBuffers;

	void Init(STriangleStageKey &key){ADD(key.piTexture);}
};

struct SPointBuffer
{
	int nUsedElements;
	float pVertexBuffer[3*2*POINT_BUFFER_SIZE];
	float pColorBuffer[4*2*POINT_BUFFER_SIZE];

	float *pVertexBufferCursor;
	float *pColorBufferCursor;

	static const unsigned int buffer_size;

	void Reset()
	{
		nUsedElements=0;
		pVertexBufferCursor=pVertexBuffer;
		pColorBufferCursor=pColorBuffer;
	}
	void Next()
	{
		nUsedElements++;
		pVertexBufferCursor+=3;
		pColorBufferCursor+=4;
	}
	SPointBuffer(){Reset();}
};

struct SPointStageKey
{
	SRenderState	sRenderState;
	double			dSize;

	bool operator <(const SPointStageKey &otherStage) const
	{
		int nStateComp=sRenderState.Compare(otherStage.sRenderState);
		if(nStateComp<0){return true;}
		if(nStateComp>0){return false;}
		if(dSize<otherStage.dSize){return true;}
		if(dSize>otherStage.dSize){return false;}
		return false;
	}

	SPointStageKey(){dSize=0;}
	SPointStageKey(SRenderState renderState,unsigned long size){sRenderState=renderState;dSize=size;}
};


struct SPointStage
{
	typedef SPointBuffer buffer_type;
	std::vector<SPointBuffer*> vBuffers;

	void Init(SPointStageKey &key){}
};

struct SLineBuffer
{
	int nUsedElements;

	float pVertexBuffer[3*2*LINE_BUFFER_SIZE];
	float pColorBuffer[4*2*LINE_BUFFER_SIZE];

	float *pVertexBufferCursor;
	float *pColorBufferCursor;

	static const unsigned int buffer_size;

	void Reset()
	{
		pVertexBufferCursor=pVertexBuffer;
		pColorBufferCursor=pColorBuffer;
		nUsedElements=0;
	}
	void Next()
	{
		pVertexBufferCursor+=6;
		pColorBufferCursor+=8;
		nUsedElements++;
	}

	SLineBuffer(){Reset();}
};

struct SLineStageKey
{
	SRenderState  sRenderState;
	unsigned long nStipple;

	bool operator <(const SLineStageKey &otherStage) const
	{
		int nStateComp=sRenderState.Compare(otherStage.sRenderState);
		if(nStateComp<0){return true;}
		if(nStateComp>0){return false;}
		if(nStipple<otherStage.nStipple){return true;}
		if(nStipple>otherStage.nStipple){return false;}
		return false;
	}

	SLineStageKey(){nStipple=0;}
	SLineStageKey(SRenderState renderState,unsigned long stipple){sRenderState=renderState;nStipple=stipple;}
};

struct SLineStage
{
	typedef SLineBuffer buffer_type;
	std::vector<SLineBuffer*> vBuffers;

	void Init(SLineStageKey &key){}
};

struct SModelInstance
{
	CVector vPos;
	CVector vAngles;
	CVector vTinting;
	double  dDistanceToCamera;
	bool bSkipRender;
	int nId;

	bool operator<(const SModelInstance &other) const 
	{ 
		if(dDistanceToCamera < other.dDistanceToCamera){return true;} 
		if(dDistanceToCamera > other.dDistanceToCamera){return false;} 
		return nId<other.nId;
	}

	SModelInstance(){bSkipRender=false;dDistanceToCamera=0;nId=0;vTinting=ColorWhite;}
};

struct SModelStageKey
{
	SRenderState  sRenderState;
	IGenericModel *piModel;
	unsigned int  nAnimation;
	unsigned int  nFrame;

	bool operator <(const SModelStageKey &otherStage) const
	{
		int nStateComp=sRenderState.Compare(otherStage.sRenderState);
		if(nStateComp<0){return true;}
		if(nStateComp>0){return false;}
		if(piModel<otherStage.piModel){return true;}
		if(piModel>otherStage.piModel){return false;}
		if(nAnimation<otherStage.nAnimation){return true;}
		if(nAnimation>otherStage.nAnimation){return false;}
		if(nFrame<otherStage.nFrame){return true;}
		if(nFrame>otherStage.nFrame){return false;}
		return false;
	}

	SModelStageKey(){piModel=NULL;nAnimation=0;nFrame=0;}
	SModelStageKey(IGenericModel *model,unsigned int animation,unsigned int frame,SRenderState &renderState){piModel=model;nAnimation=animation;nFrame=frame;sRenderState=renderState;}
};

struct SModelStage
{
	IGenericModel *piModel;
	IOpenGLModel *piGLModel;
	double 	      dNearestModel;

	std::vector<SModelInstance> vInstances;

	SModelStage(){piModel=NULL;piGLModel=NULL;dNearestModel=0;}
};

#define MAX_OPENGL_TEXTURE_COORDS 2
struct SOpenGLRenderMappings
{
	int nVertexAttribIndex;
	int pTextureAttribIndex[MAX_OPENGL_TEXTURE_COORDS];
	int nNormalAttribIndex;
	int nNormalMapCoordAttribIndex;
	int nColorAttribIndex;
	int nNormalMapTextureIndex;
	int nTangentAttribIndex;
	int nBitangentAttribIndex;
	
	SOpenGLRenderMappings()
	{
		nNormalMapTextureIndex=-1;
		
		nVertexAttribIndex=-1;
		for(int x=0;x<MAX_OPENGL_TEXTURE_COORDS;x++){pTextureAttribIndex[x]=-1;}
		nNormalAttribIndex=-1;
		nNormalMapCoordAttribIndex=-1;
		nColorAttribIndex=-1;
		nTangentAttribIndex=-1;
		nBitangentAttribIndex=-1;		
	}
};

struct SVertexBufferObject
{
	unsigned int nVertexArrayObject;
	unsigned int nBufferObject;
	unsigned int nIndexesBufferObject;
	int nVertexOffset;
	int nColorOffset;
	int nNormalOffset;
	int nTextures;
	int pTexOffsets[MAX_OPENGL_TEXTURE_COORDS];
	int nNormalMapOffset;
	int nTangentOffset;
	int nBitangentOffset;

	int nVertexStride;
	int nColorStride;
	int nNormalStride;
	int pTexStrides[MAX_OPENGL_TEXTURE_COORDS];
	int nNormalMapStride;
	int nTangentStride;
	int nBitangentStride;
	
	SVertexBufferObject()
	{
		nBufferObject=0;
		nIndexesBufferObject=0;
		nVertexArrayObject=0;
		nVertexOffset=-1;
		nColorOffset=-1;
		nNormalOffset=-1;
		nTextures=0;
		nNormalMapOffset=-1;
		nTangentOffset=-1;
		nBitangentOffset=-1;

		nVertexStride=0;
		nColorStride=0;
		nNormalStride=0;
		nNormalMapStride=0;
		nTangentStride=0;
		nBitangentStride=0;

		for(int x=0;x<MAX_OPENGL_TEXTURE_COORDS;x++)
		{
			pTexOffsets[x]=-1;
			pTexStrides[x]=0;
		}
	}
};

struct SSceneCamera
{
	CMatrix m_ProjectionMatrix;
	CMatrix m_ViewMatrix;

	CPlane m_CameraFustrumPlanes[4];
	CPlane m_CameraForwardPlane;
	CPlane m_CameraRightPlane;
	CPlane m_CameraUpPlane;

	CVector m_vCameraForward;
	CVector m_vCameraRight;
	CVector m_vCameraUp;
	CVector m_vCameraPos;
	CVector m_vCameraAngles;

	double      m_dProjectionWidth;
	double      m_dProjectionHeight;

	bool		m_bPerspectiveProjection;
	double		m_dPerspectiveViewAngle;
	double		m_dPerspectiveNearPlane;
	double		m_dPerspectiveFarPlane;

	int			m_nViewportX;
	int			m_nViewportY;
	int			m_nViewportW;
	int			m_nViewportH;
	bool bViewModified;
	bool bViewportModified;
	bool bProjectionModified;

	SSceneCamera()
	{
		bViewModified=false;
		bViewportModified=false;
		bProjectionModified=false;

		m_dProjectionWidth=1;
		m_dProjectionHeight=1;

		m_bPerspectiveProjection=false;
		m_dPerspectiveNearPlane=1.0;
		m_dPerspectiveFarPlane=10000.0;
		m_dPerspectiveViewAngle=60;

		m_nViewportX=0;
		m_nViewportY=0;
		m_nViewportW=1;
		m_nViewportH=1;
	}
};

struct SSceneObjects
{
	std::map<SPointStageKey,SPointStage>										mPointStages;
	std::map<SLineStageKey,SLineStage>											mLineStages;
	std::map<STriangleStageKey,STriangleStage>									mTriangleStages;
	std::multimap<double,std::pair<const SModelStageKey*,const SModelStage*> >  mModelStages;
};

struct SSceneLighting
{
	CVector 	                  m_vAmbientColor;
	std::vector<IGenericLight *>  m_vLights;
	IGenericLight 				 *m_piSunLight;

	CVector     m_vSunShadowPos;
	CVector     m_vSunShadowAngles;
	double		m_dSunShadowViewAngle;
	double		m_dSunShadowNearPlane;
	double		m_dSunShadowFarPlane;

	SSceneLighting(){m_piSunLight=NULL;m_dSunShadowViewAngle=0;m_dSunShadowNearPlane=0;m_dSunShadowFarPlane=0;}
};

struct SSceneSky
{
	IGenericTexture  *m_piSkyShadow;
	double m_dSkyShadowSpeed;
	double m_dSkyShadowXResolution;
	double m_dSkyShadowZResolution;
	double m_dSkyShadowOpacity;	

	SSceneSky(){m_piSkyShadow=NULL;m_dSkyShadowSpeed=0;m_dSkyShadowXResolution=1;m_dSkyShadowZResolution=1;m_dSkyShadowOpacity=1;}
};

struct SSceneWater
{
	double dSizeU;
	double dSizeV;
	double dOffsetU;
	double dOffsetV;
	
	SSceneWater(){dSizeU=dSizeV=1;dOffsetU=dOffsetV=0;}
};

struct SSceneClipping
{
	bool bEnabled;
	SGameRect rRect;
	
	SSceneClipping(){bEnabled=false;}
};

struct SSceneData
{
	float          fTime;
	bool           bClear;
	CVector        vClearColor;
	SSceneClipping clipping;   
	SSceneCamera   camera;
	SSceneObjects  objects;
	SSceneLighting lighting;
	SSceneSky	   sky;
	SSceneWater	   water;

	SSceneData(){bClear=false;fTime=0;}
};

class IOpenGLRender:virtual public ISystemUnknown
{
public:
	virtual bool Setup(IGenericRender *piRender,IGenericViewport *piViewport,SHardwareSupport &support)=0;
	virtual void StartFrame()=0;
	virtual void RenderScene(SSceneData &pScene)=0;
	virtual void EndFrame()=0;

	virtual void ReloadShaders()=0;
	
	virtual void SetVertexPointers(float *pVertex,float *pNormal,float *pColor,int nTex,float **pTex, float *pNormalTex,float *pTangent,float *pBiTangent)=0;
	virtual void SetVertexBufferObject(SVertexBufferObject *pVBO)=0;
	
	virtual void PrepareTexture(IGenericTexture *piTexture,unsigned int nTextureLevel)=0;
	virtual void UnprepareTexture(unsigned int nTextureLevel)=0;
	
	virtual void PrepareNormalMap(IGenericTexture *piNormalMap)=0;
	virtual void UnprepareNormalMap()=0;

	virtual void StartSelection(SGameRect rWindowRect,IGenericCamera *piCamera,double dx,double dy,double dPrecision)=0;
	virtual void SetSelectionId(unsigned int nId)=0;
	virtual int EndSelection()=0;

	virtual EShadingModel GetShadingModel()=0;
};

class IOpenGLModel:virtual public ISystemUnknown
{
public:

	virtual void PrepareRenderBuffer(IOpenGLRender *piGLRender,unsigned int nAnimation,unsigned int nFrame, unsigned int nBuffer,bool bRenderingShadow,SOpenGLRenderMappings *pRenderMappings)=0;
	virtual void CallRenderBuffer(IOpenGLRender *piGLRender,unsigned int nAnimation,unsigned int nFrame, unsigned int nBuffer,unsigned int nInstances)=0;
	virtual void UnPrepareRenderBuffer(IOpenGLRender *piGLRender,unsigned int nAnimation,unsigned int nFrame, unsigned int nBuffer,bool bRenderingShadow,SOpenGLRenderMappings *pRenderMappings)=0;
};
