#ifndef _mr_gui_layoutwindow_
#define _mr_gui_layoutwindow_

	/*------------ mr_gui_layoutwindow
	*/

	class CMRLayoutWindow;
	class CMRSplitter;

	//////////////////
	/// Helpers

	#define MR_DEFINE_CONTROL_METRIC(control, metric) CMRLayoutUnitWindow *metric = MR_NEW_ARG2(CMRLayoutUnitWindow, control, #control);
	#define MR_DEFINE_CHILD_LAYOUT_METRIC(metric) CMRChildLayoutWindow *metric = MR_NEW_ARG1(CMRChildLayoutWindow, "##metric##_ChildLayout");

	/*
	Replace regular expressions:
		replace this
			CMRLayoutUnitWindow\:b+[*]\(\:i\)\:b+=\:b+MR_NEW_ARG1(CMRLayoutUnitWindow,\:b+\(\:i\));

	with this
			MR_DEFINE_CONTROL_METRIC(\2, \1);

	*/

	////////////////////////
	//// Custom Constraints

	class CMRLayoutTextSizeConstraintWindow : public CMRLayoutAbsoluteSizeConstraint
	{
	public:
		CMRLayoutTextSizeConstraintWindow(int value) : CMRLayoutAbsoluteSizeConstraint(value){};
		virtual ~CMRLayoutTextSizeConstraintWindow(){};

		bool Resolve(GUID step);
	};



	class CMRLayoutSplitterPositionConstraint : public CMRLayoutRelativePositionConstraint
	{
	public:
		CMRLayoutSplitterPositionConstraint(GUID myEdge, float *pfSplitterValue, int offset = 0) : CMRLayoutRelativePositionConstraint(myEdge, crPercent, vlParent, csSize, offset)
		{
			m_pSplitterValue = pfSplitterValue;
			m_pSplitter = NULL;
		};
		CMRLayoutSplitterPositionConstraint(GUID myEdge, CMRSplitter *pSplitter, int offset = 0) : CMRLayoutRelativePositionConstraint(myEdge, crPercent, vlParent, csSize, offset)
		{
			m_pSplitterValue = NULL;
			m_pSplitter = pSplitter;
		};
		virtual ~CMRLayoutSplitterPositionConstraint(){};

		bool Resolve(GUID step);
		
		float *m_pSplitterValue;
		CMRSplitter *m_pSplitter;
	};


	class CMRLayoutUnitBoundsWindow : public CMRLayoutUnitBounds
	{
	public:
		CMRLayoutUnitBoundsWindow() : CMRLayoutUnitBounds()
			{m_hWindow = 0;}
		CMRLayoutUnitBoundsWindow(GUID id, RECT r) : CMRLayoutUnitBounds(id, r){};
		CMRLayoutUnitBoundsWindow(GUID id, RECT r, HWND window) : CMRLayoutUnitBounds(id, r) 
			{m_hWindow = window;}
		CMRLayoutUnitBoundsWindow(CMRLayoutUnitBounds ref, HWND hWnd = NULL)
		{
			m_rBounds			= ref.m_rBounds;
			m_rOriginalBounds	= ref.m_rOriginalBounds;
			m_idUnit			= ref.m_idUnit;
			m_bHorizontalClip	= ref.m_bHorizontalClip;
			m_bVerticalClip		= ref.m_bVerticalClip;
			m_hWindow = hWnd;
		}
		~CMRLayoutUnitBoundsWindow(){}

		HWND m_hWindow;
	};

	class CMRLayoutUnitWindow : public CMRLayoutUnit
	{
	protected:
	public:
		CMRLayoutUnitWindow(HWND window, char *alias = "");
		~CMRLayoutUnitWindow(){};
		
		CMRLayoutUnitBoundsWindow GetUnitBounds();
		RECT GetBoundsRectList(list<CMRLayoutUnitBoundsWindow> &rects);

		void RestoreOriginalBounds();
		

		//High level shortcuts
		void Abs(GUID property, int value);

		void Rel(GUID property, GUID relation, GUID relativeUnit			, GUID otherProperty, int value = 0, int value2 = 0);
		void Rel(GUID property, GUID relation, CMRLayoutUnit *pRelativeUnit	, GUID otherProperty, int value = 0, int value2 = 0);

		void Splitter(GUID property, float *splitterValue, int offset = 0);
		void Splitter(GUID property, CMRSplitter &splitter, int offset = 0);
		
		void LeftOf(GUID relativeUnit				, int value = 0);
		void LeftOf(CMRLayoutUnit *pRelativeUnit	, int value = 0);

		void InLeftOf(GUID relativeUnit				, int value = 0);
		void InLeftOf(CMRLayoutUnit *pRelativeUnit	, int value = 0);

		void RightOf(GUID relativeUnit				, int value = 0);
		void RightOf(CMRLayoutUnit *pRelativeUnit	, int value = 0);

		void InRightOf(GUID relativeUnit			, int value = 0);
		void InRightOf(CMRLayoutUnit *pRelativeUnit	, int value = 0);

		void Above(GUID relativeUnit				, int value = 0);
		void Above(CMRLayoutUnit *pRelativeUnit		, int value = 0);

		void InTopOf(GUID relativeUnit				, int value = 0);
		void InTopOf(CMRLayoutUnit *pRelativeUnit	, int value = 0);

		void Below(GUID relativeUnit				, int value = 0);
		void Below(CMRLayoutUnit *pRelativeUnit		, int value = 0);

		void InBottomOf(GUID relativeUnit			, int value = 0);
		void InBottomOf(CMRLayoutUnit *pRelativeUnit, int value = 0);

		void SameAs(GUID property, GUID relativeUnit			, int value = 0);
		void SameAs(GUID property, CMRLayoutUnit *pRelativeUnit	, int value = 0);

		//Low level shortcuts
		void SetPrimaryRestriction(CMRLayoutPositionConstraint *pConstraint);
		void SetSecondaryRestriction(CMRLayoutConstraint *pConstraint);
		
		HWND	m_hWindow;
	};

	class CMRChildLayoutWindow : public CMRLayoutUnitWindow
	{
	protected:
	public:
		CMRChildLayoutWindow(char *alias = "");
		virtual ~CMRChildLayoutWindow();

		void AddUnit(CMRLayoutUnit *pUnit);
		void RemoveUnit(GUID unitId);
		virtual void ClearLayout();
	};

	class CMRLayoutWindow : public CMRLayout
	{
	protected:
		void MakeAbsolute(list<CMRLayoutUnitBoundsWindow> &rects);
		void Clip(list<CMRLayoutUnitBoundsWindow> &rects);

	public:
		CMRLayoutWindow(char *alias = "");
		CMRLayoutWindow(HWND window, char *alias = "");
		virtual ~CMRLayoutWindow();

		virtual void    Layout(int cx=0xFFFFFFFF,int cy=0xFFFFFFFF);

		void GetRects(list<CMRLayoutUnitBoundsWindow> &rects, bool recursive = false);

		HWND	m_hWindow;
	};


#endif 
