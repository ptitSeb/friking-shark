#include "./stdafx.h"

CMRLayoutSegment::CMRLayoutSegment()
{
	m_iFirst = 0; m_iSecond = 0;
	m_ldDirection = LD_Horizontal;
}
CMRLayoutSegment::~CMRLayoutSegment(){}

CMRLayoutSegment::CMRLayoutSegment(const CMRLayoutSegment &seg)
{
	m_iFirst = seg.m_iFirst;
	m_iSecond = seg.m_iSecond;
	m_ldDirection = seg.m_ldDirection;
}
CMRLayoutSegment::CMRLayoutSegment(int first, int second, LayoutDirection direction)
{
	m_iFirst = first; m_iSecond = second;
	m_ldDirection = direction;
}
CMRLayoutSegment::CMRLayoutSegment(RECT r, LayoutDirection direction)
{
	m_ldDirection = direction;
	FromRect(r);
}
CMRLayoutSegment::CMRLayoutSegment(POINT p, LayoutDirection direction)
{
	m_iFirst = p.x; m_iSecond = p.y;
	m_ldDirection = direction;
}
CMRLayoutSegment::CMRLayoutSegment(POINT p)
{
	m_iFirst = p.x; m_iSecond = p.y;
	m_ldDirection = LD_Horizontal;
}

CMRLayoutSegment &CMRLayoutSegment::FromRect(RECT &r, LayoutDirection dir)
{
	if(dir==LD_Horizontal || dir==LD_Vertical){m_ldDirection = dir;}

	if(m_ldDirection == LD_Horizontal)
		{m_iFirst = r.left; m_iSecond = r.right;}
	else if(m_ldDirection == LD_Vertical)
		{m_iFirst = r.top; m_iSecond = r.bottom;}
	
	return (*this);
}


CMRLayoutSegment::operator POINT()
{
	POINT p;
	p.x=m_iFirst;
	p.y=m_iSecond;
	return p;
}

CMRLayoutSegment::operator RECT()
{
	RECT r;
	if (m_ldDirection == LD_Horizontal)
	{	
		r.left = m_iFirst; r.right = m_iSecond;
		r.top = 0; r.bottom = 0;
	}
	else
	{
		r.left = 0; r.right = 0;
		r.top = m_iFirst; r.bottom = m_iSecond;
	}
	return r;
}

CMRLayoutSegment::operator int()
{
	return m_iSecond - m_iFirst;
}

int CMRLayoutSegment::Size()
{
	return abs(m_iSecond - m_iFirst);
}


CMRLayoutSegment & CMRLayoutSegment::operator += (const int &inc)
{
	m_iFirst	+= inc;
	m_iSecond	+= inc;
	return (*this);
}

CMRLayoutSegment  CMRLayoutSegment::operator + (const int &inc)
{
	CMRLayoutSegment dim(*this);
	dim.m_iFirst	+= inc;
	dim.m_iSecond	+= inc;
	return dim;
}

CMRLayoutSegment & CMRLayoutSegment::MoveFirstTo(const int &value)
{
	int temp = value - m_iFirst;
	m_iFirst = value;
	m_iSecond +=temp;
	return (*this);
}

CMRLayoutSegment & CMRLayoutSegment::MoveSecondTo(const int &value)
{
	int temp = value - m_iSecond;
	m_iSecond = value;
	m_iFirst +=temp;
	return (*this);
}

CMRLayoutSegment & CMRLayoutSegment::SizeFromFirst(const int &value)
{
	m_iSecond = m_iFirst + value;
	return (*this);
}

CMRLayoutSegment & CMRLayoutSegment::SizeFromSecond(const int &value)
{
	m_iFirst = m_iSecond - value;
	return (*this);
}


CMRLayoutSegment &CMRLayoutSegment::TrimTo(const CMRLayoutSegment &segment)
{
	int origFirst = m_iFirst, origSecond = m_iSecond;
	if (m_iFirst < segment.m_iFirst){m_iFirst = segment.m_iFirst;}
	if (m_iSecond > segment.m_iSecond){m_iSecond = segment.m_iSecond;}

	if (m_iFirst < origFirst){m_iFirst=origFirst;}
	if (m_iFirst > origSecond){m_iFirst=origSecond;}

	if (m_iSecond < origFirst){m_iSecond=origFirst;}
	if (m_iSecond > origSecond){m_iSecond=origSecond;}

	return (*this);
}

bool CMRLayoutSegment::Intersects(CMRLayoutSegment &seg, bool inclusive)
{
	bool ret = false;
	
	if (m_iFirst <= seg.m_iFirst && m_iSecond >= seg.m_iFirst){ret = true;}
	else if (seg.m_iFirst <= m_iFirst && seg.m_iSecond >= m_iFirst){ret = true;}
	
	if (ret==true && !inclusive){ret = !Includes(seg);}

	return ret;
}


bool CMRLayoutSegment::Includes  (CMRLayoutSegment &seg)
{
	return (m_iFirst <= seg.m_iFirst && m_iSecond >= seg.m_iSecond);
}



void CMRLayoutSegment::SegmentsFromRect(CMRLayoutSegment &horizontal, CMRLayoutSegment &vertical, RECT &r)
{
	horizontal.FromRect(r, LD_Horizontal);
	vertical.FromRect(r, LD_Vertical);
}

void CMRLayoutSegment::RectFromSegments(RECT &r, CMRLayoutSegment &horizontal, CMRLayoutSegment &vertical)
{
	r.left = horizontal.m_iFirst;
	r.right = horizontal.m_iSecond;
	r.top = vertical.m_iFirst;
	r.bottom = vertical.m_iSecond;
}

void CMRLayoutSegment::Intersect (CMRLayoutSegment &s1, CMRLayoutSegment &s2)
{
	if (!s1.Intersects(s2, false) || !s2.Intersects(s1, false)){return;}

	CMRLayoutSegment *temp1, *temp2;
	
	//We order the segments
	if (s1.m_iFirst < s2.m_iFirst)
		{temp1 = &s1; temp2 = &s2;}
	else
		{temp1 = &s2; temp2 = &s1;}

	double difference = 0.0;

	difference = (temp1->m_iSecond - temp2->m_iFirst) / 2;
	
	temp1->m_iSecond -= (int)difference;
	temp2->m_iFirst  += (int)difference;
}
