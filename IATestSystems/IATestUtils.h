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
void RenderAxis(CVector vAxis,double dLength,CVector vColor);
void RenderBBox(CVector vMins,CVector vMaxs,CVector vColor);
void RenderBox(CVector vMins,CVector vMaxs,CVector vColor);
void RenderTriangle(CVector vAxis1,CVector vAxis2,double dLength1,double dLenght2,CVector vColor);
void Circle3D(double x,double y,double rx);
void Ellipse3DEx(double x,double y,double rx,double ry,CVector vAxisX,CVector vAxisY);
void Ellipse3D(double x,double y,double rx,double ry);
