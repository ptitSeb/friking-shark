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


// VectorLib.h: interface for the VectorLib class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VECTOR_LIB__
#define __VECTOR_LIB__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <deque>
#include <string>

#define FP_PRECISION	0.0002
#define PI				3.1415926535

#define YAW		0
#define PITCH	1
#define ROLL	2

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
	bool Cut(CVector p1,CVector p2, CVector *pCut);

	bool InSamePlaneAs(const CPlane p);
	bool operator==(const CPlane p);
	bool operator!=(const CPlane p){return !(*this==p);}

	inline	CPlane operator=(const CVector v){c[0]=v.c[0];c[1]=v.c[1];c[2]=v.c[2];N();return *this;}
	inline	CPlane operator=(const CPlane &p){c[0]=p.c[0];c[1]=p.c[1];c[2]=p.c[2];d=p.d;return *this;}
	inline	CPlane operator=(double _d){d=_d;return *this;}

			CPlane(CVector p1,CVector p2,CVector p3);
			CPlane(CVector vNormal,CVector vPoint);
	inline	CPlane(double c1,double c2,double c3,double _d):CVector(c1,c2,c3),d(_d){_debugtag_='P';N();}
	inline	CPlane(CVector v,double _d):CVector(v),d(_d){_debugtag_='P';N();}
	inline	CPlane(){_debugtag_='P';d=0;}
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

	virtual int			GetContent(const CVector &position,std::vector<CBSPNode *> *pvNodePath=NULL);
	virtual CTraceInfo	GetTrace(const CVector &p1,const CVector &p2,const CVector &realp1,const CVector &realp2,std::vector<CBSPNode *> *pvNodePath=NULL);
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

#define BSP_DISCARD_INVALID_PLANE 1
#define BSP_DISCARD_VERTEX_COUNT 2
#define BSP_DISCARD_SMALL_SURFACE 3
#define BSP_DISCARD_COPLANAR 4

class CBSPDrawNode
{
public:
	int								m_nDepth;
	std::map<CPolygon	*,int>		m_mPolygons;
	std::map<CPolygon	*,int>		m_mDiscardedPolygons;
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


struct SGameRect;

struct SGamePos
{
	double x;
	double y;
	
	void ClipToUnit();
	void ClipToRect(SGameRect *pRect);
	void CenterOnRect(SGameRect *pRect);
	
	SGamePos(double _x,double _y);
	SGamePos();
};

struct SGameSize
{
	double w;
	double h;
	
	SGameSize(double _w,double _h);
	SGameSize();
};

struct SGameRect
{
	double x;
	double y;
	double w;
	double h;
	
	void ClipToUnit();
	void ClipToRect(SGameRect *pRect);
	void CenterOnRect(SGameRect *pRect);
	
	bool operator==(const SGameRect &rect) const;
	bool operator!=(const SGameRect &rect) const;
	bool Contains(SGamePos point);
	
	SGameRect(double _x,double _y,double _w,double _h);
	SGameRect();
};


bool		 IsPointInPolyhedron(CPolyhedron *pPoly,CVector point,double dTolerance);
bool		 IsPointInRegion(int nPlanes,CPlane *pPlanes,CVector point,double dPrecision=FP_PRECISION);
bool		 PointFromPlanes(CPlane plane0,CPlane plane1,CPlane plane2,CVector *pPoint);
CPlane		 PlaneFromSegment(CVector v1,CVector v2,CPlane segmentPlane);
CBSPNode	*BSPFromPolyhedronList(int nDepth,std::list<CPolyhedron*> *pPolys,std::vector<CBSPDrawNode *> *pBSPDrawNodes);
CBSPNode	*BSPFromPolygonVector(CBSPNode *pParent,int nDepth,std::vector<CPolygon*> *pPolys,unsigned int dwLeafContentType,std::vector<CBSPDrawNode *> *pBSPDrawNodes,bool bFastGenerationSlowCheck=false);
CBSPNode	*BSPFromConvexPolygon(int nPoints,CVector *pPoints);
CBSPNode	*BSPFromConvexPolygon(std::list<CVector> *pvVectors);
CPolyhedron *PolyhedronFromConvexRegion(int nPlanes,CPlane *pPlanes);
CPlane		PlaneFromSegment(CVector v1,CVector v2,CPlane segmentPlane);
void		AnglesFromVector(const CVector &vForward,double &dYaw,double &dPitch);
void		AnglesFromVector(const CVector &vForward,CVector *pAngels);
CVector		AnglesFromVector(const CVector &vForward);
CVector 	AnglesFromVectors(const CVector &vForward,const CVector &vRight,const CVector &vUp);
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

CVector		RGBToHSV(const CVector &v1);
CVector 	HSVToRGB(const CVector &hsv);
CVector		HSVDifference(CVector &v1,CVector &v2);
double		drand();
double		DegreesToRadians(double dAngle);
double		RadiansToDegrees(double dAngle);

void		FromOpenGLMatrix(double *dMatrix,CMatrix *pMatrix);
void		FromOpenGLMatrix(float *dMatrix,CMatrix *pMatrix);
void		ToOpenGLMatrix(CMatrix *pMatrix,double *dMatrix);
void		ToOpenGLMatrix(CMatrix *pMatrix,float *dMatrix);

void 		ComputeReferenceSystem( CVector vRefSysPos,CVector vRefSysAngles,CVector vPosition,CVector vAngles,CVector *pvPosition,CVector *pvAngles,CVector *pvX=NULL,CVector *pvY=NULL,CVector *pvZ=NULL);
void 		ComputePerspectiveMatrix(CMatrix *pMatrix, double fovyInDegrees, double aspectRatio,double znear, double zfar);
void 		ComputeCameraMatrix(CMatrix *pMatrix,const CVector &vPosition,double dYaw, double dPitch, double dRoll);
void 		ComputeOrthographicMatrix(CMatrix *pMatrix, double dLeft, double dRight, double dBottom, double dTop,double dNear, double dFar);
void 		ComputeFrustumMatrix(CMatrix *pMatrix, double left, double right, double bottom, double top,double znear, double zfar);

void 		ComputeTangentBasis(unsigned int nVertexes, float *pVertexes, unsigned int nFaces, unsigned int *pFaces, float *pTextCoords,float *pNormals,float *pTangents, float *pBitangents);

extern CVector AxisPosX;
extern CVector AxisPosY;
extern CVector AxisPosZ;
extern CVector AxisNegX;
extern CVector AxisNegY;
extern CVector AxisNegZ;
extern CVector Origin;
extern CVector ColorWhite;
extern CVector ColorBlack;
extern CVector ColorRed;
extern CVector ColorGreen;
extern CVector ColorBlue;
extern CVector ColorYellow;
extern CVector ColorDarkRed;
extern CVector ColorDarkGreen;
extern CVector ColorDarkBlue;
extern CVector ColorDarkYellow;

bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CVector> *pItem);
bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CVector> *pItem);
bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CVector> *pItem);
void MRPersistencyInitialize(CMRPersistentReferenceT<CVector> *pItem);
void MRPersistencyFree(CMRPersistentReferenceT<CVector> *pItem);

bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CRGBColor> *pItem);
bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CRGBColor> *pItem);
bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CRGBColor> *pItem);
void MRPersistencyInitialize(CMRPersistentReferenceT<CRGBColor> *pItem);
void MRPersistencyFree(CMRPersistentReferenceT<CRGBColor> *pItem);

#include "GBSFiles.h"
#include "3DSTypes.h"
#include "3DSFiles.h"
#include "ASEFiles.h"

#endif
