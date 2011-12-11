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
#include "GameRunTimeLib.h"
#include "WorldManager.h"

CWorldManager::CWorldManager()
{
	m_pWorldEntity=NULL;
	m_pTerrainBSP=NULL;
	m_nWaterRenderBuffer=-1;
	m_nSectorsToGenerate=20;
}

CWorldManager::~CWorldManager()
{
}

bool CWorldManager::Init(std::string sClass,std::string sName,ISystem *piSystem)
{
	bool bOk=CSystemObjectBase::Init(sClass,sName,piSystem);
	if(bOk){bOk=m_EntityManagerWrapper.Attach("GameSystem","EntityManager");}
	if(bOk){bOk=m_GameControllerWrapper.Attach("GameSystem","GameController");}
	if(bOk){bOk=m_PlayAreaManagerWrapper.Attach("GameSystem","PlayAreaManager");}
	if(bOk){bOk=m_FrameManagerWrapper.Attach("GameSystem","FrameManager");}
	if(bOk){m_GameControllerWrapper.m_piGameController->RegisterManager(100,this);}
	return bOk;
}

void CWorldManager::Destroy()
{
	CloseScenario();
	
	if(m_GameControllerWrapper.m_piGameController)
	{
		m_GameControllerWrapper.m_piGameController->UnregisterManager(this);
	}
	m_GameControllerWrapper.Detach();
	m_PlayAreaManagerWrapper.Detach();
	m_EntityManagerWrapper.Detach();
	CSystemObjectBase::Destroy();
}

void CWorldManager::CreateScenario()
{
	CloseScenario();
	m_pTerrainBSP=NULL;
	m_WaterModel.Create("GameResources","Model","");
	UpdateTerrain();
}

void CWorldManager::LoadScenario(ISystemPersistencyNode *piNode)
{
	CloseScenario();
	m_WaterModel.Create("GameResources","Model","");
	PersistencyLoad(piNode->GetNode("Terrain"),"Terrain");
	
	if(m_sTerrainColorMap.length())
	{
		bool bOk=m_TerrainColorMap.Create("GameResources","Texture","");
		if(bOk){bOk=m_TerrainColorMap.m_piTexture->Load(m_sTerrainColorMap,NULL,NULL,1.0,true,true);}
		if(!bOk){m_TerrainColorMap.Destroy();}
	}
	UpdateTerrain();
}

void CWorldManager::SaveScenario(ISystemPersistencyNode *piNode)
{
	PersistencySave(piNode->AddNode("Terrain"),"Terrain");
}

void CWorldManager::CloseScenario()
{
	m_TerrainBaseModel.Destroy();
	m_TerrainColorMap.Destroy();
	m_vTerrainHeightLayers.clear();
	m_vTerrainColorLayers.clear();
	m_TerrainSectors.clear();
	m_TerrainSectorsWaterFloors.clear();
	m_WaterModel.Destroy();
	m_TerrainWater.m_Texture1.Destroy();
	m_TerrainWater.m_Texture2.Destroy();
	m_TerrainSky.m_Texture.Destroy();
	m_TerrainFog=STerrainFog();
	m_TerrainWater.m_Config=STerrainWater();
	m_TerrainSky.m_Config=STerrainSky();
	m_TerrainSun=STerrainSun();
	m_vTerrainAmbientColor=CVector(0,0,0);
	m_sTerrainBaseModel="";
	m_sTerrainColorMap="";
	m_pTerrainBSP=NULL;
}

void CWorldManager::Start()
{
	m_pWorldEntity=new CWorldEntity(this);
	SUBSCRIBE_TO_CAST(m_pWorldEntity,IEntityEvents);
}

void CWorldManager::Stop()
{
	if(m_pWorldEntity)
	{
		m_pWorldEntity->Remove();
		m_pWorldEntity=NULL;
	}
}
void CWorldManager::OnRemoved(IEntity *piEntity){if(m_pWorldEntity==piEntity){m_pWorldEntity=NULL;}}
void CWorldManager::OnKilled(IEntity *piEntity){}

void CWorldManager::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
}


void CWorldManager::Render(IGenericRender *piRender,IGenericCamera *piCurrentCamera,bool bAllSectors, bool bWaterFloor)
{
	double dWaterOffset=0;
	if(m_WaterModel.m_piModel && m_nWaterRenderBuffer!=-1 && m_TerrainWater.m_Config.dSpeed!=0)
	{
		CMatrix m;
		dWaterOffset=m_TerrainWater.m_Config.dSpeed*((double)m_FrameManagerWrapper.m_piFrameManager->GetCurrentTime()/1000.0);
		m.T(CVector(dWaterOffset,0,0));
		m_WaterModel.m_piModel->SetRenderBufferTextureMatrix(0,0,m_nWaterRenderBuffer,0,&m);
	}
	
	if(m_TerrainSectors.size()==0)
	{
		if(m_TerrainBaseModel.m_piModel)
		{
			piRender->PushState();
			piRender->DeactivateTextures();
			piRender->DeactivateLighting();
			piRender->DeactivateShadowEmission();
			piRender->ActivateShadowReception();
			piRender->ActivateBlending();
			piRender->SetDepthFunction(eDepthFunction_LessOrEqual);
			piRender->RenderModel(Origin,Origin,m_TerrainBaseModel.m_piModel);
			piRender->PopState();
		}
	}
	else
	{
		for(unsigned int s=0;s<m_TerrainSectors.size();s++)
		{
			IGenericModel *piModel=m_TerrainSectors[s].m_piModel;
			bool bRenderSector=true;
			if(!bAllSectors && m_PlayAreaManagerWrapper.m_piPlayAreaManager)
			{
				CVector vMins,vMaxs;
				piModel->GetFrameBBox(0,0,&vMins,&vMaxs);
				CVector vCenter=(vMins+vMaxs)*0.5;
				CVector vSize=vMaxs-vMins;
				CVector vPos=CVector(vCenter.c[0],vMins.c[1],vCenter.c[2]);
				bRenderSector=m_PlayAreaManagerWrapper.m_piPlayAreaManager->IsVisible(vPos,vSize.c[0]*0.5);
			}
			if(bRenderSector)
			{
				piRender->PushState();
				piRender->DeactivateShadowEmission();
				piRender->ActivateShadowReception();
				piRender->ActivateLighting();
				piRender->ActivateBlending();
				piRender->SetDepthFunction(eDepthFunction_LessOrEqual);
				piRender->RenderModel(Origin,Origin,piModel);
				piRender->PopState();
			}
		}
		if(bWaterFloor)
		{
			for(unsigned int s=0;s<m_TerrainSectorsWaterFloors.size();s++)
			{
				IGenericModel *piModel=m_TerrainSectorsWaterFloors[s].m_piModel;
				bool bRenderSector=true;
				if(!bAllSectors && m_PlayAreaManagerWrapper.m_piPlayAreaManager)
				{
					CVector vMins,vMaxs;
					piModel->GetFrameBBox(0,0,&vMins,&vMaxs);
					CVector vCenter=(vMins+vMaxs)*0.5;
					CVector vSize=vMaxs-vMins;
					CVector vPos=CVector(vCenter.c[0],vMins.c[1],vCenter.c[2]);
					bRenderSector=m_PlayAreaManagerWrapper.m_piPlayAreaManager->IsVisible(vPos,vSize.c[0]*0.5);
				}
				if(bRenderSector)
				{
					piRender->PushState();
					piRender->DeactivateShadowEmission();
					piRender->ActivateShadowReception();
					piRender->ActivateLighting();
					piRender->ActivateBlending();
					piRender->SetDepthFunction(eDepthFunction_LessOrEqual);
					piRender->RenderModel(Origin,Origin,piModel);
					piRender->PopState();
				}
			}
		}
	}
	
	if(m_WaterModel.m_piModel && m_TerrainWater.m_Config.bEnabled)
	{
		bool bRenderWater=true;
		if(!bAllSectors && m_PlayAreaManagerWrapper.m_piPlayAreaManager)
		{
			CVector vMins,vMaxs;
			m_WaterModel.m_piModel->GetFrameBBox(0,0,&vMins,&vMaxs);
			CVector vCenter=(vMins+vMaxs)*0.5;
			CVector vSize=vMaxs-vMins;
			CVector vPos=CVector(vCenter.c[0],vMins.c[1],vCenter.c[2]);
			bRenderWater=m_PlayAreaManagerWrapper.m_piPlayAreaManager->IsVisible(vPos,vSize.c[0]*0.5);
		}
		if(bRenderWater)
		{
			piRender->PushState();
			piRender->SetWaterMappingSize(m_TerrainWater.m_Config.dHorizontalResolution,m_TerrainWater.m_Config.dVerticalResolution);
			piRender->SetWaterMappingOffset(dWaterOffset,0);
			piRender->DeactivateShadowEmission();
			piRender->ActivateShadowReception();
			if(piRender->GetShadingModel()==eShadingModel_Fragment)
			{
				piRender->ActivateLighting();
			}
			else
			{
				piRender->DeactivateLighting();
			}
			piRender->ActivateBlending();
			piRender->ActivateWater();
			piRender->SetDepthFunction(eDepthFunction_LessOrEqual);
			piRender->SetBlendingLayer(1);
			piRender->RenderModel(Origin,Origin,m_WaterModel.m_piModel);
			piRender->PopState();
		}
	}
}
void CWorldManager::Render(IGenericRender *piRender,IGenericCamera *piCurrentCamera)
{
	bool bRenderWaterFloor=true;
	
	if(m_WaterModel.m_piModel && m_TerrainFog.bEnabled && piCurrentCamera->GetForwardVector()==AxisNegY)
	{
		bRenderWaterFloor=false;
		piRender->Clear(m_TerrainFog.vColor);
	}

	Render(piRender,piCurrentCamera,false,bRenderWaterFloor);
}

void CWorldManager::DesignRender(IGenericRender *piRender)
{
	piRender->Clear(ColorBlack);
	Render(piRender,NULL,true,true);
}

bool CWorldManager::SetTerrainBaseModel( std::string sModel )
{
	bool bOk=false;
	m_sTerrainBaseModel=sModel;
	bOk=m_TerrainBaseModel.Create("GameResources","Model","");
	if(bOk){m_TerrainBaseModel.m_piModel->SetBSPOptions(true);}
	if(bOk){bOk=m_TerrainBaseModel.m_piModel->Load(sModel);}
	return bOk;
}

bool CWorldManager::SetTerrainColorMap( std::string sColorMap )
{
	bool bOk=false;
	m_sTerrainColorMap=sColorMap;
	bOk=m_TerrainColorMap.Create("GameResources","Texture","");
	if(bOk){bOk=m_TerrainColorMap.m_piTexture->Load(sColorMap,NULL,NULL,1.0,true,true);}
	return bOk;
}

void CWorldManager::GetTerrainBaseModel( std::string *psModel,IGenericModel **ppiModel )
{
	if(psModel){*psModel=m_sTerrainBaseModel;}
	if(ppiModel){*ppiModel=ADD(m_TerrainBaseModel.m_piModel);}
}

void CWorldManager::GetTerrainColorMap( std::string *psColorMap,IGenericTexture **ppiTexture )
{
	if(psColorMap){*psColorMap=m_sTerrainColorMap;}
	if(ppiTexture){*ppiTexture=ADD(m_TerrainColorMap.m_piTexture);}
}

void CWorldManager::GetTerrainBBox(CVector *pvMins,CVector *pvMaxs)
{
	if(m_TerrainBaseModel.m_piModel){m_TerrainBaseModel.m_piModel->GetFrameBBox(0,0,pvMins,pvMaxs);}
}

// Color Layers

unsigned long CWorldManager::AddTerrainColorLayer( STerrainColorLayer *pLayer )
{
	int nIndex=m_vTerrainColorLayers.size();
	STerrainColorLayerData data;
	m_vTerrainColorLayers.push_back(data);

	STerrainColorLayerData *pData=&m_vTerrainColorLayers[nIndex];
	pData->m_LayerConfig=*pLayer;
	pData->m_Texture.Create("GameResources","Texture","");
	if(pData->m_Texture.m_piTexture)
	{
		pData->m_Texture.m_piTexture->Load(pLayer->sTextureFile);
	}
	if(pData->m_LayerConfig.sNormalMapFile!="")
	{
		pData->m_NormalMap.Create("GameResources","Texture","");
		if(pData->m_NormalMap.m_piTexture)
		{
			pData->m_NormalMap.m_piTexture->Load(pLayer->sNormalMapFile);
		}
	}
	return nIndex;
}

void CWorldManager::UpdateTerrainColorLayer( unsigned int nIndex,STerrainColorLayer *pLayer )
{
	if(nIndex>=m_vTerrainColorLayers.size()){return;}
	STerrainColorLayerData *pData=&m_vTerrainColorLayers[nIndex];
	pData->m_LayerConfig=*pLayer;
	pData->m_Texture.m_piTexture->Load(pLayer->sTextureFile);
	if(pData->m_LayerConfig.sNormalMapFile=="")
	{
		pData->m_NormalMap.Detach();
	}
	else 
	{
		if(pData->m_NormalMap.m_piTexture==NULL){pData->m_NormalMap.Create("GameResources","Texture","");}
		if(pData->m_NormalMap.m_piTexture)
		{
			pData->m_NormalMap.m_piTexture->Load(pLayer->sNormalMapFile);
		}
	}
}

unsigned int CWorldManager::MoveTerrainColorLayer( unsigned int nIndex,bool bUp )
{
	if(bUp)
	{
		if(nIndex<(m_vTerrainColorLayers.size()-1))
		{
			STerrainColorLayerData temp;
			temp=m_vTerrainColorLayers[nIndex];
			m_vTerrainColorLayers[nIndex]=m_vTerrainColorLayers[nIndex+1];
			m_vTerrainColorLayers[nIndex+1]=temp;
			return nIndex+1;
		}
		else
		{
			return nIndex;
		}
	}
	else 
	{
		if(nIndex>0)
		{
			STerrainColorLayerData temp;
			temp=m_vTerrainColorLayers[nIndex];
			m_vTerrainColorLayers[nIndex]=m_vTerrainColorLayers[nIndex-1];
			m_vTerrainColorLayers[nIndex-1]=temp;
			return nIndex-1;
		}
		else
		{
			return nIndex;
		}
	}
}

void CWorldManager::RemoveTerrainColorLayer( unsigned int nIndex )
{
	std::vector<STerrainColorLayerData>::iterator i;
	unsigned int x;
	for(x=0,i=m_vTerrainColorLayers.begin();i!=m_vTerrainColorLayers.end();i++,x++)
	{
		if(x==nIndex)
		{
			m_vTerrainColorLayers.erase(i);
			break;
		}
	}
}

void CWorldManager::GetTerrainColorLayer( unsigned int nIndex,STerrainColorLayer *pLayer,IGenericTexture **ppiTexture,IGenericTexture **ppiNormalMap)
{
	if(ppiTexture){*ppiTexture=NULL;}

	if(nIndex>=m_vTerrainColorLayers.size()){return;}
	STerrainColorLayerData *pData=&m_vTerrainColorLayers[nIndex];
	if(pLayer){*pLayer=pData->m_LayerConfig;}
	if(ppiTexture){*ppiTexture=ADD(pData->m_Texture.m_piTexture);}
	if(ppiNormalMap){*ppiNormalMap=ADD(pData->m_NormalMap.m_piTexture);}
}

unsigned int CWorldManager::GetTerrainColorLayers()
{
	return m_vTerrainColorLayers.size();
}

// Height Layers

unsigned long CWorldManager::AddTerrainHeightLayer( STerrainHeightLayer *pLayer )
{
	int nIndex=m_vTerrainHeightLayers.size();
	STerrainHeightLayerData data;
	m_vTerrainHeightLayers.push_back(data);

	STerrainHeightLayerData *pData=&m_vTerrainHeightLayers[nIndex];
	pData->m_LayerConfig=*pLayer;
	pData->m_Texture.Create("GameResources","Texture","");
	if(pData->m_Texture.m_piTexture)
	{
		pData->m_Texture.m_piTexture->Load(pLayer->sTextureFile);
	}
	if(pData->m_LayerConfig.sNormalMapFile!="")
	{
		pData->m_NormalMap.Create("GameResources","Texture","");
		if(pData->m_NormalMap.m_piTexture)
		{
			pData->m_NormalMap.m_piTexture->Load(pLayer->sNormalMapFile);
		}
	}
	return nIndex;
}

void CWorldManager::UpdateTerrainHeightLayer( unsigned int nIndex,STerrainHeightLayer *pLayer )
{
	if(nIndex>=m_vTerrainHeightLayers.size()){return;}
	STerrainHeightLayerData *pData=&m_vTerrainHeightLayers[nIndex];
	pData->m_LayerConfig=*pLayer;
	pData->m_Texture.m_piTexture->Load(pLayer->sTextureFile);
	if(pData->m_LayerConfig.sNormalMapFile=="")
	{
		pData->m_NormalMap.Detach();
	}
	else 
	{
		if(pData->m_NormalMap.m_piTexture==NULL){pData->m_NormalMap.Create("GameResources","Texture","");}
		if(pData->m_NormalMap.m_piTexture)
		{
			pData->m_NormalMap.m_piTexture->Load(pLayer->sNormalMapFile);
		}
	}
}

unsigned int CWorldManager::MoveTerrainHeightLayer( unsigned int nIndex,bool bUp )
{
	if(bUp)
	{
		if(nIndex<(m_vTerrainHeightLayers.size()-1))
		{
			double dMinHeight=m_vTerrainHeightLayers[nIndex].m_LayerConfig.dMinHeight;
			double dMaxHeight=m_vTerrainHeightLayers[nIndex].m_LayerConfig.dMaxHeight;
			m_vTerrainHeightLayers[nIndex].m_LayerConfig.dMinHeight=m_vTerrainHeightLayers[nIndex+1].m_LayerConfig.dMinHeight;
			m_vTerrainHeightLayers[nIndex].m_LayerConfig.dMaxHeight=m_vTerrainHeightLayers[nIndex+1].m_LayerConfig.dMaxHeight;
			m_vTerrainHeightLayers[nIndex+1].m_LayerConfig.dMinHeight=dMinHeight;
			m_vTerrainHeightLayers[nIndex+1].m_LayerConfig.dMaxHeight=dMaxHeight;

			STerrainHeightLayerData temp;
			temp=m_vTerrainHeightLayers[nIndex];
			m_vTerrainHeightLayers[nIndex]=m_vTerrainHeightLayers[nIndex+1];
			m_vTerrainHeightLayers[nIndex+1]=temp;
			return nIndex+1;
		}
		else
		{
			return nIndex;
		}
	}
	else 
	{
		if(nIndex>0)
		{
			double dMinHeight=m_vTerrainHeightLayers[nIndex].m_LayerConfig.dMinHeight;
			double dMaxHeight=m_vTerrainHeightLayers[nIndex].m_LayerConfig.dMaxHeight;
			m_vTerrainHeightLayers[nIndex].m_LayerConfig.dMinHeight=m_vTerrainHeightLayers[nIndex-1].m_LayerConfig.dMinHeight;
			m_vTerrainHeightLayers[nIndex].m_LayerConfig.dMaxHeight=m_vTerrainHeightLayers[nIndex-1].m_LayerConfig.dMaxHeight;
			m_vTerrainHeightLayers[nIndex-1].m_LayerConfig.dMinHeight=dMinHeight;
			m_vTerrainHeightLayers[nIndex-1].m_LayerConfig.dMaxHeight=dMaxHeight;

			STerrainHeightLayerData temp;
			temp=m_vTerrainHeightLayers[nIndex];
			m_vTerrainHeightLayers[nIndex]=m_vTerrainHeightLayers[nIndex-1];
			m_vTerrainHeightLayers[nIndex-1]=temp;
			return nIndex-1;
		}
		else
		{
			return nIndex;
		}
	}
}

void CWorldManager::RemoveTerrainHeightLayer( unsigned int nIndex )
{
	std::vector<STerrainHeightLayerData>::iterator i;
	unsigned int x;
	for(x=0,i=m_vTerrainHeightLayers.begin();i!=m_vTerrainHeightLayers.end();i++,x++)
	{
		if(x==nIndex)
		{
			m_vTerrainHeightLayers.erase(i);
			break;
		}
	}
}

void CWorldManager::GetTerrainHeightLayer( unsigned int nIndex,STerrainHeightLayer *pLayer,IGenericTexture **ppiTexture,IGenericTexture **ppiNormalMap)
{
	if(ppiTexture){*ppiTexture=NULL;}

	if(nIndex>=m_vTerrainHeightLayers.size()){return;}
	STerrainHeightLayerData *pData=&m_vTerrainHeightLayers[nIndex];
	if(pLayer){*pLayer=pData->m_LayerConfig;}
	if(ppiTexture){*ppiTexture=ADD(pData->m_Texture.m_piTexture);}
	if(ppiNormalMap){*ppiNormalMap=ADD(pData->m_NormalMap.m_piTexture);}
}

unsigned int CWorldManager::GetTerrainHeightLayers()
{
	return m_vTerrainHeightLayers.size();
}


struct SVertexKey
{
	double dKey[9]; // 3 coordenadas + 2 textura + 3 normal + alpha
	
	bool operator<(const SVertexKey &vOther) const
	{
		for(int x=0;x<6;x++)
		{
			if(dKey[x]<vOther.dKey[x]){return true;}
			if(dKey[x]>vOther.dKey[x]){return false;}
		}
		return false;
	}
};

enum ETerrainTexturizationLayer
{
	eTerrainTexturizationLayer_None,
	eTerrainTexturizationLayer_Height,
	eTerrainTexturizationLayer_ColorMap,
	eTerrainTexturizationLayer_WaterFloor,
	eTerrainTexturizationLayer_MAX
};
struct STemporalRenderBuffer
{
	ETerrainTexturizationLayer eType;

	vector<float> vVertexArray; // por vertice
	vector<float> vNormalArray;// por vertice
	vector<float> vTexVertexArray;// por vertice
	vector<float> vColorMapTexVertexArray; // por vertice
	vector<float> vColorArray; // por vertice
	vector<unsigned int>	vTriangleVertexes; // 3 por cara

	map<SVertexKey,unsigned long> mUniqueVertexes;


	unsigned int    nTextureWidth;
	unsigned int    nTextureHeight;

	double			dHorizontalResolution;
	double			dVerticalResolution;

	// para eTerrainTexturizationLayer_Height
	double			dMinAbsHeight;
	double			dMaxAbsHeight;
	double			dMinDecayAbsHeight;
	double			dMaxDecayAbsHeight;
	// para eTerrainTexturizationLayer_Colormap

	CVector			vColor;
	double			dColorSaturationMargin;
	double			dColorValueMargin;
	bool			bColorAbruptEnd;
	
	IGenericTexture *piTexture;
	IGenericTexture *piNormalMap;
	
	STemporalRenderBuffer()
	{
		eType=eTerrainTexturizationLayer_None;
		nTextureWidth=0;
		nTextureHeight=0;
		dHorizontalResolution=0;
		dVerticalResolution=0;
		dMinAbsHeight=0;
		dMaxAbsHeight=0;
		dMinDecayAbsHeight=0;
		dMaxDecayAbsHeight=0;
		dColorSaturationMargin=0;
		dColorValueMargin=0;
		bColorAbruptEnd=0;
		piTexture=NULL;
		piNormalMap=NULL;
	}
};

bool CWorldManager::UpdateTerrain()
{
	if(m_TerrainBaseModel.m_piModel==NULL){return false;}

	m_pTerrainBSP=m_TerrainBaseModel.m_piModel->GetBSP();

	
	bool bOk=true;

	unsigned int nColorMapWidth=0,nColorMapHeight=0;
	CVector vBaseModelMins,vBaseModelMaxs;
	CVector vModelSize=m_TerrainBaseModel.m_piModel->GetFrameSize(0,0);
	m_TerrainBaseModel.m_piModel->GetFrameBBox(0,0,&vBaseModelMins,&vBaseModelMaxs);
	if(m_TerrainColorMap.m_piTexture){m_TerrainColorMap.m_piTexture->GetSize(&nColorMapWidth,&nColorMapHeight);}

	double dSectorSize=vModelSize.c[0]/(double)m_nSectorsToGenerate;
	
	m_TerrainSectors.clear();
	m_TerrainSectorsWaterFloors.clear();
	
	std::vector<STemporalRenderBuffer> *pvTempBuffers=new std::vector<STemporalRenderBuffer>[m_nSectorsToGenerate];
	
	// Load height layers 

	for(unsigned long s=0;bOk && s<m_nSectorsToGenerate;s++)
	{
		for(unsigned long x=0;bOk && x<m_vTerrainHeightLayers.size();x++)
		{
			STemporalRenderBuffer buffer;
			buffer.eType=eTerrainTexturizationLayer_Height;
			buffer.dHorizontalResolution=m_vTerrainHeightLayers[x].m_LayerConfig.dHorizontalResolution;
			buffer.dVerticalResolution=m_vTerrainHeightLayers[x].m_LayerConfig.dVerticalResolution;
			buffer.dMinAbsHeight=vModelSize.c[1]*m_vTerrainHeightLayers[x].m_LayerConfig.dMinHeight+vBaseModelMins.c[1];
			buffer.dMaxAbsHeight=vModelSize.c[1]*m_vTerrainHeightLayers[x].m_LayerConfig.dMaxHeight+vBaseModelMins.c[1];
			buffer.dMinDecayAbsHeight=buffer.dMinAbsHeight-vModelSize.c[1]*(m_vTerrainHeightLayers[x].m_LayerConfig.dDecayMargin);
			buffer.dMaxDecayAbsHeight=buffer.dMaxAbsHeight+vModelSize.c[1]*(m_vTerrainHeightLayers[x].m_LayerConfig.dDecayMargin);
			buffer.nTextureWidth=0;
			buffer.nTextureHeight=0;
			buffer.piTexture=ADD(m_vTerrainHeightLayers[x].m_Texture.m_piTexture);
			buffer.piNormalMap=ADD(m_vTerrainHeightLayers[x].m_NormalMap.m_piTexture);
			if(m_vTerrainHeightLayers[x].m_Texture.m_piTexture)
			{
				m_vTerrainHeightLayers[x].m_Texture.m_piTexture->GetSize(&buffer.nTextureWidth,&buffer.nTextureHeight);
			}
			pvTempBuffers[s].push_back(buffer);
		}

		// Load color layers 
		// Las capas de color tienen mayor prioridad 

		for(unsigned long x=0;bOk && x<m_vTerrainColorLayers.size();x++)
		{
			STemporalRenderBuffer buffer;
			buffer.eType=eTerrainTexturizationLayer_ColorMap;
			buffer.dHorizontalResolution=m_vTerrainColorLayers[x].m_LayerConfig.dHorizontalResolution;
			buffer.dVerticalResolution=m_vTerrainColorLayers[x].m_LayerConfig.dVerticalResolution;
			buffer.vColor=m_vTerrainColorLayers[x].m_LayerConfig.vColor;
			buffer.dColorSaturationMargin=m_vTerrainColorLayers[x].m_LayerConfig.dColorSaturationMargin;
			buffer.dColorValueMargin=m_vTerrainColorLayers[x].m_LayerConfig.dColorValueMargin;
			buffer.bColorAbruptEnd=m_vTerrainColorLayers[x].m_LayerConfig.bAbruptEnd;
			buffer.nTextureWidth=0;
			buffer.nTextureHeight=0;
			buffer.piTexture=ADD(m_vTerrainColorLayers[x].m_Texture.m_piTexture);
			buffer.piNormalMap=ADD(m_vTerrainColorLayers[x].m_NormalMap.m_piTexture);
			
			if(m_vTerrainColorLayers[x].m_Texture.m_piTexture)
			{
				m_vTerrainColorLayers[x].m_Texture.m_piTexture->GetSize(&buffer.nTextureWidth,&buffer.nTextureHeight);
			}
			pvTempBuffers[s].push_back(buffer);
		}
		if(m_WaterModel.m_piModel && m_TerrainFog.bEnabled)
		{
			STemporalRenderBuffer buffer;
			buffer.eType=eTerrainTexturizationLayer_WaterFloor;
			pvTempBuffers[s].push_back(buffer);
		}
	}
	
	// Load color layers 
	// Las capas de color tienen mayor prioridad 

	if(!bOk)
	{
		return NULL;
	}
	
	CVector			vMaxs=vBaseModelMaxs;
	CVector			vMins=vBaseModelMins;
	CVector			vSize=vMaxs-vMins;

	int nDestBufs=0;
	int pnDestBuf[1024]={0,0,0,0};

	for(unsigned int nBuffer=0;nBuffer<m_TerrainBaseModel.m_piModel->GetFrameRenderBuffers(0,0);nBuffer++)
	{
		unsigned int *pBaseModelFaces=NULL,*pBaseModelFaceCursor=NULL;
		unsigned long nBaseModelFaces=0,nBaseModelVertexes=0;
		float *pBaseModelVertexes=NULL,*pBaseModelColor=NULL,*pBaseModelNormals=NULL;
		m_TerrainBaseModel.m_piModel->GetRenderBufferVertexes(0,0,nBuffer,&nBaseModelVertexes,&pBaseModelVertexes);
		m_TerrainBaseModel.m_piModel->GetRenderBufferFaces(0,0,nBuffer,&nBaseModelFaces,&pBaseModelFaces);
		m_TerrainBaseModel.m_piModel->GetRenderBufferColors(0,0,nBuffer,&pBaseModelColor);
		m_TerrainBaseModel.m_piModel->GetRenderBufferNormals(0,0,nBuffer,&pBaseModelNormals);
		pBaseModelFaceCursor=pBaseModelFaces;

		for(unsigned int nTriangle=0;nTriangle<nBaseModelFaces;nTriangle++)
		{
			// Lectura de la informacion de la cara
			// 
			CVector vVertexes[3];
			CVector vNormals[3];
			CVector vColorMapTexture[3];
			CVector vColors[3];
			double dColorAlpha[3]={0,0,0};

			for(unsigned long nVertex=0;nVertex<3;nVertex++,pBaseModelFaceCursor++)
			{
				unsigned int nVertexIndex=*pBaseModelFaceCursor;
				float *pVertexCursor=&pBaseModelVertexes[nVertexIndex*3];
				float *pColorCursor=&pBaseModelColor[nVertexIndex*4];
				float *pNormalCursor=&pBaseModelNormals[nVertexIndex*3];

				for(unsigned long nCoord=0;nCoord<3;nCoord++)
				{
					vVertexes[nVertex].c[nCoord]=*pVertexCursor++;
					vColors[nVertex].c[nCoord]=*pColorCursor++;
					vNormals[nVertex].c[nCoord]=*pNormalCursor++;
				}
				dColorAlpha[nVertex]=*pColorCursor++;

				// deducir la coordenada del bitmap a usar

				CVector vRelativeVertex=(vVertexes[nVertex]-vMins);
				vRelativeVertex.c[0]/=vSize.c[0];
				vRelativeVertex.c[1]/=vSize.c[1];
				vRelativeVertex.c[2]/=vSize.c[2];
				vColorMapTexture[nVertex].c[0]=vRelativeVertex.c[0];
				vColorMapTexture[nVertex].c[1]=1.0-vRelativeVertex.c[2];
			}

			// Determine the destination sector based on the x coordinates of the face
			double dMinX=std::min(vVertexes[0].c[0],std::min(vVertexes[1].c[0],vVertexes[2].c[0]));
			int nDestSector=(dMinX-vMins.c[0])/dSectorSize;
			if(nDestSector>=(int)m_nSectorsToGenerate){nDestSector=m_nSectorsToGenerate-1;}
			if(nDestSector<0){nDestSector=0;}

			CVector vTexture[3];

			// Deduccion de las texturas a utilizar
			nDestBufs=0;

			for(int x=pvTempBuffers[nDestSector].size()-1;x>=0;x--)
			{
				if(pvTempBuffers[nDestSector][x].eType==eTerrainTexturizationLayer_WaterFloor)
				{
					if( vVertexes[0].c[1]<=m_TerrainFog.vMins.c[1]
						&& vVertexes[1].c[1]<=m_TerrainFog.vMins.c[1]
						&& vVertexes[2].c[1]<=m_TerrainFog.vMins.c[1])
					{
						vColors[0]=vColors[1]=vColors[2]=m_TerrainFog.vColor;
						pnDestBuf[nDestBufs]=x;
						nDestBufs++;
						break;
					}
				}
				if(pvTempBuffers[nDestSector][x].eType==eTerrainTexturizationLayer_Height)
				{
					for(unsigned long nVertex=0;nVertex<3;nVertex++)
					{
						if(vVertexes[nVertex].c[1]>=pvTempBuffers[nDestSector][x].dMinDecayAbsHeight && 
							vVertexes[nVertex].c[1]<=pvTempBuffers[nDestSector][x].dMaxDecayAbsHeight)
						{
							pnDestBuf[nDestBufs]=x;
							nDestBufs++;
							break;
						}
					}
				}
				else if(pvTempBuffers[nDestSector][x].eType==eTerrainTexturizationLayer_ColorMap && m_TerrainColorMap.m_piTexture)
				{
					for(unsigned long nVertex=0;nVertex<3;nVertex++)
					{
						CVector vPixel;
						vPixel.c[0]=(vColorMapTexture[nVertex].c[0]*((double)nColorMapWidth-1));
						vPixel.c[1]=(vColorMapTexture[nVertex].c[1]*((double)nColorMapHeight-1));

						CVector vColor=m_TerrainColorMap.m_piTexture->GetPixelColor((unsigned long)vPixel.c[0],(unsigned long)vPixel.c[1]);

						CVector vYUVDiffence=HSVDifference(pvTempBuffers[nDestSector][x].vColor,vColor);
						if( fabs(vYUVDiffence.c[1])<=pvTempBuffers[nDestSector][x].dColorSaturationMargin&& 
							fabs(vYUVDiffence.c[0])<=pvTempBuffers[nDestSector][x].dColorValueMargin)
						{
							pnDestBuf[nDestBufs]=x;
							nDestBufs++;
							break;
						}
					}
				}
			}

			bool bOpaqueVertexes[3]={0};
			for (int nDestBuf=0;nDestBuf<nDestBufs;nDestBuf++)
			{
				int nDestinationBuffer=pnDestBuf[nDestBuf];
				STemporalRenderBuffer *pDestBuffer=&pvTempBuffers[nDestSector][nDestinationBuffer];
				// Generacion de coordenadas de texturas
				// y calculo del alpha

				for(unsigned long nVertex=0;nVertex<3;nVertex++)
				{
					if(pDestBuffer->eType==eTerrainTexturizationLayer_Height && pDestBuffer->nTextureWidth*pDestBuffer->nTextureHeight)
					{

						vTexture[nVertex].c[0]=vVertexes[nVertex].c[0]/((double)pDestBuffer->nTextureWidth/pDestBuffer->dHorizontalResolution);
						vTexture[nVertex].c[1]=vVertexes[nVertex].c[2]/((double)pDestBuffer->nTextureHeight/pDestBuffer->dVerticalResolution);

						if((vVertexes[nVertex].c[1]>=pDestBuffer->dMinAbsHeight && 
							vVertexes[nVertex].c[1]<=pDestBuffer->dMaxAbsHeight))
						{
							dColorAlpha[nVertex]=1.0;
						}
						else if(vVertexes[nVertex].c[1]<pDestBuffer->dMinAbsHeight)
						{
							dColorAlpha[nVertex]=1.0-(pDestBuffer->dMinAbsHeight-vVertexes[nVertex].c[1])/(pDestBuffer->dMinAbsHeight-pDestBuffer->dMinDecayAbsHeight);
						}
						else if(vVertexes[nVertex].c[1]>pDestBuffer->dMaxAbsHeight)
						{
							dColorAlpha[nVertex]=1.0-(vVertexes[nVertex].c[1]-pDestBuffer->dMaxAbsHeight)/(pDestBuffer->dMaxDecayAbsHeight-pDestBuffer->dMinAbsHeight);
						}
					}
					else if(pDestBuffer->eType==eTerrainTexturizationLayer_ColorMap && m_TerrainColorMap.m_piTexture && pDestBuffer->nTextureWidth*pDestBuffer->nTextureHeight)
					{
						// deducir la coordenada del bitmap a usar
						vTexture[nVertex].c[0]=vVertexes[nVertex].c[0]/((double)pDestBuffer->nTextureWidth/pDestBuffer->dHorizontalResolution);
						vTexture[nVertex].c[1]=vVertexes[nVertex].c[2]/((double)pDestBuffer->nTextureHeight/pDestBuffer->dVerticalResolution);

						if(pDestBuffer->bColorAbruptEnd)
						{
							dColorAlpha[nVertex]=1.0;
						}
						else
						{
							CVector vPixel;
							vPixel.c[0]=(vColorMapTexture[nVertex].c[0]*((double)nColorMapWidth-1));
							vPixel.c[1]=(vColorMapTexture[nVertex].c[1]*((double)nColorMapHeight-1));
							CVector vColor=m_TerrainColorMap.m_piTexture->GetPixelColor((unsigned long)vPixel.c[0],(unsigned long)vPixel.c[1]);
							CVector vYUVDiffence=HSVDifference(pDestBuffer->vColor,vColor);

							if(fabs(vYUVDiffence.c[1])<=pDestBuffer->dColorSaturationMargin*0.1 && 
								fabs(vYUVDiffence.c[0])<=pDestBuffer->dColorValueMargin)
							{
								dColorAlpha[nVertex]=1.0;
							}
							else if( fabs(vYUVDiffence.c[1])<=pDestBuffer->dColorSaturationMargin&& 
								fabs(vYUVDiffence.c[0])<=pDestBuffer->dColorValueMargin)
							{
								dColorAlpha[nVertex]=1.0-(pDestBuffer->dColorSaturationMargin?fabs(vYUVDiffence.c[1])/pDestBuffer->dColorSaturationMargin:0);
							}
							else 
							{
								dColorAlpha[nVertex]=0.0;
							}
						}
					}
				}
				// Se deduce si todos los vertices de la cara on opacos.
				// si lo son se dejaran de procesar el resto de capas.
				for(unsigned long nVertex=0;nVertex<3;nVertex++)
				{
					if(dColorAlpha[nVertex]>=1.0){bOpaqueVertexes[nVertex]=true;}
				}
				bool bAllOpaque=bOpaqueVertexes[0] && bOpaqueVertexes[1] && bOpaqueVertexes[2];
				
				// La ultima textura por orden de prioridad (es decir la inferior o textura base)
				// siempre se pinta opaca para evitar problemas de partes semi transparentes
				
				bool bLastTexture=(bAllOpaque || nDestBuf==nDestBufs-1);
				if(bLastTexture){dColorAlpha[0]=1;dColorAlpha[1]=1;dColorAlpha[2]=1;}
				
				// Inserccion en los  buffer de destino.
				
				for(unsigned long nVertex=0;nVertex<3;nVertex++)
				{
					SVertexKey key;
					key.dKey[0]=vVertexes[nVertex].c[0];
					key.dKey[1]=vVertexes[nVertex].c[1];
					key.dKey[2]=vVertexes[nVertex].c[2];
					key.dKey[3]=vNormals[nVertex].c[0];
					key.dKey[4]=vNormals[nVertex].c[1];
					key.dKey[5]=vNormals[nVertex].c[2];
					key.dKey[6]=vTexture[nVertex].c[0];
					key.dKey[7]=vTexture[nVertex].c[1];
					key.dKey[8]=dColorAlpha[nVertex];

					unsigned long nVertexIndex=0;

					map<SVertexKey,unsigned long>::iterator iVertex=pDestBuffer->mUniqueVertexes.find(key);
					if(iVertex==pDestBuffer->mUniqueVertexes.end())
					{
						nVertexIndex=pDestBuffer->mUniqueVertexes.size();
						pDestBuffer->mUniqueVertexes[key]=nVertexIndex;

						// por cada coordenada del vertice
						for(unsigned long nCoord=0;nCoord<2;nCoord++)
						{
							pDestBuffer->vTexVertexArray.push_back((float)vTexture[nVertex].c[nCoord]);
							pDestBuffer->vColorMapTexVertexArray.push_back((float)vColorMapTexture[nVertex].c[nCoord]);
						}
						for(unsigned long nCoord=0;nCoord<3;nCoord++)
						{
							pDestBuffer->vVertexArray.push_back((float)vVertexes[nVertex].c[nCoord]);
							pDestBuffer->vColorArray.push_back((float)vColors[nVertex].c[nCoord]);
							pDestBuffer->vNormalArray.push_back((float)vNormals[nVertex].c[nCoord]);
						}
						pDestBuffer->vColorArray.push_back((float)dColorAlpha[nVertex]);
					}
					else
					{
						nVertexIndex=iVertex->second;
					}
					pDestBuffer->vTriangleVertexes.push_back(nVertexIndex);
				}
				// Si todos los vertices son totalmente opacos no tiene sentido seguir procesando capas.
				if(bAllOpaque){break;}
			}
		}
	}

	for(unsigned long s=0;bOk && s<m_nSectorsToGenerate;s++)
	{
		CGenericModelWrapper model;
		bOk=model.Create("GameResources","Model","");
		if(bOk)
		{
			int nAnimation=model.m_piModel->AddAnimation();
			int nFrame=model.m_piModel->AddAnimationFrame(nAnimation);
			bool bAddModel=false;

			for(unsigned int b=0;b<pvTempBuffers[s].size();b++)
			{
				if(pvTempBuffers[s][b].eType==eTerrainTexturizationLayer_WaterFloor)
				{
					continue;
				}
				STemporalRenderBuffer *pBuffer=&pvTempBuffers[s][b];
				if(pBuffer->vVertexArray.size()==0)
				{
					continue;
				}
				bAddModel=true;
				
				unsigned long nFaces=pBuffer->vTriangleVertexes.size()/3;
				unsigned long nVertexes=pBuffer->vVertexArray.size()/3;
				unsigned long nBuffer=model.m_piModel->AddRenderBuffer(nAnimation,nFrame);
				
				model.m_piModel->SetRenderBufferTexture(nAnimation,nFrame,nBuffer,0,pBuffer->piTexture);
				model.m_piModel->SetRenderBufferMaterial(nAnimation,nFrame,nBuffer,CVector(1,1,1),CVector(1,1,1),CVector(0,0,0),0,1);
				model.m_piModel->SetRenderBufferNormalMap(nAnimation,nFrame,nBuffer,pBuffer->piNormalMap);
				model.m_piModel->SetRenderBufferVertexes(nAnimation,nFrame,nBuffer,nVertexes,BufferFromVector(&pBuffer->vVertexArray));
				model.m_piModel->SetRenderBufferFaces(nAnimation,nFrame,nBuffer,nFaces,BufferFromVector(&pBuffer->vTriangleVertexes));
				model.m_piModel->SetRenderBufferColors(nAnimation,nFrame,nBuffer,BufferFromVector(&pBuffer->vColorArray));
				model.m_piModel->SetRenderBufferNormals(nAnimation,nFrame,nBuffer,BufferFromVector(&pBuffer->vNormalArray));
				model.m_piModel->SetRenderBufferTextureCoords(nAnimation,nFrame,nBuffer,0,BufferFromVector(&pBuffer->vTexVertexArray));
				if(pBuffer->piNormalMap){model.m_piModel->SetRenderBufferNormalMapCoords(nAnimation,nFrame,nBuffer,BufferFromVector(&pBuffer->vTexVertexArray));}
			}
			if(bAddModel)
			{
				model.m_piModel->UpdateBufferObjects();
				m_TerrainSectors.push_back(model);
			}
		}
	}	
	
	for(unsigned long s=0;bOk && s<m_nSectorsToGenerate;s++)
	{
		CGenericModelWrapper model;
		bOk=model.Create("GameResources","Model","");
		if(bOk)
		{
			int nAnimation=model.m_piModel->AddAnimation();
			int nFrame=model.m_piModel->AddAnimationFrame(nAnimation);
			bool bAddModel=false;
			
			for(unsigned int b=0;b<pvTempBuffers[s].size();b++)
			{
				if(pvTempBuffers[s][b].eType!=eTerrainTexturizationLayer_WaterFloor)
				{
					continue;
				}
				STemporalRenderBuffer *pBuffer=&pvTempBuffers[s][b];
				if(pBuffer->vVertexArray.size()==0)
				{
					continue;
				}
				bAddModel=true;
				
				unsigned long nFaces=pBuffer->vTriangleVertexes.size()/3;
				unsigned long nVertexes=pBuffer->vVertexArray.size()/3;
				unsigned long nBuffer=model.m_piModel->AddRenderBuffer(nAnimation,nFrame);
				
				model.m_piModel->SetRenderBufferMaterial(nAnimation,nFrame,nBuffer,m_TerrainFog.vColor,m_TerrainFog.vColor,CVector(0,0,0),0,1);
				model.m_piModel->SetRenderBufferVertexes(nAnimation,nFrame,nBuffer,nVertexes,BufferFromVector(&pBuffer->vVertexArray));
				model.m_piModel->SetRenderBufferFaces(nAnimation,nFrame,nBuffer,nFaces,BufferFromVector(&pBuffer->vTriangleVertexes));
				model.m_piModel->SetRenderBufferColors(nAnimation,nFrame,nBuffer,BufferFromVector(&pBuffer->vColorArray));
			}
			if(bAddModel)
			{
				model.m_piModel->UpdateBufferObjects();
				m_TerrainSectorsWaterFloors.push_back(model);
			}
		}
	}	
	
	if(m_WaterModel.m_piModel)
	{
		m_WaterModel.m_piModel->RemoveAnimations();
		int nWaterAnimation=m_WaterModel.m_piModel->AddAnimation();
		int nWaterFrame=m_WaterModel.m_piModel->AddAnimationFrame(nWaterAnimation);
	
		m_nWaterRenderBuffer=-1;
		if(m_TerrainWater.m_Config.bEnabled)
		{
			if(m_TerrainWater.m_Texture1.m_piTexture)
			{
				m_nWaterRenderBuffer=m_WaterModel.m_piModel->AddRenderBuffer(nWaterAnimation,nWaterFrame);
				m_WaterModel.m_piModel->SetRenderBufferTexture(nWaterAnimation,nWaterFrame,m_nWaterRenderBuffer,0,m_TerrainWater.m_Texture1.m_piTexture);
				m_WaterModel.m_piModel->SetRenderBufferMaterial(nWaterAnimation,nWaterFrame,m_nWaterRenderBuffer,CVector(1,1,1),CVector(1,1,1),CVector(1,1,1),128,(float)m_TerrainWater.m_Config.dOpacity);

				float *pWaterVertexBuffer=new float[12];
				float *pCursor=pWaterVertexBuffer;
				
				*pCursor++=(float)m_TerrainWater.m_Config.vMins.c[0];
				*pCursor++=(float)m_TerrainWater.m_Config.vMaxs.c[1];
				*pCursor++=(float)m_TerrainWater.m_Config.vMins.c[2];
				
				*pCursor++=(float)m_TerrainWater.m_Config.vMins.c[0];
				*pCursor++=(float)m_TerrainWater.m_Config.vMaxs.c[1];
				*pCursor++=(float)m_TerrainWater.m_Config.vMaxs.c[2];
				
				*pCursor++=(float)m_TerrainWater.m_Config.vMaxs.c[0];
				*pCursor++=(float)m_TerrainWater.m_Config.vMaxs.c[1];
				*pCursor++=(float)m_TerrainWater.m_Config.vMaxs.c[2];
				
				*pCursor++=(float)m_TerrainWater.m_Config.vMaxs.c[0];
				*pCursor++=(float)m_TerrainWater.m_Config.vMaxs.c[1];
				*pCursor++=(float)m_TerrainWater.m_Config.vMins.c[2];
				
				m_WaterModel.m_piModel->SetRenderBufferVertexes(nWaterAnimation,nWaterFrame,m_nWaterRenderBuffer,4,pWaterVertexBuffer);

				float *pWaterNormalBuffer=new float[12];
				pCursor=pWaterNormalBuffer;
				for(int y=0;y<4;y++){*pCursor++=0;*pCursor++=1;*pCursor++=0;}
				m_WaterModel.m_piModel->SetRenderBufferNormals(nWaterAnimation,nWaterFrame,m_nWaterRenderBuffer,pWaterNormalBuffer);

				float *pWaterColorBuffer=new float[16];
				pCursor=pWaterColorBuffer;
				for(int y=0;y<4;y++){*pCursor++=1;*pCursor++=1;*pCursor++=1;*pCursor++=(float)m_TerrainWater.m_Config.dOpacity;}
				m_WaterModel.m_piModel->SetRenderBufferColors(nWaterAnimation,nWaterFrame,m_nWaterRenderBuffer,pWaterColorBuffer);

				float *pWaterTexBuffer=new float[8];
				pCursor=pWaterTexBuffer;
				*pCursor++=0;
				*pCursor++=0;

				*pCursor++=0;
				*pCursor++=(float)m_TerrainWater.m_Config.dVerticalResolution;

				*pCursor++=(float)m_TerrainWater.m_Config.dHorizontalResolution;
				*pCursor++=(float)m_TerrainWater.m_Config.dVerticalResolution;

				*pCursor++=(float)m_TerrainWater.m_Config.dHorizontalResolution;
				*pCursor++=0;
				m_WaterModel.m_piModel->SetRenderBufferTextureCoords(nWaterAnimation,nWaterFrame,m_nWaterRenderBuffer,0,pWaterTexBuffer);
		
				unsigned int *pWaterFaces=new unsigned int[6];
				pWaterFaces[0]=0;
				pWaterFaces[1]=1;
				pWaterFaces[2]=2;
				pWaterFaces[3]=0;
				pWaterFaces[4]=2;
				pWaterFaces[5]=3;
				m_WaterModel.m_piModel->SetRenderBufferFaces(nWaterAnimation,nWaterFrame,m_nWaterRenderBuffer,2,pWaterFaces);
			}
		}
		m_WaterModel.m_piModel->UpdateBufferObjects();
	}
	
	for(unsigned long s=0;s<m_nSectorsToGenerate;s++)
	{
		for(unsigned long b=0;b<pvTempBuffers[s].size();b++)
		{
			IGenericTexture *piTexture=pvTempBuffers[s][b].piTexture;
			IGenericTexture *piNormalMap=pvTempBuffers[s][b].piNormalMap;
			REL(piTexture);
			REL(piNormalMap);
		}
	}
		
	delete [] pvTempBuffers;
	pvTempBuffers=NULL;
	
	//m_TerrainModel.m_piModel->UpdateBufferObjects();
	return true;
}

bool CWorldManager::GetTerrainStats( double *pdVertexOverhead,double *pdFaceOverhead )
{
	if(pdVertexOverhead){*pdVertexOverhead=0;}
	if(pdFaceOverhead){*pdFaceOverhead=0;}

	if(m_TerrainBaseModel.m_piModel==NULL){return false;}
	if(m_TerrainSectors.size()==0){return false;}

	// Trace stats

	unsigned long nOriginalVertexes=0;
	unsigned long nOriginalFaces=0;
	unsigned long nNewVertexes=0;
	unsigned long nNewFaces=0;

	for(unsigned int x=0;x<m_TerrainBaseModel.m_piModel->GetFrameRenderBuffers(0,0);x++)
	{
		unsigned long nBufferVertexes=0,nBufferFaces=0;
		m_TerrainBaseModel.m_piModel->GetRenderBufferVertexes(0,0,x,&nBufferVertexes,NULL);
		m_TerrainBaseModel.m_piModel->GetRenderBufferFaces(0,0,x,&nBufferFaces,NULL);
		nOriginalVertexes+=nBufferVertexes;
		nOriginalFaces+=nBufferFaces;
	}
	for(unsigned int s=0;s<m_TerrainSectors.size();s++)
	{
		IGenericModel *piModel=m_TerrainSectors[s].m_piModel;
		for(unsigned int x=0;x<piModel->GetFrameRenderBuffers(0,0);x++)
		{
			unsigned long nBufferVertexes=0,nBufferFaces=0;
			piModel->GetRenderBufferVertexes(0,0,x,&nBufferVertexes,NULL);
			piModel->GetRenderBufferFaces(0,0,x,&nBufferFaces,NULL);
			nNewVertexes+=nBufferVertexes;
			nNewFaces+=nBufferFaces;
		}
	}
	if(pdVertexOverhead){*pdVertexOverhead=nOriginalVertexes?((double)nNewVertexes*100.0/(double)nOriginalVertexes)-100.0:0;}
	if(pdFaceOverhead){*pdFaceOverhead=nOriginalFaces?((double)nNewFaces*100.0/(double)nOriginalFaces)-100.0:0;}
	return true;
}

bool CWorldManager::GetTerrainHeightAt( CVector vPos ,double *pdHeight)
{
	if(pdHeight){*pdHeight=0;}
	if(m_pTerrainBSP==NULL){return false;}

	CVector vPoint1,vPoint2;
	vPoint1=vPos;
	vPoint2=vPos;
	vPoint1.c[1]=10000;
	vPoint2.c[1]=-10000;

	CTraceInfo info=m_pTerrainBSP->GetTrace(vPoint1,vPoint2,vPoint1,vPoint2);
	if(pdHeight){*pdHeight=info.m_vTracePos.c[1];}
	return info.m_bTraceHit;
}

bool CWorldManager::GetTerrainTrace( CVector vPoint1 ,CVector vPoint2 ,CVector *pHitPos )
{
	CTraceInfo info=GetTerrainTrace(vPoint1,vPoint2);
	if(pHitPos){*pHitPos=info.m_vTracePos;}
	return info.m_bTraceHit;
}

CTraceInfo CWorldManager::GetTerrainTrace( CVector vPoint1 ,CVector vPoint2)
{
	CTraceInfo info;
	info.m_vTracePos=vPoint2;
	info.m_dTraceFraction=1;
	info.m_bTraceHit=false;
	if(m_pTerrainBSP==NULL){return info;}
	
	info=m_pTerrainBSP->GetTrace(vPoint1,vPoint2,vPoint1,vPoint2);
	if(m_TerrainWater.m_Config.bEnabled && m_TerrainBaseModel.m_piModel)
	{
		CVector vMins,vMaxs;
		m_TerrainBaseModel.m_piModel->GetFrameBBox(0,0,&vMins,&vMaxs);
		//double dAbsWaterHeight=(vMaxs.c[1]-vMins.c[1])*m_TerrainWater.m_Config.dHeight+vMins.c[1];
		double dAbsWaterHeight=m_TerrainWater.m_Config.vMaxs.c[1];
		
		CPlane waterPlane(CVector(0,1,0),dAbsWaterHeight);
		double dSide1=waterPlane.GetSide(vPoint1);
		double dSide2=waterPlane.GetSide(vPoint2);
		if(dSide1*dSide2<=0)
		{
			double dLength=(dSide1-dSide2);
			double dFraction=dLength?dSide1/dLength:0;
			if(dFraction<info.m_dTraceFraction)
			{
				info.m_dTraceFraction=dFraction;
				info.m_vTracePos=vPoint1+(vPoint2-vPoint1)*dFraction;
				info.m_vTracePlane=waterPlane;
				info.m_bTraceHit=true;
				info.m_nTraceContent=CONTENT_SOLID;
			}
		}
	}
	return info;
}


bool CWorldManager::SetTerrainWater( STerrainWater *pWater )
{
	bool bOk=false;
	m_TerrainWater.m_Config=*pWater;
	bOk=m_TerrainWater.m_Texture1.Create("GameResources","Texture","");
	if(bOk){bOk=m_TerrainWater.m_Texture2.Create("GameResources","Texture","");}
	if(bOk){bOk=m_TerrainWater.m_Texture1.m_piTexture->Load(m_TerrainWater.m_Config.sTextureFile1);}
	if(bOk){bOk=m_TerrainWater.m_Texture2.m_piTexture->Load(m_TerrainWater.m_Config.sTextureFile2);}
	return bOk;
}

void CWorldManager::GetTerrainWater( STerrainWater *pWater,IGenericTexture **ppiTexture1,IGenericTexture **ppiTexture2 )
{
	if(pWater){*pWater=m_TerrainWater.m_Config;}
	if(ppiTexture1){*ppiTexture1=ADD(m_TerrainWater.m_Texture1.m_piTexture);}
	if(ppiTexture2){*ppiTexture2=ADD(m_TerrainWater.m_Texture2.m_piTexture);}
}

bool CWorldManager::SetTerrainSky( STerrainSky *pSky )
{
	bool bOk=false;
	m_TerrainSky.m_Config=*pSky;
	bOk=m_TerrainSky.m_Texture.Create("GameResources","Texture","");
	if(bOk){bOk=m_TerrainSky.m_Texture.m_piTexture->Load(m_TerrainSky.m_Config.sTextureFile);}
	return bOk;
}

void CWorldManager::GetTerrainSky( STerrainSky *pSky,IGenericTexture **ppiTexture )
{
	if(pSky){*pSky=m_TerrainSky.m_Config;}
	if(ppiTexture){*ppiTexture=ADD(m_TerrainSky.m_Texture.m_piTexture);}
}

void CWorldManager::GetTerrainSun( STerrainSun *pSun ){if(pSun){*pSun=m_TerrainSun;}}
bool CWorldManager::SetTerrainSun( STerrainSun *pSun ){m_TerrainSun=*pSun;return true;}
void CWorldManager::GetTerrainFog( STerrainFog *pFog ){if(pFog){*pFog=m_TerrainFog;}}
bool CWorldManager::SetTerrainFog( STerrainFog *pFog )
{
	bool bUpdate=(pFog->bEnabled!=m_TerrainFog.bEnabled || pFog->vMins!=m_TerrainFog.vMins|| pFog->vMaxs!=m_TerrainFog.vMaxs);
	m_TerrainFog=*pFog;
	if(bUpdate){UpdateTerrain();}
	return true;
}

void	CWorldManager::SetTerrainAmbientColor(const CVector &vAmbientColor){m_vTerrainAmbientColor=vAmbientColor;}
CVector CWorldManager::GetTerrainAmbientColor(){return m_vTerrainAmbientColor;}

void CWorldManager::SetupRenderingEnvironment( IGenericRender *piRender )
{
	if(m_TerrainFog.bEnabled)
	{
		piRender->ActivateHeightFog(m_TerrainFog.vMins,m_TerrainFog.vMaxs,m_TerrainFog.vColor);
	}
	else
	{
		piRender->DeactivateHeightFog();
	}
	
	if(m_TerrainBaseModel.m_piModel && m_TerrainSectors.size())
	{
		CVector vTerrainMaxs,vTerrainMins,vTerrainSize;

		m_TerrainBaseModel.m_piModel->GetFrameBBox(0,0,&vTerrainMins,&vTerrainMaxs);
		vTerrainSize=m_TerrainBaseModel.m_piModel->GetFrameSize(0,0);

		CVector vSunAngles(m_TerrainSun.dAzimuth,m_TerrainSun.dElevation,0);
		CVector vForward;
		CVector vBlack;
		VectorsFromAngles(vSunAngles,&vForward);
		double dTerrainMaxDimension=max(max(vTerrainSize.c[0],vTerrainSize.c[1]),vTerrainSize.c[2]);
		CVector vSunPos=vForward*(dTerrainMaxDimension*m_TerrainSun.dDistance);
		piRender->SetSunLight(vSunPos,vForward,vBlack,m_TerrainSun.vColor,m_TerrainSun.vColor);
	}
	piRender->SetAmbientLight(m_vTerrainAmbientColor);
	if(m_TerrainSky.m_Config.bEnabled && m_TerrainSky.m_Texture.m_piTexture)
	{
		piRender->ActivateSkyShadow();
		
		unsigned int nWidth=0,nHeight=0;
		m_TerrainSky.m_Texture.m_piTexture->GetSize(&nWidth,&nHeight);
		piRender->SelectSkyShadow(m_TerrainSky.m_Texture.m_piTexture);
		piRender->SetSkyShadowParameters(m_TerrainSky.m_Config.dSpeed,((double)nWidth)/m_TerrainSky.m_Config.dHorizontalResolution,((double)nHeight)/m_TerrainSky.m_Config.dVerticalResolution,m_TerrainSky.m_Config.dOpacity);
	}
	else
	{
		piRender->UnselectSkyShadow();
		piRender->DeactivateSkyShadow();
	}
}
