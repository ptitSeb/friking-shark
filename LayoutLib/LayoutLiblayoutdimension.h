#ifndef _mr_gui_layoutdimension_
#define _mr_gui_layoutdimension_

/*------------ mr_gui_layoutdimension
 */
	class CMRLayout;
	class CMRLayoutUnit;

	class CMRLayoutDimension  
	{
	protected:
		friend CMRLayoutConstraint;

		CMRLayoutPositionConstraint	*m_lcPrimary;
		map<int, CMRLayoutConstraint *> m_lSecondarys;
		int							m_iLastSecondary;

		GUID					m_idStep;

		int						m_iMinConstraint;
		int						m_iMaxConstraint;
		int						m_iTrimConstraint;
		
		bool					m_bHaveMinSize;
		int						m_iMinSize;

		bool					m_bHaveMaxSize;
		int						m_iMaxSize;

		bool					m_bTrim;
		bool					m_bClip;

	public:
		CMRLayoutDimension();
		virtual ~CMRLayoutDimension();

		bool Resolve(GUID step);
		bool IsResolved();

		void SetBounds(CMRLayoutSegment &bounds);
		void GetSegment(CMRLayoutSegment *bounds);
		void SetDirty();
		
		void SetPrimaryRestriction(CMRLayoutPositionConstraint *pPrimary);
		int  SetSecondaryRestriction(CMRLayoutConstraint *pSecondary, int index=-1);
		void SetSize(CMRLayoutSizeConstraint *pSize);

		void EnableMinSize(int minSize)
		{
			((CMRLayoutMinSizeConstraint *)m_lSecondarys[m_iMinConstraint])->m_fValue = (float)minSize;
			m_lSecondarys[m_iMinConstraint]->m_bEnabled = true;
		//		m_bHaveMinSize=true;m_iMinSize = minSize;
		};
		void DisableMinSize()
		{
			m_lSecondarys[m_iMinConstraint]->m_bEnabled = false;
		//		m_bHaveMinSize=false;
		};
		void EnableMaxSize(int maxSize)
		{
			((CMRLayoutMaxSizeConstraint *)m_lSecondarys[m_iMaxConstraint])->m_fValue = (float)maxSize;
			m_lSecondarys[m_iMaxConstraint]->m_bEnabled = true;
		//	m_bHaveMaxSize=true;m_iMaxSize = maxSize;
		};
		void DisableMaxSize()
		{
			m_lSecondarys[m_iMaxConstraint]->m_bEnabled = false;
	//		m_bHaveMaxSize=false;
		};

		void EnableTrim()
		{
			m_lSecondarys[m_iTrimConstraint]->m_bEnabled=true;
	//		m_bTrim=true;
		};
		void DisableTrim()
		{
			m_lSecondarys[m_iTrimConstraint]->m_bEnabled=false;
	//		m_bTrim=false;
		};

		void EnableClip(){m_bTrim=true;};
		void DisableClip(){m_bTrim=false;};
		bool GetClip(){return m_bTrim;};

		void GetParentRelativeBound(CMRLayoutSegment &bound);

		LayoutDirection GetDirection();

		bool					m_bPrimarySet;
		bool					m_bResolved;
		CMRLayoutSegment		m_lsBounds;
		CMRLayoutSegment		m_lsOriginalBounds;
		CMRLayout				*m_pParentLayout;
		CMRLayoutUnit			*m_pParentUnit;
	};

#endif
