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


// VectorLib.cpp: implementation of the VectorLib class.
//
//////////////////////////////////////////////////////////////////////

#include "./StdAfx.h"
#include "VectorLib.h"
#include <algorithm>

//#define _DISABLE_OPTIMIZATIONS

CVector AxisPosX(1,0,0);
CVector AxisPosY(0,1,0);
CVector AxisPosZ(0,0,1);
CVector AxisNegX(-1,0,0);
CVector AxisNegY(0,-1,0);
CVector AxisNegZ(0,0,-1);
CVector Origin(0,0,0);

CBSPNode *BSPFromConvexPolygon(CPolygon *pPolygon);

CVector CVector::operator*=(CMatrix &m)
{
	double x1,y1,z1;
	x1=c[0]*m.e[0][0]+c[1]*m.e[0][1]+c[2]*m.e[0][2]+m.e[0][3];
	y1=c[0]*m.e[1][0]+c[1]*m.e[1][1]+c[2]*m.e[1][2]+m.e[1][3];
	z1=c[0]*m.e[2][0]+c[1]*m.e[2][1]+c[2]*m.e[2][2]+m.e[2][3];
	c[0]=x1;c[1]=y1;c[2]=z1;
	return *this;
}

CVector CVector::Parse(const char *pString)
{
	CVector vVector;
	char sTemp[512]={0};
	strncpy(sTemp,pString,511);

	for(int x=0;x<3;x++)
	{
		char *pTemp=strtok(x==0?sTemp:NULL," ,\r\n");
		if(pTemp!=NULL)
		{
			vVector.c[x]=atof(pTemp);
		}
		else
		{
			break;
		}
	}
	return vVector;
}

std::string CVector::ToString(const CVector &vVector,bool bIncludeParenthesis)
{
	char sTemp[512]={0};
	if(bIncludeParenthesis)
	{
		sprintf(sTemp,"(%.2f , %.2f , %.2f)",vVector.c[0],vVector.c[1],vVector.c[2]);
	}
	else
	{
		sprintf(sTemp,"%.2f , %.2f , %.2f",vVector.c[0],vVector.c[1],vVector.c[2]);
	}
	return sTemp;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int CBSPNode::GetContent(const CVector &position,std::vector<CBSPNode *> *pvNodePath)
{
	if(content!=CONTENT_NODE)
	{
		// los ifs estan solo para depurar, para poder poner breakpoints
		if(content==CONTENT_SOLID)
		{
			return content;
		}
		else if(content==CONTENT_EMPTY)
		{
			return content;
		}
		else 
		{
			return content;
		}
	}
	if(pvNodePath){pvNodePath->push_back(this);}
	
	int nResult=content;
	RTASSERT(plane.c[0]!=0 || plane.c[1]!=0 || plane.c[2]!=0);
	double side=plane.GetSide(position);
	if(side>=0)//FP_PRECISION)
	{
		RTASSERT(pChild[0]);
		nResult=pChild[0]->GetContent(position,pvNodePath);
	}
	else
	{
		RTASSERT(pChild[1]);
		nResult=pChild[1]->GetContent(position,pvNodePath);
	}
	return nResult;
}

CTraceInfo CBSPNode::GetTrace(const CVector &p1,const CVector &p2,const CVector &realp1,const CVector &realp2,std::vector<CBSPNode *> *pvNodePath)
{
 	CTraceInfo trace;
	double side1=plane.GetSide(p1);
	double side2=plane.GetSide(p2);
	
	//Nodo con contenido
	if(content!=CONTENT_NODE)
	{
		trace.m_dTraceFraction=1.0;
		trace.m_nTraceContent=content;
		trace.m_vTracePos=p2;
		trace.m_vTracePlane=plane;
		return trace;
	}
	
	std::vector<CBSPNode *> vTempNodePath;
	
	// Si los dos lados de la recta caen en el lado exterior del plano no hay corte en el plano de este
	// nodo y se devuelve directamente lo que hay en el lado exterior
	if(side1>=0 && side2>=0)
	{
		
		RTASSERT(pChild[0]);
		CTraceInfo tempTrace=pChild[0]->GetTrace(p1,p2,realp1,realp2,pvNodePath?&vTempNodePath:NULL);
		if(tempTrace.m_bTraceHit)
		{
			if(pvNodePath)
			{
				pvNodePath->push_back(this);
				for(unsigned int x=0;x<vTempNodePath.size();x++)
				{
					pvNodePath->push_back(vTempNodePath[x]);
				}
			}
		}
		return tempTrace;
	}
	// Si los dos lados de la recta caen en el lado interior del plano no hay corte en el plano de este
	// nodo y se devuelve directamente lo que hay en el lado interior
	if(side1<=0 && side2<=0)
	{
		RTASSERT(pChild[1]);
		CTraceInfo tempTrace=pChild[1]->GetTrace(p1,p2,realp1,realp2,pvNodePath?&vTempNodePath:NULL);
		if(tempTrace.m_bTraceHit)
		{
			if(pvNodePath)
			{
				pvNodePath->push_back(this);
				for(unsigned int x=0;x<vTempNodePath.size();x++)
				{
					pvNodePath->push_back(vTempNodePath[x]);
				}
			}
		}
		return tempTrace;
	}

	// Cada punto cae a un lado, en este plano hay corte.

	double realdist1=plane.GetSide(realp1);
	double realdist2=plane.GetSide(realp2);
	double  length=realdist1-realdist2;
	trace.m_dTraceFraction=realdist1/length;
	trace.m_vTracePos=realp1+(realp2-realp1)*trace.m_dTraceFraction;
	trace.m_bTraceHit=true;
	trace.m_vTracePlane=plane;

	// 'side' contiene el indice del lado en que cae el punto inicial de la recta (si es un movimiento sera el punto de inicio del movimiento)
	// 'otherside' contiene el indice del lado en que cae el punto final de la recta (si es un movimiento sera el punto de fin del movimiento)
	// Como la recta puede cortar en varios puntos al arbol y hay que devolver el primer punto de corte se va recorriendo el arbol
	// teniendo en cuenta primero los nodos mas cercanos al punto inicial.


	int side=(side1>=0)?0:1;
	int otherside=side^1;

	// Si hay un corte en el lado donde esta el punto inicial se devuelve porque es mas cercano que el corte
	// que provoca el plano de este nodo
	
	CTraceInfo tempTrace=pChild[side]->GetTrace(p1,trace.m_vTracePos,realp1,realp2,pvNodePath?&vTempNodePath:NULL);
	if(tempTrace.m_bTraceHit)
	{
		if(pvNodePath)
		{
			pvNodePath->push_back(this);
			for(unsigned int x=0;x<vTempNodePath.size();x++)
			{
				pvNodePath->push_back(vTempNodePath[x]);
			}
		}
		return tempTrace;
	}
	else
	{
		vTempNodePath.clear();		
	}

	// Si no hay un corte en el lado donde esta el punto inicial se mira si el contenido en el punto 
	// de corte de este nodo es solido.	Aunque sabemos que este plano corta la recta no sabemos si el 
	// corte cae en la parte solida del plano o no (para crear el arbol se usan los planos en los que se 
	// apoyan los Polygonos y como todos sabemos un Polygono es finito y un plano es infinito.


	int tempContent=pChild[side]->GetContent(trace.m_vTracePos);
	if(tempContent==CONTENT_SOLID)
	{
		if(pvNodePath){pvNodePath->push_back(this);}
		trace.m_nTraceContent=tempContent;
		// Si el contenido en el punto de corte de este plano es solido este es el punto de corte.
		return trace;
	}

	tempContent=pChild[otherside]->GetContent(trace.m_vTracePos);
	if(tempContent==CONTENT_SOLID)
	{
		if(pvNodePath){pvNodePath->push_back(this);}
		trace.m_nTraceContent=tempContent;
		// Si el contenido en el punto de corte de el otro lado del plano de corte es solido este es el punto de corte.
		return trace;
	}
	// Si el contenido en el punto de corte no es solido se devuelve el corte de la parte final de la recta
	tempTrace=pChild[otherside]->GetTrace(trace.m_vTracePos,p2,realp1,realp2,pvNodePath?&vTempNodePath:NULL);
	if(tempTrace.m_bTraceHit)
	{
		if(pvNodePath)
		{
			pvNodePath->push_back(this);
			for(unsigned int x=0;x<vTempNodePath.size();x++)
			{
				pvNodePath->push_back(vTempNodePath[x]);
			}
		}
	}
	else
	{
		vTempNodePath.clear();		
	}
	return tempTrace;
}

CBSPDrawNode::CBSPDrawNode(CBSPNode *pBSPNode)
{
	m_nDepth=0;
	m_pBSPNode=pBSPNode;
	m_pNodePolygon=NULL;
}

CBSPDrawNode::~CBSPDrawNode()
{
	std::map<CPolygon	*,int>::iterator i;
	for(i=m_mPolygons.begin();i!=m_mPolygons.end();i++)
	{
		CPolygon *p=i->first;
		delete p;
	}
}

CPolygon::CPolygon()
{
	_debugtag_='O';
	m_nVertexes=0;
	m_pVertexes=NULL;
	m_pEdges=NULL;
}

CPolygon::CPolygon(std::vector<CVector> *pvVertexes)
{
	_debugtag_='O';
	m_nVertexes=0;
	m_pVertexes=NULL;
	m_pEdges=NULL;
	if(pvVertexes->size())
	{
		m_nVertexes=pvVertexes->size();
		m_pVertexes=new CVector[pvVertexes->size()];
		m_pEdges=new bool[pvVertexes->size()];

		for(unsigned x=0;x<m_nVertexes;x++)
		{
			m_pVertexes[x]=(*pvVertexes)[x];
			m_pEdges[x]=true;
		}
		m_Plane=(m_pVertexes[1]-m_pVertexes[0])^(m_pVertexes[2]-m_pVertexes[0]);
		m_Plane.N();
		m_Plane.d=((CVector)m_Plane)*m_pVertexes[0];
	}
}

CPolygon::CPolygon(CPolygon &polygon,int nMode)
{
	_debugtag_='O';
	m_nVertexes=0;
	m_pVertexes=NULL;
	m_pEdges=NULL;
	if(polygon.m_nVertexes)
	{
		m_nVertexes	=polygon.m_nVertexes;
		m_pVertexes	=new CVector[polygon.m_nVertexes];
		m_pEdges	=new bool[polygon.m_nVertexes];

		if(nMode==COPY_POLYGON)
		{
			memcpy(m_pVertexes,polygon.m_pVertexes,sizeof(CVector)*polygon.m_nVertexes);
			memcpy(m_pEdges,polygon.m_pEdges,sizeof(bool)*polygon.m_nVertexes);
			m_Plane=polygon.m_Plane;
			m_Plane.N();
			m_Plane.d=((CVector)m_Plane)*m_pVertexes[0];
		}
		else //	nMode=INVERT_POLYGON
		{
			for(unsigned x=0;x<polygon.m_nVertexes;x++)
			{
				m_pVertexes[x]=polygon.m_pVertexes[(polygon.m_nVertexes-1)-x];
				m_pEdges[x]=polygon.m_pEdges[(polygon.m_nVertexes-1)-x];
			}
			m_Plane=(m_pVertexes[1]-m_pVertexes[0])^(m_pVertexes[2]-m_pVertexes[0]);
			m_Plane.N();
			m_Plane.d=((CVector)m_Plane)*m_pVertexes[0];
		}
	}
}

CPolygon::CPolygon(int nVertexes,CVector *pVertexes)
{
	_debugtag_='O';
	m_nVertexes=0;
	m_pVertexes=NULL;
	m_pEdges=NULL;

	if(nVertexes)
	{
		m_nVertexes=nVertexes;
		m_pVertexes=new CVector[nVertexes];
		m_pEdges=new bool [nVertexes];
		memcpy(m_pVertexes,pVertexes,sizeof(CVector)*nVertexes);
		memcpy(m_pEdges,m_pEdges,sizeof(bool)*nVertexes);
		if(nVertexes>2)
		{
			m_Plane=(m_pVertexes[1]-m_pVertexes[0])^(m_pVertexes[2]-m_pVertexes[0]);
			m_Plane.N();
			m_Plane.d=((CVector)m_Plane)*m_pVertexes[0];
		}
	}
}

CPolygon &CPolygon::operator=(const CPolygon &polygon)
{
	if(m_pVertexes){delete [] m_pVertexes;m_pVertexes=NULL;}
	if(m_pEdges){delete [] m_pEdges;m_pEdges=NULL;}
	m_nVertexes=0;
	m_pVertexes=NULL;
	m_pEdges=NULL;

	if(polygon.m_nVertexes)
	{
		m_nVertexes=polygon.m_nVertexes;
		m_pVertexes=new CVector[polygon.m_nVertexes];
		memcpy(m_pVertexes,polygon.m_pVertexes,sizeof(CVector)*polygon.m_nVertexes);
		m_Plane=polygon.m_Plane;
	}
	if(polygon.m_pEdges)
	{
		m_pEdges=new bool[polygon.m_nVertexes];
		memcpy(m_pEdges,polygon.m_pEdges,sizeof(bool)*polygon.m_nVertexes);
	}
	return *this;
}

CPolygon &CPolygon::operator+=(const CVector v)
{	
	unsigned x;
	for(x=0;x<m_nVertexes;x++)
	{
		m_pVertexes[x]=m_pVertexes[x]+v;
	}
	m_Plane.N();
	m_Plane.d=((CVector)m_Plane)*m_pVertexes[0];
	return *this;
}

bool CPolygon::operator==(const CPolygon &poly)
{
	if(m_nVertexes!=poly.m_nVertexes){return false;}
	if(m_Plane!=poly.m_Plane){return false;}
	unsigned x;
	for(x=0;x<m_nVertexes;x++)
	{
		// Buscar el primer punto del otro poligono en este (aun siendo iguales pueden empezar en diferentes puntos)
		if(m_pVertexes[x]==poly.m_pVertexes[0])
		{
			for(unsigned y=0;y<poly.m_nVertexes;y++,x++)
			{
				if(x==poly.m_nVertexes){x=0;}

				if(m_pVertexes[x]!=poly.m_pVertexes[y])
				{
					return false;
				}
			}
			return true;
		}
	}
	return false;
}


CPolygon::~CPolygon()
{
	m_nVertexes=0;
	if(m_pVertexes){delete [] m_pVertexes;m_pVertexes=NULL;}
	if(m_pEdges){delete [] m_pEdges;m_pEdges=NULL;}
}


CPolyhedron::CPolyhedron()
{
	_debugtag_='E';
}

CPolyhedron::CPolyhedron(CPolygon base, double width)
{
	_debugtag_='E';

	CVector normal=base.m_Plane;
	CVector offset=normal*(0-width);
	CPolygon *pBase1=new CPolygon(base);
	CPolygon *pBase2=new CPolygon(base,INVERT_POLYGON);
	(*pBase2)+=offset;
	m_vPolygons.push_back(pBase1);
	m_vPolygons.push_back(pBase2);
	
	for(unsigned x=0;x<=base.m_nVertexes-1;x++)
	{
		unsigned index1=x;
		unsigned index2=(x==base.m_nVertexes-1)?0:x+1;

		CVector *pVertexes=new CVector[4];
		pVertexes[0]=base.m_pVertexes[index1];
		pVertexes[1]=base.m_pVertexes[index1]+offset;
		pVertexes[2]=base.m_pVertexes[index2]+offset;
		pVertexes[3]=base.m_pVertexes[index2];
		CPolygon *pTemp=new CPolygon(4,pVertexes);
		m_vPolygons.push_back(pTemp);
		delete [] pVertexes;
	}
}

CPolyhedron::~CPolyhedron()
{
	std::vector<CPolygon *>::iterator i;
	for(i=m_vPolygons.begin();i!=m_vPolygons.end();i++)
	{
		CPolygon *p=*i;
		delete p;
	}
}

// Esta funcion corta un poligono con otro poligono coplanara
// Se devuelven los poligonos del primer poligono cortado por el segundo quitando los interiores.

bool CPolygon::CutCoplanar(CPolygon *pCutted,CPolygon *pCutting,std::list<CPolygon *> *pOut)
{
	if(pCutted->m_nVertexes<3 || pCutting->m_nVertexes<3){return false;}
	if(!CheckBBoxCollision(pCutted,pCutting)){return false;}
/*
	CVector dCuttedMins=pCutted->m_pVertexes[0];
	CVector dCuttedMaxs=pCutted->m_pVertexes[0];
	CVector dCuttingMins=pCutting->m_pVertexes[0];
	CVector dCuttingMaxs=pCutting->m_pVertexes[0];

	int n,c;
	for(n=1;n<pCutted->m_nVertexes;n++)
	{
		for(c=0;c<3;c++)
		{
			if(pCutted->m_pVertexes[n].c[c]<dCuttedMins.c[c]){dCuttedMins.c[c]=pCutted->m_pVertexes[n].c[c];}
			if(pCutted->m_pVertexes[n].c[c]>dCuttedMaxs.c[c]){dCuttedMaxs.c[c]=pCutted->m_pVertexes[n].c[c];}
		}
	}
	for(n=1;n<pCutting->m_nVertexes;n++)
	{
		for(c=0;c<3;c++)
		{
			if(pCutting->m_pVertexes[n].c[c]<dCuttingMins.c[c]){dCuttingMins.c[c]=pCutting->m_pVertexes[n].c[c];}
			if(pCutting->m_pVertexes[n].c[c]>dCuttingMaxs.c[c]){dCuttingMaxs.c[c]=pCutting->m_pVertexes[n].c[c];}
		}
	}

	for(c=0;c<3;c++)
	{
		if(dCuttedMins[c]>dCuttingMaxs[c])
		{return false;}
		if(dCuttedMaxs[c]<dCuttingMins[c])
		{return false;}
		if(dCuttingMins[c]>dCuttedMaxs[c])
		{return false;}
		if(dCuttingMaxs[c]<dCuttedMins[c])
		{return false;}
	}*/
	
	bool bCut=false;
	std::list<CPolygon *> vCandidates;
	std::list<CPolygon *> vFinal;
	vCandidates.push_back(new CPolygon(*pCutted));

	while(vCandidates.size())
	{
		CPolygon *pCandidate=*vCandidates.begin();
		vCandidates.erase(vCandidates.begin());

		bool bDivided=false;
		for(unsigned x=0;x<pCutting->m_nVertexes;x++)
		{
			unsigned next=x+1;
			if(next==pCutting->m_nVertexes){next=0;}
			CVector vNormal=(pCutting->m_pVertexes[next]-pCutting->m_pVertexes[x])^pCutting->m_Plane;
			vNormal.N();
			double d=vNormal*pCutting->m_pVertexes[x];
			CPlane plane(vNormal,d);
			
			CPolygon out1,out2;
			int nPols=pCandidate->Divide(plane,pCandidate,&out1,&out2);
			if(nPols!=1)
			{
				bCut=true;
				bDivided=true;
				CPolygon *pPolygon1=new CPolygon(out1);
				CPolygon *pPolygon2=new CPolygon(out2);
				pPolygon1->CalcPlane();
				pPolygon2->CalcPlane();
				vCandidates.push_back(pPolygon1);
				vCandidates.push_back(pPolygon2);
				break;
			}
		}
		if(!bDivided)
		{
			vFinal.push_back(pCandidate);
		}
		else
		{
			delete pCandidate;
		}
	}
	if(bCut)
	{
		std::list<CPolygon *>::iterator i;
		for(i=vFinal.begin();i!=vFinal.end();i++)
		{
			CPolygon *pPolygon=*i;
			pOut->push_back(pPolygon);
		}
	}
	else
	{
		std::list<CPolygon *>::iterator i;
		for(i=vFinal.begin();i!=vFinal.end();i++)
		{
			CPolygon *pPolygon=*i;
			delete pPolygon;
		}
	}

	return bCut;
}

// Esta funcion divide un poligono con un plano.
//
// El resultado pueden ser uno o dos poligonos (el valor de retorno indica el numero)
// La funcion se basa en que en los segmentos de corte cada punto del segmento esta a un lado del plano. 
// Para conocer la posicion relativa de cada punto se usa la funcion CPlane::GetSide.
// Para conocer la posicion de corte dentro del segmento se obtiene la longitud total del segmento 
// sumando los valores devueltos por GetSide. Como la relacion entre distancias al plano es
// proporcional a la distancia de los dos puntos del segmento al punto de corte 
// se puede calcular el punto si dificultad a partir de los dos puntos del segmento.
//
// El funcionamiento general de la funcion es el siguiente:
//
//	Se van añadiendo puntos al primer poligono mientras no haya corte.
//  Si hay corte el punto se añade al primer y segundo poligono
//  Mientras no haya un nuevo corte se añaden los puntos al segundo poligono
//  Cuando se encuentre el segundo corte se añaden los puntos al primer y segundo poligono
//  El resto de puntos (si los hay) se añaden al primer poligono.
//

int CPolygon::Divide(CPlane plane,CPolygon *pIn,CPolygon *pOut1,CPolygon *pOut2,int *pOut1Side,CLine *pCutSegment)
{
	if(pOut1->m_pVertexes){delete [] pOut1->m_pVertexes;pOut1->m_pVertexes=NULL;}
	if(pOut2->m_pVertexes){delete [] pOut2->m_pVertexes;pOut2->m_pVertexes=NULL;}

	pOut1->m_nVertexes=0;
	pOut2->m_nVertexes=0;
	pOut1->m_pVertexes=new CVector[pIn->m_nVertexes+1];
	pOut2->m_pVertexes=new CVector[pIn->m_nVertexes+1];
	double side[255]={0};

	// Hay que interpretar los puntos en 0 como puntos neutrales, es decir, que pueden ser vistos
	// como que estan en ambos lados. Esto es necesario porque un poligono con todos los puntos en un
	// mismo lado menos los puntos en el plano no debe ser cortado y debe generar un solo poligono
	// 
	// En estas variables se almacena:
	//	bAllPointsInTheSameSide	indica si todos los puntos estan al mismo lado
	//	dFirstNonZeroSide		el valor del primer punto que no esta en el plano (side!=0)
	
	double bAllPointsInTheSameSide=true;
	double dFirstNonZeroSide=0;
	unsigned x;
	for(x=0;x<pIn->m_nVertexes;x++)
	{
		side[x]=plane.GetSide(pIn->m_pVertexes[x]);
		// los puntos muy cercanos al plano se ajustan al plano (distancia 0)
		if(side[x]<0 && side[x]>-FP_PRECISION){side[x]=0;}
		if(side[x]>0 && side[x]<FP_PRECISION){side[x]=0;}
		if(side[x]!=0 && dFirstNonZeroSide==0)
		{
			dFirstNonZeroSide=side[x];
		}
		if(dFirstNonZeroSide!=0 && side[x]!=dFirstNonZeroSide)
		{
			bAllPointsInTheSameSide=false;
		}
	}
	if(pOut1Side)
	{
		(*pOut1Side)=(dFirstNonZeroSide>=0)?0:1;
	}

	double side1=dFirstNonZeroSide;
	double tempSide=dFirstNonZeroSide;

	for(x=0;x<pIn->m_nVertexes;)
	{
		// Se comparan los signos de las distancias (¿este punto y el anterior estan al mismo lado?)
		if(tempSide*side1>=0)
		{
			// si el punto no es igual que el ultimo introducido o es el primer vertice
			if(pOut1->m_nVertexes==0 || !(pOut1->m_pVertexes[pOut1->m_nVertexes-1]==pIn->m_pVertexes[x]))
			{
				pOut1->m_pVertexes[pOut1->m_nVertexes]=pIn->m_pVertexes[x];
				pOut1->m_nVertexes++;
			}
			x++;
			if(x<pIn->m_nVertexes)
			{
				tempSide=side[x];
			}
		}
		else
		{
			// Ha habido un corte (este punto y el anterior estan en lados opuestos)

			// Calculo del punto de corte. El punto se calcula a partir de los puntos del segmento
			// proporcionalmente a las distancias de los dos puntos del segmento al plano .

			CVector p1=pIn->m_pVertexes[x-1];
			CVector p2=pIn->m_pVertexes[x];
			double  d1=side[x-1];
			double  d2=side[x];
			double  length=d1-d2;
			double	fraction=d1/length;
			CVector cut1=p1+(p2-p1)*fraction;
			CVector cut2;

			if(pCutSegment){pCutSegment->m_Points[0]=cut1;}

			// Si el primer corte coincide con el ultimo punto metido no se introduce para no
			// meter puntos repetidos (esto es especialmente importante con los tres primeros puntos
			// para el calculo de la normal del plano)

			if(!(pOut1->m_pVertexes[pOut1->m_nVertexes-1]==cut1))
			{
				pOut1->m_pVertexes[pOut1->m_nVertexes]=cut1;
				pOut1->m_nVertexes++;
			}

			// Primer punto del segundo poligono
			pOut2->m_pVertexes[pOut2->m_nVertexes]=cut1;
			pOut2->m_nVertexes++;

			// Se introducen todos los puntos en el segundo poligono mientras 
			// esten en su lado.
			int index=x;
			do
			{
				// Como antes, se evita introducir vertices iguales o muy muy proximos.
				if(!(pOut2->m_pVertexes[pOut2->m_nVertexes-1]==pIn->m_pVertexes[x]))
				{
					pOut2->m_pVertexes[pOut2->m_nVertexes]=pIn->m_pVertexes[x];
					pOut2->m_nVertexes++;
				}

				x++;
				index=(x==pIn->m_nVertexes)?0:x;
				tempSide=side[index];
			}
			while(tempSide*side1<0 && x<=pIn->m_nVertexes);


			// Se calcula el segundo punto de corte.

			p1=pIn->m_pVertexes[x-1];
			p2=pIn->m_pVertexes[index];
			d1=side[x-1];
			d2=side[index];
			length=d1-d2;
			fraction=d1/length;
			cut2=p1+(p2-p1)*fraction;

			if(pCutSegment){pCutSegment->m_Points[1]=cut2;}

			// Introduccion del segundo punto de corte en ambos poligonos si no esta duplicado.
			if(!(pOut1->m_pVertexes[pOut1->m_nVertexes-1]==cut2))
			{
				pOut1->m_pVertexes[pOut1->m_nVertexes]=cut2;
				pOut1->m_nVertexes++;
			}
			if(!(pOut2->m_pVertexes[pOut2->m_nVertexes-1]==cut2))
			{
				pOut2->m_pVertexes[pOut2->m_nVertexes]=cut2;
				pOut2->m_nVertexes++;
			}
		}
	}
	// Se eliminan los poligonos incompletos (menos de 3 vertices).
	if(pOut2->m_nVertexes<3)
	{
		if(pOut2->m_pVertexes){delete [] pOut2->m_pVertexes;pOut2->m_pVertexes=NULL;}
		pOut2->m_nVertexes=0;
	}
	if(pOut1->m_nVertexes<3)
	{
		if(pOut1->m_pVertexes){delete [] pOut1->m_pVertexes;pOut1->m_pVertexes=NULL;}
		pOut1->m_nVertexes=pOut2->m_nVertexes;
		pOut1->m_pVertexes=pOut2->m_pVertexes;
		pOut2->m_nVertexes=0;
		pOut2->m_pVertexes=NULL;
	}
	pOut1->CalcPlane();
	pOut2->CalcPlane();
	return pOut2->m_nVertexes?2:1;	
}

bool CheckBBoxCollision(CPolygon *pIn1,CPolygon *pIn2)
{
	if(pIn1->m_nVertexes<3 || pIn2->m_nVertexes<3){return false;}

#ifndef _DISABLE_OPTIMIZATIONS 

	CVector dIn1Mins=pIn1->m_pVertexes[0];
	CVector dIn1Maxs=pIn1->m_pVertexes[0];
	CVector dIn2Mins=pIn2->m_pVertexes[0];
	CVector dIn2Maxs=pIn2->m_pVertexes[0];
	unsigned n,c;
	for(n=1;n<pIn1->m_nVertexes;n++)
	{
		for(c=0;c<3;c++)
		{
			if(pIn1->m_pVertexes[n].c[c]<(dIn1Mins.c[c]-FP_PRECISION)){dIn1Mins.c[c]=pIn1->m_pVertexes[n].c[c];}
			if(pIn1->m_pVertexes[n].c[c]>(dIn1Maxs.c[c]+FP_PRECISION)){dIn1Maxs.c[c]=pIn1->m_pVertexes[n].c[c];}
		}
	}
	for(n=1;n<pIn2->m_nVertexes;n++)
	{
		for(c=0;c<3;c++)
		{
			if(pIn2->m_pVertexes[n].c[c]<(dIn2Mins.c[c]-FP_PRECISION)){dIn2Mins.c[c]=pIn2->m_pVertexes[n].c[c];}
			if(pIn2->m_pVertexes[n].c[c]>(dIn2Maxs.c[c]+FP_PRECISION)){dIn2Maxs.c[c]=pIn2->m_pVertexes[n].c[c];}
		}
	}

	for(c=0;c<3;c++)
	{
		if(dIn1Mins[c]>(dIn2Maxs[c]+FP_PRECISION))
		{return false;}
		if(dIn1Maxs[c]<(dIn2Mins[c]-FP_PRECISION))
		{return false;}
		if(dIn2Mins[c]>(dIn1Maxs[c]+FP_PRECISION))
		{return false;}
		if(dIn2Maxs[c]<(dIn1Mins[c]-FP_PRECISION))
		{return false;}
	}
#endif
	return true;
}

bool CheckBBoxCollision(CPolyhedron *pIn1,CPolyhedron *pIn2)
{
	if(pIn1->m_vPolygons.size()==0  || pIn2->m_vPolygons.size()==0){return false;}

#ifndef _DISABLE_OPTIMIZATIONS 

	std::vector<CPolygon *>::iterator i1;
	std::vector<CPolygon *> *vPolygons[2]={&pIn1->m_vPolygons,&pIn2->m_vPolygons};
	CVector					dMins[2];
	CVector					dMaxs[2];

	for(int x=0;x<2;x++)
	{
		bool bFirst=true;
		for(i1=vPolygons[x]->begin();i1!=vPolygons[x]->end();i1++)
		{
			CPolygon *pPolygon=*i1;

			if(bFirst)
			{
				dMins[x]=pPolygon->m_pVertexes[0];
				bFirst=false;
			}
			else
			{
				unsigned n,c;
				for(n=1;n<pPolygon->m_nVertexes;n++)
				{
					for(c=0;c<3;c++)
					{
						if(pPolygon->m_pVertexes[n].c[c]<(dMins[x].c[c]-FP_PRECISION)){dMins[x].c[c]=pPolygon->m_pVertexes[n].c[c];}
						if(pPolygon->m_pVertexes[n].c[c]>(dMaxs[x].c[c]+FP_PRECISION)){dMaxs[x].c[c]=pPolygon->m_pVertexes[n].c[c];}
					}
				}
			}
		}
	}
	for(int c=0;c<3;c++)
	{
		if(dMins[0][c]>(dMaxs[1][c]+FP_PRECISION))
		{return false;}
		if(dMaxs[0][c]<(dMins[1][c]-FP_PRECISION))
		{return false;}
		if(dMins[1][c]>(dMaxs[0][c]+FP_PRECISION))
		{return false;}
		if(dMaxs[1][c]<(dMins[0][c]-FP_PRECISION))
		{return false;}
	}
#endif

	return true;
}

bool CPolygon::Cut(CPolygon *pIn1,CPolygon *pIn2,int *pnIn1Pols,int *pnIn2Pols,CPolygon *pOutPols)
{
	if(!CheckBBoxCollision(pIn1,pIn2)){return false;}
	
	bool bCut=false;
	CLine cutSegments[2];

	int nPols=0;
	(*pnIn1Pols)=Divide(pIn2->m_Plane,pIn1,&pOutPols[nPols],&pOutPols[nPols+1],NULL,&cutSegments[1]);
	nPols+=(*pnIn1Pols);
	(*pnIn2Pols)=Divide(pIn1->m_Plane,pIn2,&pOutPols[nPols],&pOutPols[nPols+1],NULL,&cutSegments[0]);
	nPols+=(*pnIn2Pols);

	// Solo puede haber corte si el plano del poligono A corta el poligono B y ademas el 
	// plano de B corta al poligono A (hay 4 poligonos, 2 por poligono original)
	// 
	// En caso contrario es imposible que se solapen (este caso es facil de entender imaginandose 
	// los poligonos formando una T en la que el vertical y el horizontal no se cortan)

	if(nPols==4)
	{
		for(int x=0;x<2;x++)
		{
			// Se comprueba que los dos segmentos de corte se superpongan
			// Se hace midiendo las distancias a un plano basado en el primer punto
			// del primer segmento.
			
			CVector vNormal=cutSegments[x].m_Points[1]-cutSegments[x].m_Points[0];
			vNormal.N();
			double d=vNormal*cutSegments[x].m_Points[0];
			CPlane plane(vNormal,d);
			double insideSegment=plane.GetSide(cutSegments[x].m_Points[1]);
			double segmentMin=insideSegment<0.0?insideSegment:0.0;
			double segmentMax=insideSegment>0.0?insideSegment:0.0;

			double side0=plane.GetSide(cutSegments[x^1].m_Points[0]);
			double side1=plane.GetSide(cutSegments[x^1].m_Points[1]);

			bCut=! ((side0<segmentMin && side1<segmentMin) ||
					(side0>segmentMax && side1>segmentMax));
		}

	}
	if(!bCut)
	{
		(*pnIn1Pols)=1;
		(*pnIn2Pols)=1;
		pOutPols[0]=(*pIn1);
		pOutPols[1]=(*pIn2);
	}
	return bCut;
}

bool CPolyhedron::IsInternalPolygon(CPolygon *pPolygon)
{
	if(m_vPolygons.size()==0){return false;}

	// Se comprueba si algun punto es exterior
	unsigned x;
	for(x=0;x<pPolygon->m_nVertexes;x++)
	{
		std::vector<CPolygon *>::iterator i;
		for(i=m_vPolygons.begin();i!=m_vPolygons.end();i++)
		{
			CPolygon *pTempPolygon=*i;
			double side=pTempPolygon->m_Plane.GetSide(pPolygon->m_pVertexes[x]);
			if(side>FP_PRECISION){return false;}
		}
	}

	// Se comprueba si el poligono divide el poliedro (sin puntos exteriores y no coplanar)
	for(x=0;x<m_vPolygons.size();x++)
	{
		CPolygon  *pTemp=m_vPolygons[x];
		CPolygon pol1,pol2;
		if(CPolygon::Divide(pPolygon->m_Plane,pTemp,&pol1,&pol2)>1)
		{
			return true;
		}
	}

	// Se comprueba si el poligono es coplanar e interior a alguno de los poligonos del poliedro
	for(x=0;x<m_vPolygons.size();x++)
	{
		CPolygon  *pTemp=m_vPolygons[x];
		if(pTemp->m_Plane.InSamePlaneAs(pPolygon->m_Plane))
		{
			bool bAll=true;
			CBSPNode *pNode=BSPFromConvexPolygon(pTemp);
			unsigned y=0;
			for(y=0;y<pPolygon->m_nVertexes;y++)
			{
				if(pNode->GetContent(pTemp->m_pVertexes[y])!=CONTENT_SOLID)
				{
					bAll=false;
					break;
				}
			}
			delete pNode;
			if(!bAll){return true;}
		}
	}

	return false;
	

/*

	bool bAllInPlane=true;
	for(int x=0;x<pPolygon->m_nVertexes;x++)
	{
		std::vector<CPolygon *>::iterator i;
		for(i=m_vPolygons.begin();i!=m_vPolygons.end();i++)
		{
			CPolygon *pTempPolygon=*i;
			double side=pTempPolygon->m_Plane.GetSide(pPolygon->m_pVertexes[x]);
			if(side>FP_PRECISION){return false;}
		}
	}

	return bAllInPlane;
//*/

}

CPlane CPolygon::CalcPlane()
{
	RTASSERT(m_nVertexes==0 || m_nVertexes>2);
	if(m_nVertexes>2)
	{
		CVector t1=m_pVertexes[1]-m_pVertexes[0];
		CVector t2=m_pVertexes[2]-m_pVertexes[0];
		CVector p=t1^t2;

		m_Plane=p;
		m_Plane.N();
		m_Plane.d=((CVector)m_Plane)*m_pVertexes[0];
	}

	return m_Plane;
}

CBSPNode *BSPFromConvexPolygon(CPolygon *pPolygon)
{
	CBSPNode *pNode=new CBSPNode(NULL,CONTENT_NODE);
	CBSPNode *pCurrentNode=pNode;
	for(unsigned x=0;x<pPolygon->m_nVertexes;x++)
	{
		if(x<pPolygon->m_nVertexes-1)
		{
			pCurrentNode->plane=CPlane(pPolygon->m_pVertexes[x+1],pPolygon->m_pVertexes[x],pPolygon->m_pVertexes[x]+pPolygon->m_Plane*10.0);
			pCurrentNode->pChild[0]=new CBSPNode(pCurrentNode,CONTENT_EMPTY);
			pCurrentNode->pChild[1]=new CBSPNode(pCurrentNode,CONTENT_NODE);
		}
		else
		{
			pCurrentNode->plane=CPlane(pPolygon->m_pVertexes[0],pPolygon->m_pVertexes[x],pPolygon->m_pVertexes[x]+pPolygon->m_Plane*10.0);
			pCurrentNode->pChild[0]=new CBSPNode(pCurrentNode,CONTENT_EMPTY);
			pCurrentNode->pChild[1]=new CBSPNode(pCurrentNode,CONTENT_SOLID);
		}
		pCurrentNode=pCurrentNode->pChild[1];
	}
	return pNode;
}

CBSPNode *BSPFromConvexPolygon(int nPoints,CVector *pPoints)
{
	CBSPNode *pNode=new CBSPNode(NULL,CONTENT_NODE);
	CBSPNode *pCurrentNode=pNode;
	for(int x=0;x<nPoints;x++)
	{
		if(x<nPoints-1)
		{
			pCurrentNode->plane=PlaneFromSegment(pPoints[x],pPoints[x+1],CPlane(AxisNegZ,0));
			pCurrentNode->pChild[0]=new CBSPNode(pCurrentNode,CONTENT_EMPTY);
			pCurrentNode->pChild[1]=new CBSPNode(pCurrentNode,CONTENT_NODE);
		}
		else
		{
			pCurrentNode->plane=PlaneFromSegment(pPoints[x],pPoints[0],CPlane(AxisNegZ,0));
			pCurrentNode->pChild[0]=new CBSPNode(pCurrentNode,CONTENT_EMPTY);
			pCurrentNode->pChild[1]=new CBSPNode(pCurrentNode,CONTENT_SOLID);
		}
		pCurrentNode=pCurrentNode->pChild[1];
	}
	return pNode;
}

CBSPNode *BSPFromConvexPolygon(std::list<CVector> *pvVectors)
{
	int		x,nPoints=pvVectors->size();
	CVector *pPoints=new CVector[pvVectors->size()];
	std::list<CVector>::iterator i;
	for(x=0,i=pvVectors->begin();i!=pvVectors->end();x++,i++)
	{
		pPoints[x]=*i;
	}
	CBSPNode *pNode=BSPFromConvexPolygon(nPoints,pPoints);
	delete [] pPoints;
	return pNode;
}

CBSPNode *BSPFromPolyhedronList(int nDepth,std::list<CPolyhedron*> *pPolys,std::vector<CBSPDrawNode *> *pvDrawNodes)
{
	unsigned x=0;
	std::vector<CPolygon *> vPolygons;
	std::list<CPolyhedron*>::iterator i;
	for(i=pPolys->begin();i!=pPolys->end();i++)
	{
		CPolyhedron *pPolyhedron=*i;
		for(x=0;x<pPolyhedron->m_vPolygons.size();x++)
		{
			CPolygon *pPolyOrig=pPolyhedron->m_vPolygons[x];
			CPolygon *pPolyCopy=new CPolygon(pPolyOrig->m_nVertexes,pPolyOrig->m_pVertexes);
			vPolygons.push_back(pPolyCopy);
		}
	}

	CBSPNode *pBSPNode=BSPFromPolygonVector(NULL,nDepth+1,&vPolygons,CONTENT_NODE,pvDrawNodes);

	if(pvDrawNodes==NULL)
	{
		for(x=0;x<vPolygons.size();x++)
		{
			CPolygon *pPolyCopy=vPolygons[x];
			delete pPolyCopy;
		}
	}
	return pBSPNode;

}
int BSPFindCandidate(std::vector<CPolygon*> *pPolys)
{
    int nCandidate=0;
	unsigned nCurrentMinimunDifference=0xFFFFFFFF;
	unsigned x=0,y,nPolys=pPolys->size();
	for(x=0;x<nPolys;x++)
	{
		CPolygon *pCandidate=(*pPolys)[x];
		int nSidePolys[2]={0};

		for(y=0;y<nPolys;y++)
		{
			if(x==y){continue;}

			CPolygon *pPoly=(*pPolys)[y];

			// Se eliminan los planos coplanares al plano de corte (en los subespacios que se generan no tienen sentido)

			if(pPoly->m_Plane!=pCandidate->m_Plane)
			{
				CPolygon polyFragments[2];

				int		 nPolyFirstFragmentSide=0;
				int		 nPolyFragments=pPoly->Divide(pCandidate->m_Plane,pPoly,&polyFragments[0],&polyFragments[1],&nPolyFirstFragmentSide);

				nSidePolys[nPolyFirstFragmentSide]++;
				if(nPolyFragments>1)
				{
					nSidePolys[nPolyFirstFragmentSide^1]++;
				}
			}
		}
		unsigned nCurrentDifference=(unsigned)fabs((double)(nSidePolys[0]-nSidePolys[1]));
		if(nCurrentDifference<nCurrentMinimunDifference)
		{
			nCurrentMinimunDifference=nCurrentDifference;
            nCandidate=x;
			if(nCurrentMinimunDifference<2){break;}
		}
	}
	return nCandidate;
}

CBSPNode *BSPFromPolygonVector(CBSPNode *pParent,int nDepth,std::vector<CPolygon*> *pPolys,unsigned int dwLeafContentType,std::vector<CBSPDrawNode *> *pvDrawNodes,bool bFastGenerationSlowCheck)
{
	if(pPolys->size()==0)
	{
		if(dwLeafContentType==(unsigned int)CONTENT_NODE)
		{
			return NULL;
		}
		else
		{
			if(pPolys->size()==0){return new CBSPNode(pParent,dwLeafContentType);}
		}
	}
	int nCandidateIndex=bFastGenerationSlowCheck?0:BSPFindCandidate(pPolys);

	CBSPNode *pNode=new CBSPNode(pParent,(*pPolys)[nCandidateIndex]->m_Plane,CONTENT_NODE);
	
	if(pvDrawNodes)
	{
		pNode->m_pDrawNode=new CBSPDrawNode(pNode);
		pNode->m_pDrawNode->m_nDepth=nDepth;

		CPolygon *pNodePoly=new CPolygon((*pPolys)[nCandidateIndex]->m_nVertexes,(*pPolys)[nCandidateIndex]->m_pVertexes);
		pNode->m_pDrawNode->m_mPolygons[pNodePoly]=2;
		pNode->m_pDrawNode->m_pNodePolygon=pNodePoly;
		pvDrawNodes->push_back(pNode->m_pDrawNode);
	}	
	std::vector<CPolygon *> vSidePolys[2];
	int nPolys=pPolys->size();
	for(int x=0;x<nPolys;x++)
	{
		if(x==nCandidateIndex){continue;}

		CPolygon *pPoly=(*pPolys)[x];

		// Evitar procesar poligonos degenerados.
		if(pPoly->m_Plane.c[0]==0 && pPoly->m_Plane.c[1]==0 && pPoly->m_Plane.c[2]==0)
		{
			//RTTRACE("%3d:Polygon removed: Invalid plane",nCurrentDepth);
			if(pvDrawNodes)
			{
				CPolygon *pDiscardedPoly=new CPolygon(pPoly->m_nVertexes,pPoly->m_pVertexes);
				pNode->m_pDrawNode->m_mDiscardedPolygons[pDiscardedPoly]=BSP_DISCARD_INVALID_PLANE;
			}
			continue;
		}
		if(pPoly->m_nVertexes<3)
		{
			//RTTRACE("%3d:Polygon removed: Vertex count",nCurrentDepth);
			if(pvDrawNodes)
			{
				CPolygon *pDiscardedPoly=new CPolygon(pPoly->m_nVertexes,pPoly->m_pVertexes);
				pNode->m_pDrawNode->m_mDiscardedPolygons[pDiscardedPoly]=BSP_DISCARD_VERTEX_COUNT;
			}
			continue;
		}
		if((pPoly->m_nVertexes==3) && ((pPoly->m_pVertexes[0]-pPoly->m_pVertexes[1])<FP_PRECISION|| 
			(pPoly->m_pVertexes[0]-pPoly->m_pVertexes[2])<FP_PRECISION|| 
			(pPoly->m_pVertexes[1]-pPoly->m_pVertexes[2])<FP_PRECISION))
		{
			//RTTRACE("%3d:Polygon removed: Surface size",nCurrentDepth);
			if(pvDrawNodes)
			{
				CPolygon *pDiscardedPoly=new CPolygon(pPoly->m_nVertexes,pPoly->m_pVertexes);
				pNode->m_pDrawNode->m_mDiscardedPolygons[pDiscardedPoly]=BSP_DISCARD_SMALL_SURFACE;
			}
			continue;
		}
		// Se eliminan los planos coplanares al plano de corte (en los subespacios que se generan no tienen sentido)

		if(pPoly->m_Plane!=pNode->plane)
		{
			CPolygon *pPolyFragments[2]={new CPolygon(),new CPolygon()};

			int		 nPolyFirstFragmentSide=0;
			int		 nPolyFragments=pPoly->Divide(pNode->plane,pPoly,pPolyFragments[0],pPolyFragments[1],&nPolyFirstFragmentSide);

			//pPolyFragments[0]->m_Plane=pPoly->m_Plane;
			pPolyFragments[0]->CalcPlane();
			vSidePolys[nPolyFirstFragmentSide].push_back(pPolyFragments[0]);
			if(nPolyFragments>1)
			{
				//pPolyFragments[1]->m_Plane=pPoly->m_Plane;
				pPolyFragments[1]->CalcPlane();
				vSidePolys[nPolyFirstFragmentSide^1].push_back(pPolyFragments[1]);
			}
			else
			{
				delete pPolyFragments[1];
			}
		}
		else
		{
			if(pvDrawNodes)
			{
				CPolygon *pDiscardedPoly=new CPolygon(pPoly->m_nVertexes,pPoly->m_pVertexes);
				pNode->m_pDrawNode->m_mDiscardedPolygons[pDiscardedPoly]=BSP_DISCARD_COPLANAR;
			}
		}
	}
	//int nNextDepth=nCurrentDepth+1;
	pNode->pChild[0]=BSPFromPolygonVector(pNode,nDepth+1,&vSidePolys[0],CONTENT_EMPTY,pvDrawNodes,bFastGenerationSlowCheck);
	pNode->pChild[1]=BSPFromPolygonVector(pNode,nDepth+1,&vSidePolys[1],CONTENT_SOLID,pvDrawNodes,bFastGenerationSlowCheck);

	if(pvDrawNodes)
	{
		for(unsigned int x=0;x<vSidePolys[0].size();x++){pNode->m_pDrawNode->m_mPolygons[vSidePolys[0][x]]=0;}
		for(unsigned int x=0;x<vSidePolys[1].size();x++){pNode->m_pDrawNode->m_mPolygons[vSidePolys[1][x]]=1;}
	}
	else
	{
		for(unsigned int x=0;x<vSidePolys[0].size();x++){CPolygon *pPoly=vSidePolys[0][x];delete pPoly;}
		for(unsigned int x=0;x<vSidePolys[1].size();x++){CPolygon *pPoly=vSidePolys[1][x];delete pPoly;}
	}
	return pNode;
}

CMatrix::CMatrix(double pMatrix[4][4])
{
	int x,y;
	for(x=0;x<4;x++){for(y=0;y<4;y++){e[x][y]=pMatrix[x][y];}}
}

CMatrix::CMatrix(CMatrix &m)
{
	int x,y;
	for(x=0;x<4;x++){for(y=0;y<4;y++){e[x][y]=m.e[x][y];}}
}
CMatrix::CMatrix()
{
	I();
}
CMatrix::~CMatrix()
{
}


void CMatrix::R(CVector vAxis,double dAngle,CVector vOffset)
{
	CVector Vt;
	CMatrix aux;
	vAxis.N();

	//Trasladamos el eje al sist. de coord. del mundo
	Vt=Origin-vOffset;
	T(Vt);

	double seno=sin(dAngle),coseno=cos(dAngle);
	double oneminuscos=1.0-coseno;
	double x=vAxis.c[0],y=vAxis.c[1],z=vAxis.c[2];
	double x2=x*x,y2=y*y,z2=z*z,xz=x*z,yz=y*z,xy=x*y;
	
	aux.e[0][0]=coseno+x2*oneminuscos;
	aux.e[0][1]=xy*oneminuscos-z*seno;
	aux.e[0][2]=xz*oneminuscos+y*seno;
	aux.e[0][3]=0.0;
	
	aux.e[1][0]=xy*oneminuscos+z*seno;
	aux.e[1][1]=coseno+y2*oneminuscos;
	aux.e[1][2]=yz*oneminuscos-x*seno;
	aux.e[1][3]=0.0;
	
	aux.e[2][0]=xz*oneminuscos-y*seno;
	aux.e[2][1]=yz*oneminuscos+x*seno;
	aux.e[2][2]=coseno+z2*oneminuscos;
	aux.e[2][3]=0.0;
	
	(*this)*=aux; //Juntamos las matrices

	aux.T(vOffset); //Deshacemos el desplazamiento

	(*this)*=aux; //Juntamos las matrices
}

void CMatrix::R( CVector vAxis,double dAngle )
{
	double seno=sin(dAngle),coseno=cos(dAngle);
	double oneminuscos=1.0-coseno;
	double x=vAxis.c[0],y=vAxis.c[1],z=vAxis.c[2];
	double x2=x*x,y2=y*y,z2=z*z,xz=x*z,yz=y*z,xy=x*y;

	e[0][0]=coseno+x2*oneminuscos;
	e[0][1]=xy*oneminuscos-z*seno;
	e[0][2]=xz*oneminuscos+y*seno;
	e[0][3]=0.0;

	e[1][0]=xy*oneminuscos+z*seno;
	e[1][1]=coseno+y2*oneminuscos;
	e[1][2]=yz*oneminuscos-x*seno;
	e[1][3]=0.0;

	e[2][0]=xz*oneminuscos-y*seno;
	e[2][1]=yz*oneminuscos+x*seno;
	e[2][2]=coseno+z2*oneminuscos;
	e[2][3]=0.0;

	e[3][0]=0.0;
	e[3][1]=0.0;
	e[3][2]=0.0;
	e[3][3]=1.0;
}

void	CMatrix::T(CVector v)
{
	I();
	e[0][3]=v.c[0];
	e[1][3]=v.c[1];
	e[2][3]=v.c[2];
}

void	CMatrix::S(double x,double y,double z,CVector vOffset)
{
	CMatrix			m1;
	CVector vT;
	// traslate	
	vT=Origin-vOffset;
	this->T(vT);
	// Scale Matrix on m1
	m1.e[0][0]=x;
	m1.e[1][1]=y;
	m1.e[2][2]=z;
	(*this)*=m1;
	m1.T(vOffset);
	// Undo traslate
	(*this)*=m1;
}

void	CMatrix::I()
{
	e[0][0]=1.0;e[0][1]=0.0;e[0][2]=0.0;e[0][3]=0.0;
	e[1][0]=0.0;e[1][1]=1.0;e[1][2]=0.0;e[1][3]=0.0;
	e[2][0]=0.0;e[2][1]=0.0;e[2][2]=1.0;e[2][3]=0.0;
	e[3][0]=0.0;e[3][1]=0.0;e[3][2]=0.0;e[3][3]=1.0;
}

double	CMatrix::D()
{
	double d=0.0;
	for(int x=0;x<4;x++)
	{
		d+=e[x][0]*Adjunt(x,0);
	}
	return d;
}


void CMatrix::operator*=(CMatrix &m)
{
	double temp[4][4];
	
	temp[0][0]=m.e[0][0]*e[0][0]+m.e[0][1]*e[1][0]+m.e[0][2]*e[2][0]+m.e[0][3]*e[3][0];
	temp[1][0]=m.e[1][0]*e[0][0]+m.e[1][1]*e[1][0]+m.e[1][2]*e[2][0]+m.e[1][3]*e[3][0];
	temp[2][0]=m.e[2][0]*e[0][0]+m.e[2][1]*e[1][0]+m.e[2][2]*e[2][0]+m.e[2][3]*e[3][0];
	temp[3][0]=m.e[3][0]*e[0][0]+m.e[3][1]*e[1][0]+m.e[3][2]*e[2][0]+m.e[3][3]*e[3][0];
	temp[0][1]=m.e[0][0]*e[0][1]+m.e[0][1]*e[1][1]+m.e[0][2]*e[2][1]+m.e[0][3]*e[3][1];
	temp[1][1]=m.e[1][0]*e[0][1]+m.e[1][1]*e[1][1]+m.e[1][2]*e[2][1]+m.e[1][3]*e[3][1];
	temp[2][1]=m.e[2][0]*e[0][1]+m.e[2][1]*e[1][1]+m.e[2][2]*e[2][1]+m.e[2][3]*e[3][1];
	temp[3][1]=m.e[3][0]*e[0][1]+m.e[3][1]*e[1][1]+m.e[3][2]*e[2][1]+m.e[3][3]*e[3][1];
	temp[0][2]=m.e[0][0]*e[0][2]+m.e[0][1]*e[1][2]+m.e[0][2]*e[2][2]+m.e[0][3]*e[3][2];
	temp[1][2]=m.e[1][0]*e[0][2]+m.e[1][1]*e[1][2]+m.e[1][2]*e[2][2]+m.e[1][3]*e[3][2];
	temp[2][2]=m.e[2][0]*e[0][2]+m.e[2][1]*e[1][2]+m.e[2][2]*e[2][2]+m.e[2][3]*e[3][2];
	temp[3][2]=m.e[3][0]*e[0][2]+m.e[3][1]*e[1][2]+m.e[3][2]*e[2][2]+m.e[3][3]*e[3][2];
	temp[0][3]=m.e[0][0]*e[0][3]+m.e[0][1]*e[1][3]+m.e[0][2]*e[2][3]+m.e[0][3]*e[3][3];
	temp[1][3]=m.e[1][0]*e[0][3]+m.e[1][1]*e[1][3]+m.e[1][2]*e[2][3]+m.e[1][3]*e[3][3];
	temp[2][3]=m.e[2][0]*e[0][3]+m.e[2][1]*e[1][3]+m.e[2][2]*e[2][3]+m.e[2][3]*e[3][3];
	temp[3][3]=m.e[3][0]*e[0][3]+m.e[3][1]*e[1][3]+m.e[3][2]*e[2][3]+m.e[3][3]*e[3][3];
	
	e[0][0]=temp[0][0];
	e[1][0]=temp[1][0];
	e[2][0]=temp[2][0];
	e[3][0]=temp[3][0];
	e[0][1]=temp[0][1];
	e[1][1]=temp[1][1];
	e[2][1]=temp[2][1];
	e[3][1]=temp[3][1];
	e[0][2]=temp[0][2];
	e[1][2]=temp[1][2];
	e[2][2]=temp[2][2];
	e[3][2]=temp[3][2];
	e[0][3]=temp[0][3];
	e[1][3]=temp[1][3];
	e[2][3]=temp[2][3];
	e[3][3]=temp[3][3];
}

void CMatrix::Ref(CVector vAxisX,CVector vAxisY,CVector vAxisZ)
{
	I();
	
	e[0][0]=vAxisX[0];
	e[0][1]=vAxisX[1];
	e[0][2]=vAxisX[2];
	e[1][0]=vAxisY[0];
	e[1][1]=vAxisY[1];
	e[1][2]=vAxisY[2];
	e[2][0]=vAxisZ[0];
	e[2][1]=vAxisZ[1];
	e[2][2]=vAxisZ[2];

}

void CMatrix::Transpose()
{
	double dTemp;
	dTemp=e[1][0];e[1][0]=e[0][1];e[0][1]=dTemp;
	dTemp=e[2][0];e[2][0]=e[0][2];e[0][2]=dTemp;
	dTemp=e[2][1];e[2][1]=e[1][2];e[1][2]=dTemp;
	dTemp=e[3][0];e[3][0]=e[0][3];e[0][3]=dTemp;
	dTemp=e[3][1];e[3][1]=e[1][3];e[1][3]=dTemp;
	dTemp=e[3][2];e[3][2]=e[2][3];e[2][3]=dTemp;
}

void CMatrix::Inverse()
{
	double determinant=D();
	Adjunt();
	Transpose();
	for(int x=0;x<4;x++)
	{
		for(int y=0;y<4;y++)
		{	
			e[x][y]/=determinant;
		}
	}
}

double CMatrix::Adjunt( int r,int c )
{
	CMatrix33 minor;
	int nDestx=0;
	int nDesty=0;
	for(int x=0;x<4;x++)
	{
		if(r==x){continue;}
		nDesty=0;
		for(int y=0;y<4;y++)
		{
			if(c==y){continue;}
			minor.e[nDestx][nDesty]=e[x][y];
			nDesty++;
		}
		nDestx++;
	}
	double dSign=(r+c)&1?-1.0:1.0;
	return minor.D()*dSign;
}

void CMatrix::Adjunt()
{
	CMatrix temp=*this;
	for(int x=0;x<4;x++)
	{
		for(int y=0;y<4;y++)
		{
			e[x][y]=temp.Adjunt(x,y);
		}
	}
}

CMatrix33::CMatrix33(CVector v0,CVector v1,CVector v2)
{
	int x;
	for(x=0;x<3;x++){e[x][0]=v0.c[x];}
	for(x=0;x<3;x++){e[x][1]=v1.c[x];}
	for(x=0;x<3;x++){e[x][2]=v2.c[x];}
}

CMatrix33::CMatrix33(double pMatrix[3][3])
{
	int x,y;
	for(x=0;x<3;x++){for(y=0;y<3;y++){e[x][y]=pMatrix[x][y];}}
}

CMatrix33::CMatrix33(CMatrix33 &m)
{
	int x,y;
	for(x=0;x<3;x++){for(y=0;y<3;y++){e[x][y]=m.e[x][y];}}
}
CMatrix33::CMatrix33()
{
	I();
}
CMatrix33::~CMatrix33()
{
}

void	CMatrix33::I()
{
	e[0][0]=1.0;e[0][1]=0.0;e[0][2]=0.0;
	e[1][0]=0.0;e[1][1]=1.0;e[1][2]=0.0;
	e[2][0]=0.0;e[2][1]=0.0;e[2][2]=1.0;
}

double	CMatrix33::D()
{
	double d=e[0][0]*e[1][1]*e[2][2]
			-e[0][0]*e[1][2]*e[2][1]
			-e[0][1]*e[1][0]*e[2][2]
			+e[0][1]*e[1][2]*e[2][0]
			+e[0][2]*e[1][0]*e[2][1]
			-e[0][2]*e[1][1]*e[2][0];
	return d;
}

CPlane::CPlane(CVector p1,CVector p2,CVector p3)
{
	_debugtag_='P';
	CVector normal=((p1-p2)^(p3-p2));
	(*this)=normal;
	normal.N();
	d=normal*p1;
}

CPlane::CPlane(CVector vNormal,CVector vPoint)
{
	_debugtag_='P';
	(*this)=vNormal;
	d=vNormal*vPoint;
}

bool CPlane::Cut(CVector p1,CVector p2, CVector *pCut)
{
	double d1=GetSide(p1),d2=GetSide(p2);
	if(d1*d2>=0){return false;}
	double length=d1-d2;
	double dFraction=d1/length;
	*pCut=p1+(p2-p1)*dFraction;
	return true;
}

bool CPlane::InSamePlaneAs(const CPlane p)
{
	if(	fabs(p.c[0]-c[0])<FP_PRECISION &&
		fabs(p.c[1]-c[1])<FP_PRECISION &&
		fabs(p.c[2]-c[2])<FP_PRECISION &&
		fabs(p.d-d)<FP_PRECISION){return true;}
	// inverse plane (inverted normal)
	CVector tempNormal=Origin-(CVector)p;
	if(	fabs(tempNormal.c[0]-c[0])<FP_PRECISION &&
		fabs(tempNormal.c[1]-c[1])<FP_PRECISION &&
		fabs(tempNormal.c[2]-c[2])<FP_PRECISION &&
		fabs((0.0-p.d)-d)<FP_PRECISION){return true;}
	return false;
}

bool CPlane::operator==(const CPlane p)
{
/*	if(p.c[0]*c[0]<0.0){return false;}
	if(p.c[1]*c[1]<0.0){return false;}
	if(p.c[2]*c[2]<0.0){return false;}
	if(p.d*d<0){return false;}*/
	if(fabs(p.c[0]-c[0])>FP_PRECISION){return false;}
	if(fabs(p.c[1]-c[1])>FP_PRECISION){return false;}
	if(fabs(p.c[2]-c[2])>FP_PRECISION){return false;}
	if(fabs(p.d-d)>FP_PRECISION){return false;}
	return true;
}

bool PointFromPlanes(CPlane plane0,CPlane plane1,CPlane plane2,CVector *pPoint)
{
	// El punto de corte entre tres planos se resuelve por Cramer

	CMatrix33 m(plane0,plane1,plane2);
	double det=m.D(),m1det=0.0;
	if(det==0)
	{
		// El sistema no tiene solucion unica
		return false;
	}
	
	// Sustitucion de los terminos independientes para obtener las coordenadas

	CMatrix33 m1;
	m1=m;
	m1.e[0][0]=plane0.d;
	m1.e[0][1]=plane1.d;
	m1.e[0][2]=plane2.d;
	m1det=m1.D();
	pPoint->c[0]=m1det/det;

	m1=m;
	m1.e[1][0]=plane0.d;
	m1.e[1][1]=plane1.d;
	m1.e[1][2]=plane2.d;
	m1det=m1.D();
	pPoint->c[1]=m1det/det;

	m1=m;
	m1.e[2][0]=plane0.d;
	m1.e[2][1]=plane1.d;
	m1.e[2][2]=plane2.d;
	m1det=m1.D();
	pPoint->c[2]=m1det/det;

	return true;
}

bool IsPointInRegion(int nPlanes,CPlane *pPlanes,CVector point,double dPrecision)
{
	if(nPlanes==0){return false;}
	int x;
	for(x=0;x<nPlanes;x++)
	{
		double side=pPlanes[x].GetSide(point);
		if(side>=dPrecision)
		{
			return false;
		}
	}
	return true;
}

bool IsPointInPolyhedron(CPolyhedron *pPoly,CVector point,double dTolerance)
{
	std::vector<CPolygon *>::iterator i;
	if(pPoly->m_vPolygons.size()==0){return false;}
	for(i=pPoly->m_vPolygons.begin();i!=pPoly->m_vPolygons.end();i++)
	{
		CPolygon *pPolygon=*i;
		double side=pPolygon->m_Plane.GetSide(point);
		if(side>=dTolerance)
		{
			return false;
		}
	}
	return true;
}


bool CVector::operator==(const CVector v) const 
{
	if(fabs(v.c[0]-c[0])>FP_PRECISION){return false;}
	if(fabs(v.c[1]-c[1])>FP_PRECISION){return false;}
	if(fabs(v.c[2]-c[2])>FP_PRECISION){return false;}
	return true;
}

bool CVector::CompareWithPrecision(const CVector &v1,const CVector &v2,double precision) 
{
	if(fabs(v1.c[0]-v2.c[0])>precision){return false;}
	if(fabs(v1.c[1]-v2.c[1])>precision){return false;}
	if(fabs(v1.c[2]-v2.c[2])>precision){return false;}
	return true;
}

CVector &CVector::Maxs( const CVector &v1,const CVector &v2 )
{
	c[0]=v1.c[0]>v2.c[0]?v1.c[0]:v2.c[0];
	c[1]=v1.c[1]>v2.c[1]?v1.c[1]:v2.c[1];
	c[2]=v1.c[2]>v2.c[2]?v1.c[2]:v2.c[2];
	return *this;
}

CVector &CVector::Mins( const CVector &v1,const CVector &v2 )
{
	c[0]=v1.c[0]<v2.c[0]?v1.c[0]:v2.c[0];
	c[1]=v1.c[1]<v2.c[1]?v1.c[1]:v2.c[1];
	c[2]=v1.c[2]<v2.c[2]?v1.c[2]:v2.c[2];
	return *this;
}

CPlane PlaneFromSegment(CVector v1,CVector v2,CPlane segmentPlane)
{
	CVector normal=((v2-v1)^segmentPlane);
	normal.N();
	double distance=normal*v1;
	CPlane plane(normal,distance);
	return plane;
}

// Esta funcion obtiene un poliedro a partir de una region convexa.
// La region se ha de pasar como una lista de planos (es el metodo de representacion de los archivos .map del Quake)
// El metodo que se usa para obtener el poliedro es el siguiente:
//
// Para cada plano se obtienen todos los cortes con todas las demas parejas de planos
// descartandose los puntos exteriores al poliedro. Con esto se consiguen todos los puntos 
// del poliedro en ese plano, que ordenados en sentido horario forman el poligono en ese plano.
//
// Una vez procesados todos los planos se tiene el poliedro.

CPolyhedron *PolyhedronFromConvexRegion(int nPlanes,CPlane *pPlanes)
{
	CPolyhedron *pPoly=new CPolyhedron();
	int x,count=0;
	for(x=0;x<nPlanes;x++)
	{
		// Se obtienen los puntos de corte con todos los demas planos
		
		// En este vector se añaden todos los puntos del poliedro que estan en este plano,
		// a partir de ellos se obtendra el poligono
		//_MRT("Starting polygon %d",x);

		std::vector<CVector> vPoints;
		for(int y=0;y<nPlanes;y++)
		{
			for(int z=0;z<nPlanes;z++)
			{
				// Se omiten las combinaciones en las que un plano aparece repetido
				if(z!=x && z!=y && x!=y)
				{
					// Se determina si los planos cortan en un punto.

					CVector point;
					if(PointFromPlanes(pPlanes[x],pPlanes[y],pPlanes[z],&point))
					{
						// Solo se tienen en cuenta los puntos de corte interiores al poliedro

						if(IsPointInRegion(nPlanes,pPlanes,point))
						{
							bool bFound=false;
							std::vector<CVector>::iterator i;
							for(i=vPoints.begin();i!=vPoints.end();i++)
							{
								CVector v=*i;
								if(CVector::CompareWithPrecision(point,v,0.005))
								{
									bFound=true;
									break;
								}
							}
							if(!bFound)
							{
//								_MRT("Adding Point (%f,%f,%f)",point.c[0],point.c[1],point.c[2]);
								vPoints.push_back(point);
							}
						}
					}
				}
			}
		}
		
	//	int nPoints=vPoints.size();
	//	_MRT("Ending Polygon %d with %d points",x,nPoints);
		// Una vez que se tienen todos los puntos de corte de ese plano, se ordenan en sentido horario para 
		// construir el poligono. 
		// Para ordenar los puntos en sentido horario se ha seguido el siguiente metodo:
		// Partiendo de cualquier punto:
		//		Se coge un punto cualquiera de los demas
		//		Se construye un plano basado en el segmento que forman los dos puntos y perpendicular al plano (PlaneFromSegment).
		//		y se comprueba que todos los puntos restantes esten en el "interior" (side<=0) del plano.
		//		Si todos los puntos estan en el interior este es el siguiente punto en sentido horario
		//			Se añade a la solucion y se quita de los puntos pendientes de procesar
		//		Si algun punto esta en el exterior se pasa al siguiente punto
		// Se repite el proceso mientras queden puntos.

		std::vector<CVector> vRemainingPoints;
		std::vector<CVector> vPolygon;
		vRemainingPoints=vPoints;

		CVector currentPoint=*vRemainingPoints.begin();
		vRemainingPoints.erase(vRemainingPoints.begin());
		vPolygon.push_back(currentPoint);

		while(vRemainingPoints.size())
		{

			std::vector<CVector>::iterator i;
			for(i=vRemainingPoints.begin();i<vRemainingPoints.end();i++)
			{
				CVector candidatePoint=*i;
				CPlane plane=PlaneFromSegment(currentPoint,candidatePoint,pPlanes[x]);

				bool bAllOtherPointsInTheSameSide=true;
				std::vector<CVector>::iterator i2;
				for(i2=vRemainingPoints.begin();i2<vRemainingPoints.end();i2++)
				{
					if(i!=i2)
					{
						CVector remainingPoint=*i2;
						double side=plane.GetSide(remainingPoint);
						if(side>0)
						{
							bAllOtherPointsInTheSameSide=false;
							break;
						}
					}
				}
				if(bAllOtherPointsInTheSameSide)
				{
					vPolygon.push_back(candidatePoint);
					vRemainingPoints.erase(i);
					currentPoint=candidatePoint;
					break;
				}
			}
		}
		CPolygon *pPolygon=new CPolygon(&vPolygon);
		pPolygon->CalcPlane();
		pPoly->m_vPolygons.push_back(pPolygon);
	}
	count=pPoly->m_vPolygons.size();
	return pPoly;
}


CVector FromQuakeToOpenGL(CVector v)
{
	double temp=v.c[2];
	v.c[2]=-v.c[1];
	v.c[1]=temp;
	return v;
}

void VectorsFromAngles(const CVector &vAngles,CVector *pvForward,CVector *pvRight,CVector *pvUp)
{
	if(pvUp){*pvUp=AxisPosY;}
	if(pvRight){*pvRight=AxisPosZ;}
	if(pvForward){*pvForward=AxisPosX;}

	CMatrix m,aux;
	aux.R(CVector(1,0,0),vAngles.c[2]/360.0*2.0*PI);m*=aux;
	aux.R(CVector(0,0,1),vAngles.c[1]/360.0*2.0*PI);m*=aux;
	aux.R(CVector(0,1,0),vAngles.c[0]/360.0*2.0*PI);m*=aux;
	if(pvUp){*pvUp*=m;}
	if(pvRight){*pvRight*=m;}
	if(pvForward){*pvForward*=m;}
}

void VectorsFromAngles(double dYaw,double dPitch, double dRoll,CVector &vForward,CVector &vRight,CVector &vUp)
{
    vUp=AxisPosY;
	vRight=AxisPosZ;
	vForward=AxisPosX;

	CMatrix m,aux;
	aux.R(CVector(1,0,0),dRoll/360.0*2.0*PI);m*=aux;
	aux.R(CVector(0,0,1),dPitch/360.0*2.0*PI);m*=aux;
	aux.R(CVector(0,1,0),dYaw/360.0*2.0*PI);m*=aux;

	vRight*=m;
	vUp*=m;
	vForward*=m;
}


void AnglesFromVector(const CVector &vForward,double &dYaw,double &dPitch)
{
	CVector vTemp;
	vTemp.c[0]=vForward.c[0];
	vTemp.c[1]=-vForward.c[2];
	vTemp.c[2]=vForward.c[1];
	double	dForward;
	
	if (vTemp[1] == 0 && vTemp[0] == 0)
	{
		dYaw = 0;
		if (vTemp[2] > 0)
			dPitch = 90.0;
		else
			dPitch = 270.0;
	}
	else
	{
		dYaw = (atan2(vTemp.c[1], vTemp.c[0]) * 180.0 / PI);
		if (dYaw < 0.0){dYaw += 360.0;}

		dForward = sqrt (vTemp.c[0]*vTemp.c[0] + vTemp.c[1]*vTemp.c[1]);
		dPitch = (atan2(vTemp.c[2], dForward) * 180.0 / PI);
		if (dPitch < 0.0){dPitch += 360;}
	}
}

CVector AnglesFromVectors(const CVector &vForward,const CVector &vRight,const CVector &vUp)
{
	double dYaw=0,dPitch=0,dRoll=0;
	AnglesFromVector(vForward,dYaw,dPitch);
	
	CMatrix m,aux;
	aux.R(CVector(0,0,1),(dPitch/360.0)*2.0*PI);m*=aux;
	aux.R(CVector(0,1,0),(dYaw/360.0)*2.0*PI);m*=aux;

	CVector vRightWithoutRoll(0,0,1),vUpWithoutRoll(0,1,0);
	vRightWithoutRoll*=m;
	vUpWithoutRoll*=m;
	
	double dSide=vUpWithoutRoll*vRight;
	double dCos=vRightWithoutRoll*vRight;
	if(dCos>1.0){dCos=1.0;}
	if(dCos<-1.0){dCos=-1.0;}
	dRoll=RadiansToDegrees(acos(dCos));
	if(dSide>0){dRoll=360.0-dRoll;}
	
	CVector vAngles(dYaw,dPitch,dRoll);
	return vAngles;
}

void AnglesFromVector(const CVector &vForward,CVector *pAngles)
{
	pAngles->c[2]=0;
	AnglesFromVector(vForward,pAngles->c[0],pAngles->c[1]);
}

CVector AnglesFromVector(const CVector &vForward)
{
	CVector vAngles;
	AnglesFromVector(vForward,vAngles.c[0],vAngles.c[1]);
	return vAngles;
}

bool GetExternalPolygons(CPolyhedron *pCutterPolyhedron,CPolyhedron *pProcessed,bool bCutCoplanar)
{
	bool bAnyCut=false;
	std::vector<CPolygon *>::iterator i1;
	std::vector<CPolygon *>::iterator i2;

	std::list<CPolygon*>			lCandidatePols;
	std::list<CPolygon*>::iterator  iCand;

	for(i2=pProcessed->m_vPolygons.begin();i2!=pProcessed->m_vPolygons.end();i2++)
	{
		CPolygon *pPolygon=*i2;
		lCandidatePols.push_back(pPolygon);
	}
	pProcessed->m_vPolygons.clear();

	while(lCandidatePols.size())
	{
		CPolygon *pCutted=*lCandidatePols.begin();
		lCandidatePols.erase(lCandidatePols.begin());

		bool bCut=false;
		for(i1=pCutterPolyhedron->m_vPolygons.begin();i1!=pCutterPolyhedron->m_vPolygons.end();i1++)
		{
			CPolygon *pCutting=*i1;
			CPolygon pPols[4];
			int nCuttingPols=0,nCuttedPols=0;
			CPolygon::Cut(pCutted,pCutting,&nCuttedPols,&nCuttingPols,pPols);
			if(nCuttedPols+nCuttingPols==4)
			{
				bCut=true;
				bAnyCut=true;
				delete pCutted;
				pCutted=NULL;
				lCandidatePols.push_back(new CPolygon(pPols[0]));
				lCandidatePols.push_back(new CPolygon(pPols[1]));
				break;
			}
			if(pCutted->m_Plane.InSamePlaneAs(pCutting->m_Plane))
			{ 
				bool bSameOrientation=pCutted->m_Plane==pCutting->m_Plane;
				if((bCutCoplanar && bSameOrientation) || !bSameOrientation)
				{
					bCut=CPolygon::CutCoplanar(pCutted,pCutting,&lCandidatePols);
					if(bCut)
					{
						delete pCutted;
						pCutted=NULL;
						break;
					}
				}
			}
		}
		// Remove Internal Polygons.
		if(!bCut)
		{
			if(pCutterPolyhedron->IsInternalPolygon(pCutted))
			{
				delete pCutted;
				pCutted=NULL;
			}
			else
			{
				pProcessed->m_vPolygons.push_back(pCutted);
			}
		}
	}
	return bAnyCut;
}

void RemoveInternalFaces(std::list<CPolyhedron*> *plOriginalPolyhedron,std::list<CPolyhedron*> *plProcessedPolyhedrons)
{
	std::vector<CPolygon *>::iterator				iPolygon,iPolygon2;
	std::list<CPolyhedron*>::iterator				iOrig,iOrig2,iTemp;
	std::map<CPolyhedron*,CPolyhedron*>				mPolyhedrons;
	std::map<CPolyhedron*,CPolyhedron*>::iterator	iProcessed,iProcessed2;

	for(iOrig=plOriginalPolyhedron->begin();iOrig!=plOriginalPolyhedron->end();iOrig++)
	{
		CPolyhedron	*pOriginal=*iOrig;
		CPolyhedron	*pCopy=new CPolyhedron;
		mPolyhedrons[pOriginal]=pCopy;
		for(iPolygon=pOriginal->m_vPolygons.begin();iPolygon!=pOriginal->m_vPolygons.end();iPolygon++)
		{
			CPolygon	*pOriginalPolygon=*iPolygon;
			CPolygon	*pCopyPolygon=new CPolygon(*pOriginalPolygon);
			pCopy->m_vPolygons.push_back(pCopyPolygon);
		}
	}

	bool bAnyCut=false;
	do
	{
		bAnyCut=false;
		int nCurrentPol=0;
		for(iOrig=plOriginalPolyhedron->begin();iOrig!=plOriginalPolyhedron->end();iOrig++,nCurrentPol++)
		{
			for(iOrig2=plOriginalPolyhedron->begin();iOrig2!=plOriginalPolyhedron->end();iOrig2++)
			{
				CPolyhedron *pOrig1=*iOrig;
				CPolyhedron *pOrig2=*iOrig2;
				if(pOrig1!=pOrig2 && CheckBBoxCollision(pOrig1,pOrig2))
				{
					CPolyhedron *pProcessed=mPolyhedrons[pOrig2];
					bool bAnyCutTemp=GetExternalPolygons(pOrig1,pProcessed,true);//nCurrentPol<nPolCount);
					if(bAnyCutTemp)
					{bAnyCut=bAnyCutTemp;}
				}
			}
		}
		
		// Eliminacion de poligonos duplicados
		for(iProcessed=mPolyhedrons.begin();iProcessed!=mPolyhedrons.end();iProcessed++)
		{
			CPolyhedron *pPolyhedron=iProcessed->second;
			for(iPolygon=pPolyhedron->m_vPolygons.begin();iPolygon!=pPolyhedron->m_vPolygons.end();iPolygon++)
			{
				CPolygon *pPolygon1=*iPolygon;
				for(iProcessed2=mPolyhedrons.begin();iProcessed2!=mPolyhedrons.end();iProcessed2++)
				{
					CPolyhedron *pPolyhedron2=iProcessed2->second;
					for(iPolygon2=pPolyhedron2->m_vPolygons.begin();iPolygon2!=pPolyhedron2->m_vPolygons.end();iPolygon2++)
					{
						CPolygon *pPolygon2=*iPolygon2;
						if(pPolygon1!=pPolygon2)
						{
							if(*pPolygon1==*pPolygon2)
							{
								delete pPolygon2;
								pPolyhedron2->m_vPolygons.erase(iPolygon2);
								break;
							}
						}
					}
				}
			}
		}
	}while(bAnyCut);
	/*	CPolyhedron *pPoly=iProcessed->second;
		else
		{
			// comprobar que el poligono no este duplicado 

			bool bDuplicated=false;
			std::vector<CPolygon*>::iterator  iTemp;

			for(iTemp=pProcessed->m_vPolygons.begin();iTemp!=pProcessed->m_vPolygons.end();iTemp++)
			{
				CPolygon *pTemp=*iTemp;
				if(*pTemp==*pCutted)
				{
					bDuplicated=true;
					break;
				}
			}
			if(bDuplicated)
			{
				delete pCutted;
				pCutted=NULL;
			}
			else
			{
				pProcessed->m_vPolygons.push_back(pCutted);
			}
		}*/

	
	/*
	for(iOrig=plOriginalPolyhedron->begin();iOrig!=plOriginalPolyhedron->end();iOrig++,nCurrentPol++)
	{
		for(iOrig2=plOriginalPolyhedron->begin();iOrig2!=plOriginalPolyhedron->end();iOrig2++)
		{
			CPolyhedron *pOrig1=*iOrig;
			CPolyhedron *pOrig2=*iOrig2;
			if(pOrig1!=pOrig2 && CheckBBoxCollision(pOrig1,pOrig2))
			{
				CPolyhedron *pProcessed=mPolyhedrons[pOrig2];
				bool bAnyCut=GetExternalPolygons(pOrig1,pProcessed,nCurrentPol<nPolCount/2);
			}
		}
	}*/
	for(iOrig=plOriginalPolyhedron->begin();iOrig!=plOriginalPolyhedron->end();iOrig++)
	{
		CPolyhedron *pPoly=mPolyhedrons[*iOrig];
		if(pPoly->m_vPolygons.size()==0)
		{
			delete pPoly;
		}
		else
		{
			plProcessedPolyhedrons->push_back(pPoly);
		}
/*		for(iProcessed=mPolyhedrons.begin();iProcessed!=mPolyhedrons.end();iProcessed++)
		{
			CPolyhedron *pPoly=iProcessed->second;
			if(pPoly->m_vPolygons.size()==0)
			{
				delete pPoly;
			}
			else
			{
				plProcessedPolyhedrons->push_back(iProcessed->second);
			}
		}*/
	}
	mPolyhedrons.clear();
}

double drand()
{
	return (double)rand()/(double)RAND_MAX;
}
/*
	vForward.c[0] = cp*cy;
	vForward.c[1] = cp*sy;
	vForward.c[2] = -sp;
	vUp.c[0] = (sr*sp*cy+cr*-sy);
	vUp.c[1] = (sr*sp*sy+cr*cy);
	vUp.c[2] = sr*cp;
	vRight.c[0] = -1.0*(cr*sp*cy+-sr*-sy);
	vRight.c[1] = -1.0*(cr*sp*sy+-sr*cy);
	vRight.c[2] = -1.0*cr*cp;
}*/

CVector	CalcMins(CVector &v1,CVector &v2)
{
	CVector v;
	for(int c=0;c<3;c++){v.c[c]=std::min(v1.c[c],v2.c[c]);}
	return v;
}

CVector	CalcMaxs(CVector &v1,CVector &v2)
{
	CVector v;
	for(int c=0;c<3;c++){v.c[c]=std::max(v1.c[c],v2.c[c]);}
	return v;
}

double DegreesToRadians(double dAngle)
{
	return (dAngle*PI*2.0)/360.0;
}
double RadiansToDegrees(double dAngle)
{
	return (dAngle*360.0)/(PI*2.0);
}

CBSPNode::~CBSPNode()
{
	_debugtag_='B';
	pParent=NULL;
	m_pDrawNode=NULL;
	if(pChild[0]){delete pChild[0];pChild[0]=NULL;}
	if(pChild[1]){delete pChild[1];pChild[1]=NULL;}
}

// Helper para colisiones con BSPs de objetos en una posicion y angulo arbitrario y con un BBOX. 
// Pensado para objetos con los que se quiera hacer colisiones complejas. 
// Se usa un cambio de sistema de referencia para calcular la colision.
// Queda por implementar la devolucion del plano de corte
// Queda por optimizar por BBOX, aunque hay que tener en cuenta que el BBOX esta en el sistema de 
// referencia del objeto, no del mundo y ahora solo se utiliza para limitar los problemas que puedan 
// dar BSPs mal construidos.

CTraceInfo CBSPNode::GetObjectTrace(const CVector &p1,const CVector &p2,const CVector &vPosition,const CVector &vAngles,const CVector &vMins,const CVector &vMaxs)
{
	CVector vPointObject1=p1;
	CVector vPointObject2=p2;
	CVector vForward,vRight,vUp;
	VectorsFromAngles(vAngles,&vForward,&vRight,&vUp);
	CMatrix mWorlToObject;
	mWorlToObject.T(Origin-vPosition);
	vPointObject1*=mWorlToObject;
	vPointObject2*=mWorlToObject;
	mWorlToObject.Ref(vForward,vUp,vRight);
	vPointObject1*=mWorlToObject;
	vPointObject2*=mWorlToObject;
	CTraceInfo finalInfo;
	CTraceInfo info=GetTrace(vPointObject1,vPointObject2,vPointObject1,vPointObject2);
	if(info.m_bTraceHit)
	{
		finalInfo.m_bTraceHit=(	info.m_vTracePos.c[0]>=(vMins.c[0]-FP_PRECISION) && info.m_vTracePos.c[0]<=(vMaxs.c[0]+FP_PRECISION) &&
								info.m_vTracePos.c[1]>=(vMins.c[1]-FP_PRECISION) && info.m_vTracePos.c[1]<=(vMaxs.c[1]+FP_PRECISION) &&
								info.m_vTracePos.c[2]>=(vMins.c[2]-FP_PRECISION) && info.m_vTracePos.c[2]<=(vMaxs.c[2]+FP_PRECISION));
		if(finalInfo.m_bTraceHit)
		{
			finalInfo.m_vTracePos=vPosition;
			finalInfo.m_vTracePos+=vForward*info.m_vTracePos.c[0];
			finalInfo.m_vTracePos+=vRight*info.m_vTracePos.c[2];
			finalInfo.m_vTracePos+=vUp*info.m_vTracePos.c[1];
			finalInfo.m_dTraceFraction=info.m_dTraceFraction;
			finalInfo.m_nTraceContent=info.m_nTraceContent;

			CVector vPlaneNormal;
			vPlaneNormal+=vForward*info.m_vTracePlane.c[0];
			vPlaneNormal+=vRight*info.m_vTracePlane.c[2];
			vPlaneNormal+=vUp*info.m_vTracePlane.c[1];
			vPlaneNormal.N();
			finalInfo.m_vTracePlane=CPlane(vPlaneNormal,finalInfo.m_vTracePos);
		}
	}
	return finalInfo;
}

// From http://www.deadbeef.com/index.php/converting_rgb_to_hsv_in_c
CVector RGBToHSV(const CVector &v1)
{
	// RGB are from 0..1, H is from 0..360, SV from 0..1
	CVector vHSV;
	double maxC = v1.c[2];
	if (maxC < v1.c[1]) {maxC = v1.c[1];}
	if (maxC < v1.c[0]) {maxC = v1.c[0];}
	double minC = v1.c[2];
	if (minC > v1.c[1]) {minC = v1.c[1];}
	if (minC > v1.c[0]) {minC = v1.c[0];}
	double delta = maxC - minC;
	vHSV.c[2] = maxC;
	if (delta != 0)
	{
		vHSV.c[1] = delta / maxC;
		double dR = 60*(maxC - v1.c[0])/delta + 180;
		double dG = 60*(maxC - v1.c[1])/delta + 180;
		double dB = 60*(maxC - v1.c[2])/delta + 180;
		if (v1.c[0] == maxC){vHSV.c[0] = dB - dG;}
		else if (v1.c[1] == maxC){vHSV.c[0] = 120 + dR - dB;}
		else{vHSV.c[0] = 240 + dG - dR;}
	}
	if (vHSV.c[0]<0){vHSV.c[0]+=360;}
	if (vHSV.c[0]>=360){vHSV.c[0]-=360;}
	return vHSV;
}
// From http://splinter.com.au/blog/?p=29
CVector HSVToRGB(const CVector &hsv)
{
  double H = hsv.c[0];
  double S = hsv.c[1];
  double V = hsv.c[2];
  while (H < 0) { H += 360; };
  while (H >= 360) { H -= 360; };
  double R, G, B;
  if (V <= 0)
  {R = G = B = 0;}
  else if (S <= 0)
  {R = G = B = V;}
  else
  {
    double hf = H / 60.0;
    int i = (int)floor(hf);
    double f = hf - i;
    double pv = V * (1 - S);
    double qv = V * (1 - S * f);
    double tv = V * (1 - S * (1 - f));
    switch (i)
    {
	  case 0:R = V;G = tv;B = pv;break;
      case 1:R = qv;G = V;B = pv;break;
      case 2:R = pv;G = V;B = tv;break;
      case 3:R = pv;G = qv;B = V;break;
      case 4:R = tv;G = pv;B = V;break;
      case 5:R = V;G = pv;B = qv;break;
      case 6:R = V;G = tv;B = pv;break;
      case -1:R = V;G = pv;B = qv;break;
      default:R = G = B = V;break;
    }
  }
  if(R>1.0){R=1.0;}
  if(G>1.0){G=1.0;}
  if(B>1.0){B=1.0;}
  if(R<0.0){R=0.0;}
  if(G<0.0){G=0.0;}
  if(B<0.0){B=0.0;}
  CVector rgb(R,G,B);
  return rgb;
}

CVector HSVDifference(CVector &v1,CVector &v2)
{
	CVector v1HSV=RGBToHSV(v1);
	CVector v2HSV=RGBToHSV(v2);
	return v2HSV-v1HSV;
}

void FromOpenGLMatrix(double *dMatrix,CMatrix *pMatrix)
{
	pMatrix->e[0][0]=dMatrix[4*0+0];
	pMatrix->e[1][0]=dMatrix[4*0+1];
	pMatrix->e[2][0]=dMatrix[4*0+2];
	pMatrix->e[3][0]=dMatrix[4*0+3];
	pMatrix->e[0][1]=dMatrix[4*1+0];
	pMatrix->e[1][1]=dMatrix[4*1+1];
	pMatrix->e[2][1]=dMatrix[4*1+2];
	pMatrix->e[3][1]=dMatrix[4*1+3];
	pMatrix->e[0][2]=dMatrix[4*2+0];
	pMatrix->e[1][2]=dMatrix[4*2+1];
	pMatrix->e[2][2]=dMatrix[4*2+2];
	pMatrix->e[3][2]=dMatrix[4*2+3];
	pMatrix->e[0][3]=dMatrix[4*3+0];
	pMatrix->e[1][3]=dMatrix[4*3+1];
	pMatrix->e[2][3]=dMatrix[4*3+2];
	pMatrix->e[3][3]=dMatrix[4*3+3];
}

void ToOpenGLMatrix(CMatrix *pMatrix,double *dMatrix)
{
	dMatrix[4*0+0]=pMatrix->e[0][0];
	dMatrix[4*0+1]=pMatrix->e[1][0];
	dMatrix[4*0+2]=pMatrix->e[2][0];
	dMatrix[4*0+3]=pMatrix->e[3][0];
	dMatrix[4*1+0]=pMatrix->e[0][1];
	dMatrix[4*1+1]=pMatrix->e[1][1];
	dMatrix[4*1+2]=pMatrix->e[2][1];
	dMatrix[4*1+3]=pMatrix->e[3][1];
	dMatrix[4*2+0]=pMatrix->e[0][2];
	dMatrix[4*2+1]=pMatrix->e[1][2];
	dMatrix[4*2+2]=pMatrix->e[2][2];
	dMatrix[4*2+3]=pMatrix->e[3][2];
	dMatrix[4*3+0]=pMatrix->e[0][3];
	dMatrix[4*3+1]=pMatrix->e[1][3];
	dMatrix[4*3+2]=pMatrix->e[2][3];
	dMatrix[4*3+3]=pMatrix->e[3][3];
}

void CalcCameraVolume(CVector vPosition,CVector vAngles, double dViewAngle, double dAspectRatio, double dNear, double dFar,CVector *pPoints)
{
	CVector vForward,vRight,vUp;
	CVector vNearOffset,vFarOffset;

	VectorsFromAngles(vAngles,&vForward,&vRight,&vUp);

	vNearOffset.c[0]=dNear*tan(DegreesToRadians(dViewAngle*0.5))*dAspectRatio;
	vNearOffset.c[2]=dNear*tan(DegreesToRadians(dViewAngle*0.5));
	vFarOffset.c[0]=dFar*tan(DegreesToRadians(dViewAngle*0.5))*dAspectRatio;
	vFarOffset.c[2]=dFar*tan(DegreesToRadians(dViewAngle*0.5));

	CVector vNearBase,vFarBase;
	vNearBase=vPosition+vForward*dNear;
	vFarBase=vPosition+vForward*dFar;

	pPoints[0]=vNearBase+vRight*vNearOffset.c[0]+vUp*vNearOffset.c[2];
	pPoints[1]=vNearBase-vRight*vNearOffset.c[0]+vUp*vNearOffset.c[2];
	pPoints[2]=vNearBase-vRight*vNearOffset.c[0]-vUp*vNearOffset.c[2];
	pPoints[3]=vNearBase+vRight*vNearOffset.c[0]-vUp*vNearOffset.c[2];
	pPoints[4]=vFarBase+vRight*vFarOffset.c[0]+vUp*vFarOffset.c[2];
	pPoints[5]=vFarBase-vRight*vFarOffset.c[0]+vUp*vFarOffset.c[2];
	pPoints[6]=vFarBase-vRight*vFarOffset.c[0]-vUp*vFarOffset.c[2];
	pPoints[7]=vFarBase+vRight*vFarOffset.c[0]-vUp*vFarOffset.c[2];
}


void CalcCameraPlanes(CVector vPosition,CVector vAngles, double dViewAngle, double dAspectRatio, double dNear, double dFar,CPlane *pvPlanes)
{
	CVector vForward,vRight,vUp;
	CVector vNearOffset,vFarOffset;

	VectorsFromAngles(vAngles,&vForward,&vRight,&vUp);

	vNearOffset.c[0]=dNear*tan(DegreesToRadians(dViewAngle*0.5))*dAspectRatio;
	vNearOffset.c[2]=dNear*tan(DegreesToRadians(dViewAngle*0.5));
	vFarOffset.c[0]=dFar*tan(DegreesToRadians(dViewAngle*0.5))*dAspectRatio;
	vFarOffset.c[2]=dFar*tan(DegreesToRadians(dViewAngle*0.5));

	CVector vNearBase,vFarBase;
	vFarBase=vPosition+vForward*dFar;
	CVector vPoints[4];
	vPoints[0]=vFarBase+vRight*vFarOffset.c[0]+vUp*vFarOffset.c[2];
	vPoints[1]=vFarBase-vRight*vFarOffset.c[0]+vUp*vFarOffset.c[2];
	vPoints[2]=vFarBase-vRight*vFarOffset.c[0]-vUp*vFarOffset.c[2];
	vPoints[3]=vFarBase+vRight*vFarOffset.c[0]-vUp*vFarOffset.c[2];

	pvPlanes[0]=CPlane(vPoints[1],vPoints[0],vPosition);
	pvPlanes[1]=CPlane(vPoints[2],vPoints[1],vPosition);
	pvPlanes[2]=CPlane(vPoints[3],vPoints[2],vPosition);
	pvPlanes[3]=CPlane(vPoints[0],vPoints[3],vPosition);
}

void CalcBBoxVolume(const CVector &vOrigin,const CVector &vOrientation,const CVector &vMins,const CVector &vMaxs,CVector *pVolumePoints)
{
	pVolumePoints[0]=CVector(vMaxs.c[0],vMins.c[1],vMins.c[2]);
	pVolumePoints[1]=CVector(vMaxs.c[0],vMins.c[1],vMaxs.c[2]);
	pVolumePoints[2]=CVector(vMins.c[0],vMins.c[1],vMaxs.c[2]);
	pVolumePoints[3]=CVector(vMins.c[0],vMins.c[1],vMins.c[2]);
	pVolumePoints[4]=CVector(vMaxs.c[0],vMaxs.c[1],vMins.c[2]);
	pVolumePoints[5]=CVector(vMaxs.c[0],vMaxs.c[1],vMaxs.c[2]);
	pVolumePoints[6]=CVector(vMins.c[0],vMaxs.c[1],vMaxs.c[2]);
	pVolumePoints[7]=CVector(vMins.c[0],vMaxs.c[1],vMins.c[2]);

	int nRotations=0;
	if(vOrientation.c[0]!=0){nRotations++;}
	if(vOrientation.c[1]!=0){nRotations++;}
	if(vOrientation.c[2]!=0){nRotations++;}
	if(nRotations)
	{
		CMatrix m;
		if(nRotations==1)
		{
			if(vOrientation.c[0]!=0){m.R(CVector(0,1,0),DegreesToRadians(vOrientation.c[0]));}
			else if(vOrientation.c[1]!=0){m.R(CVector(0,0,1),DegreesToRadians(vOrientation.c[1]));}
			else if(vOrientation.c[2]!=0){m.R(CVector(1,0,0),DegreesToRadians(vOrientation.c[2]));}
		}
		else
		{
			CMatrix aux;
			if(vOrientation.c[2]){aux.R(CVector(1,0,0),DegreesToRadians(vOrientation.c[2]));m*=aux;}
			if(vOrientation.c[1]){aux.R(CVector(0,0,1),DegreesToRadians(vOrientation.c[1]));m*=aux;}
			if(vOrientation.c[0]){aux.R(CVector(0,1,0),DegreesToRadians(vOrientation.c[0]));m*=aux;}
		}
		// bottom
		for(int x=0;x<8;x++)
		{
			pVolumePoints[x]*=m;
			pVolumePoints[x]+=vOrigin;
		}	
	}
	else
	{
		for(int x=0;x<8;x++)
		{
			pVolumePoints[x]+=vOrigin;
		}
	}
}
void ToCameraRef( CPlane *pForwardPlane,CPlane *pRightPlane,CPlane *pUpPlane,int nPoints, CVector *pvPoints) 
{
	for(int x=0;x<nPoints;x++)
	{
		double dZ=pForwardPlane->GetSide(pvPoints[x]);
		double dX=pRightPlane->GetSide(pvPoints[x]);
		double dY=pUpPlane->GetSide(pvPoints[x]);
		pvPoints[x].c[0]=dX;
		pvPoints[x].c[1]=dY;
		pvPoints[x].c[2]=dZ;
	}
}

double GetBBoxRadius( const CVector &vMins,const CVector &vMaxs )
{
	double dRadius=0;
	for(int r=0;r<3;r++)
	{
		double dDimensionMax=std::max(fabs(vMaxs.c[r]),fabs(vMins.c[r]));
		dRadius=std::max(dRadius,dDimensionMax);
	}
	return dRadius;
}

void ComputeReferenceSystem( CVector vRefSysPos,CVector vRefSysAngles,CVector vPosition,CVector vAngles,CVector *pvPosition,CVector *pvAngles,CVector *pvX,CVector *pvY,CVector *pvZ)
{
	CVector vForward,vRight,vUp;
	VectorsFromAngles(vRefSysAngles,&vForward,&vRight,&vUp);

	CVector vLocalForward,vLocalRight,bLocalUp;
	VectorsFromAngles(vAngles,&vLocalForward,&vLocalRight,&bLocalUp);

	CVector vGlobalForward,vGlobalRight,vGlobalUp;
	vGlobalForward=vForward*vLocalForward.c[0]+vUp*vLocalForward.c[1]+vRight*vLocalForward.c[2];
	vGlobalRight=vForward*vLocalRight.c[0]+vUp*vLocalRight.c[1]+vRight*vLocalRight.c[2];
	vGlobalUp=vForward*bLocalUp.c[0]+vUp*bLocalUp.c[1]+vRight*bLocalUp.c[2];

	if(pvPosition){*pvPosition=vRefSysPos+vForward*vPosition.c[0]+vUp*vPosition.c[1]+vRight*vPosition.c[2];}
	if(pvAngles){*pvAngles=AnglesFromVectors(vGlobalForward,vGlobalRight,vGlobalUp);}
	if(pvX){*pvX=vGlobalForward;}
	if(pvY){*pvY=vGlobalUp;}
	if(pvZ){*pvZ=vGlobalRight;}
}


bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CVector> *pItem)
{
	char sTemp[1024]={0};
	CVector *pVector=pItem->GetValueAddress();
	sprintf(sTemp,"%f,%f,%f",pVector->c[0],pVector->c[1],pVector->c[2]);
	if(piNode){piNode->SetValue(sTemp);}
	return piNode?true:false;
}

bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CVector> *pItem)
{
	pItem->SetDefaultValue();
	if(!piNode || !piNode->GetValue()){return false;}

	char sTemp[1024]={0};
	strcpy(sTemp,piNode->GetValue());
	CVector vPos;
	char *pToken=strtok(sTemp,", ");
	if(pToken){vPos.c[0]=atof(pToken);pToken=strtok(NULL,", ");}
	if(pToken){vPos.c[1]=atof(pToken);pToken=strtok(NULL,", ");}
	if(pToken){vPos.c[2]=atof(pToken);}
	(*pItem->GetValueAddress())=vPos;
	return true;
}

bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CVector> *pItem)
{
	return true;
}

void MRPersistencyInitialize(CMRPersistentReferenceT<CVector> *pItem){(*pItem->GetValueAddress())=Origin;}
void MRPersistencyFree(CMRPersistentReferenceT<CVector> *pItem){}

bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CRGBColor> *pItem)
{
	char sTemp[1024]={0};
	CRGBColor *pVector=pItem->GetValueAddress();
	sprintf(sTemp,"%f,%f,%f",pVector->c[0]*255.0,pVector->c[1]*255.0,pVector->c[2]*255.0);
	if(piNode){piNode->SetValue(sTemp);}
	return piNode?true:false;
}

bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CRGBColor> *pItem)
{
	pItem->SetDefaultValue();
	if(!piNode || !piNode->GetValue()){return false;}

	char sTemp[1024]={0};
	strcpy(sTemp,piNode->GetValue());
	CRGBColor vPos;
	char *pToken=strtok(sTemp,", ");
	if(pToken){vPos.c[0]=atof(pToken)/255.0;pToken=strtok(NULL,", ");}
	if(pToken){vPos.c[1]=atof(pToken)/255.0;pToken=strtok(NULL,", ");}
	if(pToken){vPos.c[2]=atof(pToken)/255.0;}
	(*pItem->GetValueAddress())=vPos;
	return true;
}

bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CRGBColor> *pItem)
{
	return true;
}


void    MRPersistencyInitialize(CMRPersistentReferenceT<CRGBColor> *pItem){(*pItem->GetValueAddress())=Origin;}
void    MRPersistencyFree(CMRPersistentReferenceT<CRGBColor> *pItem){}
