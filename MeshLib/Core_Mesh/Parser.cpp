#pragma warning (disable : 4786)

#include "Parser.h"
#include <string>
#include <vector>
#include <stdio.h>
#include <assert.h>

using namespace XMeshLib;

void Parser::ParseToTokens()
{
	char * tp = line;
	int status = 0;
	std::string tkey="";
	std::string tvalue="";
	while (*tp)
	{
		if (*tp == ' ' && !status)
		{
			if (tkey.size())
			{
				Token ntoken;
				ntoken.key = tkey;
				ntoken.value = tvalue;
				tkey = "";
				tvalue = "";
				tokens.push_back(ntoken);
			}
		}
		else if (*tp == '=')
		{
			assert(!status);
			status = 1;
		}
		else if (*tp == '(')
		{
			status = 2;
		}
		else if (*tp == ')')
		{
			status = 1;
		}
		else if (*tp == ' ' && status == 1)
		{
			status = 0;
			Token ntoken;
			ntoken.key = tkey;
			ntoken.value = tvalue;
			tkey = "";
			tvalue = "";
			tokens.push_back(ntoken);
		}
		else
		{
			if (!status)
				tkey += *tp;
			else
			{
				tvalue += *tp;
			}
		}
		tp++;
	}
}