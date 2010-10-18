#ifndef _mr_gui_splitter_
#define _mr_gui_splitter_

	/*------------ mr_gui_layoutunit
	*/

	#define MRM_SPLITTERCHANGE WM_USER + 26

	#define MRMF_POSITION					1
	#define MRMF_CHECK						2
	#define MRMF_UNCHECK					4


	//Splitter Styles
	#define VSS_Button					1
	#define VSS_SplitterBorder			2
	#define VSS_SplitterStripes			4
	#define VSS_SplitterSparseStripes	8
	#define VSS_SplitterSingleStripe	16

	#define HAVE_TO_PAINT_SPLITER(styles) (styles & (VSS_SplitterBorder | VSS_SplitterStripes | VSS_SplitterSparseStripes | VSS_SplitterSingleStripe))
	//class CMRLayoutWindow;
	class CMRSplitter;

	struct VaSplitterWindowData
	{
		VaSplitterWindowData(){};
		VaSplitterWindowData(ULONG oldWndProc){m_oldWndProc = oldWndProc;};
		ULONG					m_oldWndProc;
		std::list<CMRSplitter *>		m_mSplitters;
	};

	class CMRSplitterManager
	{
	protected:
		static std::map<HWND, VaSplitterWindowData>	m_mWindows;

		static LRESULT CALLBACK HostWindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

	public:
		CMRSplitterManager();
		~CMRSplitterManager();

		static bool RegisterSplitter(HWND hWnd, CMRSplitter *);
		static bool UnregisterSplitter(HWND hWnd, CMRSplitter *);
	};


	class CMRSplitter
	{
	friend class CMRSplitterManager;
	protected:
		HWND		m_hWnd;
		RECT		m_rLastInvertedRect;
		bool		m_bLButtonDown;
		bool		m_bVertical;
		bool		m_bEnabled;
		bool		m_bPaint;
		bool		m_bShadowDrawn;

		bool		m_bHaveButton;
		bool		m_bChecked;

		float		m_fCheckedSplitter;
		float		m_fUncheckedSplitter;
		DWORD		m_dwButtonHeight;
		
		DWORD		m_dwStyle;

		//Button Bitmaps
		bool		m_bAutoColorKey;
		bool		m_bEnableColorKey;
		COLORREF	m_colorKey;

		HBITMAP		m_hCheckedBitmapHandle;
		HDC			m_hCheckedBitmapDC;
		BITMAP		m_checkedBitmapInfo;
		
		HBITMAP		m_hUncheckedBitmapHandle;
		HDC			m_hUncheckedBitmapDC;
		BITMAP		m_uncheckedBitmapInfo;

		bool CursorOverSplitter(POINT cursorPoint);
		void DrawSplitterShadow(bool bDraw, POINT *pCursorPoint = NULL);

		bool CursorOverButton(POINT cursorPoint);
		void DrawButton(HDC hdc, RECT *pRect, HDC bitmap, BITMAP bitmapInfo);

		void OnLButtonDown(POINT cursorPoint);
		bool OnLButtonUp(POINT cursorPoint);
		void OnMouseMove(POINT cursorPoint);
		void OnPaint();
		void OnEraseBackGround();

	public:
		float		m_fSplitter;
		DWORD		m_fMaxSplitter;
		DWORD		m_fMinSplitter;
		DWORD		m_dwSplitterWidth;

		CMRSplitter();
		~CMRSplitter();
		void Init(HWND hWnd, bool vertical, float initialValue = 50.0f, float minSplitter = 5.0, float maxSplitter = 95.0, DWORD splitterWidth = 8);
		void Destroy();

		bool GetCheck();
		void SetCheck(bool bChecked);

		void SetStyle(DWORD dwStyle);
		DWORD GetStyle();

		void Enable(bool bEnable);
		bool IsEnabled();

		void GetSplitterRect(RECT *pRect);
		void GetButtonRect(RECT *pRect);
		void SetCheckedSplitterPos(float fSplitterPos);
		void SetSplitterWidth(DWORD splitterWidth);
		void SetSplitterButtonHeight(DWORD buttonHeight);


		void SetUncheckedBitmap(HBITMAP bitmap);
		void SetCheckedBitmap(HBITMAP bitmap);

		void SetBitmaps(HBITMAP hBitmap, bool flipSides = false, bool adjustToBitmapSize = true);
		void SetBitmaps(string fileName, bool flipSides = false, bool adjustToBitmapSize = true);
		void SetBitmaps(HINSTANCE hInst, UINT resId, bool flipSides = false, bool adjustToBitmapSize = true);
		void AdjustToBitmapSize();

	};

	//MR_DECLARE_SERIALIZABLE(CMRSplitter);

#endif