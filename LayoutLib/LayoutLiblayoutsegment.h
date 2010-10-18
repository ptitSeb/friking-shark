#ifndef _mr_gui_layoutsegment_
#define _mr_gui_layoutsegment_

	/*------------ mr_gui_layoutsegment
	*/

	enum LayoutDirection
	{
		LD_Unknown		= 0,
		LD_Horizontal	= 1,
		LD_Vertical		= 2
	};

	class CMRLayoutSegment  
	{
	public:
		CMRLayoutSegment();
		CMRLayoutSegment(const CMRLayoutSegment &seg);
		CMRLayoutSegment(int first, int second, LayoutDirection direction);
		CMRLayoutSegment(RECT r, LayoutDirection direction);
		CMRLayoutSegment(POINT p, LayoutDirection direction);
		CMRLayoutSegment(POINT p);
		virtual ~CMRLayoutSegment();

		
		CMRLayoutSegment &MoveFirstTo(const int &value);
		CMRLayoutSegment &MoveSecondTo(const int &value);

		CMRLayoutSegment &SizeFromFirst(const int &value);
		CMRLayoutSegment &SizeFromSecond(const int &value);

		CMRLayoutSegment &TrimTo(const CMRLayoutSegment &segment);

		operator POINT();
		operator RECT();
		operator int();

		CMRLayoutSegment & operator += (const int &inc);
		CMRLayoutSegment  operator + (const int &inc);

		CMRLayoutSegment &FromRect(RECT &r, LayoutDirection dir=LD_Unknown);
		
		int Size();
		
		bool Intersects(CMRLayoutSegment &seg, bool inclusive = true);
		bool Includes  (CMRLayoutSegment &seg);

		static void SegmentsFromRect(CMRLayoutSegment &horizontal, CMRLayoutSegment &vertical, RECT &r);
		static void RectFromSegments(RECT &r, CMRLayoutSegment &horizontal, CMRLayoutSegment &vertical);

		static void Intersect (CMRLayoutSegment &s1, CMRLayoutSegment &s2);


		int m_iFirst;
		int m_iSecond;
		LayoutDirection m_ldDirection;
	};

#endif
