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

#include "WorldEntity.h"

struct STerrainHeightLayerData
{
	CGenericTextureWrapper				m_Texture;
	CGenericTextureWrapper				m_NormalMap;
	STerrainHeightLayer					m_LayerConfig;
};


struct STerrainColorLayerData
{
	CGenericTextureWrapper				m_Texture;
	CGenericTextureWrapper				m_NormalMap;
	STerrainColorLayer					m_LayerConfig;
};

struct STerrainSkyData
{
	CGenericTextureWrapper				m_Texture;
	STerrainSky							m_Config;
};

struct STerrainWaterData
{
	CGenericTextureWrapper				m_Texture1;
	CGenericTextureWrapper				m_Texture2;
	STerrainWater						m_Config;
};

BEGIN_STRUCT_PROPS(STerrainWater)
	PROP(bEnabled,"Enabled")
	PROP(dHorizontalResolution,"HorzResolution")
	PROP(dVerticalResolution,"VertResolution")
	PROP_FLAGS(vMins,"Mins",MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_FLAGS(vMaxs,"Maxs",MRPF_NORMAL|MRPF_OPTIONAL)
	PROP(sTextureFile1,"Texture1")
	PROP(sTextureFile2,"Texture2")
	PROP_VALUE_FLAGS(dSpeed,"Speed",0,MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(dOpacity,"Opacity",1,MRPF_NORMAL|MRPF_OPTIONAL)
END_STRUCT_PROPS()


BEGIN_STRUCT_PROPS(STerrainSky)
	PROP(bEnabled,"Enabled")
	PROP(dHorizontalResolution,"HorzResolution")
	PROP(dVerticalResolution,"VertResolution")
	PROP(sTextureFile,"Texture")
	PROP_VALUE_FLAGS(dSpeed,"Speed",0,MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(dOpacity,"Opacity",1,MRPF_NORMAL|MRPF_OPTIONAL)
END_STRUCT_PROPS()


BEGIN_STRUCT_PROPS(STerrainSun)
	PROP_VALUE(dDistance,"Distance",1)
	PROP_VALUE(dElevation,"Elevation",90)
	PROP_VALUE(dAzimuth,"Azimuth",0)
	PROP_VALUE_FLAGS(vColor,"Color",CVector(0.2,0.2,0.2),MRPF_NORMAL|MRPF_OPTIONAL)
END_STRUCT_PROPS()


BEGIN_STRUCT_PROPS(STerrainFog)
	PROP_VALUE(bEnabled,"Enabled",false)
	PROP_FLAGS(vMins,"Mins",MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_FLAGS(vMaxs,"Maxs",MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(vColor,"Color",CVector(0.2,0.2,0.2),MRPF_NORMAL|MRPF_OPTIONAL)
END_STRUCT_PROPS()

BEGIN_STRUCT_PROPS(STerrainHeightLayer)
	PROP(dMinHeight,"MinHeight")
	PROP(dMaxHeight,"MaxHeight")
	PROP(dHorizontalResolution,"HorzResolution")
	PROP(dVerticalResolution,"VertResolution")
	PROP(dDecayMargin,"HeightMargin")
	PROP(sTextureFile,"Texture")
END_STRUCT_PROPS()

BEGIN_STRUCT_PROPS(STerrainColorLayer)
	PROP(vColor,"Color")
	PROP(dHorizontalResolution,"HorzResolution")
	PROP(dVerticalResolution,"VertResolution")
	PROP(dColorSaturationMargin,"ColorSaturationMargin")
	PROP(dColorValueMargin,"ColorValueMargin")
	PROP(bAbruptEnd,"AbrupEnd")
	PROP(sTextureFile,"Texture")
END_STRUCT_PROPS()

BEGIN_STRUCT_PROPS(STerrainHeightLayerData)
	PROP(m_Texture,"Texture")
	PROP_FLAGS(m_NormalMap,"NormalMap",MRPF_NORMAL|MRPF_OPTIONAL)
	PROP(m_LayerConfig,"Config")
END_STRUCT_PROPS()

BEGIN_STRUCT_PROPS(STerrainColorLayerData)
	PROP(m_Texture,"Texture")
	PROP_FLAGS(m_NormalMap,"NormalMap",MRPF_NORMAL|MRPF_OPTIONAL)
	PROP(m_LayerConfig,"Config")
END_STRUCT_PROPS()

BEGIN_STRUCT_PROPS(STerrainWaterData)
	PROP(m_Texture1,"Texture1")
	PROP(m_Texture2,"Texture2")
	PROP(m_Config,"Config")
END_STRUCT_PROPS()

BEGIN_STRUCT_PROPS(STerrainSkyData)
	PROP_FLAGS(m_Texture,"Texture",MRPF_NORMAL|MRPF_OPTIONAL)
	PROP(m_Config,"Config")
END_STRUCT_PROPS()

class CWorldManager:virtual public CSystemObjectBase, virtual public IGameManager, virtual public IWorldManager,virtual public ITerrainDesign, virtual public IEntityEvents
{
 
    CWorldEntity            *m_pWorldEntity;
	unsigned int 			m_nSectorsToGenerate;
   
	CEntityManagerWrapper   m_EntityManagerWrapper;
	CGameControllerWrapper  m_GameControllerWrapper;
	CPlayAreaManagerWrapper m_PlayAreaManagerWrapper;
	CFrameManagerWrapper    m_FrameManagerWrapper;

	std::string				m_sTerrainColorMap;
	CGenericTextureWrapper	m_TerrainColorMap;

	std::string				m_sTerrainBaseModel;
	CGenericModelWrapper	m_TerrainBaseModel;

	STerrainWaterData		m_TerrainWater;
	STerrainSkyData			m_TerrainSky;

	STerrainSun				m_TerrainSun;
	STerrainFog				m_TerrainFog;
	CVector					m_vTerrainAmbientColor;

	std::vector<STerrainHeightLayerData> m_vTerrainHeightLayers;
	std::vector<STerrainColorLayerData>  m_vTerrainColorLayers;

	std::vector<CGenericModelWrapper> m_TerrainSectors;
	std::vector<CGenericModelWrapper> m_TerrainSectorsWaterFloors;
	CGenericModelWrapper			  m_WaterModel;
	
	int m_nWaterRenderBuffer;
	
	void Render(IGenericRender *piRender,IGenericCamera *piCurrentCamera,bool bAllSectors,bool bWaterFloor);
	
public:

	CBSPNode			   *m_pTerrainBSP;

    BEGIN_PROP_MAP(CWorldManager)
		PROP_VALUE_FLAGS(m_nSectorsToGenerate,"SectorsToGenerate",20,MRPF_NORMAL|MRPF_OPTIONAL)
     BEGIN_PROP_SUBMAP("Terrain")
		PROP(m_vTerrainHeightLayers,"HeightLayers")
		PROP(m_vTerrainColorLayers,"ColorLayers")
		PROP(m_sTerrainColorMap,"ColorMapFile")
		PROP(m_sTerrainBaseModel,"BaseModelFile")
		PROP(m_TerrainBaseModel,"BaseModel")
		PROP_FLAGS(m_TerrainSky,"Sky",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_TerrainWater,"Water",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_TerrainSun,"Sun",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_TerrainFog,"Fog",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_vTerrainAmbientColor,"AmbientColor",MRPF_NORMAL|MRPF_OPTIONAL)
      END_PROP_SUBMAP("Terrain")
    END_PROP_MAP();


    bool Init(std::string sClass,std::string sName,ISystem *piSystem);
    void Destroy();
	void Render(IGenericRender *piRender,IGenericCamera *piCamera);

   // IGameManager

    void Start();
    void Stop();
    void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);

	void CreateScenario();
	void LoadScenario(ISystemPersistencyNode *piNode);
	void SaveScenario(ISystemPersistencyNode *piNode);
	void CloseScenario();

	//IDesign

	void DesignRender(IGenericRender  *piRender);

	//ITerrainDesign

	// ITerrainDesign General settings

	bool SetTerrainBaseModel(std::string sModel);
	bool SetTerrainColorMap(std::string sColorMap);

	void GetTerrainBaseModel(std::string *psModel,IGenericModel **ppiModel);
	void GetTerrainColorMap(std::string *psColorMap,IGenericTexture **ppiTexture);

	void GetTerrainBBox(CVector *pvMins,CVector *pvMaxs);
	
	// ITerrainDesign Water

	bool SetTerrainWater(STerrainWater *pWater);
	void GetTerrainWater(STerrainWater *pWater,IGenericTexture **ppiTexture1,IGenericTexture **ppiTexture2);

	// ITerrainDesign Sky

	bool SetTerrainSky(STerrainSky *pSky);
	void GetTerrainSky(STerrainSky *pSky,IGenericTexture **ppiTexture);
	
	// ITerrainDesign Sun

	bool SetTerrainSun(STerrainSun *pSun);
	void GetTerrainSun(STerrainSun *pSun);

	// ITerrainDesign Fog

	bool SetTerrainFog(STerrainFog *pFog);
	void GetTerrainFog(STerrainFog *pFog);

	// ITerrainDesign Ambient Color

	void	SetTerrainAmbientColor(const CVector &vAmbientColor);
	CVector GetTerrainAmbientColor();

	// ITerrainDesign Color layers

	unsigned long	AddTerrainColorLayer(STerrainColorLayer *pLayer);
	void			UpdateTerrainColorLayer(unsigned int nIndex,STerrainColorLayer *pLayer);
	unsigned int	MoveTerrainColorLayer(unsigned int nIndex,bool bUp);
	void			RemoveTerrainColorLayer(unsigned int nIndex);
	void			GetTerrainColorLayer(unsigned int nIndex,STerrainColorLayer *pLayer,IGenericTexture **ppiTexture,IGenericTexture **ppiNormalMap);
	unsigned int	GetTerrainColorLayers();

	// ITerrainDesign Height layers

	unsigned long	AddTerrainHeightLayer(STerrainHeightLayer *pLayer);
	void			UpdateTerrainHeightLayer(unsigned int nIndex,STerrainHeightLayer *pLayer);
	unsigned int	MoveTerrainHeightLayer(unsigned int nIndex,bool bUp);
	void			RemoveTerrainHeightLayer(unsigned int nIndex);
	void			GetTerrainHeightLayer(unsigned int nIndex,STerrainHeightLayer *pLayer,IGenericTexture **ppiTexture,IGenericTexture **ppiNormalMap);
	unsigned int	GetTerrainHeightLayers();

	bool			GetTerrainStats(double *pdVertexOverhead,double *pdFaceOverhead);
	bool			GetTerrainHeightAt( CVector vPos ,double *pdHeight);
	bool			GetTerrainTrace( CVector vPoint1 ,CVector vPoint2 ,CVector *pHitPos);
	
	bool UpdateTerrain();

	// IWorldManager
	void SetupRenderingEnvironment(IGenericRender *piRender);

	CTraceInfo GetTerrainTrace( CVector vPoint1 ,CVector vPoint2);

	// IEntityEvents
	
	void OnRemoved(IEntity *piEntity);
	void OnKilled(IEntity *piEntity);
	
    CWorldManager();
    virtual ~CWorldManager();
};
