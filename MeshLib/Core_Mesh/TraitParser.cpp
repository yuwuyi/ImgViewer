#include "TraitParser.h"
#include <sstream>

using namespace XMeshLib;

bool TraitParser::ReadTrait(std::string &parsestr, 
							const char * keywordstr, 
							const int dim, 
							double * values)
{
	std::string nkeystr = "";
	nkeystr += keywordstr;
	return ReadTrait(parsestr, nkeystr, dim, values);
}

bool TraitParser::ReadTrait(std::string &parsestr, 
							std::string &keywordstr, 
							const int dim, 
							double * values)
{
	Parser p(parsestr);
	bool flag = false;
	for (int i=0; i<p.tokens.size(); ++i)
	{
		std::string key = p.tokens[i].key;
		std::string value = p.tokens[i].value;
		if (key == keywordstr)
		{
			flag = true;
			if (dim == 1)
				sscanf_s(value.c_str(), "%lf", &values[0]);
			else if (dim ==2)
				sscanf_s(value.c_str(), "%lf %lf", &values[0], &values[1]);
			else if (dim ==3)
				sscanf_s(value.c_str(), "%lf %lf %lf", &values[0], &values[1], &values[2]);
			else if (dim ==4)
				sscanf_s(value.c_str(), "%lf %lf %lf %lf", &values[0], &values[1], &values[2], &values[3]);
		}
	}
	return flag;
}

bool TraitParser::ReadIntTrait(std::string &parsestr, 
							   const std::string &keywordstr, 
							   const int dim, int * values)
{
	Parser p(parsestr);
	bool flag = false;
	for (int i=0; i<p.tokens.size(); ++i)
	{
		std::string key = p.tokens[i].key;
		std::string value = p.tokens[i].value;
		if (key == keywordstr)
		{
			flag = true;
			if (dim == 1)
				sscanf_s(value.c_str(), "%d", &values[0]);
			else if (dim ==2)
				sscanf_s(value.c_str(), "%d %d", &values[0], &values[1]);
			else if (dim ==3)
				sscanf_s(value.c_str(), "%d %d %d", &values[0], &values[1], &values[2]);
			else if (dim ==4)
				sscanf_s(value.c_str(), "%d %d %d %d", &values[0], &values[1], &values[2], &values[3]);
		}
	}
	return flag;
}

bool TraitParser::RemoveTrait(std::string &parsestr, 
							  std::string &keywordstr)
{
	std::string keyname = keywordstr;
	std::string keybegin = keywordstr+"=(";
	std::string keyend = ")";
	std::string space = " ";
	std::string endstring = "}";
	std::string empty = "";
	std::string::size_type pos1 = parsestr.find(keyname,0);
	if (pos1 == std::string::npos)
	{
		//nothing is found
		return false;
	}
	else
	{
		std::string::size_type pos2 = parsestr.find(keybegin,0);
		if (pos2 == std::string::npos)
		{
			//the keywordstr does not have value attributes					
			std::string::size_type pos3 = parsestr.find(space, pos1);
			if (pos3 == std::string::npos)
			{
				parsestr.replace(pos1,std::string::npos,empty);
			}
			else
			{//should also remove the space after the keyword //by shane 2009/1/10				
				parsestr.replace(pos1,pos3-pos1+1,empty);
			}
		}
		else 
		{
			//the keywordstr has value attributes
			pos1 = pos2;
			std::string::size_type pos3 = parsestr.find(keyend, pos2);
			assert(pos3 != std::string::npos);
			if (pos3<parsestr.size()-1)
			{
				char cpos3=parsestr[pos3+1];
				if (cpos3==' ')
				{
					++pos3;
				}
			}
			else
			{
				if (pos1!=0)
					--pos1;
			}
			parsestr.replace(pos1,pos3-pos1+1,empty);
		}
	}
	return true;
}

bool TraitParser::UpdateTrait(std::string &parsestr, 
							  std::string &keywordstr, 
							  const int dim, 
							  double * values)
{			
	std::string keybegin = keywordstr + "=(";
	std::string keyend = ")";
	std::ostringstream vstr;
	vstr << keywordstr;
	for (int i=0;i<dim;++i)
	{
		if (!i)
			 vstr << "=(";
		else
			vstr << " ";
		vstr << values[i];
		if (i==dim-1)
			vstr <<")";
	}
	if (!parsestr.size())
	{					
		parsestr = vstr.str();
	}
	else
	{
		std::string nkeystr = vstr.str();
		std::string::size_type pos1 = parsestr.find(keybegin,0);
		if (pos1 == std::string::npos)
		{
			parsestr += " ";
			parsestr += nkeystr;
		}
		else
		{
			std::string::size_type pos2 = parsestr.find(keyend, pos1);
			parsestr.replace(pos1,pos2-pos1+1,nkeystr);
		}
	}
	return true;
}

bool TraitParser::UpdateIntTrait(std::string &parsestr, 
								 std::string &keywordstr, 
								 const int dim, 
								 int * values)
{			
	std::string keybegin = keywordstr + "=(";
	std::string keyend = ")";
	std::ostringstream vstr;
	vstr << keywordstr;
	for (int i=0;i<dim;++i)
	{
		if (!i)
			 vstr << "=(";
		else
			vstr << " ";
		vstr << values[i];
		if (i==dim-1)
			vstr <<")";
	}

	if (!parsestr.size())
	{					
		parsestr = vstr.str();
	}
	else
	{
		std::string nkeystr = vstr.str();
		std::string::size_type pos1 = parsestr.find(keybegin,0);
		if (pos1 == std::string::npos)
		{
			parsestr += " ";
			parsestr += nkeystr;
		}
		else
		{
			std::string::size_type pos2 = parsestr.find(keyend, pos1);
			parsestr.replace(pos1,pos2-pos1+1,nkeystr);
		}
	}
	return true;
}


bool TraitParser::CopyTraits(Mesh * mesh1, 
							 Mesh * mesh2, 
							 const char * keywordstr, 
							 int dim)
{
	if (mesh1->numVertices() != mesh2->numVertices())
		return false;
	std::string nkeystr = "";
	nkeystr += keywordstr;
	MeshVertexIterator viter1(mesh1);
	MeshVertexIterator viter2(mesh2);
	double values[4];
	for (; !viter1.end(); ++viter1, ++viter2)
	{
		Vertex * v1 = *viter1;
		Vertex * v2 = *viter2;
		bool cflag = ReadTrait(v1->string(), nkeystr, dim, values);
		if (!cflag)
			return false;
		else
			UpdateTrait(v2->string(), nkeystr, dim, values);
	}
	return true;
}
