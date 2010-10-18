#pragma once


#define KEY_FORWARD		1
#define KEY_BACK		2
#define KEY_RIGHT		3
#define KEY_LEFT		4
#define KEY_UP			5
#define KEY_DOWN		6
#define KEY_PAUSE		7
#define KEY_PROCESS_ONE_FRAME		8

void RenderAxises();
void RenderObjectAxises(double dSize);
void RenderAxis(CVector vAxis,double dLength,COLORREF cColor);
void RenderBBox(CVector vMins,CVector vMaxs,CVector vColor);
void RenderBox(CVector vMins,CVector vMaxs,CVector vColor);
void RenderTriangle(CVector vAxis1,CVector vAxis2,double dLength1,double dLenght2,CVector vColor);
void Circle3D(double x,double y,double rx);
void Ellipse3DEx(double x,double y,double rx,double ry,CVector vAxisX,CVector vAxisY);
void Ellipse3D(double x,double y,double rx,double ry);
