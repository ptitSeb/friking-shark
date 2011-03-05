#include "./stdafx.h"
#include "GameGUIUtil.h"

SGamePos::SGamePos():x(0),y(0)
{
}

SGamePos::SGamePos(double _x,double _y):x(_x),y(_y)
{
}

void SGamePos::ClipToUnit()
{
	if(x<0.0){x=0.0;}if(x>1.0){x=1.0;}
	if(y<0.0){y=0.0;}if(y>1.0){y=1.0;}
}

void SGamePos::ClipToRect(SGameRect *pRect)
{
	if(x<pRect->x){x=pRect->x;}
	if(y<pRect->y){y=pRect->y;}

	if(x>(pRect->x+pRect->w)){x=(pRect->x+pRect->w);}
	if(y>(pRect->y+pRect->h)){y=(pRect->y+pRect->h);}
}

void SGamePos::CenterOnRect(SGameRect *pRect)
{
	x=pRect->x+(pRect->w/2);
	y=pRect->y+(pRect->h/2);
}

SGameSize::SGameSize():w(0),h(0)
{
}

SGameSize::SGameSize(double _w,double _h):w(_w),h(_h)
{
}


SGameRect::SGameRect():x(0),y(0),w(0),h(0)
{
}

SGameRect::SGameRect(double _x,double _y,double _w,double _h):x(_x),y(_y),w(_w),h(_h)
{
}

void SGameRect::ClipToUnit()
{
	if(x<0.0){x=0.0;}if(x>1.0){x=1.0;}
	if(y<0.0){y=0.0;}if(y>1.0){y=1.0;}
	if(w<0.0){w=0.0;}if(w>1.0){w=1.0;}
	if(h<0.0){h=0.0;}if(h>1.0){h=1.0;}

	if(x+w>1.0){w=1.0-x;}
	if(y+h>1.0){h=1.0-y;}
}

void SGameRect::ClipToRect(SGameRect *pRect)
{
	SGamePos p1,p2;
	p1.x=x;
	p1.y=y;
	p2.x=x+w;
	p2.y=y+h;

	p1.ClipToRect(pRect);
	p2.ClipToRect(pRect);
	x=p1.x;
	y=p1.y;
	w=p2.x-p1.x;
	h=p2.y-p1.y;
}

bool SGameRect::Contains(SGamePos point)
{
	if(point.x<x){return false;}
	if(point.y<y){return false;}
	if(point.x>x+w){return false;}
	if(point.y>y+h){return false;}
	return true;
}

void SGameRect::CenterOnRect(SGameRect *pRect)
{
	x=(pRect->w-w)/2;
	y=(pRect->h-h)/2;
}

