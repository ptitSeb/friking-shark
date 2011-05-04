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


#include "./stdafx.h"
#include "PlayAreaElementBase.h"
#include "PlayAreaCheckPoint.h"

CPlayAreaCheckPoint::CPlayAreaCheckPoint()
{
}

CPlayAreaCheckPoint::~CPlayAreaCheckPoint()
{
}

CVector CPlayAreaCheckPoint::GetCheckPointPosition()
{
	return m_vPosition;
}

void CPlayAreaCheckPoint::DesignRender( IGenericRender *piRender,bool bSelected)
{
}

CTraceInfo CPlayAreaCheckPoint::DesignGetTrace( const CVector &p1,const CVector &p2 )
{
	CTraceInfo info;
	info.m_dTraceFraction=1.0;
	info.m_vTracePos=p2;
	return info;
}