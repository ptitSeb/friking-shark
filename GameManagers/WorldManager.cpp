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
#include <GL/gl.h>

CWorldManager::CWorldManager()
{
	m_pWorldEntity=NULL;
	m_pTerrainBSP=NULL;
	m_pnWaterRenderBuffers[0]=-1;
	m_pnWaterRenderBuffers[1]=-1;
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
	m_TerrainModel.Create("GameResources","Model","");
	m_WaterModel.Create("GameResources","Model","");
	UpdateTerrain();
}

void CWorldManager::LoadScenario(ISystemPersistencyNode *piNode)
{
	CloseScenario();
	m_TerrainModel.Create("GameResources","Model","");
	m_WaterModel.Create("GameResources","Model","");
	PersistencyLoad(piNode->GetNode("Terrain"),"Terrain");
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
	m_TerrainModel.Destroy();
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


void CWorldManager::Render(IGenericRender *piRender,IGenericCamera *piCurrentCamera)
{
	double dWaterOffset=0;
	if(m_WaterModel.m_piModel && m_pnWaterRenderBuffers[0]!=-1  && m_pnWaterRenderBuffers[1]!=-1 && m_TerrainWater.m_Config.dSpeed!=0)
	{
		CMatrix m;
		dWaterOffset=m_TerrainWater.m_Config.dSpeed*((double)m_FrameManagerWrapper.m_piFrameManager->GetCurrentTime()/1000.0);
		m.T(CVector(dWaterOffset,0,0));
		m_WaterModel.m_piModel->SetRenderBufferTextureMatrix(0,0,m_pnWaterRenderBuffers[0],0,&m);
		m_WaterModel.m_piModel->SetRenderBufferTextureMatrix(0,0,m_pnWaterRenderBuffers[1],0,&m);
	}
	
	if(m_TerrainModel.m_piModel)
	{
		piRender->PushState();
		bool bUseBaseFrame=(m_TerrainBaseModel.m_piModel!=NULL && m_TerrainModel.m_piModel->GetFrameRenderBuffers(0,0)==0);
		if(bUseBaseFrame){piRender->DeactivateTextures();}
		piRender->DeactivateShadowEmission();
		piRender->ActivateLighting();
		piRender->ActivateBlending();
		piRender->SetDepthFunction(GL_LEQUAL);
		piRender->RenderModel(Origin,Origin,bUseBaseFrame?m_TerrainBaseModel.m_piModel:m_TerrainModel.m_piModel);
		piRender->PopState();
	}
	
	if(m_WaterModel.m_piModel)
	{
		piRender->PushState();
		piRender->SetWaterMappingSize(m_TerrainWater.m_Config.dHorizontalResolution,m_TerrainWater.m_Config.dVerticalResolution);
		piRender->SetWaterMappingOffset(dWaterOffset,0);
		piRender->DeactivateShadowEmission();
		piRender->ActivateLighting();
		piRender->ActivateBlending();
		piRender->ActivateWater();
		piRender->SetDepthFunction(GL_LEQUAL);
		piRender->SetBlendingLayer(1);
		piRender->RenderModel(Origin,Origin,m_WaterModel.m_piModel);
		piRender->PopState();
	}
}

void CWorldManager::DesignRender(IGenericRender *piRender)
{
	if(m_TerrainModel.m_piModel || m_WaterModel.m_piModel)
	{
		Render(piRender,NULL);
	}
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
	if(bOk){bOk=m_TerrainColorMap.m_piTexture->Load(sColorMap);}
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

void CWorldManager::GetTerrainModel( IGenericModel **ppiModel )
{
	bool bUseBaseFrame=(m_TerrainBaseModel.m_piModel!=NULL && m_TerrainModel.m_piModel->GetFrameRenderBuffers(0,0)==0);
	if(ppiModel){*ppiModel=ADD(bUseBaseFrame?m_TerrainBaseModel.m_piModel:m_TerrainModel.m_piModel);}
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
	return nIndex;
}

void CWorldManager::UpdateTerrainColorLayer( unsigned int nIndex,STerrainColorLayer *pLayer )
{
	if(nIndex>=m_vTerrainColorLayers.size()){return;}
	STerrainColorLayerData *pData=&m_vTerrainColorLayers[nIndex];
	pData->m_LayerConfig=*pLayer;
	pData->m_Texture.m_piTexture->Load(pLayer->sTextureFile);
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

void CWorldManager::GetTerrainColorLayer( unsigned int nIndex,STerrainColorLayer *pLayer,IGenericTexture **ppiTexture )
{
	if(ppiTexture){*ppiTexture=NULL;}

	if(nIndex>=m_vTerrainColorLayers.size()){return;}
	STerrainColorLayerData *pData=&m_vTerrainColorLayers[nIndex];
	if(pLayer){*pLayer=pData->m_LayerConfig;}
	if(ppiTexture){*ppiTexture=ADD(pData->m_Texture.m_piTexture);}
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
	return nIndex;
}

void CWorldManager::UpdateTerrainHeightLayer( unsigned int nIndex,STerrainHeightLayer *pLayer )
{
	if(nIndex>=m_vTerrainHeightLayers.size()){return;}
	STerrainHeightLayerData *pData=&m_vTerrainHeightLayers[nIndex];
	pData->m_LayerConfig=*pLayer;
	pData->m_Texture.m_piTexture->Load(pLayer->sTextureFile);
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

void CWorldManager::GetTerrainHeightLayer( unsigned int nIndex,STerrainHeightLayer *pLayer,IGenericTexture **ppiTexture )
{
	if(ppiTexture){*ppiTexture=NULL;}

	if(nIndex>=m_vTerrainHeightLayers.size()){return;}
	STerrainHeightLayerData *pData=&m_vTerrainHeightLayers[nIndex];
	if(pLayer){*pLayer=pData->m_LayerConfig;}
	if(ppiTexture){*ppiTexture=ADD(pData->m_Texture.m_piTexture);}
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
	eTerrainTexturizationLayer_MAX
};
struct STemporalRenderBuffer
{
	ETerrainTexturizationLayer eType;

	vector<GLfloat> vVertexArray; // por vertice
	vector<GLfloat> vNormalArray;// por vertice
	vector<GLfloat> vTexVertexArray;// por vertice
	vector<GLfloat> vShadowTexVertexArray;// por vertice
	vector<GLfloat> vColorMapTexVertexArray; // por vertice
	vector<GLfloat> vColorArray; // por vertice
	vector<GLuint>	vTriangleVertexes; // 3 por cara

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
};

bool CWorldManager::UpdateTerrain()
{
	if(m_TerrainBaseModel.m_piModel==NULL){return false;}
	if(m_TerrainModel.m_piModel==NULL){return false;}

	m_pTerrainBSP=m_TerrainBaseModel.m_piModel->GetBSP();

	bool bOk=true;

	unsigned int nColorMapWidth=0,nColorMapHeight=0;
	CVector vBaseModelMins,vBaseModelMaxs;
	CVector vModelSize=m_TerrainBaseModel.m_piModel->GetFrameSize(0,0);
	m_TerrainBaseModel.m_piModel->GetFrameBBox(0,0,&vBaseModelMins,&vBaseModelMaxs);
	if(m_TerrainColorMap.m_piTexture){m_TerrainColorMap.m_piTexture->GetSize(&nColorMapWidth,&nColorMapHeight);}

	m_TerrainModel.m_piModel->RemoveAnimations();
	int nAnimation=m_TerrainModel.m_piModel->AddAnimation();
	int nFrame=m_TerrainModel.m_piModel->AddAnimationFrame(nAnimation);

	std::vector<STemporalRenderBuffer> vTempBuffers;
	// Load height layers 

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
		if(m_vTerrainHeightLayers[x].m_Texture.m_piTexture)
		{
			m_vTerrainHeightLayers[x].m_Texture.m_piTexture->GetSize(&buffer.nTextureWidth,&buffer.nTextureHeight);
		}
		vTempBuffers.push_back(buffer);

		unsigned long nBuffer=m_TerrainModel.m_piModel->AddRenderBuffer(nAnimation,nFrame);
		m_TerrainModel.m_piModel->SetRenderBufferTexture(nAnimation,nFrame,nBuffer,0,m_vTerrainHeightLayers[x].m_Texture.m_piTexture);
		m_TerrainModel.m_piModel->SetRenderBufferMaterial(nAnimation,nFrame,nBuffer,CVector(1,1,1),CVector(1,1,1),CVector(0,0,0),0,1);
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
		if(m_vTerrainColorLayers[x].m_Texture.m_piTexture)
		{
			m_vTerrainColorLayers[x].m_Texture.m_piTexture->GetSize(&buffer.nTextureWidth,&buffer.nTextureHeight);
		}
		vTempBuffers.push_back(buffer);

		unsigned long nBuffer=m_TerrainModel.m_piModel->AddRenderBuffer(nAnimation,nFrame);
		m_TerrainModel.m_piModel->SetRenderBufferTexture(nAnimation,nFrame,nBuffer,0,m_vTerrainColorLayers[x].m_Texture.m_piTexture);
		m_TerrainModel.m_piModel->SetRenderBufferMaterial(nAnimation,nFrame,nBuffer,CVector(1,1,1),CVector(1,1,1),CVector(0,0,0),0,1);
	}

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
			CVector vShadow[3];
			CVector vColorMapTexture[3];
			CVector vColors[3];
			double dColorAlpha[3]={0,0,0};

			for(unsigned long nVertex=0;nVertex<3;nVertex++,pBaseModelFaceCursor++)
			{
				GLuint nVertexIndex=*pBaseModelFaceCursor;
				GLfloat *pVertexCursor=&pBaseModelVertexes[nVertexIndex*3];
				GLfloat *pColorCursor=&pBaseModelColor[nVertexIndex*4];
				GLfloat *pNormalCursor=&pBaseModelNormals[nVertexIndex*3];

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
				vShadow[nVertex].c[0]=(vVertexes[nVertex].c[0]-vMins.c[0])/vSize.c[0];
				vShadow[nVertex].c[1]=(vVertexes[nVertex].c[2]-vMins.c[2])/vSize.c[2];
			}

			CVector vTexture[3];

			// Deduccion de las texturas a utilizar
			nDestBufs=0;

			for(int x=vTempBuffers.size()-1;x>=0;x--)
			{
				if(vTempBuffers[x].eType==eTerrainTexturizationLayer_Height)
				{
					for(unsigned long nVertex=0;nVertex<3;nVertex++)
					{
						if(vVertexes[nVertex].c[1]>=vTempBuffers[x].dMinDecayAbsHeight && 
							vVertexes[nVertex].c[1]<=vTempBuffers[x].dMaxDecayAbsHeight)
						{
							pnDestBuf[nDestBufs]=x;
							nDestBufs++;
							break;
						}
					}
				}
				else if(vTempBuffers[x].eType==eTerrainTexturizationLayer_ColorMap && m_TerrainColorMap.m_piTexture)
				{
					for(unsigned long nVertex=0;nVertex<3;nVertex++)
					{
						CVector vPixel;
						vPixel.c[0]=(vColorMapTexture[nVertex].c[0]*((double)nColorMapWidth-1));
						vPixel.c[1]=(vColorMapTexture[nVertex].c[1]*((double)nColorMapHeight-1));

						CVector vColor=m_TerrainColorMap.m_piTexture->GetPixelColor((unsigned long)vPixel.c[0],(unsigned long)vPixel.c[1]);

						CVector vYUVDiffence=HSVDifference(vTempBuffers[x].vColor,vColor);
						if( fabs(vYUVDiffence.c[1])<=vTempBuffers[x].dColorSaturationMargin&& 
							fabs(vYUVDiffence.c[0])<=vTempBuffers[x].dColorValueMargin)
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
				STemporalRenderBuffer *pDestBuffer=&vTempBuffers[nDestinationBuffer];
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
							pDestBuffer->vShadowTexVertexArray.push_back((GLfloat)vShadow[nVertex].c[nCoord]);
							pDestBuffer->vTexVertexArray.push_back((GLfloat)vTexture[nVertex].c[nCoord]);
							pDestBuffer->vColorMapTexVertexArray.push_back((GLfloat)vColorMapTexture[nVertex].c[nCoord]);
						}
						for(unsigned long nCoord=0;nCoord<3;nCoord++)
						{
							pDestBuffer->vVertexArray.push_back((GLfloat)vVertexes[nVertex].c[nCoord]);
							pDestBuffer->vColorArray.push_back((GLfloat)vColors[nVertex].c[nCoord]);
							pDestBuffer->vNormalArray.push_back((GLfloat)vNormals[nVertex].c[nCoord]);
						}
						pDestBuffer->vColorArray.push_back((GLfloat)dColorAlpha[nVertex]);
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

	// Generamos los render buffers a partir de los temporales

	// Cambiamos el orden para introducir primero las capas por altura y despues las capas por color
	// para que se pinten correctamente

	for(unsigned long x=0;x<m_TerrainModel.m_piModel->GetFrameRenderBuffers(nAnimation,nFrame);x++)
	{
		unsigned long nFaces=vTempBuffers[x].vTriangleVertexes.size()/3;
		unsigned long nVertexes=vTempBuffers[x].vVertexArray.size()/3;
			
		m_TerrainModel.m_piModel->SetRenderBufferVertexes(nAnimation,nFrame,x,nVertexes,BufferFromVector(&vTempBuffers[x].vVertexArray));
		m_TerrainModel.m_piModel->SetRenderBufferFaces(nAnimation,nFrame,x,nFaces,BufferFromVector(&vTempBuffers[x].vTriangleVertexes));
		m_TerrainModel.m_piModel->SetRenderBufferColors(nAnimation,nFrame,x,BufferFromVector(&vTempBuffers[x].vColorArray));
		m_TerrainModel.m_piModel->SetRenderBufferNormals(nAnimation,nFrame,x,BufferFromVector(&vTempBuffers[x].vNormalArray));
		m_TerrainModel.m_piModel->SetRenderBufferTextureCoords(nAnimation,nFrame,x,0,BufferFromVector(&vTempBuffers[x].vTexVertexArray));
		m_TerrainModel.m_piModel->SetRenderBufferTextureCoords(nAnimation,nFrame,x,1,BufferFromVector(&vTempBuffers[x].vShadowTexVertexArray));
	}
	if(m_WaterModel.m_piModel)
	{
		m_WaterModel.m_piModel->RemoveAnimations();
		int nWaterAnimation=m_WaterModel.m_piModel->AddAnimation();
		int nWaterFrame=m_WaterModel.m_piModel->AddAnimationFrame(nWaterAnimation);
	
		m_pnWaterRenderBuffers[0]=-1;
		m_pnWaterRenderBuffers[1]=-1;
		if(m_TerrainWater.m_Config.bEnabled)
		{
			IGenericTexture *piTextures[2]={m_TerrainWater.m_Texture1.m_piTexture,m_TerrainWater.m_Texture2.m_piTexture};
			for(int x=0;x<2;x++)
			{
				if(piTextures[x]==NULL){continue;}
				m_pnWaterRenderBuffers[x]=m_WaterModel.m_piModel->AddRenderBuffer(nWaterAnimation,nWaterFrame);
				m_WaterModel.m_piModel->SetRenderBufferTexture(nWaterAnimation,nWaterFrame,m_pnWaterRenderBuffers[x],0,piTextures[x]);
				m_WaterModel.m_piModel->SetRenderBufferMaterial(nWaterAnimation,nWaterFrame,m_pnWaterRenderBuffers[x],CVector(1,1,1),CVector(1,1,1),CVector(1,1,1),128,(float)m_TerrainWater.m_Config.dOpacity);

				float *pWaterVertexBuffer=new GLfloat[12];
				float *pCursor=pWaterVertexBuffer;
				/*
				*pCursor++=(float)vWaterMins.c[0];
				*pCursor++=(float)dAbsWaterHeight;
				*pCursor++=(float)vWaterMins.c[2];

				*pCursor++=(float)vWaterMins.c[0];
				*pCursor++=(float)dAbsWaterHeight;
				*pCursor++=(float)vWaterMaxs.c[2];

				*pCursor++=(float)vWaterMaxs.c[0];
				*pCursor++=(float)dAbsWaterHeight;
				*pCursor++=(float)vWaterMaxs.c[2];

				*pCursor++=(float)vWaterMaxs.c[0];
				*pCursor++=(float)dAbsWaterHeight;
				*pCursor++=(float)vWaterMins.c[2];
				*/
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
				
				m_WaterModel.m_piModel->SetRenderBufferVertexes(nWaterAnimation,nWaterFrame,m_pnWaterRenderBuffers[x],4,pWaterVertexBuffer);

				float *pWaterNormalBuffer=new GLfloat[12];
				pCursor=pWaterNormalBuffer;
				for(int y=0;y<4;y++){*pCursor++=0;*pCursor++=1;*pCursor++=0;}
				m_WaterModel.m_piModel->SetRenderBufferNormals(nWaterAnimation,nWaterFrame,m_pnWaterRenderBuffers[x],pWaterNormalBuffer);

				float *pWaterColorBuffer=new GLfloat[16];
				pCursor=pWaterColorBuffer;
				for(int y=0;y<4;y++){*pCursor++=1;*pCursor++=1;*pCursor++=1;*pCursor++=(float)m_TerrainWater.m_Config.dOpacity;}
				m_WaterModel.m_piModel->SetRenderBufferColors(nWaterAnimation,nWaterFrame,m_pnWaterRenderBuffers[x],pWaterColorBuffer);

				float *pWaterTexBuffer=new GLfloat[8];
				pCursor=pWaterTexBuffer;
				*pCursor++=0;
				*pCursor++=0;

				*pCursor++=0;
				*pCursor++=(float)m_TerrainWater.m_Config.dVerticalResolution;

				*pCursor++=(float)m_TerrainWater.m_Config.dHorizontalResolution;
				*pCursor++=(float)m_TerrainWater.m_Config.dVerticalResolution;

				*pCursor++=(float)m_TerrainWater.m_Config.dHorizontalResolution;
				*pCursor++=0;
				m_WaterModel.m_piModel->SetRenderBufferTextureCoords(nWaterAnimation,nWaterFrame,m_pnWaterRenderBuffers[x],0,pWaterTexBuffer);
		
				unsigned int *pWaterFaces=new unsigned int[6];
				pWaterFaces[0]=0;
				pWaterFaces[1]=1;
				pWaterFaces[2]=2;
				pWaterFaces[3]=0;
				pWaterFaces[4]=2;
				pWaterFaces[5]=3;
				m_WaterModel.m_piModel->SetRenderBufferFaces(nWaterAnimation,nWaterFrame,m_pnWaterRenderBuffers[x],2,pWaterFaces);
			}
		}
		m_WaterModel.m_piModel->UpdateFrameBuffers();
	}

	m_TerrainModel.m_piModel->UpdateFrameBuffers();
	return true;
}

bool CWorldManager::GetTerrainStats( double *pdVertexOverhead,double *pdFaceOverhead )
{
	if(pdVertexOverhead){*pdVertexOverhead=0;}
	if(pdFaceOverhead){*pdFaceOverhead=0;}

	if(m_TerrainBaseModel.m_piModel==NULL){return false;}
	if(m_TerrainModel.m_piModel==NULL){return false;}

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
	for(unsigned int x=0;x<m_TerrainModel.m_piModel->GetFrameRenderBuffers(0,0);x++)
	{
		unsigned long nBufferVertexes=0,nBufferFaces=0;
		m_TerrainModel.m_piModel->GetRenderBufferVertexes(0,0,x,&nBufferVertexes,NULL);
		m_TerrainModel.m_piModel->GetRenderBufferFaces(0,0,x,&nBufferFaces,NULL);
		nNewVertexes+=nBufferVertexes;
		nNewFaces+=nBufferFaces;
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

bool CWorldManager::SetTerrainSun( STerrainSun *pSun ){m_TerrainSun=*pSun;return true;}
void CWorldManager::GetTerrainSun( STerrainSun *pSun ){if(pSun){*pSun=m_TerrainSun;}}
bool CWorldManager::SetTerrainFog( STerrainFog *pFog ){m_TerrainFog=*pFog;return true;}
void CWorldManager::GetTerrainFog( STerrainFog *pFog ){if(pFog){*pFog=m_TerrainFog;}}

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

	if(m_TerrainModel.m_piModel)
	{
		bool bUseBaseFrame=(m_TerrainBaseModel.m_piModel!=NULL && m_TerrainModel.m_piModel->GetFrameRenderBuffers(0,0)==0);
		CVector vTerrainMaxs,vTerrainMins,vTerrainSize;

		if(bUseBaseFrame)
		{
			m_TerrainBaseModel.m_piModel->GetFrameBBox(0,0,&vTerrainMins,&vTerrainMaxs);
			vTerrainSize=m_TerrainBaseModel.m_piModel->GetFrameSize(0,0);
		}
		else
		{
			m_TerrainModel.m_piModel->GetFrameBBox(0,0,&vTerrainMins,&vTerrainMaxs);
			vTerrainSize=m_TerrainModel.m_piModel->GetFrameSize(0,0);
		}

		CVector vSunAngles(m_TerrainSun.dAzimuth,m_TerrainSun.dElevation,0);
		CVector vForward;
		CVector vBlack;
		VectorsFromAngles(vSunAngles,&vForward);
		double dTerrainMaxDimension=max(max(vTerrainSize.c[0],vTerrainSize.c[1]),vTerrainSize.c[2]);
		CVector vSunPos=vForward*(dTerrainMaxDimension*m_TerrainSun.dDistance);
		piRender->SetSunLight(vSunPos,vForward,vBlack,m_TerrainSun.vColor,m_TerrainSun.vColor);
	}
	piRender->SetAmbientLight(m_vTerrainAmbientColor);
}
