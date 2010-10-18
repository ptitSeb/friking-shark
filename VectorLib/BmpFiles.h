#ifndef _mr_gui_image_
#define _mr_gui_image_

/*------------ mr_gui_image
 */

	class CMRImage
	{
		BITMAPINFO	m_Info;
		RGBQUAD 	m_rbgQuad;         
		void		*m_bits;
		int 		m_width;
		int 		m_height;
		int 		m_bitcount;
		int 		m_channels;
		HDC 		m_dc;
		HBITMAP 	m_hbitmap;
		HGDIOBJ 	m_holdBitmap;


		void Import(int w,int h,void *buffer,int bitcount);
		void DeleteImage();
		void Init_BITMAPINFO();
		void Init_All();

	public:
		CMRImage(CMRImage &image);
		CMRImage(int w,int h,void *buffer,int bitcount=24,int stretchMode=COLORONCOLOR);
		CMRImage(HWND wnd);
		CMRImage(HDC dc,int w=-1,int h=-1);
		CMRImage(HDC dc,int x, int y,int w,int h);
		CMRImage(HBITMAP hBitmap);
		CMRImage();
		~CMRImage();

		virtual CMRImage &operator=(CMRImage &);


		BITMAPINFO	GetInfo();
		void		RestoreObject(HGDIOBJ object);
		HBRUSH		SelectBrush(COLORREF col, int hatch=0, int style=BS_SOLID);
		HPEN		SelectPen(COLORREF col, int width=1, int style=PS_SOLID);

		HBRUSH		SelectNewBrush(COLORREF col, int hatch=0, int style=BS_SOLID);
		HPEN		SelectNewPen(COLORREF col, int width=1, int style=PS_SOLID);
		
		HDC 	DC();	
		int		Size();	
		int		Width();
		int		Height();
		int		BitCount();
		void	Bits(void *pbits);
		void 	Bits(void **,bool get_copy=false);
		void 	*Bits(bool get_copy=false);

		void Flip_Vertical();
		void Flip_Horizontal();

		void Draw(CMRImage &I,int x=0, int y=0, int w=-1, int h=-1,DWORD op=SRCCOPY);	
		void Draw(HDC dc,int x=0, int y=0, int w=-1, int h=-1,DWORD op=SRCCOPY);	
		void Draw(HDC dc,int x1, int y1, int w1, int h1,int x2, int y2, int w2, int h2,DWORD op=SRCCOPY);
		void Draw(HWND hwnd,int x=0, int y=0, int w=-1, int h=-1,DWORD op=SRCCOPY);	
		bool Clear(COLORREF color);
		bool Resize(int w,int h);
		bool Convert(int bitcount=24);

		BOOL SaveToFile(char *szFileName);
		BOOL LoadFromFile(const char *fullPath);
		BOOL LoadBitmapFromResource(HINSTANCE hInst,char *resId);

		BOOL GetRawRGB(DWORD *size, BYTE **buffer);
		BOOL GetRawRGBA(DWORD *size, BYTE **buffer);
		BOOL GetRawBGR(DWORD *size, BYTE **buffer);
		
		int SelectBkMode(int mode);
		HBITMAP GetBitmapHandle();
		BOOL CopyDIBits(DWORD len, BYTE *pBuffer);
	};

	int 	MRDDrawOutlinedText(HDC dc, char *text, COLORREF lineColor, COLORREF fillColor, RECT *pRect, DWORD options = DT_SINGLELINE, HFONT hFont = NULL);
	int 	MRDDrawNormalText(HDC dc, char *text, COLORREF backColor, COLORREF foreColor, RECT *pRect, DWORD options = DT_SINGLELINE, HFONT hFont = NULL);
	int 	MRDrawText(HDC dc, char *text, COLORREF backColor, COLORREF foreColor, RECT *pRect, DWORD options = DT_SINGLELINE, HFONT hFont = NULL, bool bOutlined = true);
	int 	MRDrawText(HDC dc, char *text, COLORREF backColor, COLORREF foreColor, RECT *pRect, DWORD options = DT_SINGLELINE, LOGFONT *pLogFont = NULL, bool bOutlined = true);
	HBRUSH	MRSelectBrush(HDC dc, COLORREF col, int hatch=0, int style=BS_SOLID);
	HPEN	MRSelectPen(HDC dc, COLORREF col, int width=1, int style=PS_SOLID);
	void	MRRestoreObject(HDC dc, HGDIOBJ object);

#endif
