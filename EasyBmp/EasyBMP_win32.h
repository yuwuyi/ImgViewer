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

bool HBITMAPtoBMP(HDC hDC,HBITMAP hBitmap,BMP& OutputImage);
HBITMAP BMPtoHBITMAP(HDC hDC,BMP& Input);
bool CaptureScreen( BMP& OutputImage );
bool CaptureWindow( HWND hWND , BMP& OutputImage );
bool CaptureForegroundWindow( BMP& OutputImage );

