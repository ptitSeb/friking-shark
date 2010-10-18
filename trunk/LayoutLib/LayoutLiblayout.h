#ifndef _mr_gui_layout_
#define _mr_gui_layout_

/*------------ mr_gui_layout
 */

	class CMRLayoutAbsolutePositionConstraint;
	class CMRLayoutConstraint;

	class CMRLayoutUnitBounds
	{
	public:
		CMRLayoutUnitBounds()
		{
			m_idUnit = GUID_NULL;
			memset(&m_rBounds, 0, sizeof(RECT));
			memset(&m_rOriginalBounds, 0, sizeof(RECT));
		}

		CMRLayoutUnitBounds(GUID id, RECT r)
			{m_idUnit = id; m_rBounds = r;};

		~CMRLayoutUnitBounds(){};

		RECT	m_rBounds;
		RECT	m_rOriginalBounds;
		GUID	m_idUnit;
		bool	m_bHorizontalClip;
		bool	m_bVerticalClip;
	};

	class CMRLayout  
	{
	protected:
		friend class CMRLayoutConstraint;
		friend class CMRLayoutDimension;

		RECT						m_rBounds;
		RECT						m_rRelativeBounds;

		void MakeAbsolute(list<CMRLayoutUnitBounds> &rects);
		void Clip(list<CMRLayoutUnitBounds> &rects);
		void ClipRects(RECT &r1, RECT &r2);
		
		GUID	m_idLayout;
		char	m_alias[LAYOUT_STRING_SIZE];
	public:
		CMRLayout(char *alias = "");
		virtual ~CMRLayout();

		void AddUnit(CMRLayoutUnit *pUnit);
		void RemoveUnit(GUID unitId);
		virtual void ClearLayout();
		virtual bool Resolve(RECT bounds, GUID step = GUID_NULL);
		void SetDirty();

		virtual void GetRects(list<CMRLayoutUnitBounds> &rects, bool recursive = false);
		virtual void GetRelativeRects(list<CMRLayoutUnitBounds> &rects, bool recursive = false);
			
		GUID GetId(){return m_idLayout;}
		operator GUID(){return m_idLayout;}

		std::map<GUID, CMRLayoutUnit *>	m_mUnits;
		std::deque<CMRLayoutUnit *>		m_dUnits;
	};

#endif // !defined(AFX_MRLAYOUT_H__91AF8D2B_EF4D_4551_BE37_FB07EA935F6D__INCLUDED_)
