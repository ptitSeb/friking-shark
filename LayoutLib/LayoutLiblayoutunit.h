#ifndef _mr_gui_layoutunit_
#define _mr_gui_layoutunit_

	/*------------ mr_gui_layoutunit
	*/

	#define LAYOUT_STRING_SIZE	512

	class CMRLayout;
	class CMRLayoutUnitBounds;

	class CMRLayoutUnit  
	{
	protected:
		CMRLayout				*m_vlParentLayout;
		CMRLayout				*m_vlChildLayout;

		bool					m_bHorizontalResolved;
		bool					m_bVerticalResolved;
		bool					m_bChildResolved;

		GUID					m_idUnit;
		char					m_alias[LAYOUT_STRING_SIZE];

	public:
		CMRLayoutDimension		m_ldHorizontal;
		CMRLayoutDimension		m_ldVertical;

	public:
		CMRLayoutUnit(char *alias = "");
		CMRLayoutUnit(RECT bounds, char *alias = "");

		virtual ~CMRLayoutUnit();

		virtual bool Resolve(GUID step);
		virtual bool GetDimensionBounds(CMRLayoutSegment &bounds, LayoutDirection direction, GUID step);
		
		virtual bool IsResolved();
		virtual void SetDirty();

		virtual void SetBoundsRect(RECT bounds);
		virtual RECT GetBoundsRect();
		virtual RECT GetOriginalBoundsRect();

		CMRLayoutUnitBounds GetUnitBounds();

		virtual RECT GetBoundsRectList(list<CMRLayoutUnitBounds> &rects);

		virtual void RestoreOriginalBounds();

		virtual void SetParentLayout(CMRLayout *parentLayout);
		virtual void SetChildLayout(CMRLayout *childLayout);

		operator GUID();

		friend class CMRLayout;
	};

#endif
