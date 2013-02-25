/*************************************************
*                                                *
*  EasyBMP Cross-Platform Windows Bitmap Library * 
*                                                *
*  Authors: Paul Macklin                         *
*           Michael Nistler                      *
*   email: pmacklin@math.uci.edu                 *
*          <no_email_given>                      *
*                                                *
*    file: EasyBMP_win32.h                       *
*    date: 8-22-2005                             *
* version: 1.05.00                               *
*                                                *
*   License: BSD (revised)                       *
* Copyright: 2005-2006 by the EasyBMP Project    * 
*                                                *
* description: EasyBMP add-on to interface with  *
*              win32 / GDI                       *
*                                                *
*************************************************/

#include <windows.h>
#include "EasyBMP.h"
#include "EasyBMP_win32.h"

bool CaptureScreen( BMP& OutputImage )
{
 HDC screenDC = ::CreateDC("DISPLAY", NULL, NULL, NULL);
 HDC memoryDC = ::CreateCompatibleDC(screenDC);
 
 bool output = false;
 
 GetSystemMetrics(SM_CXSCREEN);
 GetSystemMetrics(SM_CXVIRTUALSCREEN);
 
 int width = GetDeviceCaps(screenDC, HORZRES);
 int height = GetDeviceCaps(screenDC, VERTRES);
    
 HBITMAP screenBitmap = ::CreateCompatibleBitmap(screenDC, width,height );
 HGDIOBJ result =  ::SelectObject(memoryDC, screenBitmap);
	
 if( result )
 {
  ::BitBlt(memoryDC, 0, 0, width,height, screenDC, 0, 0, SRCCOPY);
  output = HBITMAPtoBMP( screenDC , screenBitmap , OutputImage );
 }
 else
 {
 }
 
 ::DeleteDC(screenDC);
 ::DeleteDC(memoryDC);
 ::DeleteObject(result);
 ::DeleteObject(screenBitmap);        
 
 return output;
}

bool CaptureWindow( HWND hWND , BMP& OutputImage )
{
 HDC screenDC = ::GetWindowDC(hWND);
 RECT rect;
 GetWindowRect(hWND, &rect);
 
 HDC memoryDC = ::CreateCompatibleDC(screenDC);
 
 bool output = false;
 
 int width = labs( rect.right - rect.left ); 
 int height = labs( rect.top - rect.bottom ); 
    
 HBITMAP screenBitmap = ::CreateCompatibleBitmap(screenDC, width,height );
 HGDIOBJ result =  ::SelectObject(memoryDC, screenBitmap);
	
 if( result )
 {
  ::BitBlt(memoryDC, 0, 0, width,height, screenDC, 0, 0, SRCCOPY);
  output = HBITMAPtoBMP( screenDC , screenBitmap , OutputImage );
 }
 else
 {
 }
 
 ::DeleteDC(screenDC);
 ::DeleteDC(memoryDC);
 ::DeleteObject(result);
 ::DeleteObject(screenBitmap);        
 
 return output;
}

bool CaptureForegroundWindow( BMP& OutputImage )
{
 HWND hWND = ::GetForegroundWindow();
 HDC screenDC = ::GetWindowDC(hWND);
 RECT rect;
 GetWindowRect(hWND, &rect);
 
 HDC memoryDC = ::CreateCompatibleDC(screenDC);
 
 bool output = false;
 
 int width = labs( rect.right - rect.left ); // GetDeviceCaps(screenDC, HORZRES);
 int height = labs( rect.top - rect.bottom ); // GetDeviceCaps(screenDC, VERTRES);
    
 HBITMAP screenBitmap = ::CreateCompatibleBitmap(screenDC, width,height );
 HGDIOBJ result =  ::SelectObject(memoryDC, screenBitmap);
	
 if( result )
 {
  ::BitBlt(memoryDC, 0, 0, width,height, screenDC, 0, 0, SRCCOPY);
  output = HBITMAPtoBMP( screenDC , screenBitmap , OutputImage );
 }
 else
 {
 }
 
 ::DeleteDC(screenDC);
 ::DeleteDC(memoryDC);
 ::DeleteObject(result);
 ::DeleteObject(screenBitmap);        
 
 return output;
}


HBITMAP BMPtoHBITMAP(HDC hDC,BMP& Input)
{
 HBITMAP Output = CreateCompatibleBitmap( hDC , Input.TellWidth(), Input.TellHeight() );
  
 // create an array of bytes from the BMP pixels

 BYTE* pData;
 pData = new BYTE [Input.TellWidth()*Input.TellHeight()*4];
 int i,j;
 int k=0;
 for( j= Input.TellHeight()-1 ; j >= 0 ; j-- )
 {
  for( i=0 ; i < Input.TellWidth() ; i++ )
  {
   *(pData+k) = Input(i,j)->Blue; k++;
   *(pData+k) = Input(i,j)->Green; k++;
   *(pData+k) = Input(i,j)->Red; k++;
   *(pData+k) = Input(i,j)->Alpha; k++;
  }
 }
 
 // create the BITMAPINFO data structure
 
 BITMAPINFO BitInfo;
 ZeroMemory(&BitInfo, sizeof(BITMAPINFO));
 BitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
 BitInfo.bmiHeader.biWidth = Input.TellWidth();
 BitInfo.bmiHeader.biHeight = Input.TellHeight();
 BitInfo.bmiHeader.biPlanes = 1; 
 BitInfo.bmiHeader.biBitCount = 32;
 BitInfo.bmiHeader.biCompression = 0;
 BitInfo.bmiHeader.biSizeImage = Input.TellWidth()*Input.TellHeight()*4;
 BitInfo.bmiHeader.biXPelsPerMeter = 0;
 BitInfo.bmiHeader.biYPelsPerMeter = 0;
 BitInfo.bmiHeader.biClrUsed = 0;
 BitInfo.bmiHeader.biClrImportant = 0;
  
 SetDIBits( hDC, Output, 0, Input.TellHeight() , pData , 
            &BitInfo , DIB_RGB_COLORS );

 return Output;
}

bool HBITMAPtoBMP(HDC hDC,HBITMAP hBitmap,BMP& OutputImage)
{
 using namespace std;
 bool output = false;
    
 BITMAPINFO BitInfo;
 ZeroMemory(&BitInfo, sizeof(BITMAPINFO));
 BitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
 
 // get all manner of information from the incoming bitmap

 if(!::GetDIBits(hDC, hBitmap, 0, 0, NULL, &BitInfo, DIB_RGB_COLORS))
 { return false; }
 
 // Set the size and bit depth of the BMP object

 OutputImage.SetSize( BitInfo.bmiHeader.biWidth ,BitInfo.bmiHeader.biHeight );
 OutputImage.SetBitDepth(32);

 // reconfigure BitInfo.bmiHeader such that the resulting bitmap will be 
 // 32 bits per pixel. This is _MUCH_ simpler 
 
 BitInfo.bmiHeader.biBitCount = 32;
 BitInfo.bmiHeader.biCompression = 0;
 BitInfo.bmiHeader.biSizeImage = OutputImage.TellHeight()*OutputImage.TellWidth()*4;
 
 // I don't understand the +5 part here. -- Paul 

// BYTE *pData = new BYTE[BitInfo.bmiHeader.biSizeImage + 5];
// BYTE *pData = new BYTE[Output.TellHeight()*Output.TellWidth()*4+5];
 BYTE *pData = new BYTE [BitInfo.bmiHeader.biSizeImage];
 
 // now get the actual data
    
 if(!::GetDIBits(hDC, hBitmap, 0, BitInfo.bmiHeader.biHeight, pData, &BitInfo, DIB_RGB_COLORS))
 { return false; }
 
 // transfer that data into the BMP object
 
 int i,j;
 int k=0;
 for( j=OutputImage.TellHeight()-1 ; j >= 0 ; j-- )
 {
  for( i=0; i < OutputImage.TellWidth() ; i++ )
  {
   OutputImage(i,j)->Blue = *(pData+k); k++;
   OutputImage(i,j)->Green = *(pData+k); k++;
   OutputImage(i,j)->Red = *(pData+k); k++;
   OutputImage(i,j)->Alpha = *(pData+k); k++;
  }
 }
 
 delete (pData);
 
 return true;
}
