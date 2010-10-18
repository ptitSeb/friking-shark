// GameRender.cpp: implementation of the GameRender class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <float.h>
#include "GameEngine.h"
#include "GameRender.h"
#include "GameScreen.h"
#include "GUIManager.h"
#include "..\VectorLib\3DSFiles.h"

void EntityOperation_RenderMaterial(IEntity *piEntity,void *pParam1,void *pParam2)
{
    piEntity->Render((CMaterial *)pParam1);
}

void EntityOperation_RenderBBox(IEntity *piEntity,void *pParam1,void *pParam2)
{
    CRender *pRender=(CRender *)pParam1;
    SPhysicInfo *pPhysicInfo=piEntity->GetPhysicInfo();
    DWORD dwAlignment=piEntity->GetAlignment();
    switch(dwAlignment)
    {
    case ENTITY_ALIGNMENT_NEUTRAL:
      glColor3d(1,1,1);
      break;
    case ENTITY_ALIGNMENT_PLAYER:
      glColor3d(0,1,0);
      break;
    case ENTITY_ALIGNMENT_ENEMIES:
      glColor3d(1,0,0);
      break;
    default:
      glColor3d(1,1,0);
      break;
    }
    pRender->RenderBBox(pPhysicInfo->vPosition+pPhysicInfo->vMins,pPhysicInfo->vPosition+pPhysicInfo->vMaxs);
}

void EntityOperation_CustomRender(IEntity *piEntity,void *pParam1,void *pParam2)
{
    piEntity->Render((IRender*)pParam1,(ICamera*)pParam2);
}


CRender::CRender()
{
    m_sName="Render";
	m_dwMaxOpenGLSkinWidth=0;
	m_dwMaxOpenGLSkinHeight=0;

	m_dwFlags=RENDER_ENABLE_LIGHTING|RENDER_ENABLE_TEXTURES|RENDER_ENABLE_MODEL_LOD|RENDER_ENABLE_TRANSPARENCY|RENDER_ENABLE_SOLID|RENDER_ENABLE_MIPMAPS;
	EnableFlags(0);
	DisableFlags(RENDER_ENABLE_TEXTURES);

	bool bLighting		=CGlobals::g_IniFile.GetBool("Render\\Iluminacion",(m_dwFlags&RENDER_ENABLE_LIGHTING)!=0);
	bool bEnableWorldLighting	=CGlobals::g_IniFile.GetBool("Render\\Iluminacion Escenario",(m_dwFlags&RENDER_ENABLE_WORLD_LIGHTING)!=0);
	bool bSolido		=CGlobals::g_IniFile.GetBool("Render\\Solido",(m_dwFlags&RENDER_ENABLE_SOLID)!=0);
	bool bTextures		=CGlobals::g_IniFile.GetBool("Render\\Texturas",(m_dwFlags&RENDER_ENABLE_TEXTURES)!=0);
	bool bShowLights	=CGlobals::g_IniFile.GetBool("Render\\Ver Luces",(m_dwFlags&RENDER_SHOW_LIGHTS)!=0);
	bool bShowBBoxes	=CGlobals::g_IniFile.GetBool("Render\\Ver BBoxes",(m_dwFlags&RENDER_SHOW_BBOXES)!=0);
	bool bFog			=CGlobals::g_IniFile.GetBool("Render\\Niebla",(m_dwFlags&RENDER_ENABLE_FOG)!=0);
	bool bAutoNormals	=CGlobals::g_IniFile.GetBool("Render\\AutoNormales",(m_dwFlags&RENDER_ENABLE_AUTO_NORMALS)!=0);
	bool bModelLod		=CGlobals::g_IniFile.GetBool("Render\\LOD",(m_dwFlags&RENDER_ENABLE_MODEL_LOD)!=0);
	bool bTransparency	=CGlobals::g_IniFile.GetBool("Render\\Transparencias",(m_dwFlags&RENDER_ENABLE_TRANSPARENCY)!=0);
	bool bMipMaps		=CGlobals::g_IniFile.GetBool("Render\\MipMaps",(m_dwFlags&RENDER_ENABLE_MIPMAPS)!=0);
	bool bRenderNormals =CGlobals::g_IniFile.GetBool("Render\\Ver Normales",(m_dwFlags&RENDER_SHOW_NORMALS)!=0);

	m_dwMaxOpenGLSkinWidth=CGlobals::g_IniFile.GetInt("Render\\AnchoMaximoTexturas",m_dwMaxOpenGLSkinWidth);
	m_dwMaxOpenGLSkinHeight=CGlobals::g_IniFile.GetInt("Render\\AltoMaximoTexturas",m_dwMaxOpenGLSkinHeight);

	m_dwFlags=0;
	m_dwFlags|=(bSolido)?RENDER_ENABLE_SOLID:0;
	m_dwFlags|=(bLighting)?RENDER_ENABLE_LIGHTING:0;
	m_dwFlags|=(bEnableWorldLighting)?RENDER_ENABLE_WORLD_LIGHTING:0;
	m_dwFlags|=(bTextures)?RENDER_ENABLE_TEXTURES:0;
	m_dwFlags|=(bShowLights)?RENDER_SHOW_LIGHTS:0;
	m_dwFlags|=(bShowBBoxes)?RENDER_SHOW_BBOXES:0;
	m_dwFlags|=(bFog)?RENDER_ENABLE_FOG:0;
	m_dwFlags|=(bAutoNormals)?RENDER_ENABLE_AUTO_NORMALS:0;
	m_dwFlags|=(bModelLod)?RENDER_ENABLE_MODEL_LOD:0;
	m_dwFlags|=(bTransparency)?RENDER_ENABLE_TRANSPARENCY:0;
	m_dwFlags|=(bMipMaps)?RENDER_ENABLE_MIPMAPS:0;
	m_dwFlags|=(bRenderNormals)?RENDER_SHOW_NORMALS:0;

	m_nLights=0;
	m_nDynamicLights=0;
	memset(m_pLights,0,sizeof(m_pLights));
	memset(m_pDynamicLights,0,sizeof(m_pDynamicLights));
}

CRender::~CRender()
{
    Reset();
}

DWORD	CRender::EnableFlags(DWORD dwFlags){m_dwFlags|=dwFlags;return m_dwFlags;}
DWORD	CRender::DisableFlags(DWORD dwFlags){m_dwFlags&=~dwFlags;return m_dwFlags;}
DWORD	CRender::GetFlag(DWORD dwFlag){return m_dwFlags&dwFlag;}
DWORD	CRender::ToggleFlag(DWORD dwFlag){if(GetFlag(dwFlag)){DisableFlags(dwFlag);}else{EnableFlags(dwFlag);}return dwFlag;}

CDetailLevelModel *CRender::GetDetailLevelModel(string sModelName)
{
	map<string,CDetailLevelModel *>::iterator i=m_DetailLevelModels.find(sModelName);
	return i==m_DetailLevelModels.end()?NULL:i->second;
}

void CRender::ResetDetailLevelModels()
{
	map<string,CDetailLevelModel *>::iterator i;
	for(i=m_DetailLevelModels.begin();i!=m_DetailLevelModels.end();i++)
	{
		CDetailLevelModel *pDetailLevelModel=i->second;
		delete pDetailLevelModel;
	}
	m_DetailLevelModels.clear();
}

void CRender::AddDetailLevelModel(string sModelName,CDetailLevelModel *pDetailLevelModel)
{
	_ASSERTE(m_DetailLevelModels.find(sModelName)==m_DetailLevelModels.end());
	m_DetailLevelModels[sModelName]=pDetailLevelModel;
}

int CRender::AddLight(CLight *pLight)
{
	m_pLights[m_nLights]=pLight;
	pLight->m_nOpenGlIndex=m_nLights;
	m_nLights++;
	return pLight->m_nOpenGlIndex;
}

void CRender::ResetLights()
{
	int x;
	for(x=0;x<m_nLights;x++)
	{
		if(m_pLights[x]){delete m_pLights[x];m_pLights[x]=NULL;}
	}
	m_nLights=0;
}

void CRender::Reset()
{
	ResetDetailLevelModels();
	ResetLights();
}

bool ShowWGLExtensions()
{
	const char *pExtensions = (const char *)wglGetExtensionsStringARB(wglGetCurrentDC());
	if(pExtensions==NULL){return false;}

	int nLen=strlen(pExtensions);
	char *pTempBuffer=new char [nLen+1];
	char *pExt=NULL;
	memcpy(pTempBuffer,pExtensions,nLen+1);
	while(pExt=strtok(pExt?NULL:pTempBuffer," "))
	{
		std::string sExtensionName=pExt;
		_MRT("ShowARBExtensions -> Extension '%s' found",pExt);
	}

	delete [] pTempBuffer;
	return true;
}


void CRender::ApplyShadowLighting(int nVertexes,float *pVertexArray,float *pVertexNormals,float *pOriginalColors,float *pColorArray,CVector &vPosition,CVector &vAngles)
{
	int cx=0,ix=0,ic=0,l=0;

	CVector vTempAxis=AxisPosX;
	CMatrix m,r;
	if(vPosition!=Origin)
	{
		m.T(Origin-vPosition);
	}
	if(vAngles.c[YAW]!=0.0)
	{
		r.R(AxisPosY,(0-vAngles.c[YAW])*2.0*PI/360.0,Origin);
		m*=r;
		vTempAxis*=r;
	}
	if(vAngles.c[PITCH]!=0.0)
	{
		r.R(vTempAxis,(0-vAngles.c[PITCH])*2.0*PI/360.0,Origin);
		m*=r;
	}
	CVector vTempLightMins;
	CVector vTempLightMaxs;
	CVector vTempLightCenter;
	for(cx=0;cx<nVertexes;cx++)
	{
		pColorArray[ic]=1;
		pColorArray[ic+1]=1;
		pColorArray[ic+2]=1;
		pColorArray[ic+3]=0;
		ic+=4;
	}
	
	for(l=0,ic=0;l<m_nDynamicLights;l++)
	{
		CDynamicLight *pLight=m_pDynamicLights[l];
		vTempLightCenter=pLight->m_vPosition;
		vTempLightCenter*=m;
		vTempLightMins=vTempLightCenter-CVector(1,1,1)*pLight->m_dRadious;
		vTempLightMaxs=vTempLightCenter+CVector(1,1,1)*pLight->m_dRadious;

		ix=0;
		ic=0;
		for(cx=0;cx<nVertexes;cx++)
		{
			if(pVertexArray[ix]>=vTempLightMins.c[0] && pVertexArray[ix]<=vTempLightMaxs.c[0] &&
				pVertexArray[ix+1]>=vTempLightMins.c[1] && pVertexArray[ix+1]<=vTempLightMaxs.c[1] &&
				pVertexArray[ix+2]>=vTempLightMins.c[2] && pVertexArray[ix+2]<=vTempLightMaxs.c[2])
			{
				CVector vVertex(pVertexArray[ix],pVertexArray[ix+1],pVertexArray[ix+2]);
				CVector vNormal(pVertexNormals[ix],pVertexNormals[ix+1],pVertexNormals[ix+2]);
				CVector vDir=vTempLightCenter-vVertex;
				double dVertexToLightDistance=vDir.N();
				double dCos=vDir*vNormal;
				if(dCos>0)
				{
					double dtempcol=(pLight->m_dRadious-dVertexToLightDistance)/pLight->m_dRadious;
					dtempcol*=dCos;
					if(dtempcol>0)
					{
						pColorArray[ic+3]+=dtempcol;
					}
				}
			}
			ix+=3;
			ic+=4;
		}
	}
	/*for(ic=0,cx=0;cx<nVertexes;cx++)
	{
		double d1,d2;
		//d1=pOriginalColors[ic]+pOriginalColors[ic+1]+pOriginalColors[ic+2];
		d2=pColorArray[ic]+pColorArray[ic+1]+pColorArray[ic+2];
		pColorArray[ic+3]=d2/3;
		ic+=4;
	}*/
}

void CRender::Render()
{
	glDisable(GL_NORMALIZE);

	if(m_dwFlags&RENDER_ENABLE_SOLID)		{glPolygonMode(GL_FRONT,GL_FILL);}else{glPolygonMode(GL_FRONT,GL_LINE);}
	if(m_dwFlags&RENDER_ENABLE_TEXTURES)	{glEnable(GL_TEXTURE_2D);}else{glDisable(GL_TEXTURE_2D);}
	if(m_dwFlags&RENDER_ENABLE_AUTO_NORMALS){glEnable(GL_AUTO_NORMAL);}else{glDisable(GL_AUTO_NORMAL);}

	if(m_dwFlags&RENDER_ENABLE_FOG)
	{
		glEnable(GL_FOG);
		glFogf(GL_FOG_START,m_dFogStart);
		glFogf(GL_FOG_END,m_dFogEnd);
		glFogfv(GL_FOG_COLOR,m_vFogColor);
	}
	else
	{
		glDisable(GL_FOG);
	}

	if(m_dwFlags&RENDER_ENABLE_LIGHTING && m_nLights)
	{
		int ligths=0;
		glGetIntegerv(GL_MAX_LIGHTS,&ligths);
		float ambient[]={0.0,0.0,0.0};
		glEnable(GL_LIGHTING);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambient);
		glLightModelf(GL_LIGHT_MODEL_TWO_SIDE ,0.0);
		glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER ,1.0);

		for(int x=0;x<m_nLights;x++)
		{
			int lIndex=GL_LIGHT0+x;
			glDisable(lIndex);
			CLight *pLight=m_pLights[x];
			if(!pLight->m_bIsActive)
			{continue;}
			float vColor[4]={0},vPos[4]={0},vDir[3]={0};
			RGBToFloat(pLight->m_cColor,vColor);
			vColor[3]=1.0;
			vPos[0]=pLight->m_vPosition[0];
			vPos[1]=pLight->m_vPosition[1];
			vPos[2]=pLight->m_vPosition[2];
			vPos[3]=pLight->m_dwType==LIGHT_TYPE_AMBIENT?0.0:1.0;
			vPos[3]=1.0;
			vDir[0]=pLight->m_vSpotDir[0];
			vDir[1]=pLight->m_vSpotDir[1];
			vDir[2]=pLight->m_vSpotDir[2];

			glLightfv(lIndex,GL_POSITION,vPos);
			glLightfv(lIndex,GL_AMBIENT,vColor);
			glLightfv(lIndex,GL_DIFFUSE ,vColor);
			glLightfv(lIndex,GL_SPECULAR,vColor);

			if(pLight->m_dwType==LIGHT_TYPE_OMNI)
			{
				glLightf(lIndex,GL_CONSTANT_ATTENUATION,pLight->m_fAttenuationConstant*1);
				glLightf(lIndex,GL_LINEAR_ATTENUATION,pLight->m_fAttenuationLinear*3);
				glLightf(lIndex,GL_QUADRATIC_ATTENUATION,pLight->m_fAttenuationQuadratic*9);
			}
			if(pLight->m_dwType==LIGHT_TYPE_SPOT)
			{
				glLightf(lIndex,GL_CONSTANT_ATTENUATION,pLight->m_fAttenuationConstant);
				glLightf(lIndex,GL_LINEAR_ATTENUATION,pLight->m_fAttenuationLinear);
				glLightf(lIndex,GL_QUADRATIC_ATTENUATION,pLight->m_fAttenuationQuadratic);

				glLightfv(lIndex,GL_SPOT_DIRECTION	,vDir);
				glLightf(lIndex,GL_SPOT_CUTOFF		,pLight->m_fSpotAngle);
				glLightf(lIndex,GL_SPOT_EXPONENT	,pLight->m_fSpotExponent);
			}
			glEnable(lIndex);
		}
	}
	else
	{
		glDisable(GL_LIGHTING);
	}

	ApplyDynamicLightingEntityTracking();

	list<IEntity *>::iterator i;
	deque<CMaterial*>::iterator iMaterial;

	int x,materialCount=CGlobals::g_pResourceStore->GetMaterialCount();
	for(x=0;x<materialCount;x++)
	{
		CMaterial *pMaterial=CGlobals::g_pResourceStore->GetMaterialAt(x);
		bool bTransparent=pMaterial->fOpacity!=1.0 || (pMaterial->pTexture && pMaterial->pTexture->m_dwColorType==GL_RGBA);
		if(!bTransparent)
		{
			PrepareMaterial(pMaterial);
            CGlobals::g_pEntityManager->PerformUnaryOperation(EntityOperation_RenderMaterial,pMaterial,NULL);
			UnPrepareMaterial(pMaterial);
		}
	}
	if(CGlobals::g_pRender->GetFlag(RENDER_ENABLE_TRANSPARENCY))
	{
		glDisable(GL_POLYGON_SMOOTH); 
		glEnable(GL_ALPHA_TEST);
		glEnable(GL_BLEND);
		glAlphaFunc(GL_GREATER,0);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
    for(x=0;x<materialCount;x++)
    {
		CMaterial *pMaterial=CGlobals::g_pResourceStore->GetMaterialAt(x);
        bool bTransparent=pMaterial->fOpacity!=1.0 || (pMaterial->pTexture && pMaterial->pTexture->m_dwColorType==GL_RGBA);
        if(bTransparent)
        {
            PrepareMaterial(pMaterial);
            CGlobals::g_pEntityManager->PerformUnaryOperation(EntityOperation_RenderMaterial,pMaterial,NULL);
            UnPrepareMaterial(pMaterial);
        }
    }
	if(CGlobals::g_pRender->GetFlag(RENDER_ENABLE_TRANSPARENCY))
	{
		glDisable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);
	}
    if(m_dwFlags&RENDER_SHOW_BBOXES)
    {
      glPushAttrib(GL_ALL_ATTRIB_BITS);
      glDisable(GL_TEXTURE_2D);
      glDisable(GL_LIGHTING);
      CGlobals::g_pEntityManager->PerformUnaryOperation(EntityOperation_RenderBBox,this,NULL);
      glPopAttrib();
    }
    CGlobals::g_pEntityManager->PerformUnaryOperation(EntityOperation_CustomRender,CGlobals::g_pRender,CGlobals::g_pScreen->GetCamera());
    
/*
	if(m_dwFlags&RENDER_SHOW_BBOXES)
	{
        SPlayAreaInfo playArea=CGlobals::g_pPlayAreaManager->GetPlayArea();

		bool bValidArea=false;
		for(int p=0;p<6;p++)
		{
			if( playArea.planes[p].c[0]!=0 || playArea.planes[p].c[1]!=0 ||
				playArea.planes[p].c[2]!=0 || playArea.planes[p].d!=0)
			{
				bValidArea=true;
				break;
			}
		}
		if(bValidArea)
		{
		
			CPolyhedron *pPolyhedron=PolyhedronFromConvexRegion(6,playArea.planes);

			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glDisable(GL_TEXTURE_2D);

			DWORD stipple=0x8888;
			glDisable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
			glEnable(GL_LINE_STIPPLE);
			glLineStipple(1,0x8888);

			glColor3d(0.7,0,0.0);

			for(int x=0;x<pPolyhedron->m_vPolygons.size();x++)
			{
				CPolygon *pPolygon=pPolyhedron->m_vPolygons[x];
				glPointSize(4);
				glBegin(GL_POINTS);
				for(int y=0;y<pPolygon->m_nVertexes;y++)
				{
					glVertex3f(pPolygon->m_pVertexes[y].c[0],pPolygon->m_pVertexes[y].c[1],pPolygon->m_pVertexes[y].c[2]);
				}
				glEnd();
				glBegin(GL_POLYGON);
				for(y=0;y<pPolygon->m_nVertexes;y++)
				{
					glVertex3f(pPolygon->m_pVertexes[y].c[0],pPolygon->m_pVertexes[y].c[1],pPolygon->m_pVertexes[y].c[2]);
				}
				glEnd();
			}

			glPopAttrib();
			glEnable(GL_TEXTURE_2D);
			delete pPolyhedron;
		}
	}
*/
	if(m_dwFlags&RENDER_SHOW_LIGHTS)
	{
		glDisable(GL_FOG);
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);

		for(int x=0;x<m_nLights;x++)
		{
			CLight *pLight=m_pLights[x];
			COLORREF cColor=m_pLights[x]->m_cColor;
			if(!pLight->m_bIsActive)
			{
				cColor=RGB(120,120,120);
			}
			float c[4]={0,0,0,1};
			RGBToFloat(cColor,c);
			glColor4fv(c);
			CVector vTemp=pLight->m_vPosition+pLight->m_vSpotDir;
			glPointSize(10);
			glBegin(GL_POINTS);
				glVertex3dv(pLight->m_vPosition.c);
			glEnd();
			if(pLight->m_dwType==LIGHT_TYPE_SPOT)
			{
				glLineWidth(2);
				glBegin(GL_LINES);
					glVertex3dv(pLight->m_vPosition.c);
					glVertex3dv(vTemp.c);
				glEnd();
				glPointSize(4);
				glBegin(GL_POINTS);
					glVertex3dv(vTemp.c);
				glEnd();
			}
		}
	}
}

void CRender::PrepareMaterial(CMaterial *pMaterial)
{
	glCallList(pMaterial->nList);
	if(pMaterial->bTwoSided)
	{
		glDisable(GL_CULL_FACE);
	}
}

void CRender::UnPrepareMaterial(CMaterial *pMaterial)
{
	if(pMaterial->bTwoSided)
	{
		glEnable(GL_CULL_FACE);
	}
}

void CRender::RenderBuffer(CPreprocessedRenderBuffer *pBuffer,CVector vPosition,CVector vAngles,bool bApplyShadows)
{
	int tex=0,vert=0;
	if(pBuffer->nBufferObject)
	{
		int nDataPerVertex=3;
		if(pBuffer->pTexVertexArray){nDataPerVertex+=2;}
		if(pBuffer->pNormalArray){nDataPerVertex+=3;}
		if(pBuffer->pColorArray){nDataPerVertex+=4;}

		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER,pBuffer->nBufferObject);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,pBuffer->nIndexesBufferObject);
		glVertexPointer(3,GL_FLOAT,0,NULL);
		glClientActiveTextureARB(GL_TEXTURE0_ARB);
		if(pBuffer->pTexVertexArray)
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2,GL_FLOAT,0,(void*)(pBuffer->nVertexes*sizeof(GLfloat)*3));
		}
		glDrawElements(GL_TRIANGLES,pBuffer->nPolygons*3,GL_UNSIGNED_INT,NULL);
		glBindBuffer(GL_ARRAY_BUFFER,0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	}
	else
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3,GL_FLOAT,0,pBuffer->pVertexArray);
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT,0,pBuffer->pNormalArray);
		glClientActiveTextureARB(GL_TEXTURE0_ARB);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2,GL_FLOAT,0,pBuffer->pTexVertexArray);
		glDrawElements(GL_TRIANGLES,pBuffer->nPolygons*3,GL_UNSIGNED_INT,pBuffer->pIndexes);
	}
	if(m_dwFlags&RENDER_SHOW_NORMALS){RenderBufferNormals(pBuffer);}
}

void CRender::RenderBufferShadow(CPreprocessedRenderBuffer *pBuffer)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,pBuffer->pVertexArray);
 	glDrawElements(GL_TRIANGLES,pBuffer->nPolygons*3,GL_UNSIGNED_INT,pBuffer->pIndexes);
}

void CRender::RenderBufferNormals(CPreprocessedRenderBuffer *pBuffer)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_TEXTURE_2D);
	glColor3f(1,1,1);
	glBegin(GL_LINES);
	for(int y=0;y<pBuffer->nVertexes;y++)
	{
		GLfloat tempVertex[3];
		tempVertex[0]=pBuffer->pVertexArray[y*3]+pBuffer->pNormalArray[y*3];
		tempVertex[1]=pBuffer->pVertexArray[y*3+1]+pBuffer->pNormalArray[y*3+1];
		tempVertex[2]=pBuffer->pVertexArray[y*3+2]+pBuffer->pNormalArray[y*3+2];
		glVertex3fv(&pBuffer->pVertexArray[y*3]);
		glVertex3fv(tempVertex);       
	}

	glEnd();
	glPopAttrib();
}


void CRender::RenderTextureRect(CTexture *pTexture,CVector vCenter,CVector vAxis1,CVector vAxis2,double s1,double s2)
{
	RenderTextureRect(pTexture->m_nOpenGlIndex,vCenter,vAxis1,vAxis2,s1,s2);
}

void CRender::RenderTextureRect(int nTextureIndex,CVector vCenter,CVector vAxis1,CVector vAxis2,double s1,double s2)
{
	glBindTexture(GL_TEXTURE_2D,nTextureIndex);
	glBegin(GL_QUADS);
	glTexCoord2f(1,1);
	glVertex3dv((vCenter+vAxis1*(s1/2.0)+vAxis2*(s2/2.0)).c);
	glTexCoord2f(0,1);
	glVertex3dv((vCenter-vAxis1*(s1/2.0)+vAxis2*(s2/2.0)).c);
	glTexCoord2f(0,0);
	glVertex3dv((vCenter-vAxis1*(s1/2.0)-vAxis2*(s2/2.0)).c);
	glTexCoord2f(1,0);
	glVertex3dv((vCenter+vAxis1*(s1/2.0)-vAxis2*(s2/2.0)).c);
	glEnd();
}

void CRender::RenderBBox(CVector vMins,CVector vMaxs)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_TEXTURE_2D);

	DWORD stipple=0x8888;
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(1,0x8888);

	glPointSize(4);
	glBegin(GL_POINTS);
		glVertex3f(vMins.c[0],vMins.c[1],vMins.c[2]);
	glEnd();
	glPointSize(8);
	glBegin(GL_POINTS);
		glVertex3f(vMaxs.c[0],vMaxs.c[1],vMaxs.c[2]);
	glEnd();
	glBegin(GL_QUADS);
		// Front

		// Front
		glVertex3f(vMaxs.c[0],vMins.c[1],vMins.c[2]);
		glVertex3f(vMaxs.c[0],vMins.c[1],vMaxs.c[2]);
		glVertex3f(vMins.c[0],vMins.c[1],vMaxs.c[2]);
		glVertex3f(vMins.c[0],vMins.c[1],vMins.c[2]);

		// Top
		glVertex3f(vMaxs.c[0],vMins.c[1],vMaxs.c[2]);
		glVertex3f(vMaxs.c[0],vMaxs.c[1],vMaxs.c[2]);
		glVertex3f(vMins.c[0],vMaxs.c[1],vMaxs.c[2]);
		glVertex3f(vMins.c[0],vMins.c[1],vMaxs.c[2]);

		// Bottom
		glVertex3f(vMins.c[0],vMins.c[1],vMins.c[2]);
		glVertex3f(vMins.c[0],vMaxs.c[1],vMins.c[2]);
		glVertex3f(vMaxs.c[0],vMaxs.c[1],vMins.c[2]);
		glVertex3f(vMaxs.c[0],vMins.c[1],vMins.c[2]);

		// Left
		glVertex3f(vMins.c[0],vMins.c[1],vMins.c[2]);
		glVertex3f(vMins.c[0],vMins.c[1],vMaxs.c[2]);
		glVertex3f(vMins.c[0],vMaxs.c[1],vMaxs.c[2]);
		glVertex3f(vMins.c[0],vMaxs.c[1],vMins.c[2]);

		// Right
		glVertex3f(vMaxs.c[0],vMaxs.c[1],vMins.c[2]);
		glVertex3f(vMaxs.c[0],vMaxs.c[1],vMaxs.c[2]);
		glVertex3f(vMaxs.c[0],vMins.c[1],vMaxs.c[2]);
		glVertex3f(vMaxs.c[0],vMins.c[1],vMins.c[2]);

		// Back
		glVertex3f(vMins.c[0],vMaxs.c[1],vMins.c[2]);
		glVertex3f(vMins.c[0],vMaxs.c[1],vMaxs.c[2]);
		glVertex3f(vMaxs.c[0],vMaxs.c[1],vMaxs.c[2]);
		glVertex3f(vMaxs.c[0],vMaxs.c[1],vMins.c[2]);


	glEnd();
	glPopAttrib();
	glEnable(GL_TEXTURE_2D);
}

CDynamicLight::CDynamicLight()
{
	m_dRadious=1.0;
	m_nIndex=0;
	m_pEntityToTrack=NULL;
}

void CDynamicLight::SetSphericalLight(CVector cColor,CVector vPosition,double dRadious)
{
	m_cColor=cColor;
	m_vPosition=vPosition;
	m_dRadious=dRadious;
	m_vMins=m_vPosition-CVector(1,1,1)*dRadious;
	m_vMaxs=m_vPosition+CVector(1,1,1)*dRadious;
}

void CRender::ApplyLighting(int nVertexes,float *pVertexArray,float *pVertexNormals,float *pColorArray,CVector &vPosition,CVector &vAngles)
{
	if(!m_nDynamicLights){return;}

	int cx=0,ix=0,ic=0;

	CVector vTempAxis=AxisPosX;
	CMatrix m,r;
	if(vPosition!=Origin)
	{
		m.T(Origin-vPosition);
	}
	if(vAngles.c[YAW]!=0.0)
	{
		r.R(AxisPosY,(0-vAngles.c[YAW])*2.0*PI/360.0,Origin);
		m*=r;
		vTempAxis*=r;
	}
	if(vAngles.c[PITCH]!=0.0)
	{
		r.R(vTempAxis,(0-vAngles.c[PITCH])*2.0*PI/360.0,Origin);
		m*=r;
	}
	CVector vTempLightMins;
	CVector vTempLightMaxs;
	CVector vTempLightCenter;

	for(int l=0;l<m_nDynamicLights;l++)
	{
		CDynamicLight *pLight=m_pDynamicLights[l];
		vTempLightCenter=pLight->m_vPosition;
		vTempLightCenter*=m;
		vTempLightMins=vTempLightCenter-CVector(1,1,1)*pLight->m_dRadious;
		vTempLightMaxs=vTempLightCenter+CVector(1,1,1)*pLight->m_dRadious;

		ix=0;
		ic=0;
		for(cx=0;cx<nVertexes;cx++)
		{
			if(pVertexArray[ix]>=vTempLightMins.c[0] && pVertexArray[ix]<=vTempLightMaxs.c[0] &&
				pVertexArray[ix+1]>=vTempLightMins.c[1] && pVertexArray[ix+1]<=vTempLightMaxs.c[1] &&
				pVertexArray[ix+2]>=vTempLightMins.c[2] && pVertexArray[ix+2]<=vTempLightMaxs.c[2])
			{
				CVector vVertex(pVertexArray[ix],pVertexArray[ix+1],pVertexArray[ix+2]);
				CVector vNormal(pVertexNormals[ix],pVertexNormals[ix+1],pVertexNormals[ix+2]);
				CVector vDir=vTempLightCenter-vVertex;
				double dVertexToLightDistance=vDir.N();
				double dCos=vDir*vNormal;
				if(dCos>0)
				{
					double dtempcol=(pLight->m_dRadious-dVertexToLightDistance)/pLight->m_dRadious;
					dtempcol*=dCos;
					if(dtempcol>0)
					{
            pColorArray[ic]+=pLight->m_cColor.c[0]*dtempcol;
            pColorArray[ic+1]+=pLight->m_cColor.c[1]*dtempcol;
            pColorArray[ic+2]+=pLight->m_cColor.c[2]*dtempcol;
          }
				}
			}
			ix+=3;
			ic+=4;
		}
	}
}

IDynamicLight *CRender::AllocDynamicLight()
{
	if(m_nDynamicLights>=MAX_DYNAMIC_LIGHTS)
	{
		LOG_ERROR_AND_EXIT("Error","Se ha superado el maximo de %d luces dinamicas",m_nDynamicLights);
	}

	CDynamicLight *pDynamicLight=new CDynamicLight;
	pDynamicLight->m_nIndex=m_nDynamicLights;
	m_pDynamicLights[m_nDynamicLights]=pDynamicLight;
	m_nDynamicLights++;
	return pDynamicLight;
}

void CRender::FreeDynamicLight(IDynamicLight *piDynamicLight)
{
    CDynamicLight *pDynamicLight=dynamic_cast<CDynamicLight *>(piDynamicLight);
	int index=pDynamicLight->m_nIndex;
	m_pDynamicLights[index]=NULL;
	m_nDynamicLights--;
	// si no era la ultima y quedan luces se intercambia la ultima con esta y se actualiza el indice
	if(index<m_nDynamicLights)
	{
		m_pDynamicLights[index]=m_pDynamicLights[m_nDynamicLights];
		m_pDynamicLights[index]->m_nIndex=index;
		m_pDynamicLights[m_nDynamicLights]=NULL;
	}
	delete pDynamicLight;
}

void CRender::ApplyDynamicLightingEntityTracking()
{
	for(int l=0;l<m_nDynamicLights;l++)
	{
		CDynamicLight *pLight=m_pDynamicLights[l];
		if(pLight->m_pEntityToTrack)
		{
			pLight->m_vPosition=pLight->m_pEntityToTrack->GetPhysicInfo()->vPosition;
		}
	}
}

void CRender::ConfigureFog(double dStart,double dEnd,CVector vColor)
{
    m_dFogStart=dStart;
    m_dFogEnd=dEnd;
	m_vFogColor[0]=vColor.c[0];
	m_vFogColor[1]=vColor.c[1];
	m_vFogColor[2]=vColor.c[2];
	m_vFogColor[3]=1.0;
}

void	CRender::PrepareOpenGLLighting(IEntity *piEntity)
{
  GLint maxLights=0;
  float ambient[]={0.0,0.0,0.0};
  SPhysicInfo *pPhysicInfo=piEntity->GetPhysicInfo();

  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glEnable(GL_LIGHTING);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambient);
  glLightModelf(GL_LIGHT_MODEL_TWO_SIDE ,0.0);
  glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER ,1.0);
  glGetIntegerv(GL_MAX_LIGHTS,&maxLights);

  int lIndex=GL_LIGHT0+m_nLights;
  for(int x=0;lIndex<GL_LIGHT0+maxLights && x<m_nDynamicLights;x++)
  {
    bool bInLightRange=true;
    CDynamicLight *pLight=m_pDynamicLights[x];
    for(int y=0;y<3;y++)
    {
      if(pLight->m_vMins.c[y]>(pPhysicInfo->vPosition.c[y]+pPhysicInfo->vMaxs.c[y]) || 
         pLight->m_vMaxs.c[y]<(pPhysicInfo->vPosition.c[y]+pPhysicInfo->vMins.c[y]))
      {
        bInLightRange=false;
        break;
      }
    }
    if(bInLightRange)
    {
      glDisable(lIndex);
    
      CVector color=pLight->GetColor(),position=pLight->GetPosition();
      float vColor[4]={0},vPos[4]={0},vDir[3]={0};
      vColor[0]=color.c[0];
      vColor[1]=color.c[1];
      vColor[2]=color.c[2];
      vColor[3]=1.0;
      vPos[0]=position.c[0];
      vPos[1]=position.c[1];
      vPos[2]=position.c[2];
      vPos[3]=1.0;

      glLightfv(lIndex,GL_POSITION,vPos);
      glLightfv(lIndex,GL_AMBIENT,vColor);
      glLightfv(lIndex,GL_DIFFUSE ,vColor);
      glLightfv(lIndex,GL_SPECULAR,vColor);

      glLightf(lIndex,GL_CONSTANT_ATTENUATION,1.0/pLight->GetRadious());
      glLightf(lIndex,GL_LINEAR_ATTENUATION,1.0/pLight->GetRadious());
      glLightf(lIndex,GL_QUADRATIC_ATTENUATION,1.0/pLight->GetRadious());
      glEnable(lIndex);
      lIndex++;
    }
  }
}

void	CRender::UnPrepareOpenGLLighting(IEntity *piEntity)
{
  GLint maxLights=0;

  glGetIntegerv(GL_MAX_LIGHTS,&maxLights);
  for(int x=m_nLights;x<maxLights && x<m_nDynamicLights+m_nLights;x++)
  {
    int lIndex=GL_LIGHT0+x;
    glDisable(lIndex);
  }
  glPopAttrib();
}
