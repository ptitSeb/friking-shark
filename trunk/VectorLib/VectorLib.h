// VectorLib.h: interface for the VectorLib class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VECTORLIB_H__B628EDC0_7542_4E57_9003_82D1A3DFD6FC__INCLUDED_)
#define AFX_VECTORLIB_H__B628EDC0_7542_4E57_9003_82D1A3DFD6FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning ( disable : 4786 )

#include <windows.h>
#include <math.h>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <deque>
#include <string>

#define FP_PRECISION	0.0002
#define PI				3.1415926535

class CMatrix;
class CBSPDrawNode;

class CVector
{
public:
	char _debugtag_;

	double c[3];

	inline CVector operator+(const CVector v)const {CVector r;r.c[0]=c[0]+v.c[0];r.c[1]=c[1]+v.c[1];r.c[2]=c[2]+v.c[2];return r;}
	inline CVector operator+=(const CVector v){c[0]+=v.c[0];c[1]+=v.c[1];c[2]+=v.c[2];return *this;}
	inline CVector operator-(const CVector v)const {CVector r;r.c[0]=c[0]-v.c[0];r.c[1]=c[1]-v.c[1];r.c[2]=c[2]-v.c[2];return r;}
	inline CVector operator-=(const CVector v){c[0]-=v.c[0];c[1]-=v.c[1];c[2]-=v.c[2];return *this;}
	inline double  operator*(const CVector v)const {return c[0]*v.c[0] + c[1]*v.c[1] + c[2]*v.c[2];}
	inline CVector operator*(double d)const {CVector r;r.c[0]=c[0]*d;r.c[1]=c[1]*d;r.c[2]=c[2]*d;return r;}
	inline CVector operator*=(double d){c[0]*=d;c[1]*=d;c[2]*=d;return *this;}
		   CVector operator*=(CMatrix &m);
	inline CVector operator/(double d)const {CVector r;r.c[0]=c[0]/d;r.c[1]=c[1]/d;r.c[2]=c[2]/d;return r;}
	inline CVector operator/=(double d){c[0]/=d;c[1]/=d;c[2]/=d;return *this;}
	inline CVector operator^(const CVector v)const {CVector r(c[1]*v.c[2] - c[2]*v.c[1],c[2]*v.c[0] - c[0]*v.c[2],c[0]*v.c[1] - c[1]*v.c[0]);return r;}
	inline CVector operator^=(const CVector v){c[0] = c[1]*v.c[2] - c[2]*v.c[1];c[1] = c[2]*v.c[0] - c[0]*v.c[2];c[2] = c[0]*v.c[1] - c[1]*v.c[0];return *this;}
	inline CVector operator=(const CVector v){c[0]=v.c[0];c[1]=v.c[1];c[2]=v.c[2];return *this;}
			bool operator==(const CVector v) const ;
			bool operator!=(const CVector v){return !(*this==v);}
	inline double operator[](int index)const {return c[index];}

	inline operator double()const {return sqrt(c[0]*c[0]+c[1]*c[1]+c[2]*c[2]);}
	inline double N(){double mod=(*this);if(mod==0){c[0]=c[1]=c[2]=0;return mod;};c[0]/=mod;c[1]/=mod;c[2]/=mod;return mod;}


	inline CVector(){_debugtag_='V';c[0]=c[1]=c[2]=0.0;}
	inline CVector(const CVector &v){_debugtag_='V';c[0]=v.c[0];c[1]=v.c[1];c[2]=v.c[2];}
	inline CVector(double c0, double c1, double c2){_debugtag_='V';c[0]=c0;c[1]=c1;c[2]=c2;}

	CVector &Maxs(const CVector &v1,const CVector &v2);
	CVector &Mins(const CVector &v1,const CVector &v2);

	static bool CompareWithPrecision(const CVector &v1,const CVector &v2,double precision) ;
	static CVector Parse(const char *pString);
	static std::string ToString(const CVector &vVector,bool bIncludeParenthesis=false);
};

// Esta clase solo es un tag para poder persistir colores RGB, de 0 a 255 y mapearlos directamente
// a colores vector, de 0 a 1
class CRGBColor:public CVector
{
public:
    inline CRGBColor(double c0, double c1, double c2){_debugtag_='C';c[0]=c0;c[1]=c1;c[2]=c2;}
    inline CRGBColor(const CVector &v){_debugtag_='C';c[0]=v.c[0];c[1]=v.c[1];c[2]=v.c[2];}
    inline CRGBColor(){}
};
// Esta clase solo es un tag para poder persistir valores en el sistema de referencia de 3DS de forma transparente
class C3DSVector:public CVector
{
public:
    inline C3DSVector(double c0, double c1, double c2){_debugtag_='C';c[0]=c0;c[1]=c1;c[2]=c2;}
    inline C3DSVector(const CVector &v){_debugtag_='C';c[0]=v.c[0];c[1]=v.c[1];c[2]=v.c[2];}
    inline C3DSVector(){}
};

class CLine
{
public:
	CVector m_Points[2];
};

class CPlane:public CVector 
{
public:
	double d;

	double GetSide(CVector v)
	{
		return ((c[0]*v.c[0] + c[1]*v.c[1] + c[2]*v.c[2])-d);
	}
	void assert();

	bool InSamePlaneAs(const CPlane p);
	bool operator==(const CPlane p);
	bool operator!=(const CPlane p){return !(*this==p);}

	inline	CPlane operator=(const CVector v){c[0]=v.c[0];c[1]=v.c[1];c[2]=v.c[2];N();assert();return *this;}
	inline	CPlane operator=(const CPlane &p){c[0]=p.c[0];c[1]=p.c[1];c[2]=p.c[2];d=p.d;assert();return *this;}
	inline	CPlane operator=(double _d){d=_d;assert();return *this;}

			CPlane(CVector p1,CVector p2,CVector p3);
			CPlane(CVector vNormal,CVector vPoint);
	inline	CPlane(double c1,double c2,double c3,double _d):CVector(c1,c2,c3),d(_d){_debugtag_='P';N();assert();}
	inline	CPlane(CVector v,double _d):CVector(v),d(_d){_debugtag_='P';N();assert();}
	inline	CPlane(){_debugtag_='P';d=0;assert();}
};

#define CONTENT_SOLID		1
#define CONTENT_EMPTY		0
#define CONTENT_NODE		-1

class CTraceInfo
{
public:
	CVector m_vTracePos;
	CPlane  m_vTracePlane;
	double	m_dTraceFraction;
	int		m_nTraceContent;
	bool	m_bTraceHit;

	CTraceInfo(){m_dTraceFraction=0.0;m_nTraceContent=CONTENT_EMPTY;m_bTraceHit=0;}
};

class CBSPNode
{
public:
	char _debugtag_;
	int		content;
	CPlane	plane;
	CBSPDrawNode *m_pDrawNode;

	CBSPNode *pChild[2];
	CBSPNode *pParent;

	virtual int			GetContent(CVector position);
	virtual CTraceInfo	GetTrace(CVector p1,CVector p2,CVector realp1,CVector realp2);
	virtual CTraceInfo	GetObjectTrace(const CVector &p1,const CVector &p2,const CVector &vPosition,const CVector &vAngles,const CVector &vMins,const CVector &vMaxs);

	CBSPNode(CBSPNode *parent,CPlane p,int c,CBSPNode *p1,CBSPNode *p2){_debugtag_='B';pParent=parent;m_pDrawNode=NULL;plane=p;content=c;pChild[0]=p1,pChild[1]=p2;}
	CBSPNode(CBSPNode *parent,CPlane p,int c){_debugtag_='B';pParent=parent;m_pDrawNode=NULL;plane=p;content=c;pChild[0]=NULL,pChild[1]=NULL;}
	CBSPNode(CBSPNode *parent,int c){_debugtag_='B';pParent=parent;m_pDrawNode=NULL;content=c;pChild[0]=NULL,pChild[1]=NULL;}
	CBSPNode(CBSPNode *parent){_debugtag_='B';pParent=parent;m_pDrawNode=NULL;content=CONTENT_SOLID;pChild[0]=NULL,pChild[1]=NULL;}
	virtual ~CBSPNode();
};

extern CVector AxisPosX;
extern CVector AxisPosY;
extern CVector AxisPosZ;
extern CVector AxisNegX;
extern CVector AxisNegY;
extern CVector AxisNegZ;

#define INVERT_POLYGON	1
#define COPY_POLYGON	0

class CPolygon
{
public:
	char _debugtag_;

	CPlane		m_Plane;
	unsigned	m_nVertexes;
	CVector		*m_pVertexes;
	bool		*m_pEdges;

	operator CPlane(){return m_Plane;}

	CPlane	CalcPlane();
	CPolygon &operator=(const CPolygon &Polygon);
	CPolygon &operator+=(const CVector v);
	bool	 operator==(const CPolygon &Polygon);

	static bool	CutCoplanar(CPolygon *pIn,CPolygon *pIn2,std::list<CPolygon *> *pOut);
	static int	Divide(CPlane plane,CPolygon *pIn,CPolygon *pOut1,CPolygon *pOut2,int *pOut1Side=NULL,CLine *pCutSegment=NULL);
	static bool Cut(CPolygon *pIn1,CPolygon *pIn2,int *pnIn1Pols,int *pnIn2Pols,CPolygon *pOutPols);

	CPolygon();
	CPolygon(std::vector<CVector> *pvVertexes);
	CPolygon(CPolygon &Polygon,int nMode=COPY_POLYGON);
	CPolygon(int nVertexes,CVector *pVertexes);
	~CPolygon();
};

#define MATERIAL_SOLID		1
#define MATERIAL_TEXTURED	2

class CTexture
{
public:

	std::string	 m_sName;
	std::string  m_sAlphaFileName;
	bool		 m_bAlphaFile;
	bool		 m_bColorKey;
	COLORREF	 m_cColorKey;
	float		 m_fOpacity;

	unsigned m_nWidth;
	unsigned m_nHeight;
	void	*m_pPixels;
	DWORD	 m_dwColorType;

	CVector GetPixelColor( unsigned long x, unsigned long y );

	unsigned m_nOpenGlIndex;
	CTexture(std::string sName,int nWidth,int nHeight,void *pPixels,DWORD dwColorType);
	~CTexture();
};

struct CMaterial
{
	DWORD		dwMaterialType;
	COLORREF	cAmbientColor;
	COLORREF	cDiffuseColor;
	COLORREF	cSpecularColor;
	int			nTextureIndex;
	int			nList;
	float		fShininess;
	float		fOpacity;
	bool		bTwoSided;
	CTexture    *pTexture;


	CMaterial();
	~CMaterial();

	void CreateList();
	void DeleteList();

	bool operator == (const CMaterial &material);
};

class CMaterialPolygon: public CPolygon
{
public:

	CMaterial	*m_pMaterial;
	CVector		*m_pTextureCoords;
	CVector		*m_pVertexNormals;
	CVector		*m_pVertexColors;

	CMaterialPolygon &operator=(const CPolygon &pol);
	CMaterialPolygon &operator=(const CMaterialPolygon &pol);

	CMaterialPolygon();
	~CMaterialPolygon();
};

class CPolyhedron
{
public:
	char _debugtag_;

	std::vector<CPolygon *> m_vPolygons;

	bool IsInternalPolygon(CPolygon *pPolygon);

	CPolyhedron();
	CPolyhedron(CPolygon base, double width); // extrusion ctor
	~CPolyhedron();
};

class CBSPDrawNode
{
public:
	int								m_nDepth;
	std::map<CPolygon	*,int>		m_mPolygons;
	CBSPNode						*m_pBSPNode;
	CPolygon						*m_pNodePolygon;

	CBSPDrawNode(CBSPNode *pBSPNode);
	~CBSPDrawNode();
};

class CMatrix
{
public:
	
	double e[4][4];
	
	void	Ref(CVector vAxisX,CVector vAxisY,CVector vAxisZ);	// Reference System Change
	void	R(CVector vAxis,double dAngle,CVector vOffset);		// Rotate
	void	R(CVector vAxis,double dAngle);						// Rotate
	void	T(CVector vOffset);									// Traslate
	void	S(double x,double y,double z,CVector vOffset);		// Scale
	void	I();												// Identity
	double	D();												// Determinant

	double Adjunt(int r,int c);
	void Adjunt();
	void Transpose();
	void Inverse();

	void operator*=(CMatrix &mMatrix);
	
	CMatrix(double pMatrix[4][4]);
	CMatrix(CMatrix &m);
	CMatrix();
	~CMatrix();
};

class CMatrix33
{
public:
	
	double e[3][3];
	
	void	I();				
	void	R(CVector vAxis,double dAngle);		// Rotate
	double	D();
	
	// Identity
	CMatrix33(CVector v0,CVector v1,CVector v2);
	CMatrix33(double pMatrix33[3][3]);
	CMatrix33(CMatrix33 &m);
	CMatrix33();
	~CMatrix33();
};

bool		 IsPointInPolyhedron(CPolyhedron *pPoly,CVector point,double dTolerance);
bool		 IsPointInRegion(int nPlanes,CPlane *pPlanes,CVector point,double dPrecision=FP_PRECISION);
bool		 PointFromPlanes(CPlane plane0,CPlane plane1,CPlane plane2,CVector *pPoint);
CPlane		 PlaneFromSegment(CVector v1,CVector v2,CPlane segmentPlane);
CBSPNode	*BSPFromPolyhedronList(int nDepth,std::list<CPolyhedron*> *pPolys,std::vector<CBSPDrawNode *> *pBSPDrawNodes);
CBSPNode	*BSPFromPolygonVector(CBSPNode *pParent,int nDepth,std::vector<CPolygon*> *pPolys,DWORD dwLeafContentType,std::vector<CBSPDrawNode *> *pBSPDrawNodes,bool bFastGenerationSlowCheck=false);
CBSPNode	*BSPFromConvexPolygon(int nPoints,CVector *pPoints);
CBSPNode	*BSPFromConvexPolygon(std::list<CVector> *pvVectors);
CPolyhedron *PolyhedronFromConvexRegion(int nPlanes,CPlane *pPlanes);
CPlane		PlaneFromSegment(CVector v1,CVector v2,CPlane segmentPlane);
void		AnglesFromVector(const CVector &vForward,double &dYaw,double &dPitch, double &dRoll);
void		AnglesFromVector(const CVector &vForward,CVector *pAngels);
CVector		AnglesFromVector(const CVector &vForward);
void		VectorsFromAngles(const CVector &vAngles,CVector *pvForward,CVector *pvRight=NULL,CVector *pvUp=NULL);
void		VectorsFromAngles(double dYaw,double dPitch, double dRoll,CVector &vForward,CVector &vRight,CVector &vUp);
void		RemoveInternalFaces(std::list<CPolyhedron*> *plOriginalPolyhedron,std::list<CPolyhedron*> *plProcessedPolyhedrons);
bool		CheckBBoxCollision(CPolygon *pIn1,CPolygon *pIn2);
bool		CheckBBoxCollision(CPolyhedron *pIn1,CPolyhedron *pIn2);
double		GetBBoxRadius(const CVector &vMins,const CVector &vMaxs);

void 		CalcBBoxVolume(const CVector &vOrigin,const CVector &vOrientation,const CVector &vMins,const CVector &vMaxs,CVector *pVolumePoints);
void 		CalcCameraPlanes(CVector vPosition,CVector vAngles, double dViewAngle, double dAspectRatio, double dNear, double dFar,CPlane *pvPlanes);
void 		CalcCameraVolume(CVector vPosition,CVector vAngles, double dViewAngle, double dAspectRatio, double dNear, double dFar,CVector *pPoints);
void		ToCameraRef( CPlane *pForwardPlane,CPlane *pRightPlane,CPlane *pUpPlane,int nPoints, CVector *pvPoints);

CVector		CalcMins(CVector &v1,CVector &v2);
CVector		CalcMaxs(CVector &v1,CVector &v2);

void		RGBToFloat(COLORREF cColor,float *pColor);
CVector		RGBToVector(COLORREF cColor);
COLORREF	FloatToRGB(float *pColor);
COLORREF	VectorToRGB(CVector *pColor);
CVector		RGBToHSV(CVector &v1);
CVector		HSVDifference(CVector &v1,CVector &v2);
double		drand();
double		DegreesToRadians(double dAngle);
double		RadiansToDegrees(double dAngle);

void		FromOpenGLMatrix(double *dMatrix,CMatrix *pMatrix);
void		ToOpenGLMatrix(CMatrix *pMatrix,double *dMatrix);

extern CVector AxisPosX;
extern CVector AxisPosY;
extern CVector AxisPosZ;
extern CVector AxisNegX;
extern CVector AxisNegY;
extern CVector AxisNegZ;
extern CVector Origin;

#include "OpenGLViewPortBase.h"
#include "OpenGLExtensions.h"
#include "Utilities.h"
#ifndef VECTOR_LIB_PROYECT
	#include "QuakeFiles.h"
	#include "BmpFiles.h"
	#include "IniFiles.h"
	#include "3DSTypes.h"
	#include "3DSFiles.h"
	#include "ASEFiles.h"
	#include "GBSFiles.h"
#endif

#endif // !defined(AFX_VECTORLIB_H__B628EDC0_7542_4E57_9003_82D1A3DFD6FC__INCLUDED_)
