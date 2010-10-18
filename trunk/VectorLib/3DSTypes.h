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
	DWORD		dwMaterialType;
	COLORREF	cAmbientColor;
	COLORREF	cDiffuseColor;
	COLORREF	cSpecularColor;
	char		sName[MAX_PATH];
	char		sFile[MAX_PATH];
	char		sAlphaFile[MAX_PATH];
	float		fShininess;
	float		fShininessStrength;
	float		fTranparency;
	bool		bTwoSided;

	float		fTextureUOffset;
	float		fTextureVOffset;
	float		fTextureUScale;
	float		fTextureVScale;
	float		fTextureAngle;

	bool		bSubMaterial; // Solo para ASE
	DWORD		dwMaterialId; // Solo para ASE
	DWORD		dwSubMaterialId; // Solo para ASE


	S3DSMaterial();
};

struct S3DSObjectMaterial
{
	char		sName[MAX_PATH];
	int			nFaces;
	int			*pFaces;

	S3DSObjectMaterial();
	~S3DSObjectMaterial();
};
struct S3DSTextureFace
{
	int	  nFaceIndex;
	DWORD pTextVertexes[3];

	S3DSTextureFace();
};
struct S3DSColorFace
{
	int	  nFaceIndex;
	DWORD pColorVertexes[3];

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
	DWORD								*pFaceSubMaterials;// solo para archivos ASE
	std::vector<S3DSObjectMaterial*>	 sObjectMaterials;

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
	DWORD								 dwMaterialId; //solo para archivos ASE
	COLORREF							 cWireframeColor;//solo para archivos ASE

	S3DSFrame							 baseFrame;
	std::vector<S3DSFrame*>				 vAnimationFrames;	// frames de la animacion, pueden haber punteros repetidos.
	std::vector<S3DSFrame*>				 vFrameInstances;	// frames alocados (para no borrar punteros duplicados en vAnimationFrames.

	S3DSObject();
	~S3DSObject();
};

struct S3DSLight
{
	char	sName[MAX_PATH];
	DWORD   dwType;

	CMatrix		mTransformMatrix;
	CVector		vAxises[3];
	float		fRotationAngle;
	CVector		vRotationAxis;
	CVector		vScale;

	CVector		vPosition;
	COLORREF	cColor;
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

CVector From3DSToOpenGL(CVector *pV);
CVector From3DSToOpenGL(CVector pV);

CVector FromOpenGLTo3DS(CVector *pV);
CVector FromOpenGLTo3DS(CVector pV);

#endif
