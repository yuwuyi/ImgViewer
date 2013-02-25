#ifndef X_TRAITPARSER_H
#define X_TRAITPARSER_H

#include "Parser.h"
#include "Mesh.h"
#include "Iterators.h"

#pragma warning (disable: 4018)

namespace XMeshLib
{
	class TraitParser
	{
	public:
		/*
			static method: ReadTrait, parse the "keywordstr" in "parsestr", the result will be return by values[0..dim-1]
				parsestr: the string to be parsed
				keywordstr:	the trait name
				dim: dimention of the key
				values: store the parsing results
		*/
		static bool ReadTrait(std::string &parsestr, 
			const char * keywordstr, const int dim, double * values);

		static bool ReadTrait(std::string &parsestr, 
			std::string &keywordstr, const int dim, double * values);

		static bool ReadIntTrait(std::string &parsestr, const std::string &keywordstr, 
			const int dim, int * values);
	
		static bool RemoveTrait(std::string &parsestr, std::string &keywordstr);

		static bool UpdateTrait(std::string &parsestr, 
			std::string &keywordstr, 
			const int dim, 
			double * values);

		static bool UpdateIntTrait(std::string &parsestr, 
			std::string &keywordstr, 
			const int dim, 
			int * values);
		
		static bool CopyTraits(Mesh * mesh1, Mesh * mesh2, 
			const char * keywordstr, int dim);
	
	};
}

#endif

