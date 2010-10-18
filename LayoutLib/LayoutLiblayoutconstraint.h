#ifndef _mr_gui_layoutconstraint_
#define _mr_gui_layoutconstraint_

/*------------ mr_gui_layoutconstraint
 */

	#define MRLAYOUT_MINSIZE_PRIORITY  1005
	#define MRLAYOUT_MAXSIZE_PRIORITY  1015
	#define MRLAYOUT_TRIMSIZE_PRIORITY 1025


	static const GUID vlParent = GUID_NULL;

	//Constraint Edge
	static const GUID ceFirstComponent  = { 0x121c117f, 0x6c7b, 0x47e8, { 0xb9, 0x5a, 0x2a, 0xe0, 0xf2, 0x1c, 0x80, 0x45 } };
	static const GUID ceSecondComponent = { 0x7f9721a1, 0x52a2, 0x4492, { 0xbe, 0xce, 0x56, 0x97, 0x50, 0x4d, 0x7c, 0x2c } };
	static const GUID ceLeft	= { 0x23534fd7, 0x70d7, 0x4c2b, { 0x9d, 0x33, 0x9b, 0x61, 0x39, 0x25, 0xad, 0x62 } };//= ceFirstComponent;
	static const GUID ceRight	= { 0xcad5d2cb, 0x2e69, 0x43db, { 0xb7, 0xf1, 0x57, 0x73, 0x12, 0x0 , 0x67, 0xb1 } };//= ceSecondComponent;
	static const GUID ceTop		= { 0xdb1da1a2, 0xa858, 0x4380, { 0x8c, 0xe7, 0xd8, 0x91, 0xd3, 0x94, 0xae, 0x1d } };//= ceFirstComponent;
	static const GUID ceBottom	= { 0x84a55612, 0xaa34, 0x44c0, { 0xb7, 0x3a, 0x34, 0x6b, 0x42, 0xae, 0x78, 0x98 } };//= ceSecondComponent;

	//Constraint Size
	static const GUID csWidth  = { 0xb15518b1, 0x2a14, 0x46a3, { 0x8e, 0xeb, 0xe6, 0x39, 0x8a, 0xcf, 0x15, 0x1f } };
	static const GUID csHeight = { 0x2b327617, 0xfd6d, 0x4bfe, { 0x81, 0xd9, 0xdf, 0xba, 0x29, 0x16, 0x64, 0xe1 } };

	static const GUID csSize   = { 0x18c5a11b, 0x6ff8, 0x4f1d, { 0xa9, 0x3b, 0xf1, 0xe5, 0xf3, 0x0, 0x4, 0x5c } };

	//Constraint Relationship
	static const GUID crBefore = { 0x121c117f, 0x6c7b, 0x47e8, { 0xb9, 0x5a, 0x2a, 0xe0, 0xf2, 0x1c, 0x80, 0x45 } };
	static const GUID crAfter  = { 0x7f9721a1, 0x52a2, 0x4492, { 0xbe, 0xce, 0x56, 0x97, 0x50, 0x4d, 0x7c, 0x2c } };
	static const GUID crLeftOf = crBefore;
	static const GUID crRightOf = crAfter;
	static const GUID crAbove = crBefore;
	static const GUID crBelow = crAfter;
	static const GUID crMiddleOf = { 0xf4945515, 0x8b98, 0x48fd, { 0x84, 0x8d, 0x9c, 0x9e, 0xc4, 0x7, 0x80, 0x89 } };

	static const GUID crSameAs = { 0x789f0911, 0x6fb9, 0x4a33, { 0xb0, 0x12, 0x40, 0x19, 0xd4, 0xb5, 0x37, 0xf } };

	//Constraint Size Relationship
	static const GUID crPercent = { 0xb96a31b0, 0x7108, 0x414c, { 0xa6, 0x4f, 0x55, 0x5a, 0x17, 0x93, 0x7, 0x46 } };


	#define _NORMALIZE_EDGE(x, y) 		x = y; \
										if(x == ceLeft || x == ceTop){x = ceFirstComponent;}\
										else if	(x == ceRight || x == ceBottom){x = ceSecondComponent;}


	class CMRLayoutDimension;
	class CMRLayoutConstraint  
	{
	protected:
		RECT GetParentLayoutBounds();
		CMRLayoutConstraint *GetParentPrimaryRestriction();

	public:
		CMRLayoutConstraint();
		virtual ~CMRLayoutConstraint();

		void SetDirty();

		virtual bool Resolve(GUID step)=0;
		virtual void SetParentDimension(CMRLayoutDimension *parentDimension, bool primary)
			{m_ldParentDimension = parentDimension;	m_bPrimary=primary;}

		virtual int GetPriority(){return -1;}

		bool m_bEnabled;
		bool m_bResolved;
		bool m_bPrimary;
		CMRLayoutDimension	*m_ldParentDimension;

		friend class CMRLayoutDimension;
	};



	////////////////////////////////////////
	/////
	/////          Position
	/////
	class CMRLayoutPositionConstraint : public CMRLayoutConstraint
	{
	public:
		CMRLayoutPositionConstraint();
		virtual ~CMRLayoutPositionConstraint(){};

		bool Resolve(GUID step);

	};

	class CMRLayoutAbsolutePositionConstraint : public CMRLayoutPositionConstraint
	{
		CMRLayoutAbsolutePositionConstraint():CMRLayoutPositionConstraint(){};
	public:
		CMRLayoutAbsolutePositionConstraint(GUID myEdge, int value):CMRLayoutPositionConstraint()
		{
			m_ceMyEdge = myEdge;
			m_fValue = (float)value;
		};
		virtual ~CMRLayoutAbsolutePositionConstraint(){};

		bool Resolve(GUID step);

		float			m_fValue;
		GUID			m_ceMyEdge;
	};

	class CMRLayoutRelativePositionConstraint : public CMRLayoutPositionConstraint
	{
		CMRLayoutRelativePositionConstraint():CMRLayoutPositionConstraint(){};
	public:
		CMRLayoutRelativePositionConstraint(GUID myEdge, GUID relation, GUID relativeUnit, GUID itsEdge, int value, int value2 = 0):CMRLayoutPositionConstraint()
		{
			_NORMALIZE_EDGE(m_ceMyEdge, myEdge);
			if(m_ceMyEdge != ceFirstComponent && m_ceMyEdge != ceSecondComponent)
				{_ASSERTE(false);}
			
			if(relation != crPercent)
			{
				_NORMALIZE_EDGE(m_ceItsEdge, itsEdge);
				if(m_ceItsEdge != ceFirstComponent && m_ceItsEdge != ceSecondComponent)
					{_ASSERTE(false);}
			}
			else
			{
				m_ceItsEdge = csSize;
			}

			m_crRelation = relation;
			_ASSERTE(m_crRelation == crAfter || m_crRelation == crBefore || m_crRelation == crMiddleOf || m_crRelation == crSameAs || m_crRelation == crPercent);
			
			m_idRelativeUnit = relativeUnit;
			m_fValue = (float)value;
			m_fValue2 = (float)value2;
		};
		virtual ~CMRLayoutRelativePositionConstraint(){};

		bool Resolve(GUID step);

		GUID			m_ceMyEdge;
		GUID			m_ceItsEdge;
		GUID			m_idRelativeUnit;
		GUID			m_crRelation;
		float			m_fValue;
		float			m_fValue2;
	};

	class CMRLayoutBetweenPositionConstraint : public CMRLayoutPositionConstraint
	{
		CMRLayoutBetweenPositionConstraint():CMRLayoutPositionConstraint(){};
	public:
		CMRLayoutBetweenPositionConstraint(GUID relativeUnit1, GUID relativeUnit2, int value1, int value2):CMRLayoutPositionConstraint()
		{
			m_idRelativeUnit1 = relativeUnit1;
			m_idRelativeUnit2 = relativeUnit2;
			m_iValue1 = value1;
			m_iValue2 = value2;
		};
		virtual ~CMRLayoutBetweenPositionConstraint(){};

		bool Resolve(GUID step);

		GUID			m_idRelativeUnit1;
		GUID			m_idRelativeUnit2;
		int				m_iValue1;
		int				m_iValue2;
	};




	////////////////////////////////////////
	/////
	/////          Size
	/////
	class CMRLayoutSizeConstraint : public CMRLayoutConstraint
	{
	public:
		CMRLayoutSizeConstraint() : CMRLayoutConstraint(){};
		virtual ~CMRLayoutSizeConstraint(){};

		bool Resolve(GUID step){return true;};
	};

	class CMRLayoutAbsoluteSizeConstraint : public CMRLayoutSizeConstraint
	{
		CMRLayoutAbsoluteSizeConstraint():CMRLayoutSizeConstraint(){};
	public:
		CMRLayoutAbsoluteSizeConstraint(int value):CMRLayoutSizeConstraint()
		{
			m_fValue = (float)value;
		};
		virtual ~CMRLayoutAbsoluteSizeConstraint(){};

		bool Resolve(GUID step);

		float			m_fValue;
	};

	class CMRLayoutRelativeSizeConstraint : public CMRLayoutSizeConstraint
	{
		CMRLayoutRelativeSizeConstraint():CMRLayoutSizeConstraint(){};
	public:
		CMRLayoutRelativeSizeConstraint(GUID relation, GUID relativeUnit, GUID itsSize, int value) : CMRLayoutSizeConstraint()
		{
			m_csItsSize = itsSize;
			m_idRelativeUnit = relativeUnit;
			m_crRelation = relation;
			m_fValue = (float)value;
		};
		virtual ~CMRLayoutRelativeSizeConstraint(){};

		bool Resolve(GUID step);

		GUID			m_csItsSize;
		GUID			m_idRelativeUnit;
		GUID			m_crRelation;
		float			m_fValue;
	};

	class CMRLayoutMinSizeConstraint : public CMRLayoutAbsoluteSizeConstraint
	{
	public:
		CMRLayoutMinSizeConstraint():CMRLayoutAbsoluteSizeConstraint(0){};
		virtual bool Resolve(GUID step);
		virtual int GetPriority(){return MRLAYOUT_MINSIZE_PRIORITY;}
	};
	class CMRLayoutMaxSizeConstraint : public CMRLayoutAbsoluteSizeConstraint
	{
	public:
		CMRLayoutMaxSizeConstraint():CMRLayoutAbsoluteSizeConstraint(0){};
		virtual bool Resolve(GUID step);
		virtual int GetPriority(){return MRLAYOUT_MAXSIZE_PRIORITY;}
	};
	class CMRLayoutTrimSizeConstraint : public CMRLayoutSizeConstraint
	{
	public:
		CMRLayoutTrimSizeConstraint():CMRLayoutSizeConstraint(){};
		virtual bool Resolve(GUID step);
		virtual int GetPriority(){return MRLAYOUT_TRIMSIZE_PRIORITY;}

	};

#endif 
