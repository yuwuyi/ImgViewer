#ifndef _XMESHLIB_CORE_PARSER_H_
#define _XMESHLIB_CORE_PARSER_H_

#pragma warning (disable : 4786)
#include <string>
#include <vector>
#include <stdio.h>
#include <assert.h>

namespace XMeshLib
{
	class Token
	{
	public:
		std::string key;
		std::string value;
	};

	class Parser
	{
	public:
		Parser(const std::string inS)
		{
			int temp = 1;
			sprintf_s(line, 2048, "%s ", inS.c_str());
			ParseToTokens();
		}
		~Parser(){;};
		void ParseToTokens();

		std::vector<Token> tokens;

	private:
		char line[2048];
		
	};
}

#endif