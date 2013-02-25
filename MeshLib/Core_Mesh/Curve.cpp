#pragma warning (disable : 4018)
#include "Curve.h"
#include "Iterators.h"
#include "TraitParser.h"
#include "MeshUtility.h"
#include <iostream>
#include <fstream>


using namespace XMeshLib;

Curve::Curve(int length, HalfEdge * hes[])
{
	for (int i=0; i<length; ++i)
	{
		helist.push_back(hes[i]);
	}
	valid = true;
}

Curve::Curve(std::vector<HalfEdge *> & inhelist)
{
	for (int i=0; i<inhelist.size(); ++i)
	{
		helist.push_back(inhelist[i]);
	}
	valid = true;
}

Curve::Curve(std::list<Edge *> edges, Vertex * startV)
{
	BuildList(edges, startV);
}

Curve::Curve(std::list<Edge *> edges)
{
	std::list<Edge *>::iterator eiter = edges.begin();
	Edge * e  = *eiter;
	HalfEdge * he = e->halfedge(0);
	Vertex * v = he->source();
	BuildList(edges, v);
}

Curve::~Curve()
{
	;
}

void Curve::BuildList(std::list<Edge *> edges, Vertex * startV)
{
	// first we find the startV;
	valid = false;
	HalfEdge * startHE;
	std::list<Edge *>::iterator listEiter;
	for (listEiter=edges.begin(); listEiter!=edges.end(); ++listEiter)
	{
		//Edge * e = edges[i];
		Edge * e = *listEiter;
		
		if (e->halfedge(0)->source() == startV)
		{
			valid = true;
			startHE = e->halfedge(0);
			break;
		}
		else if (e->halfedge(1) && e->halfedge(1)->source() == startV)
		{
			valid = true;
			startHE = e->halfedge(1);
			break;
		}			
	}

	if (!valid)
		return;
	
	HalfEdge * che = startHE;
	helist.push_back(che);
	edges.erase(listEiter);
	
				
	while (edges.size()>0)
	{
		valid = false;
		for (listEiter=edges.begin(); listEiter!=edges.end(); ++listEiter)
		{
			Edge * e = *listEiter;
			HalfEdge * he1 = e->halfedge(0);
			//HalfEdge * he2 = e->halfedge(1);
			if (he1->source() == che->target())
			{
				// use he1
				valid = true;
				che = he1;	
				helist.push_back(che);
				break;
			}
			else if (he1->target() == che->target())
			{

				// he1
				che = e->halfedge(1);
				if (!che)
				{
					valid = false;
					return;							
				}
				else
				{
					valid = true;
					helist.push_back(che);
					break;
				}
			}
		}
		if (valid)
			edges.erase(listEiter);
		else
			return;
	}
}

Vertex * Curve::VertexAt(int const ind)
{
	if (!valid)
		return NULL;
	else if (ind < 0 || ind > helist.size())
	{
		return NULL;
	}
	else if (!ind)
	{
		return helist[0]->source();
	}
	else
	{
		return helist[ind-1]->target();
	}
}

int Curve::verSize()
{
	if (!valid)
		return -1;
	else
		return helist.size()+1;
}

void Curve::Clear()
{
	helist.clear();
	valid = true; //empty curve is valid
}

bool Curve::WriteToFile(const char file[])
{
	//if (!valid)
	//{
	//	std::cerr << "Error: The curve is invalid" << std::endl;
	//	return false;
	//}

	std::ofstream fp;
	fp.open(file);
	if (!fp.good())
	{
		std::cerr << "I/O Error: Cannot write into file " << file << " !" << std::endl;
		return false;
	}
	fp << helist.size() << std::endl;
	for (int i=0;i<helist.size(); ++i)
	{
		HalfEdge * he = helist[i];
		fp << he->source()->id() << " " << he->target()->id() << std::endl;
	}
	fp.close();
	return true;
}

bool Curve::ReadFromFile(XMeshLib::Mesh *mesh, const char file[])
{
	helist.clear();
	std::ifstream fp;
	fp.open(file);
	if (!fp.good())
	{
		std::cerr << "I/O Error: Cannot read from file " << file << " !" << std::endl;
		return false;
	}
	int hesize;
	fp >> hesize;	
	for (int i=0;i<hesize; ++i)
	{
		int id1, id2;
		fp >> id1;
		fp >> id2;
		Vertex * v1 = mesh->idVertex(id1);
		Vertex * v2 = mesh->idVertex(id2);
		if (!v1 || !v2)
		{
			std::cerr << "Error: input data donot fit the mesh" << std::endl;
			return false;
		}
		HalfEdge * he = mesh->vertexHalfedge(v1, v2);
		if (!he)
		{
			std::cerr << "Error: input data do not fit the mesh" << std::endl;
			assert(0);
			return false;
		}
		else
			helist.push_back(he);
		//bool flag = false;
		//for (VertexOutHalfedgeIterator vhe(mesh, v1); !vhe.end(); ++vhe)
		//{
		//	HalfEdge * he = *vhe;
		//	if (he->target() == v2)
		//	{
		//		helist.push_back(he);
		//		flag = true;
		//		break;
		//	}
		//}
		//if (!flag)
		//{
		//	std::cerr << "Error: input data do not fit the mesh" << std::endl;
		//	return false;
		//}
	}
	fp.close();
	return true;
}

void Curve::MarkOnMesh() const
{
	std::string strSharp = "sharp";
	for (int i=0;i<helist.size(); ++i)
	{
		Edge * e = helist[i]->edge();
		TraitParser::UpdateTrait(e->string(), strSharp, 0, NULL);
	}
}

//bool Curve::AppendHalfEdge(HalfEdge * he)
//{
//	helist.push_back(he);
//	return true;
//
//	//if (helist.size()==0)
//	//{
//	//	helist.push_back(he);
//	//	return true;
//	//}
//	//else
//	//{
//	//	//if (Tail() != he->source())
//	//	//{
//	//	//	return false;
//	//	//}
//	//	//else
//	//	//{
//	//		helist.push_back(he);
//	//		return true;
//	//	//}
//	//}
//}

void Curve::BruteConcatenate(const Curve & c)
{
	for (int i=0;i<c.heSize();++i)
	{
		helist.push_back(c[i]);
	}
}


bool Curve::Concatenate(const Curve & c)
{
	bool success=false;
	if (Tail() != c.Head())
		return success;
	else
	{
		for (int i=0; i<c.heSize(); ++i)
		{
			helist.push_back(c[i]);			
		}
	}
	return success;
}

void Curve::RemoveHalfEdges(int ind)
{
	assert(ind >= 0 && ind < helist.size());
	std::vector<HalfEdge *>::iterator heiter = helist.begin();
	heiter += ind;
	//for (int i=0;i<ind; i++, ++heiter)
	//{
	//	;
	//}
	helist.erase(heiter);
}

void Curve::SwapHalfEdge(int ind, HalfEdge * nhe)
{
	assert(ind >= 0 && ind < helist.size());
	helist[ind] = nhe;
}

void Curve::ReplaceSections(int startInd, int endInd, std::vector<HalfEdge  *> & replaceList)
{
	std::vector<HalfEdge *> nhelist;
	int i;
	for (i=0;i<startInd; ++i)
	{
		nhelist.push_back(helist[i]);
	}
	for (i=0;i<replaceList.size(); ++i)
	{
		nhelist.push_back(replaceList[i]);
	}
	for (i=endInd+1; i<helist.size(); ++i)
	{
		nhelist.push_back(helist[i]);
	}
	helist.clear();
	for (i=0;i<nhelist.size();++i)
	{
		helist.push_back(nhelist[i]);
	}
}

void Curve::ReplaceSections(Vertex * startV, Vertex * endV, std::vector<HalfEdge  *> & replaceList)
{	
	int i,j;
	int startInd, endInd;
	startInd = endInd = -1;
	for (i=0;i<helist.size(); ++i)
	{
		HalfEdge * he = helist[i];
		if (he->source() == startV)
		{
			startInd = i;
			break;
		}
	}
	assert(startInd != -1);
	for (j=i; j<helist.size();++j)
	{
		HalfEdge * he = helist[j];
		if (he->target() == endV)
		{
			endInd = j;
			break;
		}
	}
	assert(endInd != -1);
	ReplaceSections(startInd, endInd, replaceList);
}


int Curve::CreateBoundaryLoop(Vertex * startV)
{
	HalfEdge * he = startV->most_ccw_out_halfedge();
	if (!he->edge()->boundary())
	{
		he = startV->most_clw_out_halfedge();
		assert(he->edge()->boundary());
	}
	return CreateBoundaryLoop(he);
}

bool Curve::RobustTraceBoundaryLoop(HalfEdge* startHe, int & loopSize)
{
	HalfEdge * che = startHe;
	if (!che->source()->boundary() || !che->target()->boundary())
	{
		std::cerr << "Error: the input halfedge is not on the boundary!" << std::endl;
		loopSize=0;
		return false;
	}
	bool ccw;
	Vertex * ver = che->source();
	HalfEdge * the = ver->most_ccw_out_halfedge();
	if (the == che)
	{ //Possiblly ccw is right, except the case that there is only one out halfedge starting from ver
		//in that case, check one more step forward is enough
		HalfEdge * tempthe = the->target()->most_ccw_out_halfedge();
		if (tempthe->edge()->boundary())
			ccw = true;
		else
		{
			assert(ver->most_ccw_out_halfedge() == ver->most_clw_out_halfedge());
			ccw = false;
		}
	}
	else
	{
		ccw = false;
		the = ver->most_clw_out_halfedge();
		assert(the == che);
	}
	the = che;
	std::vector<HalfEdge *>::iterator finditer;
	while (the->target() != ver)
	{
		finditer = std::find(helist.begin(), helist.end(), the);
		if (finditer != helist.end())
		{//found
			std::cerr << "Possible Error: not go back to the starting point!" << std::endl;
			helist.erase(helist.begin(), finditer-1);
			loopSize =  helist.size();
			return false;
		}
		else
		{//not found
			helist.push_back(the);
		}	
		
		if (ccw)
			the = the->target()->most_ccw_out_halfedge();
		else 
			the = the->target()->most_clw_out_halfedge();
	}
	helist.push_back(the);
	loopSize = helist.size();
	return true;
}

int Curve::CreateBoundaryLoop(HalfEdge* startHe)
{
	HalfEdge * che = startHe;
	if (!che->source()->boundary() || !che->target()->boundary())
	{
		std::cerr << "Error: the input halfedge is not on the boundary!" << std::endl;
		return 0;
	}
	bool ccw;
	Vertex * ver = che->source();
	HalfEdge * the = ver->most_ccw_out_halfedge();
	if (the == che)
	{ //Possiblly ccw is right, except the case that there is only one out halfedge starting from ver
		//in that case, check one more step forward is enough
		HalfEdge * tempthe = the->target()->most_ccw_out_halfedge();
		if (tempthe->edge()->boundary())
			ccw = true;
		else
		{
			assert(ver->most_ccw_out_halfedge() == ver->most_clw_out_halfedge());
			ccw = false;
		}
	}
	else
	{
		ccw = false;
		the = ver->most_clw_out_halfedge();
		assert(the == che);
	}
	the = che;
	std::vector<HalfEdge *>::iterator finditer;
	while (the->target() != ver)
	{
		finditer = std::find(helist.begin(), helist.end(), the);
		if (finditer != helist.end())
		{//found
			std::cerr << "Possible Error: not go back to the starting point!" << std::endl;
			assert(0);
			return helist.size();
		}
		else
		{//not found
			helist.push_back(the);
		}	
		
		if (ccw)
			the = the->target()->most_ccw_out_halfedge();
		else 
			the = the->target()->most_clw_out_halfedge();
	}
	helist.push_back(the);
	return helist.size();
}

void Curve::TraceBoundary(HalfEdge* startHe, int step)
{
	HalfEdge * che = startHe;
	if (!che->source()->boundary() || !che->target()->boundary())
	{
		std::cerr << "Error: the input halfedge is not on the boundary!" << std::endl;
		return;
	}
	bool ccw;
	Vertex * ver = che->source();
	HalfEdge * the = ver->most_ccw_out_halfedge();
	if (the == che)
	{ //Possiblly ccw is right, except the case that there is only one out halfedge starting from ver
		//in that case, check one more step forward is enough
		HalfEdge * tempthe = the->target()->most_ccw_out_halfedge();
		if (tempthe->edge()->boundary())
			ccw = true;
		else
		{
			assert(ver->most_ccw_out_halfedge() == ver->most_clw_out_halfedge());
			ccw = false;
		}
	}
	else
	{
		ccw = false;
		the = ver->most_clw_out_halfedge();
		assert(the == che);
	}
	the = che;
	std::vector<HalfEdge *>::iterator finditer;

	for (int i=0;i<step;++i)
	{
		helist.push_back(the);
		if (ccw)
			the = the->target()->most_ccw_out_halfedge();
		else 
			the = the->target()->most_clw_out_halfedge();
	}
}

int Curve::TraceFeatureCurve(Mesh * mesh, HalfEdge * heStart)
{
	helist.clear();
	MeshUtility::MarkSharpEdges(mesh);
	HalfEdge * che = heStart;
	Edge * ce = che->edge();
	if (!ce->sharp())
	{
		std::cerr << "Error: the input halfedge is not a feature edge!" << std::endl;
		return 0;
	}
	helist.push_back(che);

	Vertex * startV = che->source();
	Vertex * cv = che->target();

	while (cv!=startV)
	{
		bool flag = false;
		for (VertexOutHalfedgeIterator vhe(mesh, cv); !vhe.end(); ++vhe)
		{
			HalfEdge * he = *vhe;
			if (he->edge()->sharp() && he->edge() != ce)
			{
				che = he;
				cv = che->target();
				ce = che->edge();				
				flag = true;
				break;
			}
		}
		if (!flag)
		{
			std::cerr << "Cannot find a circle!" << std::endl;
			helist.clear();
			return 0;
		}

		helist.push_back(che);
		if (che->target() == startV)
		{
			//succeed
			return helist.size();
		}	
	}
	return helist.size();
}

void Curve::TraceBoundary(HalfEdge * startHe, Vertex * endV)
{
	HalfEdge * che = startHe;
	if (!che->source()->boundary() || !che->target()->boundary())
	{
		std::cerr << "Error: the input halfedge is not on the boundary!" << std::endl;
		return;
	}
	bool ccw;
	Vertex * ver = che->source();
	HalfEdge * the = ver->most_ccw_out_halfedge();
	if (the == che)
	{ //Possiblly ccw is right, except the case that there is only one out halfedge starting from ver
		//in that case, check one more step forward is enough
		HalfEdge * tempthe = the->target()->most_ccw_out_halfedge();
		if (tempthe->edge()->boundary())
			ccw = true;
		else
		{
			assert(ver->most_ccw_out_halfedge() == ver->most_clw_out_halfedge());
			ccw = false;
		}
	}
	else
	{
		ccw = false;
		the = ver->most_clw_out_halfedge();
		assert(the == che);
	}
	the = che;
	std::vector<HalfEdge *>::iterator finditer;

	while (the->vertex() != endV)
	{
		helist.push_back(the);
		if (ccw)
			the = the->target()->most_ccw_out_halfedge();
		else 
			the = the->target()->most_clw_out_halfedge();
		if (helist.size()>10000000)
		{
			assert(0);
		}
	}
}
