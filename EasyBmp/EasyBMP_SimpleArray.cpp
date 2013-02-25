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

#include "EasyBMP_SimpleArray.h"
#include <cstdio>
#include <cctype>
#include <cstring>
#include <cstdlib>
char DetermineDelimiter( char* FileName );
int CountColumns( char* FileName , char Delimiter );
double GetDoubleFromFile( FILE* fp , char Delimiter );
int CountRowsInFile( char* FileName , char Delimiter );

char __COMMA__ = ',';
char __SPACE__ = ' ';
char __TAB__ = '\t';
char __SEMICOLON__ = ';';
char __NEWLINE__ = '\n';

char DetermineDelimiter( char* FileName )
{
 char output = '0';
 FILE* fp;
 fp = fopen( FileName , "r" );
 if( !fp ) 
 { return output; }
  
 bool done = false;
 
 // eat any delimiters at the beginning of the file 
 // I have to do this to deal with programs that insert extra whitespace. 
 while( !done && !feof( fp ) )
 {
  char c = getc(fp);
  if( c != __COMMA__ && c != __SPACE__ &&
      c != __TAB__   && c != __SEMICOLON__ )
  { done = true; ungetc( c , fp ); }
 }
 done = false;

 // now search for the first nontrivial delimiter 
 while( !done && !feof( fp ) )
 {
  char c = fgetc(fp);
  if( c == __COMMA__ || 
      c == __SPACE__ || 
      c == __TAB__ ||
      c == __SEMICOLON__ || 
	  c == __NEWLINE__ ) // added the possibility of newline
  { 
   done = true; 
   output = c;
  }
 }
 fclose( fp );  
 // do this just to make it work a tad bit better 
 // ideally, we should fix the code that looks for 
 // nontrivial lines
 if( output == __NEWLINE__ )
 { output = __SPACE__; }
 
 return output;
}

int CountColumns( char* FileName , char Delimiter )
{
 int output = 0;
 FILE* fp;
 fp = fopen( FileName , "r" );
 if( !fp ) 
 { return output; }

 bool done = false;
 
 // eat any delimiters at the beginning of the file 
 // I have to do this to deal with programs that insert extra whitespace. 
 while( !done && !feof( fp ) )

 {
  char c = getc(fp);
  if( c != __COMMA__ && c != __SPACE__ &&
      c != __TAB__   && c != __SEMICOLON__ )
  { done = true; ungetc( c , fp ); }
 }
 done = false;
 
 bool CharFound = false;
 
 while( !done && !feof( fp ) )
 {
  char c = getc(fp);
  
  // stop when we reach what must be a newline
  if( c <= 8 )
  { done = true; }
  if( c >= 10 && c <= 31 )
  { done = true; }
  
  if( isdigit(c) || c == '-' || c == 'e' )
  { CharFound = true; }
  if( CharFound && c == Delimiter )
  { CharFound = false; output++; }
  
 }
 
 if( CharFound == true )
 { output++; }
 
 fclose( fp );
 return output;
}

// returns the next double in the file buffer

double GetDoubleFromFile( FILE* fp , char Delimiter )
{
 char Output[2050];
 int Position = 0;
 bool done = false;
 
 // eat whitespace, preceding delimiters, and funny crap 
 
 while( !feof( fp ) && Position < 2049 && !done )
 {
  char c = getc( fp );
  bool internalstop = true;
  if( c <= 8 )
  { internalstop = false; }
  if( c >= 10 && c <= 31 )
  { internalstop = false; }  
  if( c == Delimiter )
  { internalstop = false; }
  if( internalstop )
  { ungetc( c, fp ); done = true; }
 } 
 
 // get all non-delimiter, non-weird characters before next delimiter
 done = false;
 while( !feof( fp ) && Position < 2049 && !done )
 {
  char c = getc( fp );
  if( c == Delimiter )
  { done = true; ungetc( c , fp ); }
  if( c <= 8 )
  { done = true; ungetc( c , fp ); }
  if( c >= 10 && c <= 31 )
  { done = true; ungetc( c , fp ); }
  if( !done )
  { Output[Position] = c; Position++; }
 }

 // insert null character 
 Output[Position] = '\0';
 
 return strtod( Output, NULL );
}

// 

int CountRowsInFile( char* FileName , char Delimiter )
{
 int output = 0;
 FILE* fp;
 fp = fopen( FileName , "r" );
 if( !fp ) 
 { return output; }

 bool really_done = false;
 
 while( !really_done && !feof( fp ) )
 {
  bool done = false; 
  bool MeaningfulRow = false;
  while( !done && !feof( fp ) )
  {
   char c = getc(fp);
   // stop when we reach what must be a newline
   if( c <= 8 )
   { done = true; }
   if( c >= 10 && c <= 31 )
   { done = true; }
   if( c == Delimiter || isdigit(c) || c == '.' || c == 'e' ) // added isdigit and forth
   { MeaningfulRow = true; }
  }
  if( MeaningfulRow )
  {
   output++;  
  }
 }
 
 fclose( fp );
 return output;
}

SimpleArray::~SimpleArray()
{
 int i=0;
 for(i=0;i<Rows;i++)
 {
  delete [] Data[i];
 }
 delete [] Data;
}

SimpleArray::SimpleArray()
{
 Delimiter = __TAB__;
 Rows = 1;
 Cols = 1;
 Data = new double* [1];

 Data[0] = new double [1];
 Data[0][0] = 0;
}

SimpleArray::SimpleArray(int nR, int nC)
{
 Delimiter = __TAB__;
 Rows = nR;
 Cols = nC; 
 Data = new double* [ nR ];
 int i=0, j=0; 
 for(i=0; i<nR; i++)
 {
  Data[i] = new double [ nC ];
 }
 for(i=0; i < nR; i++)
 {
  for(j=0; j < nC; j++)
  {
   Data[i][j] = 0;  
  }
 }
}

void SimpleArray::SetSize(int nR, int nC)
{
 Delimiter = __TAB__;
 int i=0, j=0; 

 for(i=0;i<Rows;i++)
 {
  delete [] Data[i];
 }
 
 delete [] Data;

 Rows = nR;
 Cols = nC; 
 Data = new double* [ nR ]; 
 
 for(i=0; i<nR; i++)
 {
  Data[i] = new double [ nC ];
 }
 
 for(i=0; i < nR; i++)
 {
  for(j=0; j < nC; j++)
  {
   Data[i][j] = 0;  
  }
 }
 
 return; 
}

bool SimpleArray::ReadFromFile( char* FileName )
{
 Delimiter = DetermineDelimiter( FileName ); 
 
 Rows = CountRowsInFile( FileName , Delimiter );
 Cols = CountColumns( FileName , Delimiter ); 
 
 int EntriesRead = 0;
 int DesiredEntries = Rows*Cols;
  
 Data = new double* [ Rows ];
 int i=0, j=0; 
 for(i=0; i<Rows; i++)
 {
  Data[i] = new double [ Cols ];
 } 
 if( Cols == 0 || Rows == 0 || Delimiter == '0' )
 { return false; }  
 
 FILE* fp;
 fp = fopen( FileName , "r" );
 if( !fp ) 
 { return false; }
 
 for(i=0; i < Rows; i++)
 {
  for(j=0; j < Cols; j++)
  {
   Data[i][j] = GetDoubleFromFile( fp , Delimiter );
   EntriesRead++;
  }
 }
 
 fclose( fp );
 
 if( EntriesRead == DesiredEntries )
 { return true; }
 return false;
}

void SimpleArray::WriteToFile( char* FileName )
{
 FILE* fp;
 fp = fopen( FileName , "w" );
 if( !fp ) 
 { return; }

 int i,j,k;
 for(i=0; i < Rows; i++)
 {
  for(j=0; j < Cols; j++)
  {
   char Temp [2049];
   sprintf( Temp,  "%2.12f" , Data[i][j] );
   for(k=0 ; k < strlen(Temp) ; k++)
   {
    putc( Temp[k] , fp );
   }
   putc( Delimiter , fp );
  }
  putc( '\n' , fp );
 }

 fclose( fp ); 
 return; 
}
 
double* SimpleArray::operator()(int i, int j)
{
 if( Rows == 1 )
 {
  return &(Data[0][j]);
 }
 if( Cols == 1 )
 {
  return &(Data[i][0]); 
 }
 return &(Data[i][j]);
}

double* SimpleArray::operator()(int i)
{
 if( Rows == 1 )
 {
  return &(Data[0][i]);
 }
 if( Cols == 1 )
 {
  return &(Data[i][0]); 
 }
 return &(Data[i][i]);
}
