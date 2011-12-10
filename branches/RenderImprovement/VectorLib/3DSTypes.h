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


#if !defined(AFX_3DSTYPES_H__F896600F_BB9E_4A01_81D9_84C4F67C0C23__INCLUDED_)
#define AFX_3DSTYPES_H__F896600F_BB9E_4A01_81D9_84C4F67C0C23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define S3DS_LIGHT_TYPE_FREE	1
#define S3DS_LIGHT_TYPE_SPOT	2
#define S3DS_LIGHT_TYPE_OMNI	3

struct S3DSMaterial
{
	CVector			vAmbientColor;
	CVector			vDiffuseColor;
	CVector 		vSpecularColor;
	char			sName[MAX_PATH];
	char			sFile[MAX_PATH];
	char			sNormalFile[MAX_PATH];
	char			sAlphaFile[MAX_PATH];
	float			fShininess;
	float			fShininessStrength;
	float			fTranparency;
	bool			bTwoSided;

	float			fTextureUOffset;
	float			fTextureVOffset;
	float			fTextureUScale;
	float			fTextureVScale;
	float			fTextureAngle;

	bool			bSubMaterial; // Solo para ASE
	unsigned int	dwMaterialId; // Solo para ASE
	unsigned int	dwSubMaterialId; // Solo para ASE


	S3DSMaterial();
};

struct S3DSObjectMaterial
{
	char		sName[MAX_PATH];
	int			nFaces;
	int			*pFaces;
	unsigned int dwSubMaterialId;

	S3DSObjectMaterial();
	~S3DSObjectMaterial();
};
struct S3DSTextureFace
{
	int	  nFaceIndex;
	unsigned int pTextVertexes[3];

	S3DSTextureFace();
};
struct S3DSColorFace
{
	int	  nFaceIndex;
	unsigned int pColorVertexes[3];

	S3DSColorFace();
};
struct S3DSScene
{
	double	dFirstFrame;
	double	dLastFrame;
	double	dFrameSpeed;
	double	dTicksPerFrame;
	int		nFrameCount;
	S3DSScene();
};

struct S3DSFrame
{
	double								 dTimeStamp;
	int									 nVertexes;
	CVector								*pVertexes;
	CVector								*pVertexNormals;
	int									 nTextVertexes;
	CVector								*pTextVertexes;
	int									 nColorVertexes;
	CVector								*pColorVertexes;
	int									 nFaces;
	int									*pFaces;
	bool								*pEdges;
	int									 nTextFaces;
	S3DSTextureFace						*pTextFaces;
	int									 nColorFaces;
	S3DSColorFace						*pColorFaces;
	unsigned int						*pFaceSubMaterials;// solo para archivos ASE
	std::vector<S3DSObjectMaterial*>	 sObjectMaterials;
	bool								*pbFaceSmooth;
	CVector								*pFaceNormals;
	
	S3DSFrame();
	~S3DSFrame();
};

struct S3DSObject
{
	char								 sName[MAX_PATH];
	bool								 bVisible;
	CMatrix								 mTransformMatrix;
	bool								 bHasFirstFrameInfo;
	CVector								 vOrigin;
	CVector								 vAxises[3];
	CVector								 vPivotPos;
	CVector								 vFirstFramePos;
	CVector								 vFirstFrameScale;
	float								 fFirstFrameRotationAngle;
	CVector								 vFirstFrameRotationAxis;
	unsigned int						 dwMaterialId; //solo para archivos ASE
	CVector								 vWireframeColor;//solo para archivos ASE

	S3DSFrame							 baseFrame;
	std::vector<S3DSFrame*>				 vAnimationFrames;	// frames de la animacion, pueden haber punteros repetidos.
	std::vector<S3DSFrame*>				 vFrameInstances;	// frames alocados (para no borrar punteros duplicados en vAnimationFrames.

	S3DSObject();
	~S3DSObject();
};

struct S3DSLight
{
	char			sName[MAX_PATH];
	unsigned int   dwType;

	CMatrix		mTransformMatrix;
	CVector		vAxises[3];
	float		fRotationAngle;
	CVector		vRotationAxis;
	CVector		vScale;

	CVector		vPosition;
	CVector		vColor;
	float		fMultiplier;
	bool		bAttenuationOn;
	float		fAttenuationStart;
	float		fAttenuationEnd;

	CVector		vSpotTarget;
	float		fSpotHotSpot;
	float		fSpotFallOff;
	bool		bIsOff;
	bool		bHasTarget;
	float		fTargetDist;
	
	S3DSLight();
};

struct S3DSCamera
{
	char	sName[MAX_PATH];
	CVector vPosition;
	CVector vTarget;
	float	fRoll;
	float	fLens;

	S3DSCamera();
};

class C3DSFileTypeBase
{
protected:

	S3DSObject *FindObject(const char *sFileName);

	virtual void Close();

public:

	S3DSScene					m_Scene;
	std::vector<S3DSObject *>	m_vObjects;
	std::vector<S3DSMaterial *> m_vMaterials;
	std::vector<S3DSLight *>	m_vLights;
	std::vector<S3DSCamera *>	m_vCameras;

	virtual bool Open(const char *pFile)=0;

	C3DSFileTypeBase();
	virtual ~C3DSFileTypeBase();
};

// Esta clase solo es un tag para poder persistir valores en el sistema de referencia de 3DS de forma transparente
class C3DSVector:public CVector
{
public:
	inline C3DSVector(double c0, double c1, double c2){c[0]=c0;c[1]=c1;c[2]=c2;}
	inline C3DSVector(const CVector &v){c[0]=v.c[0];c[1]=v.c[1];c[2]=v.c[2];}
	inline C3DSVector(){}
};

CVector From3DSToOpenGL(CVector *pV);
CVector From3DSToOpenGL(CVector pV);

CVector FromOpenGLTo3DS(CVector *pV);
CVector FromOpenGLTo3DS(CVector pV);

bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<C3DSVector> *pItem);
bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<C3DSVector> *pItem);
bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<C3DSVector> *pItem);
void MRPersistencyInitialize(CMRPersistentReferenceT<C3DSVector> *pItem);
void MRPersistencyFree(CMRPersistentReferenceT<C3DSVector> *pItem);

#endif
