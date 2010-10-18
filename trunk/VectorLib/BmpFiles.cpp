#include "stdafx.h"
#include <windows.h>
#include <stdlib.h>
#include "BMPFiles.h"
#include "Utilities.h"

BOOL SaveBitmapToBMPFile(LPTSTR szFileName, int w,int h,int planes,int bitCount,void *bits,HDC dc);

CMRImage::CMRImage()
{
	Init_All();
}

CMRImage::CMRImage(CMRImage &I)
{
	Init_All();
	*this=I;
}

CMRImage::CMRImage(int w,int h,void *buffer,int bitcount,int stretchMode)
{
	Init_All();
	
	m_width=w;
	m_height=h;
	m_bitcount=bitcount;
	m_channels = bitcount / 8;
	m_rbgQuad.rgbBlue=255;
	m_rbgQuad.rgbGreen=255;
	m_rbgQuad.rgbRed=255;
	m_rbgQuad.rgbReserved=0;
	
	
	Init_BITMAPINFO();
	
	HDC tdc;
	if(Size())
	{
		m_dc=CreateCompatibleDC(tdc=GetDC(GetDesktopWindow()));
		if(m_dc==NULL)
			//_MRT("CMRImage::CMRImage: Unable to CreateCompatibleDC.\n");
		
		SetStretchBltMode(m_dc,stretchMode);
		
		ReleaseDC(GetDesktopWindow(),tdc);
		if(!m_dc) {
			//_MRT("CMRImage::CMRImage: Unable to CreateCompatibleDC.\n");
		}
		
		if(GdiFlush()==0)
		{
			//_MRT("CMRImage::CMRImage: Unable to GdiFlush.\n");
		}
		
		m_hbitmap=CreateDIBSection(m_dc,&m_Info,/*m_bitcount==8?DIB_PAL_COLORS:*/DIB_RGB_COLORS,(void **)(&m_bits),NULL,0); 
		if(m_hbitmap==NULL)
		{
			LPVOID lpMsgBuf = NULL;
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), 0, (LPTSTR)&lpMsgBuf, 0, NULL);
			char A[1000] = "";
			wsprintf(A,"CMRImage::CMRImage(int,int,void*,int) CreateDIBSection FAILED (%s)\n", lpMsgBuf);
			//_MRT(A);
			LocalFree(lpMsgBuf);
		}
		if(m_bits!=NULL)
		{
			if(buffer==NULL)
			{
				memset(m_bits,0,Size());
			}
			else
			{
				memcpy(m_bits,buffer,Size());
			}
		}
		m_holdBitmap=SelectObject(m_dc,m_hbitmap);
	}
	else
	{
		//_MRT("CMRImage::CMRImage = 0");
	}
}

CMRImage::CMRImage(HWND wnd)
{
	Init_All();
	HDC dc=GetDC(wnd);
	RECT R = {0};
	GetClientRect(wnd,&R);
	
	Import(R.right-R.left,R.bottom-R.top,NULL,GetDeviceCaps(dc,BITSPIXEL));
	BitBlt(m_dc,0,0,m_width,m_height,dc,0,0,SRCCOPY);
	ReleaseDC(wnd,dc);
}


CMRImage::CMRImage(HDC dc,int w,int h)
{
	Init_All();
	if(w==-1){w=GetDeviceCaps(dc,HORZRES);}
	if(h==-1){h=GetDeviceCaps(dc,VERTRES);}
	Import(w,h,NULL,GetDeviceCaps(dc,BITSPIXEL));
	BitBlt(m_dc,0,0,m_width,m_height,dc,0,0,SRCCOPY);
}

CMRImage::CMRImage(HDC dc,int x, int y,int w,int h)
{
	Init_All();
	Import(w,h,NULL,GetDeviceCaps(dc,BITSPIXEL));
	BitBlt(m_dc,0,0,m_width,m_height,dc,x,y,SRCCOPY);
}

CMRImage::CMRImage(HBITMAP hBitmap)
{
	Init_All();

	BITMAP	bitmapInfo = {0};
	::GetObject(hBitmap, sizeof(bitmapInfo), &bitmapInfo);

	Import(bitmapInfo.bmWidth, bitmapInfo.bmHeight, NULL, 24);//bitmapInfo.bmBitsPixel);

	HDC hdc = CreateCompatibleDC(NULL);
	HBITMAP hOldbitmap = (HBITMAP)::SelectObject(hdc, hBitmap);

	BitBlt(m_dc, 0, 0, m_width, m_height, hdc, 0, 0, SRCCOPY);
	GdiFlush();

	::SelectObject(hdc, hOldbitmap);
	::DeleteDC(hdc);	
}

CMRImage::~CMRImage()
{
	DeleteImage();
}

void CMRImage::DeleteImage()
{
	if(m_dc!=NULL){SelectObject(m_dc,m_holdBitmap);m_holdBitmap=NULL;} // Bounds checker
	if(m_hbitmap!=NULL){DeleteObject(m_hbitmap);m_hbitmap=NULL;}
	if(m_dc!=NULL){DeleteDC(m_dc);m_dc=NULL;}
	
	Init_All();
}

void CMRImage::Import(int w,int h,void *buffer,int bitcount)
{
	int oldMode=COLORONCOLOR;
	if(m_dc!=NULL){oldMode=GetStretchBltMode(m_dc);}
	
	DeleteImage();
	
	m_width=w;
	m_height=h;
	m_bitcount=(bitcount==0)?24:bitcount;
	m_channels=m_bitcount/8;
	
	Init_BITMAPINFO();
	
	if(Size())
	{
		m_dc=CreateCompatibleDC(NULL);
		SetStretchBltMode(m_dc,oldMode);
		m_hbitmap=CreateDIBSection(m_dc,&m_Info,DIB_RGB_COLORS,(void **)(&m_bits),NULL,0); 
		if(m_hbitmap==NULL)
		{
			//_MRT("CMRImage::Import -> CreateDIBSection FAILED");
		}
		if(m_bits!=NULL)
		{
			if(buffer==NULL)
			{memset(m_bits,0,Size());}
			else
			{memcpy(m_bits,buffer,Size());}
		}
		m_holdBitmap=SelectObject(m_dc,m_hbitmap);
	}
}

void CMRImage::Init_All()
{
	m_width=0;
	m_height=0;
	m_bitcount=0;
	m_channels=0;
	m_dc=NULL;
	m_bits=NULL;
	m_hbitmap=NULL;
	m_holdBitmap=NULL;

	Init_BITMAPINFO();
}

void CMRImage::Init_BITMAPINFO()
{
	memset(&m_Info,0,sizeof(BITMAPINFO));
	m_Info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	m_Info.bmiHeader.biBitCount=m_bitcount;
	m_Info.bmiHeader.biCompression=BI_RGB ;
	m_Info.bmiHeader.biHeight=m_height;
	m_Info.bmiHeader.biWidth=m_width;
	m_Info.bmiHeader.biPlanes=1;
	m_Info.bmiHeader.biSizeImage=m_width*m_height*(m_bitcount/8);
	m_Info.bmiHeader.biClrUsed=m_Info.bmiHeader.biBitCount==8?2:0;
	m_Info.bmiHeader.biClrImportant=0;
	m_Info.bmiHeader.biXPelsPerMeter=50;
	m_Info.bmiHeader.biYPelsPerMeter=50;
}

CMRImage &CMRImage::operator=(CMRImage &I)
{
	if(I.Width()==Width() && I.Height()==Height() && I.BitCount()==BitCount() && I.Bits()!=NULL && Bits()!=NULL)
	{
		memcpy(Bits(),I.Bits(),Size());
		return *this;
	}

	Import(I.Width(),I.Height(),I.Bits(),I.BitCount());

	return *this;
}

BITMAPINFO CMRImage::GetInfo()
{
	return m_Info;
}
HDC CMRImage::DC()
{
	return m_dc;
}
int CMRImage::Size()		
{
	if(m_width * m_height == 0){return 0;}
	return ((m_width * m_bitcount +31) & ~31)/8*m_height;
}

int CMRImage::Width()		{return m_width;}
int CMRImage::Height()	{return m_height;}
int CMRImage::BitCount()	{return m_bitcount;}
void CMRImage::Bits(void *pbits)
{
	memcpy(pbits,m_bits,Size());
}

void CMRImage::Bits(void **ppbits,bool get_copy)
{
	if(!get_copy){*ppbits=m_bits;return;}
	
	void *t=new char[Size()];
	memcpy(t,m_bits,Size());
	*ppbits=t;
}

void *CMRImage::Bits(bool get_copy)
{
	void *t;
	Bits(&t,get_copy);
	return t;
}

bool CMRImage::Clear(COLORREF color)
{
	HBRUSH old_brush;
	HPEN   old_pen;
	
	old_brush=(HBRUSH)SelectBrush(color);
	old_pen=(HPEN)SelectPen(color);
	
	Rectangle(m_dc,0,0,m_width,m_height);
	
	RestoreObject(old_pen);
	RestoreObject(old_brush);
	return true;
}
bool CMRImage::Resize(int w,int h)
{
	if(w<0){w=0;}
	if(h<0){h=0;}
	if(m_width==w && m_height==h){return false;}
	if(m_bits==NULL || m_width==0 || m_height==0)
	{
		Import(w,h,NULL,m_bitcount);
		return true;
	}
	
	CMRImage I(w,h,NULL,m_bitcount);
	Draw(I,0,0,w,h);
	*this=I;
	return true;
}
bool CMRImage::Convert(int bitcount)
{
	CMRImage I(m_width,m_height,NULL,bitcount);
	Draw(I);
	*this=I;
	return false;
}


void CMRImage::Draw(HDC dc,int x, int y, int w, int h,DWORD op)
{
	bool success=false;
	if((w==-1 || w==m_width) && (h==-1 || h==m_height))
	{
	success= (0 != BitBlt(dc,x,y,m_width,m_height,m_dc,0,0,op));
	}
	else
	{
		success= (0 != StretchBlt(dc,x,y,w,h,m_dc,0,0,m_width,m_height,op));
		if (GetStretchBltMode(m_dc) == HALFTONE)
		{
			SetBrushOrgEx(m_dc, 0, 0, NULL);
		}
	}
	if(!success)
	{
		//_MRT("CMRImage::Draw -> BitBlt FAILED");
	}
}

void CMRImage::Draw(HDC dc,int x1, int y1, int w1, int h1,int x2, int y2, int w2, int h2,DWORD op)
{
	bool success=false;
	success= (0 != StretchBlt(dc,x1,y1,w1,h1,m_dc,x2,y2,w2,h2,op));
	if (GetStretchBltMode(m_dc) == HALFTONE)
	{
		SetBrushOrgEx(m_dc, 0, 0, NULL);
	}

	if(!success)
	{
		//_MRT("CMRImage::Draw -> BitBlt FAILED");
	}
}

void CMRImage::Draw(CMRImage &I,int x, int y, int w, int h,DWORD op)
{
	if(I.DC()!=NULL){Draw(I.DC(),x,y,w,h,op);}
}

void CMRImage::Draw(HWND hwnd,int x, int y, int w, int h,DWORD op)
{
	HDC dc=GetDC(hwnd);
	Draw(dc,x,y,w,h,op);
	ReleaseDC(hwnd,dc);
}

void CMRImage::Flip_Vertical()
{
	if(Size()==0 || m_bits==NULL){return;}
	
	int y,tamx=m_width*(m_bitcount/8);
	char *buffer;
	buffer=new char[Size()];
	
	for(y=0;y<m_height;y++)
	{
		memcpy(buffer+(m_height-y-1)*tamx,((char *)m_bits)+tamx*y,tamx);
	}
	
	memcpy(m_bits,buffer,Size());
	if(buffer){delete [] buffer;buffer=NULL;}
}

void CMRImage::Flip_Horizontal()
{
	if(m_bitcount%8)
	{
		//_MRT("CMRImage::Flip_Horizontal solo funciona con 8,16 y 24 bpp");
		return;
	}
	
	int x,y,tamx=m_width*(m_bitcount/8),bytes_pixel=m_bitcount/8;
	char *buffer;
	buffer=new char[Size()];
	
	for(x=0;x<m_width;x++)
	{
		for(y=0;y<m_height;y++)
		{
			memcpy(buffer+x*bytes_pixel+tamx*y,((char *)m_bits + tamx*y+(m_width-1-x)*bytes_pixel),bytes_pixel);
		}
	}
	
	memcpy(m_bits,buffer,Size());
	if(buffer){delete [] buffer;buffer=NULL;}

}

HBRUSH CMRImage::SelectBrush(COLORREF col, int hatch, int style)
{
	HBRUSH  brush;
	LOGBRUSH LB;
	LB.lbColor=col;
	LB.lbHatch=hatch;
	LB.lbStyle=style;
	brush=CreateBrushIndirect(&LB);
	return (HBRUSH)SelectObject(m_dc,brush);
}

HPEN CMRImage::SelectPen(COLORREF col, int grosor, int style)
{
	HPEN  pen;
	LOGPEN LP;
	LP.lopnColor=col;
	LP.lopnStyle=style;
	LP.lopnWidth.x=grosor;
	LP.lopnWidth.y=0;
	
	pen=CreatePenIndirect(&LP);
	
	return (HPEN)SelectObject(m_dc,pen);
	
}

HBRUSH CMRImage::SelectNewBrush(COLORREF col, int hatch, int style)
{
	HBRUSH  brush;
	LOGBRUSH LB;
	LB.lbColor=col;
	LB.lbHatch=hatch;
	LB.lbStyle=style;
	brush=CreateBrushIndirect(&LB);
	return (HBRUSH)SelectObject(m_dc,brush);
}

HPEN CMRImage::SelectNewPen(COLORREF col, int grosor, int style)
{
	HPEN  pen;
	LOGPEN LP;
	LP.lopnColor=col;
	LP.lopnStyle=style;
	LP.lopnWidth.x=grosor;
	LP.lopnWidth.y=0;
	
	pen=CreatePenIndirect(&LP);
	
	return (HPEN)SelectObject(m_dc,pen);
	
}

void CMRImage::RestoreObject(HGDIOBJ objeto)
{
	DeleteObject(SelectObject(m_dc,objeto));
}




BOOL LoadBitmapFromBMPFile( LPCTSTR szFileName, HBITMAP *phBitmap,HPALETTE *phPalette )
{
	   BITMAP  bm;
	   
	   *phBitmap = NULL;
	   *phPalette = NULL;
	   
	   // Use LoadImage() to get the image loaded into a DIBSection
	   *phBitmap = (HBITMAP)LoadImage( NULL, szFileName, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE );
	   
	   if(*phBitmap==NULL){return FALSE;}
	   
	   // Get the color depth of the DIBSection
	   GetObject(*phBitmap, sizeof(BITMAP), &bm );
	   // If the DIBSection is 256 color or less, it has a color table
	   if( ( bm.bmBitsPixel * bm.bmPlanes ) <= 8 )
	   {
		   HDC           hMemDC;
		   HBITMAP       hOldBitmap;
		   RGBQUAD       rgb[256];
		   LPLOGPALETTE  pLogPal;
		   WORD          i;
		   
		   // Create a memory DC and select the DIBSection into it
		   hMemDC = CreateCompatibleDC( NULL );
		   hOldBitmap = (HBITMAP)SelectObject( hMemDC, *phBitmap );
		   // Get the DIBSection's color table
		   GetDIBColorTable( hMemDC, 0, 256, rgb );
		   // Create a palette from the color tabl
		   pLogPal = (LOGPALETTE *)malloc(sizeof(LOGPALETTE) + (256*sizeof(PALETTEENTRY)) );
		   pLogPal->palVersion = 0x300;
		   pLogPal->palNumEntries = 256;
		   for(i=0;i<256;i++)
		   {
			   pLogPal->palPalEntry[i].peRed = rgb[i].rgbRed;
			   pLogPal->palPalEntry[i].peGreen = rgb[i].rgbGreen;
			   pLogPal->palPalEntry[i].peBlue = rgb[i].rgbBlue;
			   pLogPal->palPalEntry[i].peFlags = 0;
		   }
		   *phPalette = CreatePalette( pLogPal );
		   // Clean up
		   	if(pLogPal){free(pLogPal);pLogPal=NULL;}
		   if(hOldBitmap!=NULL){SelectObject( hMemDC, hOldBitmap );} // el if lo he metido por el Bounds Checker
		   DeleteDC( hMemDC );
	   }
	   else   // It has no color table, so use a halftone palette
	   {
		   HDC    hRefDC;
		   
		   hRefDC = ::GetDC( NULL );
		   *phPalette = CreateHalftonePalette( hRefDC );
		   ::ReleaseDC( NULL, hRefDC );
	   }
	   return TRUE;
} 

BOOL CMRImage::SaveToFile(char *szFileName)
{
	BOOL res = FALSE;
	
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath(szFileName, drive, dir, fname, ext);
	_strupr(ext);
	if(strcmp(ext, ".BMP")==0)
		{res=SaveBitmapToBMPFile(szFileName, Width(),Height(),1,BitCount(),Bits(),DC());}
//	else if(strcmp(ext, ".JPG")==0 || strcmp(ext, ".JPEG")==0)
//		{res=SaveBitmapToJPEGFile(szFileName, Width(),Height(),BitCount(),(BYTE *)Bits());}

	return res;
}

BOOL CMRImage::LoadBitmapFromResource(HINSTANCE hInst,char *resId)
{
	HBITMAP hBitmap = (HBITMAP)LoadImage(hInst, resId, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
	if(hBitmap==NULL){return FALSE;}

	BITMAP bm = {0};
	::GetObject(hBitmap,sizeof(BITMAP),&bm);
	Resize(bm.bmWidth,bm.bmHeight);
	
	HDC hdcMem = CreateCompatibleDC(m_dc);
	if(hdcMem!=NULL)
	{
		HGDIOBJ hOldBMP = SelectObject(hdcMem, (HGDIOBJ)hBitmap);
		RECT R={0,0,bm.bmWidth,bm.bmHeight};
		BOOL bRet=BitBlt(m_dc,0,0,bm.bmWidth,bm.bmHeight,hdcMem,0,0,SRCCOPY);
		SelectObject(hdcMem,hOldBMP);
		DeleteDC(hdcMem);
		hdcMem=NULL;
	}

	DeleteObject(hBitmap);hBitmap=NULL;
	SetStretchBltMode(m_dc,HALFTONE);
	return TRUE;
}


BOOL CMRImage::LoadFromFile(const char *fullPath)
{
	BOOL ret = FALSE;
	HRESULT hr=S_OK;
	SIZE mapSize={0,0};
	SetStretchBltMode(m_dc, HALFTONE);
	
//	_MRT("CMRImage::LoadFromFile -> Enter, updating %s",fullPath);
//	_MRT("CMRImage::LoadFromFile -> Enter, updating %s, IS BMP FILE",fullPath);

	HBITMAP				hMapBitmap=NULL;
	HPALETTE			hMapPalette=NULL;
	
	LoadBitmapFromBMPFile(fullPath, &hMapBitmap, &hMapPalette);
	
	BITMAP bm;
	memset(&bm,0,sizeof(bm));
	bm.bmBits=NULL;
	if(hMapBitmap!=NULL){::GetObject(hMapBitmap,sizeof(BITMAP),&bm);}
	
	Resize(bm.bmWidth,bm.bmHeight);
	
	HDC hdcMem = CreateCompatibleDC(m_dc);
	if (hdcMem!=NULL)
	{
		HGDIOBJ hOldBMP=SelectObject(hdcMem,(HGDIOBJ)hMapBitmap);
		RECT R={0,0,bm.bmWidth,bm.bmHeight};
		BOOL bRet=BitBlt(m_dc,0,0,bm.bmWidth,bm.bmHeight,hdcMem,0,0,SRCCOPY);
		SelectObject(hdcMem,hOldBMP);
		DeleteDC(hdcMem);
		hdcMem=NULL;
	}
	if(hMapBitmap==NULL){Resize(0,0);hr=E_FAIL;}
	if(hMapBitmap!=NULL){DeleteObject(hMapBitmap);hMapBitmap=NULL;}
	if(hMapPalette!=NULL){DeleteObject(hMapPalette);hMapPalette=NULL;}

	if(SUCCEEDED(hr))
	{
		SetStretchBltMode(m_dc,HALFTONE);
		return TRUE;
	}
	return FALSE;
}

BOOL CMRImage::GetRawRGB(DWORD *size, BYTE **buffer)
{
	*size = m_height * m_width * m_bitcount/8;
	*buffer = new BYTE[*size];
	
	int lineSize = m_width * m_bitcount/8;
	int paddedLineSize = (lineSize/4)*4;
	if(paddedLineSize < lineSize){paddedLineSize += 4;}

	BYTE *bufferPointer = (*buffer) + (lineSize * (m_height-1));
	BYTE *imagePointer = (BYTE *)m_bits;

	int x, y;
	for(x = m_height-1; x >= 0; x--)
	{
		for(y = 0; y < lineSize; y+=3)
		{
			bufferPointer[y]   = imagePointer[y+2];
			bufferPointer[y+1] = imagePointer[y+1];
			bufferPointer[y+2] = imagePointer[y];
		}
		 
		bufferPointer -= lineSize;
		imagePointer  += paddedLineSize;
	}
	
	return TRUE;
}

BOOL CMRImage::GetRawRGBA(DWORD *size, BYTE **buffer)
{
	int lineSize = m_width * 24/8;
	int lineSizeAlpha = m_width * 32/8;
	int paddedLineSize = (lineSize/4)*4;
	int paddedLineSizeAlpha = (lineSizeAlpha/4)*4;

	if(paddedLineSize < lineSize){paddedLineSize += 4;}
	if(paddedLineSizeAlpha < lineSizeAlpha){lineSizeAlpha+= 4;}

	*size = m_height * paddedLineSizeAlpha;
	*buffer = new BYTE[*size];

	BYTE *bufferPointer = (*buffer) + (paddedLineSizeAlpha* (m_height-1));
	BYTE *imagePointer = (BYTE *)m_bits;

	int x, y,y2;
	for(x = 0; x < m_height; x++)
	{
		for(y = 0,y2=0; y < lineSize; y+=3,y2+=4)
		{
			bufferPointer[y2]   = imagePointer[y+2];
			bufferPointer[y2+1] = imagePointer[y+1];
			bufferPointer[y2+2] = imagePointer[y];
			bufferPointer[y2+3] = 255;
		}
		 
		bufferPointer -= paddedLineSizeAlpha;//lineSize;
		imagePointer  += paddedLineSize;//lineSize;
	}
	
	return TRUE;
}

BOOL CMRImage::GetRawBGR(DWORD *size, BYTE **buffer)
{
	*size = m_height * m_width * m_bitcount/8;
	*buffer = new BYTE[*size];
	
	int lineSize = m_width * m_bitcount/8;
	int paddedLineSize = (lineSize/4)*4;
	if(paddedLineSize < lineSize){paddedLineSize += 4;}

	BYTE *bufferPointer = (*buffer) + (lineSize * (m_height-1));
	BYTE *imagePointer = (BYTE *)m_bits;

	int x;
	for(x = m_height-1; x >= 0; x--)
	{
		memcpy(bufferPointer, imagePointer, lineSize);
		 
		bufferPointer -= lineSize;
		imagePointer  += paddedLineSize;
	}
	
	return TRUE;
}

int CMRImage::SelectBkMode(int mode)
{
	return SetBkMode(m_dc,mode);
}
HBITMAP CMRImage::GetBitmapHandle()
{
	return m_hbitmap;
}

BOOL CMRImage::CopyDIBits(DWORD len, BYTE *pBuffer)
{
	if(m_bits == NULL){return FALSE;}
	if(pBuffer == NULL){return FALSE;}

	DWORD l = m_width * m_height * 3;
	if(len > l){l = len;}
	memcpy(m_bits, pBuffer, l);

	return true;
}



////////////////////////////////////
//// Generic functions
/////////////////////////////////////

PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp)
{ 
    BITMAP bmp; 
    PBITMAPINFO pbmi; 
    WORD    cClrBits; 
	
    // Retrieve the bitmap's color format, width, and height. 
    if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp)) 
        return NULL;
	
    // Convert the color format to a count of bits. 
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 
    if (cClrBits == 1) 
        cClrBits = 1; 
    else if (cClrBits <= 4) 
        cClrBits = 4; 
    else if (cClrBits <= 8) 
        cClrBits = 8; 
    else if (cClrBits <= 16) 
        cClrBits = 16; 
    else if (cClrBits <= 24) 
        cClrBits = 24; 
    else cClrBits = 32; 
	
    // Allocate memory for the BITMAPINFO structure. (This structure 
    // contains a BITMAPINFOHEADER structure and an array of RGBQUAD 
    // data structures.) 
	
	if (cClrBits != 24) 
	{
		pbmi = (PBITMAPINFO) new char[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1<< cClrBits)]; 
		memset(pbmi,0,sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1<< cClrBits));
	}
	
	// There is no RGBQUAD array for the 24-bit-per-pixel format. 
	
	else 
	{
		pbmi = (PBITMAPINFO)new BITMAPINFOHEADER; 
		memset(pbmi,0,sizeof(BITMAPINFOHEADER));
	}
	
	
    // Initialize the fields in the BITMAPINFO structure. 
	
    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
    pbmi->bmiHeader.biWidth = bmp.bmWidth; 
    pbmi->bmiHeader.biHeight = bmp.bmHeight; 
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes; 
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
    if (cClrBits < 24) 
        pbmi->bmiHeader.biClrUsed = (1<<cClrBits); 
	
    // If the bitmap is not compressed, set the BI_RGB flag. 
    pbmi->bmiHeader.biCompression = BI_RGB; 
	
    // Compute the number of bytes in the array of color 
    // indices and store the result in biSizeImage. 
    // For Windows NT/2000, the width must be DWORD aligned unless 
    // the bitmap is RLE compressed. This example shows this. 
    // For Windows 95/98, the width must be WORD aligned unless the 
    // bitmap is RLE compressed.
    pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8
		* pbmi->bmiHeader.biHeight; 
    // Set biClrImportant to 0, indicating that all of the 
    // device colors are important. 
	pbmi->bmiHeader.biClrImportant = 0; 
	return pbmi; 
} 


BOOL CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi,void *lpBits,HDC hDC) 
{ 
	HANDLE hf;                 // file handle 
    BITMAPFILEHEADER hdr;       // bitmap file-header 
    PBITMAPINFOHEADER pbih;     // bitmap info-header 
    DWORD dwTotal;              // total count of bytes 
    DWORD cb;                   // incremental count of bytes 
    BYTE *hp;                   // byte pointer 
    DWORD dwTmp; 
	
    pbih = (PBITMAPINFOHEADER) pbi; 
	
    if (!lpBits) 
		return FALSE;


    // Create the .BMP file. 
    hf = CreateFile(pszFile, 
		GENERIC_READ | GENERIC_WRITE, 
		(DWORD) 0, 
		NULL, 
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		(HANDLE) NULL); 
    if (hf == INVALID_HANDLE_VALUE) {return FALSE;}
    hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M" 
    // Compute the size of the entire file. 
    hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + 
		pbih->biSize + pbih->biClrUsed 
		* sizeof(RGBQUAD) + pbih->biSizeImage); 
    hdr.bfReserved1 = 0; 
    hdr.bfReserved2 = 0; 
	
    // Compute the offset to the array of color indices. 
    hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + 
		pbih->biSize + pbih->biClrUsed 
		* sizeof (RGBQUAD); 
	
    // Copy the BITMAPFILEHEADER into the .BMP file. 
    if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), 
        (LPDWORD) &dwTmp,  NULL)) 
	{CloseHandle(hf);return FALSE;}
	
    // Copy the BITMAPINFOHEADER and RGBQUAD array into the file. 
    if(!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) 
		+ pbih->biClrUsed * sizeof (RGBQUAD), 
		(LPDWORD) &dwTmp, ( NULL)))
	{CloseHandle(hf);return FALSE;}
	
    // Copy the array of color indices into the .BMP file. 
    dwTotal = cb = pbih->biSizeImage; 
    hp = (unsigned char *)lpBits; 
    if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL)) 
	{CloseHandle(hf);return FALSE;}
	
	
    // Close the .BMP file. 
	CloseHandle(hf);
	
 	return TRUE;
}


BOOL CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi, 
				   HBITMAP hBMP, HDC hDC) 
{ 
	HANDLE hf;                 // file handle 
    BITMAPFILEHEADER hdr;       // bitmap file-header 
    PBITMAPINFOHEADER pbih;     // bitmap info-header 
    LPBYTE lpBits;              // memory pointer 
    DWORD dwTotal;              // total count of bytes 
    DWORD cb;                   // incremental count of bytes 
    BYTE *hp;                   // byte pointer 
    DWORD dwTmp; 
	
    pbih = (PBITMAPINFOHEADER) pbi; 
    lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED,sizeof(BYTE)*pbih->biSizeImage);
	
    if (!lpBits) 
		return FALSE;
	
    // Retrieve the color table (RGBQUAD array) and the bits 
    // (array of palette indices) from the DIB. 
    if (!GetDIBits(hDC, hBMP, 0, (WORD) pbih->biHeight, lpBits, pbi, 
        DIB_RGB_COLORS)) 
	{return FALSE;}

    // Create the .BMP file. 
    hf = CreateFile(pszFile, 
		GENERIC_READ | GENERIC_WRITE, 
		(DWORD) 0, 
		NULL, 
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		(HANDLE) NULL); 
    if (hf == INVALID_HANDLE_VALUE) {return FALSE;}
    hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M" 
    // Compute the size of the entire file. 
    hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + 
		pbih->biSize + pbih->biClrUsed 
		* sizeof(RGBQUAD) + pbih->biSizeImage); 
    hdr.bfReserved1 = 0; 
    hdr.bfReserved2 = 0; 
	
    // Compute the offset to the array of color indices. 
    hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + 
		pbih->biSize + pbih->biClrUsed 
		* sizeof (RGBQUAD); 
	
    // Copy the BITMAPFILEHEADER into the .BMP file. 
    if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), 
        (LPDWORD) &dwTmp,  NULL)) 
	{CloseHandle(hf);return FALSE;}
	
    // Copy the BITMAPINFOHEADER and RGBQUAD array into the file. 
    if(!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) 
		+ pbih->biClrUsed * sizeof (RGBQUAD), 
		(LPDWORD) &dwTmp, ( NULL)))
	{CloseHandle(hf);return FALSE;}
	
    // Copy the array of color indices into the .BMP file. 
    dwTotal = cb = pbih->biSizeImage; 
    hp = lpBits; 
    if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL)) 
	{CloseHandle(hf);return FALSE;}
	
	
    // Close the .BMP file. 
	CloseHandle(hf);
	
    // Free memory. 
    GlobalFree(lpBits);
	return TRUE;
}

BOOL SaveBitmapToBMPFile(LPTSTR szFileName, int w,int h,int planes,int bitCount,void *bits,HDC dc)
{
	SetLastError(0);

	HBITMAP hBitmap=CreateBitmap(w,h,planes,bitCount,const_cast<const void*>(bits));
	if(hBitmap==NULL){return FALSE;}
	
	BITMAPINFO* pInfo=CreateBitmapInfoStruct(NULL,hBitmap);
	BOOL res=CreateBMPFile(NULL,szFileName,pInfo,bits,dc);
	DWORD err=GetLastError();
	
	if(pInfo){delete pInfo;pInfo=NULL;}
	DeleteObject(hBitmap);
	
	SetLastError(err);
	return res;
}


HBRUSH MRSelectBrush(HDC dc, COLORREF col, int hatch, int style)
{
	HBRUSH  brush;
	LOGBRUSH LB;
	LB.lbColor=col;
	LB.lbHatch=hatch;
	LB.lbStyle=style;
	brush=CreateBrushIndirect(&LB);
	return (HBRUSH)SelectObject(dc,brush);
}

HPEN MRSelectPen(HDC dc, COLORREF col, int width, int style)
{
	HPEN  pen;
	LOGPEN LP;
	LP.lopnColor=col;
	LP.lopnStyle=style;
	LP.lopnWidth.x=width;
	LP.lopnWidth.y=0;
	
	pen=CreatePenIndirect(&LP);
	
	return (HPEN)SelectObject(dc,pen);
	
}
void MRRestoreObject(HDC dc, HGDIOBJ object)
{
	DeleteObject(SelectObject(dc,object));
}

int MRDrawOutlinedText(HDC dc, char *text, COLORREF lineColor, COLORREF fillColor, RECT *pRect, DWORD options, HFONT hFont)
{
	int res = 0;

	HFONT oldFont = NULL;
	if(hFont){oldFont = (HFONT)SelectObject(dc, hFont);}
	if(options & DT_CALCRECT)
	{
		res = DrawText(dc, text, strlen(text), pRect, options);
		if(oldFont){(HFONT)SelectObject(dc, oldFont);}
		return res;
	}
	

	RECT newRect = *pRect;
	RECT R = {0};

	if(options & DT_CENTER || options & DT_VCENTER)
	{
		res = DrawText(dc, text, strlen(text), &R, ((options & ~DT_CENTER) & ~DT_VCENTER) | DT_CALCRECT);
		int left = newRect.left, top = newRect.top, right = newRect.right, bottom = newRect.bottom, width = 0, height = 0;
		

		if(options & DT_CENTER)
		{
			width = R.right - R.left;
			left = newRect.left + (((newRect.right-newRect.left) - width) /2);
			if(left < newRect.left){left = newRect.left;}
			right = left + width;
			if(right < newRect.right){right = newRect.right;}
		}

		if(options & DT_VCENTER)
		{
			height = R.bottom - R.top;
			top = newRect.top + (((newRect.bottom-newRect.top) - height) /2);
			if(top < newRect.top){top = newRect.top;}
			bottom = left + height;
			if(bottom < newRect.bottom){bottom = newRect.bottom;}
		}
		newRect.left = left;
		newRect.top = top;
		newRect.right = right;
		newRect.bottom = bottom;

		options = (options & ~DT_CENTER) & ~DT_VCENTER;
	}

	R = newRect;
	
	R.bottom += 4;
	R.right += 4;

	int oldMode = SetBkMode(dc, TRANSPARENT);
	COLORREF oldColor = SetTextColor(dc, lineColor);

	int x,y;
	for(R.left = newRect.left-2, x = 0; x < 5; x++,R.left++)
	{
		for(R.top = newRect.top-2,  y = 0; y < 5; y++,R.top++)
		{
			if(x != 0 && y != 0){DrawText(dc, text, strlen(text), &R, options);}
		}
	}

	SetTextColor(dc, fillColor);
	res = DrawText(dc, text, strlen(text), &newRect, options);

	if(oldFont){(HFONT)SelectObject(dc, oldFont);}
	SetTextColor(dc, oldColor);
	SetBkMode(dc, oldMode);

	return res;
}

int MRDrawNormalText(HDC dc, char *text, COLORREF backColor, COLORREF foreColor, RECT *pRect, DWORD options, HFONT hFont)
{
	int res = 0;

	HFONT oldFont = NULL;
	if(hFont){oldFont = (HFONT)SelectObject(dc, hFont);}
	if(options & DT_CALCRECT)
	{
		res = DrawText(dc, text, strlen(text), pRect, options);
		if(oldFont){(HFONT)SelectObject(dc, oldFont);}
		return res;
	}
	
	RECT R;
	R = *pRect;

	int oldMode = SetBkMode(dc, OPAQUE);
	COLORREF oldForeColor = SetTextColor(dc, foreColor);
	COLORREF oldBackColor = SetBkColor(dc, backColor);

	res = DrawText(dc, text, strlen(text), pRect, options);

	if(oldFont){(HFONT)SelectObject(dc, oldFont);}
	SetTextColor(dc, oldForeColor);
	SetBkColor(dc, oldBackColor);
	SetBkMode(dc, oldMode);

	return res;
}

int MRDrawText(HDC dc, char *text, COLORREF backColor, COLORREF foreColor, RECT *pRect, DWORD options, HFONT hFont, bool bOutlined)
{
	int res = 0;
	if(bOutlined)
		{res = MRDrawOutlinedText(dc, text, backColor, foreColor, pRect, options, hFont);}
	else
		{res = MRDrawNormalText(dc, text, backColor, foreColor, pRect, options, hFont);}
	return res;
}

int MRDrawText(HDC dc, char *text, COLORREF backColor, COLORREF foreColor, RECT *pRect, DWORD options, LOGFONT *pLogFont, bool bOutlined)
{
	HFONT hFont = NULL;
	if(pLogFont){hFont = CreateFontIndirect(pLogFont);}
	int res = MRDrawText(dc, text, backColor, foreColor, pRect, options, hFont, bOutlined);
	DeleteObject(hFont);
	return res;
}
