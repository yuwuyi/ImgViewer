/*************************************************
*                                                *
*  EasyBMP Cross-Platform Windows Bitmap Library * 
*                                                *
*  Author: Paul Macklin                          *
*   email: pmacklin@math.uci.edu                 *
*                                                *
*    file: EasyBMP_SimpleArray.h                 *
*    date: 2-17-2005                             *
* version: 1.05.00                               *
*                                                *
*   License: BSD (revised)                       *
* Copyright: 2005-2006 by the EasyBMP Project    * 
*                                                *
* description: Easy matrix class for data import *
*                                                *
*************************************************/

#ifndef _EasyBMP_SimpleArray_h_
#define _EasyBMP_SimpleArray_h_

class SimpleArray{
public:
 int Rows;
 int Cols;
 char Delimiter;
 double** Data;

 SimpleArray();
 SimpleArray(int nR, int nC); 
 bool ReadFromFile( char* FileName );
 void WriteToFile( char* FileName );
 void SetSize(int nR, int nC);
 
 double* operator()(int i, int j);
 double* operator()(int i );
 
 ~SimpleArray();
};

#endif
