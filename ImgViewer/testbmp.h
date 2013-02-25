#include "EasyBMP.h"



inline void test_bmp(const char *filename) {
	BMP inputBMP;
	inputBMP.ReadFromFile(filename);
	
	std::cout << (int)inputBMP(0, 0)->Red << " " << (int)inputBMP(0, 0)->Green << " " << (int)inputBMP(0, 0)->Blue << "\n";
}