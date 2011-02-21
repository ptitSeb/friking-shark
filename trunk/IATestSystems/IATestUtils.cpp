#include "./stdafx.h"
#include <GL/gl.h>
#include "IATestUtils.h"

#ifdef WIN32
#pragma warning ( disable : 4244 )
#endif

void RenderBBox(CVector vMins,CVector vMaxs,CVector vColor)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glColor3d(vColor.c[0],vColor.c[1],vColor.c[2]);
	glDisable(GL_TEXTURE_2D);

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
void RenderTriangle(CVector vAxis1,CVector vAxis2,double dLength1,double dLenght2,CVector vColor)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glColor3d(vColor.c[0],vColor.c[1],vColor.c[2]);
	glDisable(GL_TEXTURE_2D);

	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glBegin(GL_TRIANGLES);

	CVector vPoints[3];
	vPoints[2]=vAxis1*(dLength1*0.5);
	vPoints[1]=CVector(0,0,0)-vAxis1*(dLength1*0.5)-vAxis2*(dLenght2*0.5);
	vPoints[0]=CVector(0,0,0)-vAxis1*(dLength1*0.5)+vAxis2*(dLenght2*0.5);

	// Top
	glVertex3f(vPoints[0].c[0],vPoints[0].c[1],vPoints[0].c[2]);
	glVertex3f(vPoints[1].c[0],vPoints[1].c[1],vPoints[1].c[2]);
	glVertex3f(vPoints[2].c[0],vPoints[2].c[1],vPoints[2].c[2]);


	glEnd();
	glPopAttrib();
	glEnable(GL_TEXTURE_2D);
}
void RenderBox(CVector vMins,CVector vMaxs,CVector vColor)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glColor3d(vColor.c[0],vColor.c[1],vColor.c[2]);
	glDisable(GL_TEXTURE_2D);

	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
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

void RenderAxises()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_DEPTH_TEST);
	RenderAxis(AxisPosX,100,RGB(0  ,0  ,255));
	RenderAxis(AxisPosY,100,RGB(255,255,0));
	RenderAxis(AxisPosZ,100,RGB(255,0  ,0));
	glEnable(GL_DEPTH_TEST);
	glPopAttrib();
}

void RenderObjectAxises(double dSize)
{
	RenderAxis(AxisPosX,dSize,RGB(0  ,0  ,255));
	RenderAxis(AxisPosY,dSize,RGB(255,255,0));
	RenderAxis(AxisPosZ,dSize,RGB(255,0  ,0));
}

void RenderAxis(CVector vAxis,double dLength,COLORREF cColor)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_DEPTH_TEST);
	CVector vEndPoint=vAxis*dLength;
	double dPointSize=0.0;

	glGetDoublev(GL_POINT_SIZE,&dPointSize);
	glPointSize(6.0);

	glColor3ub(GetRValue(cColor),GetGValue(cColor),GetBValue(cColor));

	glLineWidth(1);

	// dibujado de la linea del eje

	glBegin(GL_LINES);
	glVertex3d(0,0,0);
	glVertex3d(vEndPoint.c[0],vEndPoint.c[1],vEndPoint.c[2]);
	glEnd();


	glBegin(GL_POINTS);
	glVertex3d(vEndPoint.c[0],vEndPoint.c[1],vEndPoint.c[2]);
	glEnd();

	glPointSize(dPointSize);
	glEnable(GL_DEPTH_TEST);
	glPopAttrib();
}

void Ellipse3D(double x,double y,double rx,double ry)
{
	glLineWidth(1.0);
	glBegin(GL_LINE_STRIP);
	double dSlices=48;
	double dSliceSize=((2.0*PI)/(dSlices));
	for(double i=0;i<dSlices+1.0;i++)
	{
		double dAngle=i*dSliceSize;
		double dx1=x+cos(dAngle)*(double)rx;
		double dy1=y+sin(dAngle)*(double)ry;
		glVertex3d(dx1,dy1,5);
	}
	glEnd();
}

void Ellipse3DEx(double x,double y,double rx,double ry,CVector vAxisX,CVector vAxisY)
{
	glLineWidth(1.0);
	glBegin(GL_LINE_STRIP);
	double dSlices=48;
	double dSliceSize=((2.0*PI)/(dSlices));
	for(double i=0;i<dSlices+1.0;i++)
	{
		double dAngle=i*dSliceSize;
		double dx1=cos(dAngle)*(double)rx;
		double dy1=sin(dAngle)*(double)ry;
		CVector vPos=CVector(dx1,dy1,0);

		CMatrix m;
		m.Ref(vAxisX,vAxisY,CVector(0,0,1));
		vPos*=m;

		dx1=x+vPos.c[0];
		dy1=y+vPos.c[1];
		glVertex3d(dx1,dy1,5);
	}
	glEnd();
}

void Circle3D(double x,double y,double rx)
{
	Ellipse3D(x,y,rx,rx);
}