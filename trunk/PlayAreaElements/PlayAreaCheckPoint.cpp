#include "StdAfx.h"
#include ".\PlayAreaElementBase.h"
#include ".\PlayAreaCheckPoint.h"

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