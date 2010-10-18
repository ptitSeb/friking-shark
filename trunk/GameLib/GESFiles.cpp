
// GESFiles.cpp: implementation of the CGESFileType class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "crtdbg.h"
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <stack>

#include "GameLib.h"
#include "GESFiles.h"

using namespace std;
/*
Historico de Versiones
----------------------

v1.1 Funcionalidad standard
v1.2 Escenario por Sectores
*/
#define GES_CURRENT_VERSION 1.2

#define GES_FILE_DELIMITER " \t:\r\n\"="
#define GES_FILE_STRING_DELIMITER "\r\n\""
#define SKIP_GES_TOKEN() strtok(NULL,GES_FILE_DELIMITER)


enum
{
	GES_NODE_NONE=0,
	GES_NODE_ROOT=0,
	GES_NODE_MODELS=1,
	GES_NODE_MODEL,
	GES_NODE_TEXTURES,
	GES_NODE_TEXTURE,
	GES_NODE_DETAIL_LEVEL,
	GES_NODE_SCENARIO,
	GES_NODE_FOG,
	GES_NODE_CLOUDS_SHADOW,
	GES_NODE_PLAYER_ROUTE,
	GES_NODE_UNIT_TYPES,
	GES_NODE_UNIT_TYPE,
	GES_NODE_UNIT_DEBRIS,
	GES_NODE_FORMATION_TEMPLATES,
	GES_NODE_FORMATION_TEMPLATE,
	GES_NODE_FORMATION_UNIT,
	GES_NODE_FORMATION_ROUTE_POINT,
	GES_NODE_UNITS,				
	GES_NODE_STATIC_UNIT,		
	GES_NODE_FORMATIONS,
	GES_NODE_FORMATION,
	GES_NODE_CONTROLLERS,		
	GES_NODE_CONTROLLER,
	GES_NODE_GUI,
	GES_NODE_EFFECTS_CONFIG,
	GES_NODE_EFFECT_SMOKE,
	GES_NODE_EFFECT_EXPLOSION,
	GES_NODE_WEAPONS_CONFIG,
	GES_NODE_WEAPON_MACHINEGUN_CONFIG,
	GES_NODE_WEAPON_MISSILES_CONFIG,
	GES_NODE_SECTORS,
	GES_NODE_SECTOR,
	GES_ELEMENT_COUNT
};

char *FindGESNodeEnd(char *pBuffer);

template <class T1>
void PopulateMapFromVector(char *pObjectTypeText,map<string,T1*> &mDest,vector<T1*> &vOrig)
{
	char pTempName[MAX_PATH]={0};
	int x;
	for(x=0;x<vOrig.size();x++)
	{
		T1*pObj=vOrig[x];
		if(pObj->sName=="")
		{
			sprintf(pTempName,"Unnamed_%s %d",pObjectTypeText,x);
			pObj->sName=pTempName;
		}
		mDest[pObj->sName]=pObj;
	}
}
template <class T1>
void DumpMap(char *pObjectTypeText,map<string,T1*> &mObjects)
{
	//_MRT("CGESFileType::DumpMap -> %d %s",mObjects.size(),pObjectTypeText);
	map<string,T1*>::iterator i;
	for(i=mObjects.begin();i!=mObjects.end();i++)
	{
		//_MRT("CGESFileType::DumpMap ->     '%s'",i->first.c_str());
	}

}
template <class T1>
void DeleteMap(map<string,T1*> &mObjects)
{
	map<string,T1*>::iterator i;
	for(i=mObjects.begin();i!=mObjects.end();i++)
	{
		T1 *pObject=i->second;
		delete pObject;
	}
	mObjects.clear();

}

SGESNode::~SGESNode()
{
	multimap<string,SGESNode*>::iterator i;
	for(i=mNodes.begin();i!=mNodes.end();i++)
	{
		SGESNode *pNode=i->second;
		delete pNode;
	}
}

WORD			SGESNode::GetWord(char *pName)  {WORD value=0;value=atoi(mData[pName].c_str());return value;}
DWORD			SGESNode::GetDWord(char *pName) {DWORD value=0;value=atol(mData[pName].c_str());return value;}
float			SGESNode::GetFloat(char *pName) {float value=0;value=atof(mData[pName].c_str());return value;}
unsigned char	SGESNode::GetByte(char *pName)  {unsigned char value=0;value=atoi(mData[pName].c_str());return value;}
string			SGESNode::GetString(char *pName){return mData[pName].c_str();}
bool			SGESNode::HasString(char *pName){return mData.find(pName)!=mData.end();}
CVector			SGESNode::GetVector(char *pName)
{
	char  pTemp[MAX_PATH]={0};
	char *pToken=NULL;
	strcpy(pTemp,mData[pName].c_str());
	CVector v;
	for(int c=0;c<3;c++)
	{
		if(pToken=strtok(pToken?NULL:pTemp,GES_FILE_DELIMITER ","))
		{
			v.c[c]=atof(pToken);
		}
	}
	return v;
}
COLORREF		SGESNode::GetColor(char *pName) {CVector c=GetVector(pName);return RGB(c.c[0],c.c[1],c.c[2]);}

SGESNode *SGESNode::GetNode(char *pNodePath)
{
	char *pSubNodeEnd=strchr(pNodePath,'\\');
	if(pSubNodeEnd)
	{
		char sSubNodeName[MAX_PATH]={0};
		strncpy(sSubNodeName,pNodePath,(DWORD)(pSubNodeEnd-pNodePath));
		multimap<string,SGESNode*>::iterator i=mNodes.find((string)sSubNodeName);
		if(i!=mNodes.end())
		{
			SGESNode *pNode=i->second;
			return pNode->GetNode(pSubNodeEnd+1);
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		multimap<string,SGESNode*>::iterator i=mNodes.find((string)pNodePath);
		if(i!=mNodes.end())
		{
			SGESNode *pNode=i->second;
			return pNode;
		}
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGESFileType::CGESFileType()
{
	m_dwSaveTabCount=0;
	m_RootNode.sName="*ROOT*";
	m_mKeyNames["*ROOT*"]				= GES_NODE_ROOT;
	m_mKeyNames["*Modelos"]				= GES_NODE_MODELS;
	m_mKeyNames["*Modelo"]				= GES_NODE_MODEL;
	m_mKeyNames["*Texturas"]			= GES_NODE_TEXTURES;
	m_mKeyNames["*Textura"]				= GES_NODE_TEXTURE;
	m_mKeyNames["*NivelDetalle"]		= GES_NODE_DETAIL_LEVEL;
	m_mKeyNames["*Escenario"]			= GES_NODE_SCENARIO;
	m_mKeyNames["*Niebla"]				= GES_NODE_FOG;
	m_mKeyNames["*SombraNubes"]			= GES_NODE_CLOUDS_SHADOW;
	m_mKeyNames["*RutaJugador"]			= GES_NODE_PLAYER_ROUTE;
	m_mKeyNames["*TiposUnidades"]		= GES_NODE_UNIT_TYPES;
	m_mKeyNames["*TipoUnidad"]			= GES_NODE_UNIT_TYPE;
	m_mKeyNames["*Trozo"]				= GES_NODE_UNIT_DEBRIS;
	m_mKeyNames["*Unidades"]			= GES_NODE_UNITS;		
	m_mKeyNames["*UnidadEstatica"]		= GES_NODE_STATIC_UNIT;
	m_mKeyNames["*PlantillasFormacion"]	= GES_NODE_FORMATION_TEMPLATES;
	m_mKeyNames["*PlantillaFormacion"]	= GES_NODE_FORMATION_TEMPLATE;
	m_mKeyNames["*UnidadFormacion"]		= GES_NODE_FORMATION_UNIT;
	m_mKeyNames["*PuntoDeRuta"]			= GES_NODE_FORMATION_ROUTE_POINT;
	m_mKeyNames["*Formaciones"]			= GES_NODE_FORMATIONS;
	m_mKeyNames["*Formacion"]			= GES_NODE_FORMATION;
	m_mKeyNames["*Controladores"]		= GES_NODE_CONTROLLERS;
	m_mKeyNames["*Controlador"]			= GES_NODE_CONTROLLER;
	m_mKeyNames["*GUI"]					= GES_NODE_GUI;
	m_mKeyNames["*ConfiguracionEfectos"]= GES_NODE_EFFECTS_CONFIG;
	m_mKeyNames["*Humo"]				= GES_NODE_EFFECT_SMOKE;
	m_mKeyNames["*Explosion"]			= GES_NODE_EFFECT_EXPLOSION;
	m_mKeyNames["*ConfiguracionArmas"]	= GES_NODE_WEAPONS_CONFIG;
	m_mKeyNames["*Ametralladora"]		= GES_NODE_WEAPON_MACHINEGUN_CONFIG;
	m_mKeyNames["*Misiles"]				= GES_NODE_WEAPON_MISSILES_CONFIG;
	m_mKeyNames["*Sectores"]			= GES_NODE_SECTORS;
	m_mKeyNames["*Sector"]				= GES_NODE_SECTOR;

	m_pFile=NULL;
	m_pBuffer=NULL;
}

CGESFileType::~CGESFileType()
{
	DeleteMap<SGESModel>(m_mModels);
	DeleteMap<SGESTexture>(m_mTextures);
	DeleteMap<SGESUnitType>(m_mUnitTypes);
	DeleteMap<SGESFormationTemplate>(m_mFormationTemplates);

	DeleteMap<SGESUnit>(m_mUnits);
	DeleteMap<SGESFormation>(m_mFormations);
	DeleteMap<SGESController>(m_mControllers);
}

bool CGESFileType::Open(const char *sFileName)
{
	DWORD dwFileLength=0;

	if ((m_pFile=fopen (sFileName, "rb"))== NULL) return false; //Open the file
	dwFileLength=_filelength(_fileno (m_pFile));
	if(!dwFileLength){return false;}

	//_MRT("CGESFileType::Open -> File: %s",sFileName);

	WORD	wChunckId=0; //Chunk identifier
	DWORD	dwChunckLength=0; //Chunk lenght

	m_pBuffer=new char [dwFileLength+1];
	fread(m_pBuffer,dwFileLength,1,m_pFile);
	m_pBuffer[dwFileLength]=0;

	// Se leen los nodos del archivo
	SGESNode *pCurrentNode=&m_RootNode;
	stack<SGESNode*> sNodeStack;
	sNodeStack.push(pCurrentNode);

	// usando pSearchBuffer podemos poner pToken a cero para seguir la busqueda en otro punto
	char *pToken=NULL;
	char *pSearchBuffer=m_pBuffer; 
	while(pToken=strtok(pToken?NULL:pSearchBuffer,GES_FILE_DELIMITER))
	{
		if(pToken[0]=='*')
		{
			string sToken=pToken;
			SGESNode *pNode=new SGESNode;
			pNode->sName=pToken;
			pCurrentNode->mNodes.insert(pair<string,SGESNode*>(sToken,pNode));
			pCurrentNode=pNode;
			//_MRT("CGESFileType::Open -> Node %s",sToken.c_str());
		}
		else if(pToken[0]=='{')
		{
			sNodeStack.push(pCurrentNode);
		}
		else if(pToken[0]=='}')
		{
			sNodeStack.pop();
			pCurrentNode=sNodeStack.top();
		}
		else
		{
			int nLen=strlen(pToken);
			int pos=nLen;
			// Se esquivan los espacios y el igual entre el nombre del dato y el valor
			while(strchr("\t =",pToken[pos])){pos++;}
			char *pBuf=NULL;
			// Los strings y los demas valores se tratan de diferente manera
			if(pToken[pos]=='"')
			{
				pos++;
				if(strchr("\"\r\n",pToken[pos])) // esquivar cadenas vacias o no terminadas
				{
					pos++;
				}
				else
				{
					pBuf=strtok(pToken+pos,"\"\r\n");
				}
			}
			else
			{
				if(strchr("\r\n",pToken[pos])) // esquivar valores vacios
				{
					pos++;
				}
				else
				{
					pBuf=strtok(pToken+pos,"\t\r\n ");
				}
			}
			if(pBuf)
			{
				pSearchBuffer=pBuf+strlen(pBuf)+1;
				pCurrentNode->mData[pToken]=pBuf;
			}
			else
			{
				pSearchBuffer=pToken+pos;
				pCurrentNode->mData[pToken]="";
			}
			//_MRT("CGESFileType::Open -> Node %s : %s = %s",pCurrentNode->sName.c_str(),pToken,pBuf);
		}
	}
	double dVersion=1.0;
	multimap<string,SGESNode*>::iterator i=m_RootNode.mNodes.find("*Version");
	if(i!=m_RootNode.mNodes.end())
	{
		pCurrentNode=i->second;
		dVersion=pCurrentNode->GetFloat("Version");
	}
	ReadNodeRecursive(&m_RootNode,dVersion);

	// Compatibilidad con archivos hasta la version 1.1 (que no tenian el escenario dividido en sectores).

	if(m_Scenario.vSectors.size()==0) 
	{
		SGESScenarioSector *pSector=new SGESScenarioSector;
		pSector->sModel=m_Scenario.sModel;
		pSector->bSelectedForEditing=true;
		m_Scenario.vSectors.push_back(pSector);
	}

	if(m_pBuffer){delete [] m_pBuffer;m_pBuffer=NULL;}
	fclose (m_pFile); // Closes the file stream
	m_pFile=NULL;
	m_pBuffer=NULL;
	return true;
}

bool CGESFileType::Save(const char *sFileName)
{
	if ((m_pFile=fopen (sFileName, "wb"))== NULL) return false; //Open the file
	m_dwSaveTabCount=0;

	SaveBeginSection("*Version");
		SaveValueFloat("Version",GES_CURRENT_VERSION);
	SaveEndSection();

	if(m_mModels.size())
	{
		SaveBeginSection("*Modelos");
		map<string,SGESModel *>::iterator				iModel;
		for(iModel=m_mModels.begin();iModel!=m_mModels.end();iModel++)
		{
			SGESModel *pModel=iModel->second;

			SaveBeginSection("*Modelo");
			SaveValueString("Nombre",pModel->sName);
			if(pModel->sFilePath!="")
			{
				SaveValueString("Archivo",pModel->sFilePath);
			}
			if(pModel->vDetailLevels.size())
			{
				for(unsigned x=0;x<pModel->vDetailLevels.size();x++)
				{
					SaveBeginSection("*NivelDetalle");
					SaveValueFloat("DistanciaMinima",pModel->vDetailLevels[x]->dMinimunDistance);
					SaveValueString("Archivo",pModel->vDetailLevels[x]->sFilePath);
					SaveEndSection();
				}
			}
			SaveEndSection();
		}
		SaveEndSection();
	}

	if(m_mTextures.size())
	{
		SaveBeginSection("*Texturas");
		map<string,SGESTexture *>::iterator				iTexture;
		for(iTexture=m_mTextures.begin();iTexture!=m_mTextures.end();iTexture++)
		{
			SGESTexture *pTexture=iTexture->second;

			SaveBeginSection("*Textura");
			SaveValueString("Nombre",pTexture->sName);
			SaveValueString("Archivo",pTexture->sFilePath);
			if(pTexture->bColorKey){SaveValueColor("ColorKey",pTexture->cColorKey);}
			if(pTexture->bAlphaFile){SaveValueString("ArchivoAlpha",pTexture->sAlphaPath);}
			SaveEndSection();
		}
		SaveEndSection();
	}

	if(m_mUnitTypes.size())
	{
		SaveBeginSection("*TiposUnidades");
		map<string,SGESUnitType *>::iterator				iUnitType;
		for(iUnitType=m_mUnitTypes.begin();iUnitType!=m_mUnitTypes.end();iUnitType++)
		{
			SGESUnitType *pUnitType=iUnitType->second;

			SaveBeginSection("*TipoUnidad");
			SaveValueString("Nombre",pUnitType->sName);
			SaveValueString("Modelo",pUnitType->sModel);
			SaveValueString("Clase",pUnitType->sClass);
			SaveValueFloat("Vida",pUnitType->dHealth);
			SaveValueFloat("MinVelAngularTrozos",pUnitType->dMinDebrisAngleSpeed);
			SaveValueFloat("MaxVelAngularTrozos",pUnitType->dMaxDebrisAngleSpeed);
			SaveValueVector("MinVectorVelTrozos",FromOpenGLTo3DS(pUnitType->vMinDebrisSpeed));
			SaveValueVector("MaxVectorVelTrozos",FromOpenGLTo3DS(pUnitType->vMaxDebrisSpeed));
			if(pUnitType->sDestroyedModel!=""){SaveValueString("ModeloDestruido",pUnitType->sDestroyedModel);}
			if(pUnitType->vDebris.size())
			{
				for(unsigned x=0;x<pUnitType->vDebris.size();x++)
				{
					SaveBeginSection("*Trozo");
					SaveValueString("Modelo",pUnitType->vDebris[x]->sModel);
					SaveEndSection();
				}
			}
			SaveEndSection();
		}
		SaveEndSection();
	}


	if(m_mFormationTemplates.size())
	{
		SaveBeginSection("*PlantillasFormacion");
		map<string,SGESFormationTemplate *>::iterator				iFormationTemplate;
		for(iFormationTemplate=m_mFormationTemplates.begin();iFormationTemplate!=m_mFormationTemplates.end();iFormationTemplate++)
		{
			SGESFormationTemplate *pFormationTemplate=iFormationTemplate->second;

			SaveBeginSection("*PlantillaFormacion");
			SaveValueString("Nombre",pFormationTemplate->sName);

			if(pFormationTemplate->mUnits.size())
			{
				map<DWORD,SGESFormationUnit *>::iterator iUnit;
				for(iUnit=pFormationTemplate->mUnits.begin();iUnit!=pFormationTemplate->mUnits.end();iUnit++)
				{
					DWORD				dwIndex=iUnit->first;
					SGESFormationUnit	*pUnit=iUnit->second;

					SaveBeginSection("*UnidadFormacion");
					SaveValueDWord("Indice",dwIndex);
					SaveValueString("Tipo",pUnit->sType);
					SaveValueVector("Angulos",pUnit->vStartAngles);

					if(pFormationTemplate->mUnits.size())
					{
						map<DWORD,SGESFormationUnitRoutePoint>::iterator iPoints;
						for(iPoints=pUnit->routePoints.begin();iPoints!=pUnit->routePoints.end();iPoints++)
						{
							DWORD						dwRouteIndex=iPoints->first;
							SGESFormationUnitRoutePoint point=iPoints->second;
							SaveBeginSection("*PuntoDeRuta");
							SaveValueDWord("Indice",dwRouteIndex);
							SaveValueVector("Posicion",FromOpenGLTo3DS(point.vPosition));
							SaveEndSection();
						}
					}

					SaveEndSection();
				}
			}
			SaveEndSection();
		}
		SaveEndSection();
	}
	if(m_mUnits.size())
	{
		SaveBeginSection("*Unidades");
		map<string,SGESUnit *>::iterator				iUnit;
		for(iUnit=m_mUnits.begin();iUnit!=m_mUnits.end();iUnit++)
		{
			SGESUnit *pUnit=iUnit->second;

			SaveBeginSection("*UnidadEstatica");
			SaveValueVector("Posicion",FromOpenGLTo3DS(pUnit->vPosition));
			SaveValueVector("Angulos",pUnit->vAngles);
			SaveValueString("Tipo",pUnit->sType);
			if(pUnit->bHealthOverride){SaveValueFloat("Vida",pUnit->dHealth);}
			SaveEndSection();
		}
		SaveEndSection();
	}

	if(m_mFormations.size())
	{
		SaveBeginSection("*Formaciones");
		map<string,SGESFormation *>::iterator				iFormation;
		for(iFormation=m_mFormations.begin();iFormation!=m_mFormations.end();iFormation++)
		{
			SGESFormation *pFormation=iFormation->second;

			SaveBeginSection("*Formacion");
			SaveValueVector("PosicionActivacion",FromOpenGLTo3DS(pFormation->vPosition));
			SaveValueString("Plantilla",pFormation->sTemplate);
			if(pFormation->sActivationType!="")
			{
				SaveValueString("TipoActivacion",pFormation->sActivationType);
			}

			if(pFormation->mOverloadedUnits.size())
			{
				map<DWORD,SGESFormationUnit *>::iterator iOverloadedUnit;
				for(iOverloadedUnit=pFormation->mOverloadedUnits.begin();iOverloadedUnit!=pFormation->mOverloadedUnits.end();iOverloadedUnit++)
				{
					DWORD				dwIndex=iOverloadedUnit->first;
					SGESFormationUnit	*pUnit=iOverloadedUnit->second;

					SaveBeginSection("*UnidadFormacion");
					SaveValueDWord("Indice",dwIndex);
					if(pUnit->dwValidFields&SGESFormationUnit::SGESFormationUnit_Type)
					{
						SaveValueString("Tipo",pUnit->sType);
					}
					if(pUnit->dwValidFields&SGESFormationUnit::SGESFormationUnit_StartAngles)
					{
						SaveValueVector("Angulos",pUnit->vStartAngles);
					}
					SaveEndSection();
				}
			}
			SaveEndSection();
		}
		SaveEndSection();
	}

	SaveBeginSection("*Escenario");
		SaveValueString("Modelo",m_Scenario.sModel);
		if(m_Scenario.sSkyModel!=""){SaveValueString("ModeloCielo",m_Scenario.sSkyModel);}
		SaveValueString("ModeloJugador",m_Scenario.sPlayerModel);
		SaveValueFloat("PlanoLejano",m_Scenario.dFarPlane);
		SaveValueFloat("DistanciaCamara",m_Scenario.dCameraDistance);
		SaveValueFloat("InclinacionVista",m_Scenario.dCameraPitch);
		SaveValueFloat("AnguloVisualizacion",m_Scenario.dCameraViewAngle);
		SaveValueFloat("VelocidadInspeccion",m_Scenario.dInspectionSpeed);
		SaveValueVector("PosicionInspeccion",FromOpenGLTo3DS(m_Scenario.vInspectionPosition));
		SaveValueVector("AngulosInspeccion",m_Scenario.vInspectionAngles);

		if(m_Scenario.pFog)
		{
			SaveBeginSection("*Niebla");
				SaveValueFloat("Inicio",m_Scenario.pFog->dStart);
				SaveValueFloat("Fin",m_Scenario.pFog->dEnd);
				SaveValueColor("Color",m_Scenario.pFog->cColor);
			SaveEndSection();
		}

		if(m_Scenario.pCloudsShadow)
		{
			SaveBeginSection("*SombraNubes");
				SaveValueVector("Velocidad",FromOpenGLTo3DS(m_Scenario.pCloudsShadow->vVelocity));
				SaveValueString("Textura",m_Scenario.pCloudsShadow->sTexture);
			SaveEndSection();
		}

		SaveBeginSection("*Sectores");
		for(unsigned nSector=0;nSector<m_Scenario.vSectors.size();nSector++)
		{
			SGESScenarioSector *pSector=m_Scenario.vSectors[nSector];
			SaveBeginSection("*Sector");
				SaveValueString("Modelo",pSector->sModel);
				SaveValueVector("AbsMins",FromOpenGLTo3DS(pSector->vAbsMins));
				SaveValueVector("AbsMaxs",FromOpenGLTo3DS(pSector->vAbsMaxs));
				SaveValueVector("Offset",FromOpenGLTo3DS(pSector->vOffset));
				SaveValueDWord("SeleccionadoParaEdicion",pSector->bSelectedForEditing);
			SaveEndSection();
		}
		SaveEndSection();

	SaveEndSection();

	SaveBeginSection("*RutaJugador");
		SaveValueVector("Origen",FromOpenGLTo3DS(m_PlayerRoute.vStart));
		SaveValueVector("Destino",FromOpenGLTo3DS(m_PlayerRoute.vEnd));
		SaveValueFloat("VelocidadCamara",m_PlayerRoute.dCameraMovementSpeed);
		SaveValueFloat("VelocidadJugador",m_PlayerRoute.dPlayerMovementSpeed);
		SaveValueFloat("MinimoMovHorz",m_PlayerRoute.dMovementMinRight);
		SaveValueFloat("MaximoMovHorz",m_PlayerRoute.dMovementMaxRight);
		SaveValueFloat("MinimoActivacionHorz",m_PlayerRoute.dActivationMinRight);
		SaveValueFloat("MaximoActivacionHorz",m_PlayerRoute.dActivationMaxRight);
		SaveValueFloat("MinimoActivacionVert",m_PlayerRoute.dActivationMinUp);
		SaveValueFloat("MaximoActivacionVert",m_PlayerRoute.dActivationMaxUp);
		SaveValueFloat("MinimoActivacionProf",m_PlayerRoute.dActivationMinForward);
		SaveValueFloat("MaximoActivacionProf",m_PlayerRoute.dActivationMaxForward);
	SaveEndSection();

	SaveBeginSection("*ConfiguracionEfectos");
		SaveBeginSection("*Humo");
			SaveValueString("Textura",m_SmokeConfig.sTexture);
		SaveEndSection();
		SaveBeginSection("*Explosion");
			SaveValueString("Modelo",m_ExplosionConfig.sModel);
		SaveEndSection();
	SaveEndSection();

	SaveBeginSection("*ConfiguracionArmas");
		SaveBeginSection("*Ametralladora");
			SaveValueString("ModeloProyectil",m_MachineGunConfig.sProjectileModel);
			SaveValueFloat("NumeroProyectiles",m_MachineGunConfig.dProjectileCount);
			SaveValueFloat("Intervalo",m_MachineGunConfig.dInterval);
		SaveEndSection();
		SaveBeginSection("*Misiles");
			SaveValueString("ModeloProyectil",m_MissilesConfig.sProjectileModel);
			SaveValueFloat("Intervalo",m_MissilesConfig.dInterval);
		SaveEndSection();
	SaveEndSection();

/*
	map<string,SGESFormationTemplate *>::iterator	iFormationTemplates;

	map<string,SGESUnit *>::iterator			iUnits;
	map<string,SGESFormation *>::iterator		iFormations;
	map<string,SGESController *>::iterator	iControllers;
*/
	fclose (m_pFile); // Closes the file stream
	m_pFile=NULL;
	return true;
}

void CGESFileType::ReadNodeRecursive(SGESNode *pNode,double dVersion)
{
	static SGESModel				*pCurrentModel=NULL;
	static SGESUnitType				*pCurrentUnitType=NULL;
	static SGESUnit					*pCurrentUnit=NULL;
	static SGESFormation			*pCurrentFormation=NULL;
	static SGESFormationUnit		*pCurrentFormationUnit=NULL;
	static SGESFormationTemplate	*pCurrentFormationTemplate=NULL;
	static DWORD					 dwUnnamedObjectIndex=0;

	map<string,DWORD>::iterator i=m_mKeyNames.find(pNode->sName);
	if(i==m_mKeyNames.end()){return;}
	DWORD dwKeyId=i->second;
	switch(dwKeyId)
	{
	case GES_NODE_MODEL:
		{
			pCurrentModel=new SGESModel();
			pCurrentModel->sName=pNode->GetString("Nombre");
			pCurrentModel->sFilePath=pNode->GetString("Archivo");
			m_mModels[pCurrentModel->sName]=pCurrentModel;
		}
		break;
	case GES_NODE_DETAIL_LEVEL:
		{
			SGESDetailLevel *pDetailLevel=new SGESDetailLevel();
			pDetailLevel->dMinimunDistance=pNode->GetFloat("DistanciaMinima");
			pDetailLevel->sFilePath=pNode->GetString("Archivo");
			pCurrentModel->vDetailLevels.push_back(pDetailLevel);
		}
		break;
	case GES_NODE_TEXTURE:
		{
			SGESTexture *pTexture=new SGESTexture();
			pTexture->sName=pNode->GetString("Nombre");
			pTexture->sFilePath=pNode->GetString("Archivo");
			if(pNode->HasString("ColorKey"))
			{
				pTexture->bColorKey=true;
				pTexture->cColorKey=pNode->GetColor("ColorKey");
			}
			if(pNode->HasString("ArchivoAlpha"))
			{
				pTexture->bAlphaFile=true;
				pTexture->sAlphaPath=pNode->GetString("ArchivoAlpha");
			}
			
			m_mTextures[pTexture->sName]=pTexture;
		}
		break;
	case GES_NODE_SCENARIO:		
		{
			m_Scenario.sModel=pNode->GetString("Modelo");
			m_Scenario.sSkyModel=pNode->GetString("ModeloCielo");
			m_Scenario.sPlayerModel=pNode->GetString("ModeloJugador");
			m_Scenario.dFarPlane=pNode->GetFloat("PlanoLejano");

			if(dVersion>=1.1)
			{
				m_Scenario.dCameraDistance=pNode->GetFloat("DistanciaCamara");
				m_Scenario.dCameraPitch=pNode->GetFloat("InclinacionVista");
				m_Scenario.dCameraViewAngle=pNode->GetFloat("AnguloVisualizacion");
				m_Scenario.dInspectionSpeed=pNode->GetFloat("VelocidadInspeccion");
				m_Scenario.vInspectionPosition=From3DSToOpenGL(pNode->GetVector("PosicionInspeccion"));
				m_Scenario.vInspectionAngles=pNode->GetVector("AngulosInspeccion");
			}
		}
		break;
	case GES_NODE_GUI:		
		{
			m_GUI.sCabinModel=pNode->GetString("ModeloCabina");
			m_GUI.sCrosshairModel=pNode->GetString("ModeloPuntoMira");
		}
		break;
	case GES_NODE_FOG:
		{
			SGESFog *pFog=new SGESFog();
			pFog->dStart=pNode->GetFloat("Inicio");
			pFog->dEnd=pNode->GetFloat("Fin");
			pFog->cColor=pNode->GetColor("Color");
			m_Scenario.pFog=pFog;
		}
		break;
	case GES_NODE_CLOUDS_SHADOW:
		{
			SGESCloudsShadow *pCloudsShadow=new SGESCloudsShadow();
			pCloudsShadow->vVelocity=From3DSToOpenGL(pNode->GetVector("Velocidad"));
			pCloudsShadow->sTexture=pNode->GetString("Textura");
			m_Scenario.pCloudsShadow=pCloudsShadow;
		}
		break;
	case GES_NODE_SECTOR:
		{
			SGESScenarioSector *pSector=new SGESScenarioSector();
			pSector->sModel=pNode->GetString("Modelo");
			pSector->vAbsMins=From3DSToOpenGL(pNode->GetVector("AbsMins"));
			pSector->vAbsMaxs=From3DSToOpenGL(pNode->GetVector("AbsMaxs"));
			pSector->vOffset=From3DSToOpenGL(pNode->GetVector("Offset"));
			pSector->bSelectedForEditing=pNode->GetDWord("SeleccionadoParaEdicion")!=0;
			m_Scenario.vSectors.push_back(pSector);
		}
		break;
	case GES_NODE_PLAYER_ROUTE:
		{
			m_PlayerRoute.vStart=From3DSToOpenGL(pNode->GetVector("Origen"));
			m_PlayerRoute.vEnd=From3DSToOpenGL(pNode->GetVector("Destino"));
			m_PlayerRoute.dMovementMinRight=pNode->GetFloat("MinimoMovHorz");
			m_PlayerRoute.dMovementMaxRight=pNode->GetFloat("MaximoMovHorz");
			m_PlayerRoute.dActivationMinRight=pNode->GetFloat("MinimoActivacionHorz");
			m_PlayerRoute.dActivationMinUp=pNode->GetFloat("MinimoActivacionVert");
			m_PlayerRoute.dActivationMinForward=pNode->GetFloat("MinimoActivacionProf");
			m_PlayerRoute.dActivationMaxRight=pNode->GetFloat("MaximoActivacionHorz");
			m_PlayerRoute.dActivationMaxUp=pNode->GetFloat("MaximoActivacionVert");
			m_PlayerRoute.dActivationMaxForward=pNode->GetFloat("MaximoActivacionProf");

			if(dVersion>=1.1)
			{
				m_PlayerRoute.dCameraMovementSpeed=pNode->GetFloat("VelocidadCamara");
				m_PlayerRoute.dPlayerMovementSpeed=pNode->GetFloat("VelocidadJugador");
			}
		}
		break;
	case GES_NODE_UNIT_TYPE:
		{
			SGESUnitType *pType=new SGESUnitType();
			pType->sName=pNode->GetString("Nombre");
			pType->sModel=pNode->GetString("Modelo");
			pType->sClass=pNode->GetString("Clase");
			pType->sDestroyedModel=pNode->GetString("ModeloDestruido");
			pType->dHealth=pNode->GetFloat("Vida");
			pType->vMinDebrisSpeed=From3DSToOpenGL(pNode->GetVector("MinVectorVelTrozos"));
			pType->vMaxDebrisSpeed=From3DSToOpenGL(pNode->GetVector("MaxVectorVelTrozos"));
			pType->dMinDebrisAngleSpeed=pNode->GetFloat("MinVelAngularTrozos");
			pType->dMaxDebrisAngleSpeed=pNode->GetFloat("MaxVelAngularTrozos");
			pType->dHealth=pNode->GetFloat("Vida");
			pCurrentUnitType=pType;
			m_mUnitTypes[pType->sName]=pType;
		}
		break;
	case GES_NODE_UNIT_DEBRIS:
		{
			SGESDebris *pDebris=new SGESDebris();
			pDebris->sModel=pNode->GetString("Modelo");
			pCurrentUnitType->vDebris.push_back(pDebris);
		}
		break;
	case GES_NODE_STATIC_UNIT:
		{
			SGESUnit *pUnit=new SGESUnit();
			pUnit->sName=pNode->GetString("Nombre");
			pUnit->vPosition=From3DSToOpenGL(pNode->GetVector("Posicion"));
			pUnit->vAngles=pNode->GetVector("Angulos");
			pUnit->sType=pNode->GetString("Tipo");

			if(pUnit->sName=="")
			{
				char pTempName[MAX_PATH]={0};
				sprintf(pTempName,"Unnamed_Unit %d",dwUnnamedObjectIndex);
				dwUnnamedObjectIndex++;
				pUnit->sName=pTempName;
			}

			pCurrentUnit=pUnit;

			if(pNode->HasString("Vida"))
			{
				pUnit->dHealth=pNode->GetFloat("Vida");
				pUnit->bHealthOverride=true;
			}
			m_mUnits[pUnit->sName]=pUnit;
		}
		break;
	case GES_NODE_FORMATION_TEMPLATE:
		{
			SGESFormationTemplate *pTemplate=new SGESFormationTemplate();
			pTemplate->sName=pNode->GetString("Nombre");

			pCurrentFormationTemplate=pTemplate;
			pCurrentFormation=NULL;
			
			m_mFormationTemplates[pTemplate->sName]=pTemplate;
		}
		break;
	case GES_NODE_FORMATION_UNIT:
		{
			SGESFormationUnit *pUnit=new SGESFormationUnit();
			if(pNode->HasString("Tipo")){pUnit->dwValidFields|=SGESFormationUnit::SGESFormationUnit_Type;}
			if(pNode->HasString("Angulos")){pUnit->dwValidFields|=SGESFormationUnit::SGESFormationUnit_StartAngles;}

			DWORD dwIndex=pNode->GetDWord("Indice");
			pUnit->vStartAngles=pNode->GetVector("Angulos");
			pUnit->sType=pNode->GetString("Tipo");

			pCurrentFormationUnit=pUnit;
			if(pCurrentFormationTemplate){pCurrentFormationTemplate->mUnits[dwIndex]=pUnit;}
			else						 {pCurrentFormation->mOverloadedUnits[dwIndex]=pUnit;}
		}
		break;
	case GES_NODE_FORMATION_ROUTE_POINT:
		{
			SGESFormationUnitRoutePoint point;
			point.vPosition=From3DSToOpenGL(pNode->GetVector("Posicion"));
			pCurrentFormationUnit->routePoints[pNode->GetDWord("Indice")]=point;
		}
		break;
	case GES_NODE_FORMATION:
		{
			SGESFormation *pFormation=new SGESFormation();
			pFormation->sName=pNode->GetString("Nombre");
			pFormation->vPosition=From3DSToOpenGL(pNode->GetVector("PosicionActivacion"));
			pFormation->sTemplate=pNode->GetString("Plantilla");
			pFormation->sActivationType=pNode->GetString("TipoActivacion");

			pCurrentFormationTemplate=NULL;
			pCurrentFormation=pFormation;
			
			if(pFormation->sName=="")
			{
				char pTempName[MAX_PATH]={0};
				sprintf(pTempName,"Unnamed_Formation %d",dwUnnamedObjectIndex);
				dwUnnamedObjectIndex++;
				pFormation->sName=pTempName;
			}

			m_mFormations[pFormation->sName]=pFormation;
		}
		break;
	case GES_NODE_CONTROLLER:
		{
			SGESController *pController=new SGESController();
			pController->sName=pNode->GetString("Nombre");
			pController->sClass=pNode->GetString("Clase");
			pController->vPosition=From3DSToOpenGL(pNode->GetVector("Posicion"));
			pController->sActivationType=pNode->GetString("TipoActivacion");
			pController->sDeactivationType=pNode->GetString("TipoDesactivacion");
			m_mControllers[pController->sName]=pController;
		}
		break;
	case GES_NODE_EFFECT_SMOKE:
		{
			m_SmokeConfig.sTexture=pNode->GetString("Textura");
		}
		break;
	case GES_NODE_EFFECT_EXPLOSION:
		{
			m_ExplosionConfig.sModel=pNode->GetString("Modelo");
		}
		break;
	case GES_NODE_WEAPON_MACHINEGUN_CONFIG:
		{
			m_MachineGunConfig.dInterval=pNode->GetFloat("Intervalo");
			m_MachineGunConfig.sProjectileModel=pNode->GetString("ModeloProyectil");
			m_MachineGunConfig.dProjectileCount=pNode->GetFloat("NumeroProyectiles");
		}
		break;
	case GES_NODE_WEAPON_MISSILES_CONFIG:
		{
			m_MissilesConfig.dInterval=pNode->GetFloat("Intervalo");
			m_MissilesConfig.sProjectileModel=pNode->GetString("ModeloProyectil");
		}
		break;
	}
	multimap<string,SGESNode*>::iterator iSubNode;
	for(iSubNode=pNode->mNodes.begin();iSubNode!=pNode->mNodes.end();iSubNode++)
	{
		SGESNode *pSubNode=iSubNode->second;
		ReadNodeRecursive(pSubNode,dVersion);
	}
}

void CGESFileType::SaveValueByte(string sName,unsigned char	cValue){char sTemp[512]={0};sprintf(sTemp,"%s=%d",sName.c_str(),cValue);SaveLine(sTemp);}
void CGESFileType::SaveValueFloat(string sName,double fValue){char sTemp[512]={0};sprintf(sTemp,"%s=%f",sName.c_str(),fValue);SaveLine(sTemp);}
void CGESFileType::SaveValueDWord(string sName,DWORD dwValue){char sTemp[512]={0};sprintf(sTemp,"%s=%d",sName.c_str(),dwValue);SaveLine(sTemp);}
void CGESFileType::SaveValueWord(string sName,WORD wValue){char sTemp[512]={0};sprintf(sTemp,"%s=%d",sName.c_str(),wValue);SaveLine(sTemp);}
void CGESFileType::SaveValueVector(string sName,CVector vValue){char sTemp[512]={0};sprintf(sTemp,"%s=%f,%f,%f",sName.c_str(),vValue.c[0],vValue.c[1],vValue.c[2]);SaveLine(sTemp);}
void CGESFileType::SaveValueColor(string sName,COLORREF cValue){char sTemp[512]={0};sprintf(sTemp,"%s=%d,%d,%d",sName.c_str(),GetRValue(cValue),GetGValue(cValue),GetBValue(cValue));SaveLine(sTemp);}
void CGESFileType::SaveValueString(string sName,string sValue){char sTemp[512]={0};sprintf(sTemp,"%s=\"%s\"",sName.c_str(),sValue.c_str());SaveLine(sTemp);}
void CGESFileType::SaveLine(string sValue)
{
	char sTemp[512]={0};
	memset(sTemp,'\t',m_dwSaveTabCount);
	strcat(sTemp,sValue.c_str());
	strcat(sTemp,"\r\n");
	fwrite(sTemp,strlen(sTemp),1,m_pFile);
}

void CGESFileType::SaveBeginSection(string sName)
{
	SaveLine(sName);
	SaveLine("{");
	m_dwSaveTabCount++;
}

void CGESFileType::SaveEndSection()
{
	m_dwSaveTabCount--;
	SaveLine("}");
}

char *FindGESNodeEnd(char *pBuffer)
{
	int index=0;
	while(pBuffer[index]!='}' && pBuffer[index]!=0)
	{
		if(pBuffer[index]=='{')
		{
			char *pResult=FindGESNodeEnd(pBuffer+index+1);
			index=pResult-pBuffer;
		}
		if(pBuffer[index]=='"')
		{
			do
			{
				index++;
			}
			while(pBuffer[index]!='"' && pBuffer[index]!=0);
		}
		index++;
	}
	if(pBuffer[index]=='}'){index++;}
	return pBuffer+index;
}

